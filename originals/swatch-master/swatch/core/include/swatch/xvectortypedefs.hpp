#ifndef __SWATHC_XVECTORTYPEDEFS_HPP__
#define __SWATHC_XVECTORTYPEDEFS_HPP__

#include "swatch/xsimpletypedefs.hpp"

#include "xdata/Vector.h"

namespace swatch {

typedef xdata::Vector<xdata::Boolean>           XVectorBool_t;
typedef xdata::Vector<xdata::Integer>           XVectorInt_t;
typedef xdata::Vector<xdata::Integer32>         XVectorInt32_t;
typedef xdata::Vector<xdata::Integer64>         XVectorInt64_t;
typedef xdata::Vector<xdata::UnsignedInteger>   XVectorUInt_t;
typedef xdata::Vector<xdata::UnsignedInteger32> XVectorUInt32_t;
typedef xdata::Vector<xdata::UnsignedInteger64> XVectorUInt64_t;
typedef xdata::Vector<xdata::UnsignedShort>     XVectorShort_t;
typedef xdata::Vector<xdata::UnsignedLong>      XVectorLong_t;
typedef xdata::Vector<xdata::Float>             XVectorFloat_t;
typedef xdata::Vector<xdata::Double>            XVectorDouble_t;
typedef xdata::Vector<xdata::String>            XVectorString_t;

}

#endif /* __SWATHC_XVECTORTYPEDEFS_HPP__ */
