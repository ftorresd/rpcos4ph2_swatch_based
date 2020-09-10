#ifndef RPCOS4PH2DUMMY_VERSION_H_
#define RPCOS4PH2DUMMY_VERSION_H_

#include "config/PackageInfo.h"

#define RPCOS4PH2DUMMY_VERSION_MAJOR 0
#define RPCOS4PH2DUMMY_VERSION_MINOR 1
#define RPCOS4PH2DUMMY_VERSION_PATCH 1

//
// Template macros
//

namespace rpcos4ph2
{
    namespace dummy
    {
        const std::string project = "rpcos4ph2";
        const std::string package = "dummy";
        const std::string versions = "0.1.1";
        const std::string description = "RPCOS4PH2 DUMMY Swatch cell";
        const std::string authors = "Felipe Silva, Kevin Mot";
        const std::string summary = "RPCOS4PH2 DUMMY Swatch system";
        const std::string link = "http://cern.ch/rpcos4ph2";

        config::PackageInfo getPackageInfo();
        void checkPackageDependencies();
        std::set<std::string, std::less<std::string>> getPackageDependencies();
    } // namespace dummy
} // namespace rpcos4ph2
#endif
