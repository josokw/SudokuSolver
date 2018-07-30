#ifndef SETOPERATIONS_H
#define SETOPERATIONS_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &set)
{
   os << "{ ";
   std::copy(std::cbegin(set), std::cend(set),
             std::ostream_iterator<T>(os, " "));
   os << "}";
   return os;
}

template <typename T>
std::set<T> setsUnion(const std::set<T> &setA, const std::set<T> &setB)
{
   std::set<T> setC;
   std::set_union(std::cbegin(setA), std::cend(setA), std::cbegin(setB),
                  std::cend(setB),
                  std::insert_iterator<std::set<T>>(setC, std::begin(setC)));
   return setC;
}

template <typename T>
inline std::set<T> operator+(const std::set<T> &lhs, const std::set<T> &rhs)
{
   return setsUnion(lhs, rhs);
}

template <typename T>
std::set<T> setsDifference(const std::set<T> &setA, const std::set<T> &setB)
{
   std::set<T> setC;
   std::set_difference(
      std::cbegin(setA), std::cend(setA), std::cbegin(setB), std::cend(setB),
      std::insert_iterator<std::set<T>>(setC, std::begin(setC)));
   return setC;
}

template <typename T>
inline std::set<T> operator-(const std::set<T> &lhs, const std::set<T> &rhs)
{
   return setsDifference(lhs, rhs);
}

template <typename T>
std::set<T> setsIntersection(const std::set<T> &setA, const std::set<T> &setB)
{
   std::set<T> setC;
   std::set_intersection(
      std::cbegin(setA), std::cend(setA), std::cbegin(setB), std::cend(setB),
      std::insert_iterator<std::set<T>>(setC, std::begin(setC)));
   return setC;
}

/// Calculates the intersection of two sets.
template <typename T>
inline std::set<T> operator*(const std::set<T> &setA, const std::set<T> &setB)
{
   return setsIntersection(setA, setB);
}

template <typename T>
inline std::set<T> operator^(const std::set<T> &setA, const std::set<T> &setB)
{
   return (setA + setB) - (setA * setB);
}

template <typename T>
inline std::set<T> Not(const std::set<T> &universe, const std::set<T> &set)
{
   return universe - set;
}

template <typename T>
bool isASubsetOf(const std::set<T> &setA, const std::set<T> &setB)
{
   if (&setA == &setB) {
      return true;
   }
   auto iSetA = std::cbegin(setA);
   auto iSetB = std::cbegin(setB);
   while (iSetB != std::cend(setB)) {
      iSetA = setA.find(*iSetB++);
      if (iSetA == std::cend(setA)) {
         return false;
      }
   }
   return true;
}

template <typename T>
inline bool isAnElementOf(const T &element, const std::set<T> &set)
{
   return set.find(element) != std::cend(set);
}

template <typename T>
inline bool removeElements(const std::set<T> &tobeRemoved, std::set<T> &set)
{
   std::set<T> temp{set};
   set = set - tobeRemoved;
   return temp != set;
}

template <typename T> inline int sizeOf(const std::set<T> &set)
{
   return set.size();
}

/// Generates a set containing [b..e)
template <typename T> std::set<T> makeRange(T b, T e)
{
   std::set<T> result;
   for (auto i = b; i < e; ++i) {
      result.insert(i);
   }
   return result;
}

#endif
