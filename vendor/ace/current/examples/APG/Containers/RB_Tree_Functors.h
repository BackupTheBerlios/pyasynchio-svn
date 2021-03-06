/* -*- C++ -*- */
// RB_Tree_Functors.h,v 1.1 2004/01/01 21:01:00 shuston Exp

#ifndef __RB_TREE_FUNCTORS_H_
#define __RB_TREE_FUNCTORS_H_

#include "ace/Functor.h"

// Listing 1 code/ch05
// Same key type.
class KeyType
{
public:
  KeyType () : val_(0) {}
  KeyType (int i) : val_ (i) {}
  KeyType (const KeyType& kt) { this->val_ = kt.val_; }
  operator int() const { return val_; };

private:
  int val_;
};

ACE_TEMPLATE_SPECIALIZATION
class ACE_Less_Than<KeyType>
{
public:
  int operator() (const KeyType k1, const KeyType k2)
  { return k1 < k2; }
};
// Listing 1

#endif /* __RB_TREE_FUNCTORS_H_ */
