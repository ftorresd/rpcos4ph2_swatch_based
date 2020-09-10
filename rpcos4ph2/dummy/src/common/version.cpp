#include "config/version.h"

#include "rpcos4ph2/dummy/version.h"


GETPACKAGEINFO(rpcos4ph2::dummy)

void
rpcos4ph2::dummy::checkPackageDependencies() 
{
    CHECKDEPENDENCY(config);
}


std::set<std::string, std::less<std::string> > rpcos4ph2::dummy::getPackageDependencies()
{
    std::set<std::string, std::less<std::string> > dependencies;
    ADDDEPENDENCY(dependencies,config);
    return dependencies;
}

