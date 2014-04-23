//-------------------------------------------------------------- SetOperations.h

#ifndef SETOPERATIONS_H
#define SETOPERATIONS_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& set)
{
  os << "{ ";
  std::copy(begin(set), end(set), std::ostream_iterator<T>(os, " "));
  os << "}";
  return os;
}

template<typename T>
std::set<T> Union(const std::set<T>& setA, const std::set<T>& setB)
{
  std::set<T> setC;
  std::set_union(begin(setA), end(setA), begin(setB), end(setB),
                 std::insert_iterator<std::set<T> >(setC, begin(setC)));
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
  std::set_difference(begin(setA), end(setA), begin(setB), end(setB),
                      std::insert_iterator<std::set<T>>(setC, begin(setC)));
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
  std::set_intersection(begin(setA), end(setA), begin(setB), end(setB),
                        std::insert_iterator<std::set<T>>(setC, begin(setC)));
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
std::set<T> Not(const std::set<T>& Universe, const std::set<T>& set)
{
  return Universe - set;
}

template<typename T>
bool isASubsetOf(const std::set<T>& setA, const std::set<T>& setB)
{
  if (&setA == &setB)
  {
    return true;
  }
  auto iSetA = std::begin(setA);
  auto iSetB = std::begin(setB);
  while (iSetB != end(setB))
  {
    iSetA = setA.find(*iSetB++);
    if (iSetA == end(setA))
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
  return set.find(element) != end(set);
}

template<typename T>
inline
int sizeOf(const std::set<T>& set)
{
  return set.size();
}

/// Generates a set containing [b..e)
template<typename T>
std::set<T> makeRange(T b, T e)
{
  std::set<T> result;
  for (auto i = b; i < e; ++i)
  {
    result.insert(i);
  }
  return result;
}

#endif

//------------------------------------------------------------ eof SetOperations
