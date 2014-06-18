//
//  DecisionTree.cpp
//  Random-Forest
//
//  Created by Cho-Yeung Lam on 12/6/14.
//  Copyright (c) 2014 Cho-Yeung Lam. All rights reserved.
//

#include "DecisionTree.h"
#include <algorithm>
#include <math.h>

// Private methods

double DecisionTree::regLeaf_mode(vector<int> span)
{
    vector<int> histogram(NUM_CATEGORIES, 0);
    for (vector<int>::iterator iter = span.begin(); iter != span.end(); iter++) {
        histogram[dataSet[*iter * NUM_COLUMN]]++;
    }
    
    double label = 0;
    int maxcount = 0;
    for (size_t i = 0; i < NUM_CATEGORIES; i++)
        if (histogram.at(i) >= maxcount) {
            label = i;
            maxcount = histogram.at(i);
        }
    
    vector<int> maxVector;
    for (size_t i = 0; i < NUM_CATEGORIES; i++)
        if (histogram.at(i) == maxcount)
            maxVector.push_back((int)i);
    
    if (maxVector.size() > 1) {
        srand((unsigned)time(NULL));
        label = maxVector.at(rand() % (maxVector.size()-1));
    }
    
//    cout << "[LABEL on leaf] " << label << endl;
    labelCount[(int)label]++;
    return label;
}

double DecisionTree::regLeaf_mean(vector<int> span)
{
    vector<double> allLabel;
    vector<int>::iterator iter;
    for (iter = span.begin(); iter != span.end(); iter++) {
        allLabel.push_back(dataSet[*iter * NUM_COLUMN]);
    }
    sort(allLabel.begin(), allLabel.end());
    
    
    double mean = 0;
    size_t total = allLabel.size();
    if (total % 2 == 1)
        mean = allLabel.at(total/2+1);
    else
        mean = (allLabel.at(total/2-1) + allLabel.at(total/2)) / 2.0;
    
    return mean;
}

double DecisionTree::Gini(vector<int> span)
{
    double result = 0;
    int labelCnt[NUM_CATEGORIES] = {0};
    int label = 0;
    for (vector<int>::iterator iter = span.begin(); iter != span.end(); iter++) {
        label = (int)dataSet[*iter * NUM_COLUMN];
        labelCnt[label]++;
    }
    
    double totalCheck = 0;
    for (int label = 0; label < NUM_CATEGORIES; label++) {
        double percent = (double)labelCnt[label] / (double)span.size();
        result += percent * percent;
        totalCheck += percent;
    }
    result = 1.0 - result;
    return result;
}

void DecisionTree::chooseBestSplit(vector<int> span, int &bestIndex, double &bestValue)
{
    size_t counter = 1;
    double sameVal = dataSet[span.at(0) * NUM_COLUMN];
    for (size_t cnt = 1; cnt < span.size(); cnt++)
        if (dataSet[span.at(cnt) * NUM_COLUMN] == sameVal) counter++;
    if (counter == span.size()) {
        bestIndex = -1;
        bestValue = sameVal;
        return;
    }
    
    double G = Gini(span);
    double bestG = INFINITY;
    double newG = INFINITY;
    vector<int> lSpan, rSpan;
    
    for (set<int>::iterator feature = trainFeatures.begin(); feature != trainFeatures.end(); feature++) {
  
//        cout << "[TrainFeatures size] " << trainFeatures.size() << endl;
        
        // Optimization[1]: vector -> set
        set<int> valueSet;
        for (vector<int>::iterator iter = span.begin(); iter != span.end(); iter++) {
            valueSet.insert(dataSet[*iter * NUM_COLUMN + *feature]);
        }
        
        for (set<int>::iterator iter = valueSet.begin(); iter != valueSet.end(); iter++) {
//            cout << "[Inner] " << *iter << endl;
            binSplitData(span, lSpan, rSpan, *feature, dataSet[*iter * NUM_COLUMN + *feature]);
            
            if (lSpan.size() < tolN || rSpan.size() < tolN) {
                continue;
            }
            newG = ((double)lSpan.size() / (double)span.size()) * Gini(lSpan) + ((double)rSpan.size() / (double)span.size()) * Gini(rSpan);
            if (newG < bestG) {
                bestIndex = *feature;
                bestValue = dataSet[*iter * NUM_COLUMN + *feature];
                bestG = newG;
            }
        }
    }
    
    if (G - bestG < tolS) {
        bestIndex = -1, bestValue = regLeaf_mode(span);
        return;
    }
    binSplitData(span, lSpan, rSpan, bestIndex, bestValue);
    if (lSpan.size() < tolN || rSpan.size() < tolN) {
        bestIndex = -1, bestValue = regLeaf_mode(span);
        return;
//        int actualCnt = 0;
//        for (vector<int>::iterator iter = span.begin(); iter != span.end(); iter++) {
//            if (dataSet[*iter*NUM_COLUMN] == bestValue) actualCnt++;
//        }
//        cout << "size " << lSpan.size() << " " << rSpan.size() << " " << bestValue << " " << actualCnt <<endl;
    }
    else {
        /*already stored bestIndex and bestValue*/
        trainFeatures.erase(bestIndex);
//        cout << "[TrainFeatures size] " << trainFeatures.size() << endl;
    }
}

void DecisionTree::binSplitData(vector<int> pSpan, vector<int> &lSpan, vector<int> &rSpan, int feature, double value)
{
    lSpan.clear(), rSpan.clear();
    vector<int>::iterator it;
    for (it = pSpan.begin(); it != pSpan.end(); it++) {
        if (dataSet[*it * NUM_COLUMN + feature] <= value)
            lSpan.push_back(*it);
        else
            rSpan.push_back(*it);
    }
}

void DecisionTree::recursive_create_tree(vector<int> span, Node* &subroot)
{
    static int node_count = 0;
    int bestIndex = 0;
    double bestValue = 0;
    chooseBestSplit(span, bestIndex, bestValue);
    
    subroot = new Node(bestIndex, bestValue);
    // No bestIndex: Leaf
    if (bestIndex == -1) {
        return;
    }
    
    node_count++;
    featureChosen[bestIndex] = true;
    
    vector<int> lSpan, rSpan;
    binSplitData(span, lSpan, rSpan, bestIndex, bestValue);
    recursive_create_tree(lSpan, subroot->left);
    recursive_create_tree(rSpan, subroot->right);
}

// Public methods

DecisionTree::DecisionTree(double *dataSet)
{
    this->dataSet = dataSet;
    featureChosen[0] = true; // The labels
    for (int cnt = 1; cnt < NUM_COLUMN; cnt++)
        featureChosen[cnt] = false;
    
    // Random m = √p features(aka 56 in this case)
    int rand_feature = 0;
    srand((unsigned)time(NULL));
    while (trainFeatures.size() <= NUM_TRAIN_FEATURES) {
        rand_feature = (rand() % (NUM_COLUMN-1)) + 1;
        trainFeatures.insert(rand_feature);
    }
    
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        labelCount[i] = 0;
    }
}

void DecisionTree::createTree()
{
    // Recursive begin with the root Node
    // Random 10000 rows
    int modified_num_row = (int)(NUM_ROW * 2.0 / 3.0);
    set<int> wholeSpan;
    srand((unsigned)time(NULL));
    int rand_row = 0;
    while (wholeSpan.size() <= modified_num_row) {
        rand_row = rand() % NUM_ROW;
        wholeSpan.insert(rand_row);
    }
    
    vector<int> wholeSpan_vec;
    for (set<int>::iterator iter = wholeSpan.begin(); iter != wholeSpan.end(); iter++) {
        wholeSpan_vec.insert(wholeSpan_vec.end(), *iter);
    }
    
    recursive_create_tree(wholeSpan_vec, this->root);
    /*
    cout << "=================LABEL OF TREE================" << endl;
    int totalLeaves = 0;
    for (int label = 0; label < NUM_CATEGORIES; label++) {
        cout << "[" << label << "] " << labelCount[label] << endl;
        totalLeaves += labelCount[label];
    }
    cout << "[Leaves Count] " << totalLeaves << endl;
     */
}

Node* DecisionTree::getRoot()
{
    return this->root;
}
