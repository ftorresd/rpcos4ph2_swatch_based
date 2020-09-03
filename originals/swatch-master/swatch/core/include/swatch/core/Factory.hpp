/**
 * @file    Factory.hpp
 * @author  Alessandro Thea
 * @brief   Template class to provide support for abstract classes
 * @date    April 2015
 *
 */

#ifndef __SWATCH_CORE_FACTORY_HPP__
#define __SWATCH_CORE_FACTORY_HPP__

#include "swatch/core/Object.hpp"
#include "swatch/core/AbstractFactory.hpp"

namespace swatch {
namespace core {

typedef AbstractFactory<Object> Factory;

} // namespace system
} // namespace swatch

#define SWATCH_REGISTER_CLASS( classname ) _SWATCH_ABSTRACT_REGISTER_CLASS( swatch::core::Object, classname)
#define SWATCH_REGISTER_CREATOR( creatorname ) _SWATCH_ABSTRACT_REGISTER_CREATOR( swatch::core::Object, creatorname)


#endif  /* __SWATCH_CORE_FACTORY_HPP__ */

