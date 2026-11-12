#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool checkPaths(Node* current, int currDepth, int &fDepth)
{
  if(current == nullptr){ //case 1: node is null 
    return true;
  }

  //case 2: child pointers are null
  if(current->right == nullptr && current->left == nullptr){ 
    if(fDepth == -1){
      fDepth = currDepth;   
    }

    return currDepth == fDepth;
  }

  return checkPaths(current->left, currDepth + 1, fDepth) && checkPaths(current->right, currDepth + 1, fDepth);
}


bool equalPaths(Node * root)
{
  int firstDepth = -1; 
  return checkPaths(root, 0, firstDepth);

}

