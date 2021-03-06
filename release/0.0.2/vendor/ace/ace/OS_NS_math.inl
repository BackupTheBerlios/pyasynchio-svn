// -*- C++ -*-
// OS_NS_math.inl,v 1.4 2004/05/27 06:55:17 ossama Exp

ACE_INLINE double
ACE_OS::floor (double x)
{
  // This method computes the largest integral value not greater than x.
  return double (static_cast<long> (x));
}

ACE_INLINE double
ACE_OS::ceil (double x)
{
  // This method computes the smallest integral value not less than x.
  const double floor = ACE_OS::floor (x);
  if (floor == x)
    return floor;
  else
    return floor + 1;
}
