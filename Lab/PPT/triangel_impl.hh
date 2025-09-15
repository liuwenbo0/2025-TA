#include "base_def.hh"
#include "cache.h"
#include "ooo_cpu.h"
#include "associative_set_impl.hh"
#include "set_associative.hh"

std::mt19937_64 gen;

extern uint64_t metadataAccesses, lookupWrong, lookupCorrect;

template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T>
get_rand(T min, T max)
{
    std::uniform_int_distribution<T> dist(min, max);
    return dist(gen);
}


Addr
blockIndex(Addr a) {
    // return a >> OFFSET_BITS;
    return a >> lBlkSize;
}

template <class T>
class GenericSatCounter
{
  public:
    /** The default constructor should never be used. */
    // GenericSatCounter() = delete;
    GenericSatCounter() {}
    /**
     * Constructor for the counter. The explicit keyword is used to make
     * sure the user does not assign a number to the counter thinking it
     * will be used as a counter value when it is in fact used as the number
     * of bits.
     *
     * @param bits How many bits the counter will have.
     * @param initial_val Starting value for the counter.
     *
     * @ingroup api_sat_counter
     */
    explicit GenericSatCounter(unsigned bits, T initial_val = 0)
        : initialVal(initial_val), maxVal((1ULL << bits) - 1),
          counter(initial_val)
    {

    }

    /**
     * Copy constructor.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter(const GenericSatCounter& other)
        : initialVal(other.initialVal), maxVal(other.maxVal),
          counter(other.counter)
    {
    }

    /**
     * Copy assignment.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter& operator=(const GenericSatCounter& other) {
        if (this != &other) {
            GenericSatCounter temp(other);
            this->swap(temp);
        }
        return *this;
    }

    /**
     * Move constructor.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter(GenericSatCounter&& other)
    {
        initialVal = other.initialVal;
        maxVal = other.maxVal;
        counter = other.counter;
        GenericSatCounter temp(0);
        other.swap(temp);
    }

    /**
     * Move assignment.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter& operator=(GenericSatCounter&& other) {
        if (this != &other) {
            initialVal = other.initialVal;
            maxVal = other.maxVal;
            counter = other.counter;
            GenericSatCounter temp(0);
            other.swap(temp);
        }
        return *this;
    }

    /**
     * Swap the contents of every member of the class. Used for the default
     * copy-assignment created by the compiler.
     *
     * @param other The other object to swap contents with.
     *
     * @ingroup api_sat_counter
     */
    void
    swap(GenericSatCounter& other)
    {
        std::swap(initialVal, other.initialVal);
        std::swap(maxVal, other.maxVal);
        std::swap(counter, other.counter);
    }

    /**
     * Pre-increment operator.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter&
    operator++()
    {
        if (counter < maxVal) {
            ++counter;
        }
        return *this;
    }

    /**
     * Post-increment operator.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter
    operator++(int)
    {
        GenericSatCounter old_counter = *this;
        ++*this;
        return old_counter;
    }

    /**
     * Pre-decrement operator.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter&
    operator--()
    {
        if (counter > 0) {
            --counter;
        }
        return *this;
    }

    /**
     * Post-decrement operator.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter
    operator--(int)
    {
        GenericSatCounter old_counter = *this;
        --*this;
        return old_counter;
    }

    /**
     * Shift-right-assignment.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter&
    operator>>=(const int& shift)
    {
        assert(shift >= 0);
        this->counter >>= shift;
        return *this;
    }

    /**
     * Shift-left-assignment.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter&
    operator<<=(const int& shift)
    {
        assert(shift >= 0);
        this->counter <<= shift;
        if (this->counter > maxVal) {
            this->counter = maxVal;
        }
        return *this;
    }

    /**
     * Add-assignment.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter&
    operator+=(const long long& value)
    {
        if (value >= 0) {
            if (maxVal - this->counter >= value) {
                this->counter += value;
            } else {
                this->counter = maxVal;
            }
        } else {
            *this -= -value;
        }
        return *this;
    }

    /**
     * Subtract-assignment.
     *
     * @ingroup api_sat_counter
     */
    GenericSatCounter&
    operator-=(const long long& value)
    {
        if (value >= 0) {
            if (this->counter > value) {
                this->counter -= value;
            } else {
                this->counter = 0;
            }
        } else {
            *this += -value;
        }
        return *this;
    }

    /**
     * Read the counter's value.
     *
     * @ingroup api_sat_counter
     */
    operator T() const { return counter; }

    /**
     * Reset the counter to its initial value.
     *
     * @ingroup api_sat_counter
     */
    void reset() { counter = initialVal; }

    /**
     * Calculate saturation percentile of the current counter's value
     * with regard to its maximum possible value.
     *
     * @return A value between 0.0 and 1.0 to indicate which percentile of
     *         the maximum value the current value is.
     *
     * @ingroup api_sat_counter
     */
    double calcSaturation() const { return (double) counter / maxVal; }

    /**
     * Whether the counter has achieved its maximum value or not.
     *
     * @return True if the counter saturated.
     *
     * @ingroup api_sat_counter
     */
    bool isSaturated() const { return counter == maxVal; }

    /**
     * Saturate the counter.
     *
     * @return The value added to the counter to reach saturation.
     *
     * @ingroup api_sat_counter
     */
    T saturate()
    {
        const T diff = maxVal - counter;
        counter = maxVal;
        return diff;
    }

  private:
    T initialVal;
    T maxVal;
    T counter;
};

/** @ingroup api_sat_counter
 *  @{
 */
typedef GenericSatCounter<uint8_t> SatCounter8;

class TriangelHashedSetAssociative : public SetAssociative
{
  public:
    uint32_t extractSet(const Addr addr) const override;
    Addr extractTag(const Addr addr) const override;

  public:
    int ways;
    int max_ways;
    TriangelHashedSetAssociative(
        const SetAssociativeParams &p)
      : SetAssociative(p), ways(0),max_ways(8)
    {
    }
    ~TriangelHashedSetAssociative() = default;
};

struct TriangelPrefetcherParams;

class Triangel {
public:

        /** Number of maximum prefetches requests created when predicting */
     unsigned degree;

    /**
     * Training Unit Entry datatype, it holds the last accessed address and
     * its secure flag
     */

    Triangel();

    unsigned cacheDelay;
    bool should_lookahead;
    bool should_rearrange;
    bool use_scs;
    bool use_bloom;
    bool use_reuse;
    bool use_pattern;
    bool use_pattern2;
    bool use_mrb;
    bool perfbias;
    bool smallduel;
    bool timed_scs;
    bool useSampleConfidence;
    
    // BaseTags* sctags;

    bool randomChance(int r, int s);
    int max_size;
    int size_increment;
    static int64_t global_timestamp;
    uint64_t second_chance_timestamp;
    uint64_t lowest_blocked_entry;
    static int current_size;
    static int target_size;
    int maxWays;
    
    std::vector<int> way_idx;
    
    SatCounter8  globalReuseConfidence;
    SatCounter8  globalPatternConfidence;
    SatCounter8 globalHighPatternConfidence;    


    void calculatePrefetch(uint64_t addr, uint64_t ip, uint8_t cache_hit, uint8_t type, uint32_t metadata_in, uint64_t instr_id, uint64_t current_cycle,
        std::vector<Addr> &addresses, OutFile& out_f, bool warmup_complete);

    
    struct TrainingUnitEntry : public TaggedEntry
    {
        Addr lastAddress;
        Addr lastLastAddress;
        int64_t local_timestamp;
        SatCounter8  reuseConfidence;
        SatCounter8  patternConfidence;
        SatCounter8 highPatternConfidence;
        SatCounter8 replaceRate;
        SatCounter8 hawkConfidence;
        bool lastAddressSecure;
        bool lastLastAddressSecure;
        bool currently_twodist_pf;



        TrainingUnitEntry() : lastAddress(0), lastLastAddress(0), local_timestamp(0),reuseConfidence(4,8), patternConfidence(4,8), highPatternConfidence(4,8), replaceRate(4,8), hawkConfidence(4,8), lastAddressSecure(false), lastLastAddressSecure(false),currently_twodist_pf(false)
        {}

        void
        invalidate()
        {
        	TaggedEntry::invalidate();
                lastAddress = 0;
                lastLastAddress = 0;
                //local_timestamp=0; //Don't reset this, to handle replacement and still give contiguity of timestamp
                reuseConfidence.reset();
                patternConfidence.reset();
                highPatternConfidence.reset();
                replaceRate.reset();
                currently_twodist_pf = false;
                
        }
    };
    /** Map of PCs to Training unit entries */
    AssociativeSet<TrainingUnitEntry> trainingUnit;

    Addr lookupTable[1024];
    uint64_t lookupTick[1024];
    int lookupAssoc;
    int lookupOffset;

  static std::vector<uint32_t> setPrefetch; 

    struct SizeDuel
    {
        int idx;
        uint64_t set;
        uint64_t setMask;
        uint64_t temporalMod; //[0..12] Entries Per Line
        
        uint64_t temporalModMax; //12 by default
        uint64_t cacheMaxAssoc;
        
        
        std::vector<Addr> cacheAddrs; // [0..16] should be set by the nsets of the L3 cache.
        std::vector<uint64_t> cacheAddrTick;
        std::vector<Addr> temporalAddrs;
        std::vector<uint64_t> temporalAddrTick;
        std::vector<bool> inserted;

        SizeDuel()
            {
            }
            void reset(uint64_t mask, uint64_t modMax, uint64_t cacheAssoc) {
                setMask=mask;
                temporalModMax = modMax;
                cacheMaxAssoc=cacheAssoc;
                cacheAddrTick.resize(cacheMaxAssoc);
                temporalAddrs.resize(cacheMaxAssoc);
                cacheAddrs.resize(cacheMaxAssoc);
                temporalAddrTick.resize(cacheMaxAssoc);
                inserted.resize(cacheMaxAssoc,false);
                for(int x=0;x<cacheMaxAssoc;x++) {
                cacheAddrTick[x]=0;
                temporalAddrs[x]=0;
                cacheAddrs[x]=0;
                temporalAddrTick[x]=0;
            }
            set = get_rand<uint64_t>(0,setMask);
            temporalMod = get_rand<uint64_t>(0,modMax-1); // N-1, as range is inclusive.	
            }

        int checkAndInsert(Addr addr, bool should_pf, OutFile& out_f) {
            int ret = 0;
            bool foundInCache=false;
            bool foundInTemp=false;
            if((addr & setMask) != set) return ret;
            for(int x=0;x<cacheMaxAssoc;x++) {
                if(addr == cacheAddrs[x]) {
                    foundInCache=true; 
                        int index=cacheMaxAssoc-1;
                        for(int y=0;y<cacheMaxAssoc;y++) {
                            if(cacheAddrTick[x]>cacheAddrTick[y]) index--;
                            assert(index>=0);
                        }  
                        cacheAddrTick[x] = curTick();
                        ret += index+1;	
                    if (out_f.active) {
                        out_f.f << " foundInCache x: " << x << " cacheAddrs[x]: " << cacheAddrs[x] << " cacheAddrTick[x]: " << cacheAddrTick[x] << " index: " << index  << " ret: " << ret << std::endl;
                        out_f.f.flush();
                    }
                }
                if(should_pf && addr == temporalAddrs[x]) {
                    
                    
                    foundInTemp=true;
                    
                        int index=cacheMaxAssoc-1;
                        for(int y=0;y<cacheMaxAssoc;y++) {
                            if(temporalAddrTick[x]>temporalAddrTick[y]) index--;
                            assert(index>=0);
                        }  

                        ret += 128*(index+1);

                    temporalAddrTick[x] = curTick();
                    inserted[x]=true;

                    if (out_f.active) {
                        out_f.f << " foundInTemp x: " << x << " temporalAddrs[x]: " << temporalAddrs[x] << " index: " << index << " temporalAddrTick[x]: " << temporalAddrTick[x] << " ret: " << ret << std::endl;
                        out_f.f.flush();
                    }
                }
            }
            if(!foundInCache) {
                uint64_t oldestTick = (uint64_t)-1;
                int idx = -1;
                for(int x=0; x<cacheMaxAssoc;x++) {
                    if(cacheAddrTick[x]<oldestTick) {idx = x; oldestTick = cacheAddrTick[x];}
                }
                assert(idx>=0);
                cacheAddrs[idx]=addr;
                cacheAddrTick[idx]=curTick();
                if (out_f.active) {
                    out_f.f << " !foundInCache idx: " << idx << " cacheAddrs[idx]: " << cacheAddrs[idx] << " cacheAddrTick[idx]: " << cacheAddrTick[idx] << std::endl;
                    out_f.f.flush();
                }
            }
            if(!foundInTemp && should_pf && (((addr / (setMask+1)) % temporalModMax) == temporalMod)) {
                uint64_t oldestTick = (uint64_t)-1;
                int idx = -1;
                for(int x=0; x<cacheMaxAssoc;x++) {
                    if(temporalAddrTick[x]<oldestTick) {idx = x; oldestTick = temporalAddrTick[x]; }
                }  			
    assert(idx>=0);
                temporalAddrs[idx]=addr;
                temporalAddrTick[idx]=curTick();

                if (out_f.active) {
                    out_f.f << " !foundInTemp idx: " << idx << " temporalAddrs[idx]: " << temporalAddrs[idx] << " temporalAddrTick[idx]: " << temporalAddrTick[idx] << std::endl;
                    out_f.f.flush();
                }
            }  
            return ret;		
        }

        int checkAndInsert(Addr addr, bool should_pf) {
            int ret = 0;
            bool foundInCache=false;
            bool foundInTemp=false;
            if((addr & setMask) != set) return ret;
            for(int x=0;x<cacheMaxAssoc;x++) {
                if(addr == cacheAddrs[x]) {
                    foundInCache=true; 
                        int index=cacheMaxAssoc-1;
                        for(int y=0;y<cacheMaxAssoc;y++) {
                            if(cacheAddrTick[x]>cacheAddrTick[y]) index--;
                            assert(index>=0);
                        }  
                        cacheAddrTick[x] = curTick();
                        ret += index+1;	
                }
                if(should_pf && addr == temporalAddrs[x]) {
                    foundInTemp=true;
                    
                        int index=cacheMaxAssoc-1;
                        for(int y=0;y<cacheMaxAssoc;y++) {
                            if(temporalAddrTick[x]>temporalAddrTick[y]) index--;
                            assert(index>=0);
                        }  

                        ret += 128*(index+1);
                    
                    temporalAddrTick[x] = curTick();
                    inserted[x]=true;
                }
            }
            if(!foundInCache) {
                uint64_t oldestTick = (uint64_t)-1;
                int idx = -1;
                for(int x=0; x<cacheMaxAssoc;x++) {
                    if(cacheAddrTick[x]<oldestTick) {idx = x; oldestTick = cacheAddrTick[x];}
                }
                assert(idx>=0);
                cacheAddrs[idx]=addr;
                cacheAddrTick[idx]=curTick();
            }
            if(!foundInTemp && should_pf && (((addr / (setMask+1)) % temporalModMax) == temporalMod)) {
                uint64_t oldestTick = (uint64_t)-1;
                int idx = -1;
                for(int x=0; x<cacheMaxAssoc;x++) {
                    if(temporalAddrTick[x]<oldestTick) {idx = x; oldestTick = temporalAddrTick[x]; }
                }  			
    assert(idx>=0);
                temporalAddrs[idx]=addr;
                temporalAddrTick[idx]=curTick();
            }  
            return ret;		
        }
    
    };
    SizeDuel sizeDuels[256];
    static SizeDuel* sizeDuelPtr;

  struct Hawkeye
    {
      int iteration;
      uint64_t set;
      uint64_t setMask; // address_map_rounded_entries/ maxElems - 1
      Addr logaddrs[64];
      Addr logpcs[64];
      int logsize[64];
      int maxElems = 8;
      
      Hawkeye(uint64_t mask, bool history) : iteration(0), set(0), setMask(mask)
        {
           reset();
        }
        
      Hawkeye() : iteration(0), set(0)
        {       }
      
      void reset() {
        iteration=0;
        for(int x=0;x<64;x++) {
        	logsize[x]=0;
        	logaddrs[x]=0;
        	logpcs[x]=0;
        }
        set = get_rand<uint64_t>(0,setMask-1);
      }
      
      void decrementOnLRU(Addr addr,AssociativeSet<TrainingUnitEntry>* trainer) {
      	 if((addr % setMask) != set) return;
         for(int y=iteration;y!=((iteration+1)&63);y=(y-1)&63) {
               if(addr==logaddrs[y]) {
               	    Addr pc = logpcs[y];
               	    TrainingUnitEntry *entry = trainer->findEntry(pc, false); //TODO: is secure
               	    if(entry!=nullptr) {
               	    	if(entry->hawkConfidence>=8) {
               	    		entry->hawkConfidence--;
               	    		//printf("%s evicted, pc %s, temporality %d\n",addr, pc,entry->temporal);
               	    	}
               	    	
               	    }
               	    return;
               }
         }            
      }
      
      void add(Addr addr,  Addr pc,AssociativeSet<TrainingUnitEntry>* trainer) {
        if((addr % setMask) != set) return;
        logaddrs[iteration]=addr;
        logpcs[iteration]=pc;
        logsize[iteration]=0;

        
        TrainingUnitEntry *entry = trainer->findEntry(pc, false); //TODO: is secure
        if(entry!=nullptr) {
          for(int y=(iteration-1)&63;y!=iteration;y=(y-1)&63) {
               
               if(logsize[y] == maxElems) {
                 //no match
                 //printf("%s above max elems, pc %s, temporality %d\n",addr, pc,entry->temporal-1);
                 entry->hawkConfidence--;
                 break;
               }
               if(addr==logaddrs[y]) {
                 //found a match
                 //printf("%s fits, pc %s, temporality %d\n",addr, pc,entry->temporal+1);
                   entry->hawkConfidence++;
                   for(int z=y;z!=iteration;z=(z+1)&63){
                   	logsize[z]++;
                   }
                break;
               }
            }            
        }
        iteration++;
        iteration = iteration % 64;
      }
      
    };
    
    Hawkeye hawksets[64];
    bool useHawkeye;

    /** Address Mapping entry, holds an address and a confidence counter */
    struct MarkovMapping : public TaggedEntry
    {
      	Addr index; //Just for maintaining HawkEye easily. Not real.
        Addr address;
        int lookupIndex; //Only one of lookupIndex/Address are real.
        bool confident;
        Tick cycle_issued; // only for prefetched cache and only in simulation
        MarkovMapping() : index(0), address(0), confident(false), cycle_issued(0)
        {}


        void
        invalidate() override
        {
                TaggedEntry::invalidate();
                address = 0;
                index = 0;
                confident = false;
                cycle_issued=0;
        }
    };

        /** Sample unit entry, tagged by data address, stores PC, timestamp, next element **/
    struct SampleEntry : public TaggedEntry
    {
    	TrainingUnitEntry* entry;
    	bool reused;
    	uint64_t local_timestamp;
    	Addr next;
    	bool confident;

    	SampleEntry() : entry(nullptr), reused(false), local_timestamp(0), next(0)
        {}

        void
        invalidate() override
        {
            TaggedEntry::invalidate();
        }

        void clear() {
                entry=nullptr;
                reused = false;
                local_timestamp=0;
                next = 0;
                confident=false;
        }
    };
    AssociativeSet<SampleEntry> historySampler;

    /** Test pf entry, tagged by data address**/
    struct SecondChanceEntry: public TaggedEntry
    {
    	Addr pc;
    	uint64_t global_timestamp;
    	bool used;
    };
    AssociativeSet<SecondChanceEntry> secondChanceUnit;

    /** History mappings table */
    AssociativeSet<MarkovMapping> markovTable;
    static AssociativeSet<MarkovMapping>* markovTablePtr;

    AssociativeSet<MarkovMapping> metadataReuseBuffer;
    bool lastAccessFromPFCache;

    MarkovMapping* getHistoryEntry(Addr index, bool is_secure, bool replace, bool readonly, bool clearing, bool hawk);


};
