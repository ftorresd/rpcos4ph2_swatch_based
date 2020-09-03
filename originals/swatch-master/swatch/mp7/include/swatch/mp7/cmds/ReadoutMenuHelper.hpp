#ifndef __SWATCH_MP7_READOUTMENUHELPER_HPP__
#define __SWATCH_MP7_READOUTMENUHELPER_HPP__

#include "swatch/core/XParameterSet.hpp"

#include "mp7/ReadoutMenu.hpp"
#include "swatch/mp7/cmds/ReadoutMenuConstraint.hpp"

namespace swatch {

namespace core {
class XPSetConstraint;
}

namespace action {
class Command;
}

namespace mp7 {
namespace cmds {

class ReadoutMenuConstraint;

/**
 * @brief      Helper class that encapsulates the interactions of the mp7
 *             readout menu with swatch parameters and commands.
 *             P.S.: The is room for improvement
 */
class ReadoutMenuHelper
{
public:
	
	/**
	 * @brief      { function_description }
	 *
	 * @param[in]  aBanks     A banks
	 * @param[in]  aModes     A modes
	 * @param[in]  aCaptures  A captures
	 */
	ReadoutMenuHelper(uint32_t aBanks, uint32_t aModes, uint32_t aCaptures);
	
	/**
	 * @brief      Destroys the object.
	 */
	~ReadoutMenuHelper();

	/**
	 * @brief      { function_description }
	 *
	 * @param[in]  aParams  A parameters
	 *
	 * @return     { description_of_the_return_value }
	 */
	::mp7::ReadoutMenu import( const ::swatch::core::XParameterSet& aParams );

	/**
	 * @brief      { function_description }
	 *
	 * @param      aCommand  A command
	 */
	void registerParameters( swatch::action::Command& aCommand );

	/**
	 * @brief      Adds requirements.
	 *
	 * @param      aConstraint  A constraint
	 */
	void addRequirements( ReadoutMenuConstraint& aConstraint );

private:

  uint32_t mBanks;
  uint32_t mModes;
  uint32_t mCaptures;

};
	
} // namespace cmds
} // namespace mp7
} // namespace swatch

#endif /* __SWATCH_MP7_READOUTMENUHELPER_HPP__ */