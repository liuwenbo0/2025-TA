#pragma once
#include<iostream>
struct node{
	int num;
	node *prior, *next;
};

using Linklist = node*;