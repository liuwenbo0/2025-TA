#include <cstdlib>
#include <fstream>
#include <limits>
#include <unordered_map>
#include <vector>

#include "triangel_impl.hh"
// #include "fifo_rp.hh"

int Triangel::target_size=0;
int Triangel::current_size=0;
int64_t Triangel::global_timestamp=0;
AssociativeSet<Triangel::MarkovMapping>* Triangel::markovTablePtr=NULL;
std::vector<uint32_t> Triangel::setPrefetch(17,0);
Triangel::SizeDuel* Triangel::sizeDuelPtr=nullptr;
uint64_t metadataAccesses = 0, lookupWrong = 0, lookupCorrect = 0;
constexpr bool FILL_THIS_LEVEL = false;


constexpr int address_map_actual_entries = 196608;
constexpr int address_map_max_ways = 8;
constexpr int address_map_actual_cache_assoc = 12;
constexpr int address_map_rounded_entries = 262144;
constexpr int address_map_rounded_cache_assoc = 16;

constexpr int training_unit_assoc = 16;
constexpr int training_unit_entries = 512;

constexpr int sample_assoc = 2;
constexpr int sample_entries = 512;

constexpr int metadata_reuse_assoc = 2;
constexpr int metadata_reuse_entries = 256;

constexpr int secondchance_assoc = 2;
constexpr int secondchance_entries = 64;

constexpr int lookup_assoc = 0;
constexpr int lookup_offset = 11;

int getWayAllocationMax = 0;

replacement_policy::Base* training_unit_replacement_policy;
BaseIndexingPolicy* training_unit_indexing_policy;

replacement_policy::Base*  sample_replacement_policy;
BaseIndexingPolicy* sample_indexing_policy;

BaseIndexingPolicy* secondchance_indexing_policy;
replacement_policy::Base* secondchance_replacement_policy;

BaseIndexingPolicy* address_map_cache_indexing_policy;
replacement_policy::Base* address_map_cache_replacement_policy;

BaseIndexingPolicy* metadata_reuse_indexing_policy;
replacement_policy::Base* metadata_reuse_replacement_policy;

OutFile* meta_outf_ptr;

vector<long long> cache_hit_cnt(100), pref_hit_cnt(100);
vector<long long> pref_hit_value_cnt(100), cache_hit_value_cnt(100);

void init_polices() {
    replacement_policy::LRURPParams lru_replace_p;
    replacement_policy::FIFORPParams fifo_replace_p;

    SetAssociativeParams index_p;

    using replacement_policy::LRU;
    using replacement_policy::FIFO;

    training_unit_replacement_policy = new LRU(lru_replace_p);
    
    index_p.entry_size = 1;
    index_p.assoc = training_unit_assoc;
    index_p.size = training_unit_entries;
    training_unit_indexing_policy = new SetAssociative(index_p);

    sample_replacement_policy = new LRU(lru_replace_p);
    
    index_p.entry_size = 1;
    index_p.assoc = sample_assoc;
    index_p.size = sample_entries;
    sample_indexing_policy = new SetAssociative(index_p);

    secondchance_replacement_policy = new FIFO(fifo_replace_p);

    index_p.entry_size = 1;
    index_p.assoc = secondchance_assoc;
    index_p.size = secondchance_entries;
    secondchance_indexing_policy = new SetAssociative(index_p);

    address_map_cache_replacement_policy = new LRU(lru_replace_p);
    
    index_p.entry_size = 1;
    index_p.assoc = address_map_rounded_cache_assoc;
    index_p.size = address_map_rounded_entries;
    address_map_cache_indexing_policy = new TriangelHashedSetAssociative(index_p);

    metadata_reuse_replacement_policy = new LRU(lru_replace_p);
    
    index_p.entry_size = 1;
    index_p.assoc = metadata_reuse_assoc;
    index_p.size = metadata_reuse_entries;
    metadata_reuse_indexing_policy = new SetAssociative(index_p);
}

Triangel::Triangel()
:
    trainingUnit(training_unit_assoc, training_unit_entries,
                 training_unit_indexing_policy,
                 training_unit_replacement_policy),
    historySampler(sample_assoc,
    		  sample_entries,
    		  sample_indexing_policy,
    		  sample_replacement_policy),
    secondChanceUnit(secondchance_assoc,
    		  secondchance_entries,
    		  secondchance_indexing_policy,
    		  secondchance_replacement_policy),
    markovTable(address_map_rounded_cache_assoc,
                          address_map_rounded_entries,
                          address_map_cache_indexing_policy,
                          address_map_cache_replacement_policy,
                          MarkovMapping()),                   
    metadataReuseBuffer(metadata_reuse_assoc,
                          metadata_reuse_entries,
                          metadata_reuse_indexing_policy,
                          metadata_reuse_replacement_policy,
                          MarkovMapping())
{
    degree = 1;
    // cachetags(p.cachetags),
    cacheDelay = 25;
    should_lookahead = true;
    should_rearrange = true;
    use_scs = true;
    use_bloom = false;
    use_reuse = true;
    use_pattern = true;
    use_pattern2 = true;
    use_mrb = true;
    perfbias = false;
    smallduel = false;
    timed_scs = true;
    useSampleConfidence = false;
    // sctags(p.sctags),
    max_size = address_map_actual_entries;
    size_increment = address_map_actual_entries/address_map_max_ways;
    second_chance_timestamp = 0;
    //global_timestamp(0),
    //current_size(0),
    //target_size(0),
    maxWays = address_map_max_ways;    
    
    // bloomset = -1;
    way_idx = {address_map_actual_entries/(address_map_max_ways*address_map_actual_cache_assoc),0};

    globalReuseConfidence = SatCounter8(7,64);
    globalPatternConfidence = SatCounter8(7,64);
    globalHighPatternConfidence = SatCounter8(7,64);

    // trainingUnit = AssociativeSet(training_unit_assoc, training_unit_entries,
    //              training_unit_indexing_policy,
    //              training_unit_replacement_policy);

    lookupAssoc = lookup_assoc;
    lookupOffset = lookup_offset;        
    //setPrefetch(cachetags->getWayAllocationMax()+1,0),      
    useHawkeye = false;


    // historySampler = AssociativeSet(sample_assoc,
    // 		  sample_entries,
    // 		  sample_indexing_policy,
    // 		  sample_replacement_policy);

    // secondChanceUnit = AssociativeSet(secondchance_assoc,
    // 		  secondchance_entries,
    // 		  secondchance_indexing_policy,
    // 		  secondchance_replacement_policy);

    // markovTable = AssociativeSet(address_map_rounded_cache_assoc,
    //                       address_map_rounded_entries,
    //                       address_map_cache_indexing_policy,
    //                       address_map_cache_replacement_policy,
    //                       MarkovMapping());
    // metadataReuseBuffer = AssociativeSet(metadata_reuse_assoc,
    //                       metadata_reuse_entries,
    //                       metadata_reuse_indexing_policy,
    //                       metadata_reuse_replacement_policy,
    //                       MarkovMapping());
    lastAccessFromPFCache = false;
 
	markovTablePtr = &markovTable;
    assert(markovTablePtr != nullptr);

    getWayAllocationMax = CACHE::LLC_POINTER->NUM_WAY;

	setPrefetch.resize(getWayAllocationMax+1,0);
	assert(address_map_rounded_entries / address_map_rounded_cache_assoc == address_map_actual_entries / address_map_actual_cache_assoc);
	markovTable.setWayAllocationMax(address_map_actual_cache_assoc);
	assert(getWayAllocationMax> maxWays);
	int bloom_size = address_map_actual_entries/128 < 1024? 1024: address_map_actual_entries/128;
	// assert(bloom_init2(&bl,bloom_size, 0.01)==0);
	// blptr = &bl;
	for(int x=0;x<64;x++) {
		hawksets[x].setMask = address_map_actual_entries/ hawksets[x].maxElems;
		hawksets[x].reset();
	}
		sizeDuelPtr= sizeDuels;	
	for(int x=0;x<64;x++) {
		sizeDuelPtr[x].reset(size_increment/address_map_actual_cache_assoc - 1 ,address_map_actual_cache_assoc,getWayAllocationMax);
	}

	for(int x=0;x<1024;x++) {
		lookupTable[x]=0;
    		lookupTick[x]=0;
	}	
	current_size = 0;
	target_size=0;

}

bool
Triangel::randomChance(int reuseConf, int replaceRate) {
	replaceRate -=8;

	uint64_t baseChance = 1000000000l * historySampler.numEntries / markovTable.numEntries;
	baseChance = replaceRate>0? (baseChance << replaceRate) : (baseChance >> (-replaceRate));
	baseChance = reuseConf < 3 ? baseChance / 16 : baseChance;
	uint64_t chance = get_rand<uint64_t>(0,1000000000ul);

	return baseChance >= chance;
}

void
Triangel::calculatePrefetch(uint64_t ori_addr, uint64_t ip, uint8_t cache_hit, uint8_t type, uint32_t metadata_in, uint64_t instr_id, uint64_t current_cycle,
    std::vector<Addr> &addresses, OutFile& out_f, bool warmup_complete)
{
    uint64_t pc = ip;
    Addr addr = blockIndex(ori_addr);
    second_chance_timestamp++;

    // This prefetcher requires a PC
    if (type == RFO || type == WRITEBACK) {
    // if (type != LOAD) {
        for(int x = 0; x < 64; x++) {
        int res = 0;
        if (x == 0) {
            if (out_f.active) {
                if (warmup_complete && (x == 0)) {
                    out_f.f << " sizeDuelPtr.checkAndInsert x: " << x << " addr: " << addr << " setMask: " << sizeDuelPtr[x].setMask << " set: " << sizeDuelPtr[x].set << " same: " << ((addr & sizeDuelPtr[x].setMask) == sizeDuelPtr[x].set);
                    out_f.f.flush();
                }
            }
            if (warmup_complete) res =    	sizeDuelPtr[x].checkAndInsert(addr,false, out_f); //TODO: combine with hawk?
            else res =    	sizeDuelPtr[x].checkAndInsert(addr,false); //TODO: combine with hawk?

            if (out_f.active) {
                if (warmup_complete && (x == 0)) {
                    out_f.f <<  " res " << res << " cycle: " << current_cycle  << std::endl;
                    out_f.f.flush();
                }
            }
        } else {
            res =    	sizeDuelPtr[x].checkAndInsert(addr,false); //TODO: combine with hawk?
        }


            if(res==0)continue;
            int cache_hit = res%128;
            int cache_set = cache_hit-1;
            assert(!cache_hit || (cache_set<setPrefetch.size()-1 && cache_set>=0));
            if(cache_hit) for(int y= setPrefetch.size()-2-cache_set; y>=0; y--) {
                setPrefetch[y]++; 
                cache_hit_cnt[y]++;
                cache_hit_value_cnt[y]++;
            }
            // cache partition hit at this size or bigger. So hit in way 14 = y=17-2-14=1 and 0: would hit with 0 ways reserved or 1, not 2.
        }
        return;
    }


    std::string NAME = "Triangel";
    // if (out_f.active) {
    //     // if (warmup_complete) {
    //         out_f.f << "[" << NAME << "] " << __func__ << " 1";
    //         out_f.f << " instr_id: " << instr_id << " addr: " << std::hex << addr;
    //         out_f.f << " ip: " << ip << std::dec << " cycle: " << current_cycle << std::endl;
    //     // }
    //     out_f.f.flush();
    // }

    bool is_secure = false;

    // Looks up the last address at this PC
    TrainingUnitEntry *entry = trainingUnit.findEntry(pc, is_secure);

    volatile bool correlated_addr_found = false;
    Addr index = 0;
    Addr target = 0;

    const int upperHistory=globalPatternConfidence>64?7:8;
    const int highUpperHistory=globalHighPatternConfidence>64?7:8;
    const int superHistory=14;
    
    const int upperReuse=globalReuseConfidence>64?7:8;

    bool should_pf=false;
    bool should_hawk=false;
    bool should_sample = false;

    if (entry != nullptr) { //This accesses the training table at this PC.
        trainingUnit.accessEntry(entry);
        correlated_addr_found = true;
        index = entry->lastAddress;

        if(addr == entry->lastAddress) return; // to avoid repeat trainings on sequence.
        if(entry->highPatternConfidence >= superHistory || !use_pattern2) entry->currently_twodist_pf=true;
        if(entry->patternConfidence < upperHistory && use_pattern2) entry->currently_twodist_pf=false; 
        //if very sure, index should be lastLastAddress. TODO: We could also try to learn timeliness here, by tracking PCs at the MSHRs.
        if(entry->currently_twodist_pf && should_lookahead) index = entry->lastLastAddress;
        target = addr;
        should_pf = (entry->reuseConfidence > upperReuse || !use_reuse) && (entry->patternConfidence > upperHistory || !use_pattern); //8 is the reset point.

        global_timestamp++;
    }


    // TODO: DIFF
    should_pf = false;

    if(entry==nullptr && (randomChance(8,8) || ((!use_reuse || globalReuseConfidence > 64) && (!use_pattern2 || globalHighPatternConfidence > 64) && (!use_pattern || globalPatternConfidence > 64)))){ //only start adding samples for frequent entries.
	//TODO: should this instead just be tagless?
    	if(!((!use_reuse || globalReuseConfidence > 64) && (!use_pattern2 || globalHighPatternConfidence > 64)  && (!use_pattern || globalPatternConfidence > 64)))should_sample = true;
        entry = trainingUnit.findVictim(pc);
        // DPRINTF(HWPrefetch, "Replacing Training Entry %x\n",pc);
        if (out_f.active) {
            if (warmup_complete) {
                out_f.f << "Replacing Training Entry " << std::hex << pc << std::dec << " cycle: " << current_cycle << std::endl;
            }
            out_f.f.flush();
        }


        assert(entry != nullptr);
        assert(!entry->isValid());


        trainingUnit.insertEntry(pc, is_secure, entry);

        //printf("local timestamp %ld\n", entry->local_timestamp);
        if(globalHighPatternConfidence>96) entry->currently_twodist_pf=true;
    }

    if(correlated_addr_found) {
    	//Check second-chance sampler for a recent history. If so, update pattern confidence accordingly.
    	SecondChanceEntry* tentry = secondChanceUnit.findEntry(addr, is_secure);
    	if(tentry!= nullptr && !tentry->used) {
    		tentry->used=true;
    		TrainingUnitEntry *pentry = trainingUnit.findEntry(tentry->pc, is_secure);
            if((!timed_scs || (tentry->global_timestamp + 512 > second_chance_timestamp)) && pentry != nullptr) {  
                if(tentry->pc==pc) {
                    pentry->patternConfidence++;
                        pentry->highPatternConfidence++;
                        globalPatternConfidence++;
                        globalHighPatternConfidence++;
                    }
            } else if(pentry!=nullptr) {
                                    pentry->patternConfidence--;
                                globalPatternConfidence--;
                                if(!perfbias) { pentry->patternConfidence--; globalPatternConfidence--;} //bias 
                                for(int x=0;x<(perfbias? 2 : 5);x++) {pentry->highPatternConfidence--;  globalHighPatternConfidence--;}
            }
    	}
    	
    	//Check history sampler for entry.
    	SampleEntry *sentry = historySampler.findEntry(entry->lastAddress, is_secure);
    	if(sentry != nullptr && sentry->entry == entry) {    		
    		
		int64_t distance = sentry->entry->local_timestamp - sentry->local_timestamp;
		if(distance > 0 && distance < max_size) { entry->reuseConfidence++;globalReuseConfidence++;}
		else if(!sentry->reused){ entry->reuseConfidence--;globalReuseConfidence--;}
    		sentry->reused = true;

     	    	// DPRINTF(HWPrefetch, "Found reuse for addr %x, PC %x, distance %ld (train %ld vs sample %ld) confidence %d\n",addr, pc, distance, entry->local_timestamp, sentry->local_timestamp, entry->reuseConfidence+0);
        if (out_f.active) {
            if (warmup_complete) {
                out_f.f << "Found reuse for addr " << std::hex << addr << ", PC " << pc << ", distance " << std::dec << distance << " (train " <<  entry->local_timestamp << " vs sample " << sentry->local_timestamp << ") confidence " << (entry->reuseConfidence+0) << " cycle: " << current_cycle << std::endl;
            }
            out_f.f.flush();
        }


		bool willBeConfident = addr == sentry->next;
    		
            uint32_t set = CACHE::L2_POINTER->get_set((uint64_t)(sentry->next<<lBlkSize));
            uint32_t way = CACHE::L2_POINTER->get_way((uint64_t)(sentry->next<<lBlkSize), set);
            auto s_block = CACHE::L2_POINTER->block[set * CACHE::L2_POINTER->NUM_WAY + way];
            
    		// if(addr == sentry->next ||  (use_scs && sctags->findBlock(sentry->next<<lBlkSize,is_secure) && !sctags->findBlock(sentry->next<<lBlkSize,is_secure)->wasPrefetched())) {
    		if(addr == sentry->next || (s_block.valid)) {
    			if(addr == sentry->next) {
    				entry->patternConfidence++;
    				entry->highPatternConfidence++;
    				globalPatternConfidence++;
	    			globalHighPatternConfidence++;
    			}
    			//if(entry->replaceRate < 8) entry->replaceRate.reset();
    		}
    		
    		else {
    			//We haven't spotted the (x,y) pattern we expect, on seeing y. So put x in the SCS.
			if(use_scs) {
				SecondChanceEntry* tentry = secondChanceUnit.findVictim(addr);
				if(tentry->pc !=0 && !tentry->used) {
    			   		TrainingUnitEntry *pentry = trainingUnit.findEntry(tentry->pc, is_secure);	
    			   		if(pentry != nullptr) {
			    			pentry->patternConfidence--;
			    			globalPatternConfidence--;
			    			if(!perfbias) { pentry->patternConfidence--; globalPatternConfidence--;} //bias 
			    			for(int x=0;x<(perfbias? 2 : 5);x++) {pentry->highPatternConfidence--;  globalHighPatternConfidence--;  	 }		   		
    			   		}			
				}
	    			secondChanceUnit.insertEntry(sentry->next, is_secure, tentry);
	    			tentry->pc = pc;
	    			tentry->global_timestamp = second_chance_timestamp;
	    			tentry->used = false;
    			} else {
    				entry->patternConfidence--;
    				globalPatternConfidence--;
			    	if(!perfbias) { entry->patternConfidence--; globalPatternConfidence--;} 
			    	for(int x=0;x<(perfbias? 2 : 5);x++) {entry->highPatternConfidence--;  globalHighPatternConfidence--;  	 }	 			   		
    			}
    		}
    		
        	// if(addr == sentry->next) DPRINTF(HWPrefetch, "Match for address %x confidence %d\n",addr, entry->patternConfidence+0);
            if (out_f.active) {
                if (warmup_complete) {
                    out_f.f << "Match for address " << std::hex << addr << " confidence " << std::dec << (entry->patternConfidence+0) << " cycle: " << current_cycle << std::endl;
                    out_f.f.flush();
                }
            }

            if(sentry->entry == entry) if(!useSampleConfidence || !sentry->confident) 
                sentry->next=addr;
            sentry->confident = willBeConfident;
    	}
    	else if(should_sample || randomChance(entry->reuseConfidence,entry->replaceRate)) {
    		//Fill sample table, as we're taking a sample at this PC. Should_sample also set by randomChance earlier, on first insert of PC intro training table.
    		sentry = historySampler.findVictim(entry->lastAddress);
    		assert(sentry != nullptr);
    		if(sentry->entry !=nullptr) {
    			    TrainingUnitEntry *pentry = sentry->entry;
    			    if(pentry != nullptr) {
    			    	    int64_t distance = pentry->local_timestamp - sentry->local_timestamp;
    			    	    // DPRINTF(HWPrefetch, "Replacing Entry %x with PC %x, old distance %d\n",sentry->entry, pc, distance);
                            if (out_f.active) {
                                if (warmup_complete) {
                                    out_f.f << "Replacing Entry " << std::hex << sentry->entry << " with PC " << std::hex << pc << ", old distance " << std::dec << distance << " cycle: " << current_cycle  << std::endl;
                                    out_f.f.flush();
                                }
                            }

    			    	    if(distance > max_size) {
    			    	        //TODO: Change max size to be relative, based on current tracking set?
    			            	trainingUnit.accessEntry(pentry);
    			            	if(!sentry->reused) { 
    			            	    	//Reuse conf decremented, as very old.
    			            		pentry->reuseConfidence--;
						globalReuseConfidence--;
    			            	}
    			            	entry->replaceRate++; //only replacing oldies -- can afford to be more aggressive.
    			            } else if(distance > 0 && !sentry->reused) { //distance goes -ve due to lack of training-entry space
    			            	entry->replaceRate--;
    			            }
    			    } else entry->replaceRate++;
    		}
    		assert(!sentry->isValid());
    		sentry->clear();
    		historySampler.insertEntry(entry->lastAddress, is_secure, sentry);
    		sentry->entry = entry;
    		sentry->reused = false;
    		sentry->local_timestamp = entry->local_timestamp+1;
    		sentry->next = addr;
    		sentry->confident=false;
    	}
    }


    // if (out_f.active) {
    //     // if (warmup_complete) {
    //         out_f.f << "[" << NAME << "] " << __func__ << " 4";
    //         out_f.f << " instr_id: " << instr_id << " addr: " << std::hex << addr;
    //         out_f.f << " ip: " << ip << std::dec << " cycle: " << current_cycle << std::endl;
    //     // }
    //     out_f.f.flush();
    // }

    if(!use_bloom) {
	    
	    for(int x=0;x<(64);x++) {
	    	//Here we update the size duellers, to work out for each cache set whether it is better to be markov table or L3 cache.

        
        int res = 0;
        if (x == 0) {
            if (out_f.active) {
                if (warmup_complete && (x == 0)) {
                    out_f.f << " sizeDuelPtr.checkAndInsert x: " << x << " addr: " << addr << " setMask: " << sizeDuelPtr[x].setMask << " set: " << sizeDuelPtr[x].set << " same: " << ((addr & sizeDuelPtr[x].setMask) == sizeDuelPtr[x].set);
                    out_f.f.flush();
                }
            }
            if (warmup_complete) res =    	sizeDuelPtr[x].checkAndInsert(addr,should_pf, out_f); //TODO: combine with hawk?
            else res =    	sizeDuelPtr[x].checkAndInsert(addr,should_pf); //TODO: combine with hawk?

            if (out_f.active) {
                if (warmup_complete && (x == 0)) {
                    out_f.f <<  " res " << res << " cycle: " << current_cycle  << std::endl;
                    out_f.f.flush();
                }
            }
        } else {
            res =    	sizeDuelPtr[x].checkAndInsert(addr,should_pf); //TODO: combine with hawk?
        }
        

		if(res==0)continue;
		const int ratioNumer=(perfbias?4:2);
		const int ratioDenom=4;//should_pf && entry->highPatternConfidence >=upperHistory? 4 : 8;
		int cache_hit = res%128; //This is just bit encoding of cache hits.
		int pref_hit = res/128; //This is just bit encoding of prefetch hits.
		int cache_set = cache_hit-1; //Encodes which nth most used replacement-state we hit at, if any.
		int pref_set = pref_hit-1; //Encodes which nth most used replacement-state we hit at, if any.
		assert(!cache_hit || (cache_set<setPrefetch.size()-1 && cache_set>=0));
		assert(!pref_hit || (pref_set<setPrefetch.size()-1 && pref_set>=0));
		if(cache_hit) {
            // if (meta_outf_ptr->active) {
            //     meta_outf_ptr->f << "cache_hit, add " << (setPrefetch.size()-2-cache_set) << " to 0, current_cycle: " << current_cycle << " cache_set: " << cache_set << endl;
            //     meta_outf_ptr->f.flush();
            // }
            for(int y= setPrefetch.size()-2-cache_set; y>=0; y--) {
                setPrefetch[y]++;
                cache_hit_cnt[y]++;
                cache_hit_value_cnt[y]++;
            }
        }
        // cache partition hit at this size or bigger. So hit in way 14 = y=17-2-14=1 and 0: would hit with 0 ways reserved or 1, not 2.
		if(pref_hit) {
            // if (meta_outf_ptr->active) {
            //     meta_outf_ptr->f << "pref_hit, add " << (pref_set+1) << " to " << setPrefetch.size() << ", value: " << ((ratioNumer*sizeDuelPtr[x].temporalModMax)/ratioDenom) << ", current_cycle: " << current_cycle << " pref_set: " << pref_set << endl;
            //     meta_outf_ptr->f.flush();
            // }
            for(int y=pref_set+1;y<setPrefetch.size();y++) {
                setPrefetch[y]+=(ratioNumer*sizeDuelPtr[x].temporalModMax)/ratioDenom; 
                pref_hit_cnt[y]++;
                pref_hit_value_cnt[y] += (ratioNumer*sizeDuelPtr[x].temporalModMax)/ratioDenom;
            }
        }
        // ^ pf hit at this size or bigger. one-indexed (since 0 is an alloc on 0 ways). So hit in way 0 = y=1--16 ways reserved, not 0.
		
		//if(cache_hit) printf("Cache hit\n");
		//else printf("Prefetch hit\n");
	    }
	    

	    if(global_timestamp > 500000) {
	    //Here we choose the size of the Markov table based on the optimum for the last epoch
	    int counterSizeSeen = 0;

	    for(int x=0;x<setPrefetch.size() && x*size_increment <= max_size;x++) {
            if (meta_outf_ptr->active) {
                meta_outf_ptr->f << "current_cycle: " << current_cycle << " x: " << x << ", setPrefetch[x]: " << setPrefetch[x] << ". counterSizeSeen: " << counterSizeSeen;
                meta_outf_ptr->f << " cache_hit: " << cache_hit_cnt[x] << " cache_hit_val: " << cache_hit_value_cnt[x];
                meta_outf_ptr->f << " pref_hit: " << pref_hit_cnt[x] << " pref_hit_val: " << pref_hit_value_cnt[x] << endl;
                meta_outf_ptr->f.flush();
            }
	    	if(setPrefetch[x]>counterSizeSeen) {
	    		 target_size= size_increment*x;
	    		 counterSizeSeen = setPrefetch[x];
	    	}
	    }

	    int currentscore = setPrefetch[current_size/size_increment];
	    // currentscore = currentscore + (currentscore>>4); //Slight bias against changing for minimal benefit.
	    int targetscore = setPrefetch[target_size/size_increment];

        if (meta_outf_ptr->active) {
            meta_outf_ptr->f << "Reset, current_cycle: " << current_cycle << " current_size: " << current_size << ", target_size: " << target_size << ". current_score: " << currentscore << ", target_score: " << targetscore << endl;
            meta_outf_ptr->f.flush();
        }

        // if (target_size != current_size) {
	    if(target_size != current_size && targetscore>currentscore) {
            cout << "current_size: " << current_size << ", target_size: " << target_size << ". current_score: " << currentscore << ", target_score: " << targetscore << endl;
	    	current_size = target_size;
            // printf("size: %d, tick %ld \n",current_size,curTick());
            if (out_f.active) {
                if (warmup_complete) {
                    out_f.f <<  "size " << current_size << " tick " << curTick() << " cycle: " << current_cycle  << std::endl;
                    out_f.f.flush();
                }
            }

            //for(int x=0;x<setPrefetch.size(); x++) {
            //	printf("%d: %d\n", x, setPrefetch[x]);
            //}
            assert(current_size >= 0);
            
        
            for(int x=0;x<64;x++) {
                hawksets[x].setMask = current_size / hawksets[x].maxElems;
                hawksets[x].reset();
            }
            std::vector<MarkovMapping> ams;
            if(should_rearrange) {		    	
                for(MarkovMapping am: *markovTablePtr) {
                    if(am.isValid()) ams.push_back(am);
                }
                for(MarkovMapping& am: *markovTablePtr) {
                    am.invalidate(); //for RRIP's sake
                }
            }
            TriangelHashedSetAssociative* thsa = dynamic_cast<TriangelHashedSetAssociative*>(markovTablePtr->indexingPolicy);
                if(thsa) { thsa->ways = current_size/size_increment; thsa->max_ways = maxWays; assert(thsa->ways <= thsa->max_ways);}
                else assert(0);
            //rearrange conditionally
            if(should_rearrange) {        	
                if(current_size >0) {
                    for(MarkovMapping am: ams) {
                            MarkovMapping *mapping = getHistoryEntry(am.index, am.isSecure(),true,false,true,true);
                            mapping->address = am.address;
                            mapping->index=am.index;
                            mapping->confident = am.confident;
                            mapping->lookupIndex=am.lookupIndex;
                            markovTablePtr->weightedAccessEntry(mapping,1,false); //For RRIP, touch
                    }    	
                }
            }
                        
            for(MarkovMapping& am: *markovTablePtr) {
                if(thsa->ways==0 || (thsa->extractSet(am.index) % maxWays)>=thsa->ways)  am.invalidate();
            }
            // TODO: DIFF
            // cachetags->setWayAllocationMax(setPrefetch.size()-1-thsa->ways);
            cout << "change way from " << CACHE::LLC_POINTER->NUM_WAY;
            CACHE::LLC_POINTER->NUM_WAY = setPrefetch.size()-1-thsa->ways;
            cout << " to " << CACHE::LLC_POINTER->NUM_WAY << endl;
	    }
        // printf("End of epoch:\n");
        if (out_f.active) {
            if (warmup_complete) {
                out_f.f <<  "End of epoch:" << " cycle: " << current_cycle  << std::endl;
                out_f.f.flush();
            }
        }

		for(int x=0;x<setPrefetch.size(); x++) {

			// printf("%d: %d\n", x, setPrefetch[x]);
            if (out_f.active) {
                if (warmup_complete) {
                    out_f.f <<  "setPrefetch: " << x << " " << setPrefetch[x] << " cycle: " << current_cycle  << std::endl;
                    out_f.f.flush();
                }
            }
		}
	    	global_timestamp=0;
		for(int x = 0; x < setPrefetch.size(); x++) {
            setPrefetch[x]=0;
            cache_hit_cnt[x] = pref_hit_cnt[x] = pref_hit_value_cnt[x] = cache_hit_value_cnt[x] = 0;
		}
	    	//Reset after 2 million prefetch accesses -- not quite the same as after 30 million insts but close enough
        }
	     
    }


    if(useHawkeye && correlated_addr_found && should_pf) {
        // If a correlation was found, update the Markov table accordingly
        	for(int x=0; x<64; x++)hawksets[x].add(addr,pc,&trainingUnit);
        	should_hawk = entry->hawkConfidence>7;
    }


    if (correlated_addr_found && should_pf && (current_size>0)) {
        // If a correlation was found, update the Markov table accordingly
	//DPRINTF(HWPrefetch, "Tabling correlation %x to %x, PC %x\n", index << lBlkSize, target << lBlkSize, pc);
    if (out_f.active) {
        if (warmup_complete) {
            out_f.f << "Tabling correlation " << std::hex << (index << lBlkSize) << " to " << (target << lBlkSize) << ", PC " << pc  << " cycle: " << current_cycle << std::endl;
            out_f.f.flush();
        }
    }
	MarkovMapping *mapping = getHistoryEntry(index, is_secure,false,false,false, should_hawk);
	if(mapping == nullptr) {
        	mapping = getHistoryEntry(index, is_secure,true,false,false, should_hawk);
        	mapping->address = target;
        	mapping->index=index; //for HawkEye
        	mapping->confident = false;
        }
        assert(mapping != nullptr);
        bool confident = mapping->address == target; 
        bool wasConfident = mapping->confident;
        mapping->confident = confident; //Confidence is just used for replacement. I haven't tested how important it is for performance to use it; this is inherited from Triage.
        if(!wasConfident) {
        	mapping->address = target;
        }
        if(wasConfident && confident && use_mrb) {
        	MarkovMapping *cached_entry =
        		metadataReuseBuffer.findEntry(index, is_secure);
        	if(cached_entry != nullptr) {
                // TODO: DIFF
                metadataAccesses--;
        		// prefetchStats.metadataAccesses--;
        		//No need to access L3 again, as no updates to be done.
        	}
        }
        
        int index=0;
        uint64_t time = -1;
        if(lookupAssoc>0){
		int lookupMask = (1024/lookupAssoc)-1;
		int set = (target>>lookupOffset)&lookupMask;
		for(int x=lookupAssoc*set;x<lookupAssoc*(set+1);x++) {
			if(target>>lookupOffset == lookupTable[x]) {
				index=x;
				break;
			}
			if(time > lookupTick[x]) {
				time = lookupTick[x];
				index=x;
			}
		}
		
		lookupTable[index]=target>>lookupOffset;
		lookupTick[index]=curTick();
		mapping->lookupIndex=index;
        }
        
    }

    // if (out_f.active) {
    //     // if (warmup_complete) {
    //         out_f.f << "[" << NAME << "] " << __func__ << " 7";
    //         out_f.f << " instr_id: " << instr_id << " addr: " << std::hex << addr;
    //         out_f.f << " ip: " << ip << std::dec << " cycle: " << current_cycle << std::endl;
    //     // }
    //     out_f.f.flush();
    // }

    if(target != 0 && should_pf && (current_size>0)) {
  	 MarkovMapping *pf_target = getHistoryEntry(target, is_secure,false,true,false, should_hawk);
   	 unsigned deg = 0;
  	//  unsigned delay = cacheDelay;
  	 bool high_degree_pf = pf_target != nullptr
  	         && (entry->highPatternConfidence>highUpperHistory || !use_pattern2)/*&& pf_target->confident*/;
   	 unsigned max = high_degree_pf? degree : (should_pf? 1 : 0);
   	 //if(pf_target == nullptr && should_pf) DPRINTF(HWPrefetch, "Target not found for %x, PC %x\n", target << lBlkSize, pc);
   	 while (pf_target != nullptr && deg < max  /*&& (pf_target->confident || entry->highPatternConfidence>upperHistory)*/
   	 ) { //TODO: do we always pf at distance 1 if not confident?
    		// DPRINTF(HWPrefetch, "Prefetching %x on miss at %x, PC \n", pf_target->address << lBlkSize, addr << lBlkSize, pc);
            if (out_f.active) {
                if (warmup_complete) {
                    out_f.f <<  "Prefetching " << std::hex << (pf_target->address << lBlkSize) << " on miss at " << (addr << lBlkSize) << ", PC " << pc  << " cycle: " << current_cycle << std::endl;
                    out_f.f.flush();
                }
            }
    		// int extraDelay = cacheDelay;
    		// if(lastAccessFromPFCache && use_mrb) {
    			// Cycles time = curCycle() - pf_target->cycle_issued;
    			// if(time >= cacheDelay) extraDelay = 0;
    			// else if (time < cacheDelay) extraDelay = time;
    		// }
    		
    		Addr lookup = pf_target->address;
   	        if(lookupAssoc>0){
	   	 	int index=pf_target->lookupIndex;
	   	 	int lookupMask = (1<<lookupOffset)-1;
	   	 	lookup = (lookupTable[index]<<lookupOffset) + ((pf_target->address)&lookupMask);
	   	 	lookupTick[index]=curTick();
	   	 	// if(lookup == pf_target->address)prefetchStats.lookupCorrect++;
	    	// 	else prefetchStats.lookupWrong++;
	   	 	if(lookup == pf_target->address)lookupCorrect++;
	    		else lookupWrong++;
    		}
    		
    		// if(extraDelay == cacheDelay) addresses.push_back(AddrPriority(lookup << lBlkSize, delay));
    		// delay += extraDelay;
            addresses.push_back(lookup << lBlkSize);
    		deg++;
    		
    		if(deg<max /*&& pf_target->confident*/) pf_target = getHistoryEntry(lookup, is_secure,false,true,false, should_hawk);
    		else pf_target = nullptr;

   	 }
    }

    // if (out_f.active) {
    //     // if (warmup_complete) {
    //         out_f.f << "[" << NAME << "] " << __func__ << " 8";
    //         out_f.f << " instr_id: " << instr_id << " addr: " << std::hex << addr;
    //         out_f.f << " ip: " << ip << std::dec << " cycle: " << current_cycle << std::endl;
    //     // }
    //     out_f.f.flush();
    // }

        // Update the entry
    if(entry != nullptr) {
    	entry->lastLastAddress = entry->lastAddress;
    	entry->lastLastAddressSecure = entry->lastAddressSecure;
    	entry->lastAddress = addr;
    	entry->lastAddressSecure = is_secure;
    	entry->local_timestamp ++;
    }
}


Triangel::MarkovMapping*
Triangel::getHistoryEntry(Addr paddr, bool is_secure, bool add, bool readonly, bool clearing, bool hawk)
{
	//The weird parameters above control whether we replace entries, and how the number of metadata accesses are updated, for instance. They're basically a simulation thing.
  	    TriangelHashedSetAssociative* thsa = dynamic_cast<TriangelHashedSetAssociative*>(markovTablePtr->indexingPolicy);
	  				if(!thsa)  assert(0);  

        // TODO: DIFFERENT
    	// cachetags->clearSetWay(thsa->extractSet(paddr)/maxWays, thsa->extractSet(paddr)%maxWays);
        // uint32_t set = CACHE::L2_POINTER->get_set((uint64_t)(sentry->next<<lBlkSize));
        // uint32_t way = CACHE::L2_POINTER->get_way((uint64_t)(sentry->next<<lBlkSize), set);
        uint32_t set = thsa->extractSet(paddr)/maxWays;
        uint32_t way = thsa->extractSet(paddr)%maxWays;
        way = markovTablePtr->indexingPolicy->assoc-1-way;

        auto s_block = CACHE::L2_POINTER->block[set * CACHE::L2_POINTER->NUM_WAY + way];
        if (!s_block.dirty && s_block.valid) s_block.valid = false;
        // else if (s_block.dirty && s_block_valid) 

        // CacheBlk* blk = static_cast<CacheBlk*>(findBlockBySetAndWay(set,indexingPolicy->assoc-1-way));
        // if(!blk->isSet(CacheBlk::DirtyBit) && blk->isValid()) blk->invalidate();//evictBlock(blk,writebacks);
        // else if(blk->isSet(CacheBlk::DirtyBit) && blk->isValid()) badBlocks.push_back(blk);

    if(should_rearrange) {    

	    int index= paddr % (way_idx.size()); //Not quite the same indexing strategy, but close enough.
	    
	    if(way_idx[index] != thsa->ways) {
            // TODO: DIFF
	    	// if(way_idx[index] !=0) prefetchStats.metadataAccesses+= thsa->ways + way_idx[index];
	    	if(way_idx[index] !=0) metadataAccesses+= thsa->ways + way_idx[index];

	    	way_idx[index]=thsa->ways;
	    }
    }

    if(readonly) { //check the cache first.
        MarkovMapping *pf_entry =
        	use_mrb ? metadataReuseBuffer.findEntry(paddr, is_secure) : nullptr;
        if (pf_entry != nullptr) {
        	lastAccessFromPFCache = true;
        	return pf_entry;
        }
        lastAccessFromPFCache = false;
    }

    MarkovMapping *ps_entry =
        markovTablePtr->findEntry(paddr, is_secure);
    // TODO: DIFF
    // if(readonly || !add) prefetchStats.metadataAccesses++;
    if(readonly || !add) metadataAccesses++;

    if (ps_entry != nullptr) {
        // A PS-AMC line already exists
        markovTablePtr->weightedAccessEntry(ps_entry,hawk?1:0,false);
    } else {
        if(!add) return nullptr;
        ps_entry = markovTablePtr->findVictim(paddr);
        assert(ps_entry != nullptr);
        if(useHawkeye && !clearing) for(int x=0;x<64;x++) hawksets[x].decrementOnLRU(ps_entry->index,&trainingUnit);
	assert(!ps_entry->isValid());
        markovTablePtr->insertEntry(paddr, is_secure, ps_entry);
        markovTablePtr->weightedAccessEntry(ps_entry,hawk?1:0,true);
    }

    if(readonly && use_mrb) {
    	    MarkovMapping *pf_entry = metadataReuseBuffer.findVictim(paddr);
    	    metadataReuseBuffer.insertEntry(paddr, is_secure, pf_entry);
    	    pf_entry->address = ps_entry->address;
    	    pf_entry->confident = ps_entry->confident;
            // TODO: DIFFERENT
    	    // pf_entry->cycle_issued = curCycle();
            pf_entry->cycle_issued = curTick();

    	    //This adds access time, to set delay appropriately.
    }

    return ps_entry;
}

uint32_t
TriangelHashedSetAssociative::extractSet(const Addr addr) const
{
	//Input is already blockIndex so no need to remove block again.
    Addr offset = addr;
    
   /* const Addr hash1 = offset & ((1<<16)-1);
    const Addr hash2 = (offset >> 16) & ((1<<16)-1);
        const Addr hash3 = (offset >> 32) & ((1<<16)-1);
    */
        offset = ((offset) * max_ways) + (extractTag(addr) % ways);
        return offset & setMask;   //setMask is numSets-1

}


Addr
TriangelHashedSetAssociative::extractTag(const Addr addr) const
{
    //Input is already blockIndex so no need to remove block again.

    //Description in Triage-ISR confuses whether the index is just the 16 least significant bits,
    //or the weird index above. The tag can't be the remaining bits if we use the literal representation!


    Addr offset = addr / (numSets/max_ways); 
    int result = 0;
    
    //This is a tag# as described in the Triangel paper.
    const int shiftwidth=10;

    for(int x=0; x<64; x+=shiftwidth) {
       result ^= (offset & ((1<<shiftwidth)-1));
       offset = offset >> shiftwidth;
    }
    return result;
}

Triangel* triangel;

void CACHE::prefetcher_initialize() 
{
    // cout << "CPU " << cpu << " L2 from_file prefetcher" << endl;
    cout << "CPU " << cpu << " " << NAME << " Triangel prefetcher. FILL_THIS_LEVEL: " << FILL_THIS_LEVEL << endl;
    init_polices();
    triangel = new Triangel();

    meta_outf_ptr = &prepare_file("meta_adjust");

    cout << "CPU " << cpu << " " << NAME << " Triangel prefetcher: Initialize done." << endl;
}

uint32_t CACHE::prefetcher_cache_operate(uint64_t addr, uint64_t ip, uint8_t cache_hit, uint8_t type, uint32_t metadata_in, uint64_t instr_id)
{
    set_curr_tick(current_cycle);

    auto &out_f = prepare_file("debug");
    // if (out_f.active) {
    //     if (warmup_complete[cpu]) {
    //         out_f.f << "[" << NAME << "] " << __func__ ;
    //         out_f.f << " instr_id: " << instr_id << " addr: " << std::hex << addr;
    //         out_f.f << " ip: " << ip << std::dec << " cycle: " << current_cycle << std::endl;
    //     }
    //     out_f.f.flush();
    // }

    vector<Addr> addrs;
    triangel->calculatePrefetch(addr, ip , cache_hit, type, metadata_in, instr_id, current_cycle, addrs, out_f, warmup_complete[cpu]);


    if (out_f.active) {
        if (warmup_complete[cpu]) {
            if (addrs.size() > 0) {
                out_f.f << "[" << NAME << "] " << __func__ ;
                out_f.f << " addrs len: " << addrs.size() << std::endl;
                out_f.f.flush();
            }
        }
    }

    auto &pf_f = prepare_file("pf_trace");


    for (const auto& paddr : addrs) {
        int ret = prefetch_line(paddr, FILL_THIS_LEVEL, 0);

        if (pf_f.active) {
            if (warmup_complete[cpu]) {
                pf_f.f << "[" << NAME << "] " << __func__ ;
                pf_f.f << " instr_id: " << instr_id << " addr: " << std::hex << addr << " pf_addr: " << paddr;
                pf_f.f << " ip: " << ip << std::dec << " cycle: " << current_cycle << std::endl;
            }
        }
        if (out_f.active) {
            if (warmup_complete[cpu]) {
                out_f.f << "[" << NAME << "] " << __func__ ;
                out_f.f << " instr_id: " << instr_id << " addr: " << std::hex << addr << " pf_addr: " << paddr;
                out_f.f << " ip: " << ip << std::dec << " cycle: " << current_cycle << std::endl;
            }
        }
        // cout << "prefetch " << addr << " ret: " << ret << endl;
    }

    return metadata_in;
}

uint32_t CACHE::prefetcher_cache_fill(uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr, uint32_t metadata_in)
{
    return metadata_in;
}

void CACHE::prefetcher_cycle_operate() {}


void CACHE::prefetcher_final_stats()
{
    for (int i = 0; i < 100; ++i) {
        cout << "x: " << i << " cache_hit: " << cache_hit_cnt[i] << " cache_hit_val: " << cache_hit_value_cnt[i];
        cout << " pref_hit: " << pref_hit_cnt[i] << " pref_hit_val: " << pref_hit_value_cnt[i] << endl;
    }
}