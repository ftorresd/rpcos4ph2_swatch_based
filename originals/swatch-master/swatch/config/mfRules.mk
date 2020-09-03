# Sanitize package path
PackagePath := $(shell cd ${PackagePath}; pwd)

# Library sources 
LibrarySources = $(wildcard src/common/*.cpp) $(wildcard src/common/**/*.cpp)
# Filter undesired files
LibrarySourcesFiltered = $(filter-out ${IgnoreSources}, ${LibrarySources})
# Turn them into objects
LibraryObjectFiles = $(patsubst src/common/%.cpp,${PackagePath}/obj/%.o,${LibrarySourcesFiltered})

ExecutableSources = $(wildcard src/common/*.cxx)
# Filter undesired files
ExecutableSourcesFiltered = $(filter-out ${IgnoreSources}, ${ExecutableSources})
# Turn them into objects
ExecutableObjectFiles = $(patsubst src/common/%.cxx,${PackagePath}/obj/%.o,${ExecutableSourcesFiltered})
# And binaries
Executables = $(patsubst src/common/%.cxx,${PackagePath}/bin/%.exe,${ExecutableSourcesFiltered})

# $(info LibrarySourcesFiltered = ${LibrarySourcesFiltered})
# $(info ExecutableSourcesFiltered = ${ExecutableSourcesFiltered})
# $(info ExecutableObjectFiles = ${ExecutableObjectFiles})
# $(info Executables = ${Executables})

# Compiler Flags
IncludePaths = $(addprefix -I,${Includes})

#LinkAllFlags += ${LinkFlags}
#ExecutableLinkAllFlags += ${ExecutableLinkFlags}

# Library dependencies
DependentLibraries += $(addprefix -L,${LibraryPaths})
DependentLibraries += $(addprefix -l,${Libraries})  

# Executable dependencies
ExecutableDependentLibraries += $(addprefix -L,${LibraryPaths})
ExecutableDependentLibraries += $(addprefix -l,${ExecutableLibraries})

# LibFolder := lib
# BinFolder := bin
# ObjFolder := obj

ifeq ("${Library}","")
LibraryTarget :=
else
LibraryTarget ?= lib/lib${Library}.so
endif

.PHONY: default
default: build

.PHONY: clean _cleanall
clean: _cleanall
_cleanall:
	${At} rm -rf obj
	${At} rm -rf bin
	${At} rm -rf lib

.PHONY: all _all build buildall
all: _all
build: _all
buildall: _all
_all: ${LibraryTarget} ${Executables} ${ExtraTargets}

.PHONY: objects
objects: ${LibraryObjectFiles} ${ExecutableObjectFiles}


# Implicit rule for .cpp -> .o 
${PackagePath}/obj/%.o : ${PackagePath}/src/common/%.cpp 
	${MakeDir} $(@D)
	${CPP} -c ${CxxFlags} ${IncludePaths} $< -o $@

# Implicit rule for .cxx -> .o 
${PackagePath}/obj/%.o : ${PackagePath}/src/common/%.cxx 
	${MakeDir} $(@D)
	${CPP} -c ${CxxFlags} ${IncludePaths} $< -o $@
	
# Main target: shared library
${LibraryTarget}: ${LibraryObjectFiles}
	${MakeDir} $(@D)
	${LD} ${LinkFlags} ${DependentLibraries} ${LibraryObjectFiles} -o $@

# Include automatically generated dependencies
-include $(LibraryObjectFiles:.o=.d)
	
# Static Pattern rule for binaries
${Executables} : ${PackagePath}/bin/%.exe : ${PackagePath}/obj/%.o ${LibraryTarget}
	${MakeDir} $(@D)
	${LD} ${ExecutableLinkFlags} ${ExecutableDependentLibraries} $< -o $@

# Include automatically generated dependencies
-include $(ExecutableObjectFiles:.o=.d)
