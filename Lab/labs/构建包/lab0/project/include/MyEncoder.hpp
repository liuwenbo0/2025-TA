#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include "dessert.hpp"

// 将命名空间设定为std，调用中可以省去““
using namespace std;

class MyEncoder {
public:
	// 样例数据文件所在位置
	string sample_input_file = "./data/samples/data.in";

public:
	// @param
	// key: 加密密钥，key仅由小写英文字母及 ' ' 组成，key 包含英文字母表中每个字符（'a' 到 'z'）至少一次
	// secrets: 待解密的密文数组，secrets中的每一个元素为string类型

	// @return
	// messages: 解密后的消息数组，messages中每一个元素为string类型，与secrets中每个元素一一对应
	vector<string> Encoding(string key, vector<string> secrets) {
		vector<string> messages;
		/* YOUR SOLUTION HERE */
		/* ... */
		return messages;
	}
	/*---------------------------------*/
	//                                 //
	//                                 //
	//  测试代码，请不要修改以下的任何内容！ //
	//                                 //
	//                                 //
	/*---------------------------------*/
	void test() {
		vector<string> ans = {"welcome", "to", "the", "world", "of", "algorithm"};
		ifstream ifs(sample_input_file);
		if (!ifs.is_open()) {
			cout << "open input_file failed" << endl;
			return;
		}
		string key;
		ifs >> key;
		vector<string> secrets;
		string secret;
		while(ifs >> secret) {
			secrets.push_back(secret);
		}

		vector<string> messages = Encoding(key, secrets);

		dessert(messages.size() == secrets.size());
		dessert(messages.size() == ans.size());
		
		bool pass = true;
		for(int i=0; i<messages.size(); i++) {
			if(dessert(messages[i] == ans[i])) {
				cout << messages[i] << (i == messages.size()-1 ? "\n" : " ");
			}
		}
	}
};
