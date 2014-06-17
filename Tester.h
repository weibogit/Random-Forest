//
//  Tester.h
//  Random-Forest
//
//  Created by Cho-Yeung Lam on 16/6/14.
//  Copyright (c) 2014 Cho-Yeung Lam. All rights reserved.
//

#ifndef __Random_Forest__Tester__
#define __Random_Forest__Tester__

#include "Constants.h"
#include "DecisionTree.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

class Tester
{
private:
    double temp_values[NUM_COLUMN-1];
    string resultFilePath;
    string testFilePath;
    Node *root;
    void split(const string& src, const string& delim);
    void testResult(int rowNum);
    int cateCount[NUM_CATEGORIES];
    
public:
    int result[NUM_TEST_ROW];
    
    Tester(string testFilePath, string resultFilePath, Node *root = NULL);
    void changeRoot(Node *newRoot);
    void begin();
    void writeResult(int *votes);
    void clearCateCount();
};

#endif /* defined(__Random_Forest__Tester__) */