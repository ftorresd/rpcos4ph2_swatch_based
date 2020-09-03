
#include "swatch/processor/test/utilities.hpp"


// boost headers
#include "boost/lexical_cast.hpp"

// SWATCH headers
#include "swatch/core/toolbox/IdSliceParser.hpp"


namespace swatch {
namespace processor {
namespace test {


void pushBackPortStubs(std::vector<ProcessorPortStub>& aPortStubs, const std::string& aName, const std::string& aIndex)
{
  std::vector<std::string> lNames = core::toolbox::IdSliceParser::parse(aName);
  std::vector<std::string> lIndices = core::toolbox::IdSliceParser::parse(aIndex);

  if (lNames.size() != lIndices.size())
    XCEPT_RAISE(core::RuntimeError,boost::lexical_cast<std::string>(lNames.size()) + " port names created from name \"" + aName + "\" using slice syntax, but " + boost::lexical_cast<std::string>(lIndices.size()) + " indices created from \"" + aIndex + "\"");

  for (size_t i = 0; i < lNames.size(); i++) {

    ProcessorPortStub lStub(lNames.at(i));
    lStub.number = boost::lexical_cast<unsigned>(lIndices.at(i));
    aPortStubs.push_back(lStub);
  }
}


}
}
}
