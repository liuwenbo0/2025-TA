#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

using namespace std;

class MySort {
public:
	virtual void mysort(std::vector<int>& nums) = 0;
};