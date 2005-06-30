/* -*- C++ -*- */
// Containers.i,v 4.23 1998/09/18 03:57:26 levine Exp

ACE_INLINE
ACE_DLList_Node::ACE_DLList_Node (void)
  : item_ (0),
    next_ (0),
    prev_ (0)
{
}

ACE_INLINE
ACE_DLList_Node::ACE_DLList_Node (void *&i,
                                  ACE_DLList_Node *n,
                                  ACE_DLList_Node *p)
  : item_ (i),
    next_ (n),
    prev_ (p)
{
}

ACE_INLINE
ACE_DLList_Node::~ACE_DLList_Node (void)
{
}
