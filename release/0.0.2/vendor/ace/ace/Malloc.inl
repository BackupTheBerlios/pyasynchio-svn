/* -*- C++ -*- */
// Malloc.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

ACE_INLINE
ACE_Control_Block::ACE_Name_Node::~ACE_Name_Node (void)
{
}

ACE_INLINE void
ACE_Control_Block::ACE_Malloc_Header::init_ptr
  (ACE_Malloc_Header **ptr, ACE_Malloc_Header *init, void *)
{
  *ptr = init;
}

ACE_INLINE void
ACE_Control_Block::ACE_Name_Node::init_ptr
  (ACE_Name_Node **ptr, ACE_Name_Node *init, void *)
{
  *ptr = init;
}
