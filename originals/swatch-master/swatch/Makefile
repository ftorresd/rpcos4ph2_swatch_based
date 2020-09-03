
Set?=core


SWATCH_PACKAGES = \
	logger \
	core \
	action \
	processor \
	dtm \
	system \
	xml \
	dummy

SWATCH_TEST_PACKAGES = \
	logger/test \
	core/test \
	action/test \
	processor/test \
	dtm/test \
	system/test \
	xml/test \
	test

SWATCH_HW_PACKAGES = \
	amc13 \
	mp7 \
	mp7/test

ifeq ($(Set), core)
	PACKAGES = \
		$(SWATCH_PACKAGES) \
		$(SWATCH_TEST_PACKAGES)
else ifeq ($(Set), hwdevel)
	PACKAGES = \
		$(SWATCH_PACKAGES) \
		$(SWATCH_TEST_PACKAGES) \
		$(SWATCH_HW_PACKAGES)
else ifeq ($(Set), no_hw)
	PACKAGES = \
		$(SWATCH_PACKAGES) \
		$(SWATCH_TEST_PACKAGES) \
		cell
else ifeq ($(Set), all)
	PACKAGES = \
		$(SWATCH_PACKAGES) \
		$(SWATCH_TEST_PACKAGES) \
		$(SWATCH_HW_PACKAGES) \
		cell
else
	$(error Invalid value used for Set variable)
endif



VIRTUAL_PACKAGES = $(addsuffix /.virtual.Makefile,${PACKAGES})

FLAGS = $(ifeq $(MAKEFLAGS) "","",-$(MAKEFLAGS))

ifdef jobs
FLAGS+=-j ${jobs}
endif

# Makefile targets declared as phony
TARGETS = build rpm clean cleanrpm objects 
.PHONY = $(TARGETS)

default: build

# Each target in TARGET will be built using the targets in VIRTUAL_PACKAGES
$(TARGETS): $(VIRTUAL_PACKAGES)

$(VIRTUAL_PACKAGES):
	$(MAKE) $(FLAGS) -C $(@D) $(MAKECMDGOALS)
