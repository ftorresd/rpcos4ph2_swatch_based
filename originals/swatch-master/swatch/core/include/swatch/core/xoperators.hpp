/**
 * @file    xoperators.hpp
 * @author  Alessandro Thea
 * @brief   Streamers for xdata classes
 * @date    January 2015
 *
 */
#ifndef __SWATCH_CORE_XOPERATORS_HPP__
#define __SWATCH_CORE_XOPERATORS_HPP__


// Standard headers
#include <iosfwd>                       // for ostream
#include <string>                       // for string

// XDAQ headers
#include "xdata/SimpleType.h"
#include "xdata/String.h"


namespace xdata {

class Serializable;


std::ostream& operator<< (std::ostream& aStream, const xdata::Serializable& aSer);

/*
 * xdata::SimpleType operators
 */

/*
 * operator==
 */
template<typename T>
int operator==(const xdata::SimpleType<T>& aSer1, const xdata::SimpleType<T>& aSer2)
{
  return (aSer1 == aSer2);
}

template<typename T>
int operator==(const xdata::SimpleType<T>& aSer1, const T& aSer2)
{
  return (aSer1 == aSer2);
}

template<typename T>
int operator==(const T& aSer1, const xdata::SimpleType<T>& aSer2)
{
  return (aSer2 == aSer1);
}

/*
 * xdata::String
 */

/*
* operator==
*/
int operator==(const std::string& aStr1, const xdata::String& aStr2);

int operator==(const char*& aStr1, const xdata::String& aStr2);

}

#endif /* __SWATCH_CORE_XOPERATORS_HPP__ */
