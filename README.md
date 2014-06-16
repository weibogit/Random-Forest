Random-Forest
=============

Parallelization of the machine learning algorithm.

Well..The mathematical and logical stuff is exactly the grasping idea under the hood!

##Implementation Steps
* Decision Tree
* Random Forest
* MPI

##Keynotes: Intriguing Stuffs
###1.Optimization measures

* Use STL set to avoid duplicated value split operation.
* Use non-recursive method (Heap) to build up the tree.
* Parallelization operation, multithreading when operating prediction in both one single tree and the whole forest.

###2.Mathematical ideas
* When reducing the number of training features from p to √p, the number of rows for training should also be shrank to 1/2 of the original count. Avoiding high variances in the leaf node of the tree.

###3.Adjustment
* Choosing the MODE but not MEAN when creating leaf node. MEAN appears to be meaningless in this specific case. 