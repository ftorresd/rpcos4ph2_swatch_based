
.NOTPARALLEL:


PACKAGES = \
	framework \
	example

# You should not need to set BUILD_POLYMER_ELEMENTS
# Since builds are committed (to the framework/html folder)
ifneq ("$(BUILD_POLYMER_ELEMENTS)", "")
	PACKAGES += framework/src/polymer
endif


VIRTUAL_PACKAGES = $(addsuffix /.virtual.Makefile,${PACKAGES})

FLAGS = $(ifeq $(MAKEFLAGS) "","",-$(MAKEFLAGS))

TARGETS = build rpm clean cleanrpm
.PHONY = $(TARGETS)

default: build

$(TARGETS) : $(VIRTUAL_PACKAGES)

$(VIRTUAL_PACKAGES):
	$(MAKE) $(FLAGS) -C $(@D) $(MAKECMDGOALS)
