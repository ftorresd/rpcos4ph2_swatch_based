# swatch::logger

## Using the log4cplus logger
Example for a `swatch::core` module.
```
#include "swatch/logger/Logger.hpp
#include <log4cplus/loggingmacros.h>
swatch::logger logger = Logger::getInstance("swatch.core.componentA");
// logging messages with various log levels
LOG4CPLUS_FATAL(logger, "Hello world");
LOG4CPLUS_ERROR(logger, "Hello world " << myIntVar);
LOG4CPLUS_INFO(logger, "This is a long(hex): " << hex << (long) 100000000););
LOG4CPLUS_DEBUG(logger, "I am debug");
LOG4CPLUS_WARN(logger, "This is the last warning");
LOG4CPLUS_TRACE(logger, "tracing the call...);
```
more examples are in swatch/logger/test/src/common/runLogger.cxx and of course http://sourceforge.net/p/log4cplus/wiki/CodeExamples/.

Logger instances are structured in a hierarchy:
```

                                   root
                                     |
                                   swatch
                                 /   |    \
                                /    |     \
                               /     |      \
                            core  processor  system etc.
                            /
                      component
```
e.g. for accessing the logger for a component in core one would
```
log4cplus::Logger logger = Logger::getInstance("swatch.core.componentA");
```

Real-life examples of this can be found in `swatch/core/src/common/ActionableObject.cpp` (look for mLogger).

## Using default macro (deprecated)
The `LOG` macro is included in `swatch::logger::Log` and can be used as shown:
```
LOG(logger::LogLevel) << "my log message";
```
and will log to the `swatch` logger.

