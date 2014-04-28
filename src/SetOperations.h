//-------------------------------------------------------------- SetOperations.h

#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& set)
{
   os << "{ ";
   std::copy(set.cbegin(), set.cend(), std::ostream_iterator<T>(os, " "));
   os << "}";
   return os;
}

template<typename T>
std::set<T> Union(const std::set<T>& setA, const std::set<T>& setB)
{
   std::set<T> setC;
   std::set_union(setA.cbegin(), setA.cend(), setB.cbegin(), setB.cend(),
                  std::insert_iterator<std::set<T> >(setC, setC.begin()));
   return setC;
}

template<typename T>
inline
std::set<T> operator+(const std::set<T>& setA, const std::set<T>& setB)
{
   return Union(setA, setB);
}

template<typename T>
std::set<T> difference(const std::set<T>& setA, const std::set<T>& setB)
{
   std::set<T> setC;
   std::set_difference(setA.cbegin(), setA.cend(), setB.cbegin(), setB.cend(),
                       std::insert_iterator<std::set<T> >(setC, setC.begin()));
   return setC;
}

template<typename T>
inline
std::set<T> operator-(const std::set<T>& setA, const std::set<T>& setB)
{
   return difference(setA, setB);
}

template<typename T>
std::set<T> intersection(const std::set<T>& setA, const std::set<T>& setB)
{
  std::set<T> setC;
  std::set_intersection(setA.cbegin(), setA.cend(), setB.cbegin(), setB.cend(),
                        std::insert_iterator<std::set<T> >(setC, setC.begin()));
  return setC;
}

/// Calculates the intersection of two sets.
template<typename T>
inline
std::set<T> operator*(const std::set<T>& setA, const std::set<T>& setB)
{
  return intersection(setA, setB);
}

template<typename T>
inline
std::set<T> operator^(const std::set<T>& setA, const std::set<T>& setB)
{
  return (setA + setB) - (setA * setB);
}

template<typename T>
inline
std::set<T> Not(const std::set<T>& universe, const std::set<T>& set)
{
  return universe - set;
}

template<typename T>
bool isASubsetOf(const std::set<T>& setA, const std::set<T>& setB)
{
  if (&setA == &setB)
  {
    return true;
  }
  auto iSetA = setA.cbegin();
  auto iSetB = setB.cbegin();
  while (iSetB != setB.cend())
  {
    iSetA = setA.find(*iSetB++);
    if (iSetA == setA.cend())
    {
      return false;
    }
  }
  return true;
}

template<typename T>
inline
bool isAnElementOf(const T& element, const std::set<T>& set)
{
  return set.find(element) != set.cend();
}

template<typename T>
inline
int sizeOf(const std::set<T>& set)
{
  return set.size();
}

//------------------------------------------------------------ eof SetOperations
