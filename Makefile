export BUILDTYPE ?= Debug
#export WITH_CXX11ABI ?= $(shell scripts/check-cxx11abi.sh)
export WITH_CXX11ABI = 0

ifeq ($(BUILDTYPE), Release)
else ifeq ($(BUILDTYPE), RelWithDebInfo)
else ifeq ($(BUILDTYPE), Sanitize)
else ifeq ($(BUILDTYPE), Debug)
else
  $(error BUILDTYPE must be Debug, Sanitize, Release or RelWithDebInfo)
endif

buildtype := $(shell echo "$(BUILDTYPE)" | tr "[A-Z]" "[a-z]")

ifeq ($(shell uname -s), Darwin)
  HOST_PLATFORM = macos
  HOST_PLATFORM_VERSION = $(shell uname -m)
  #export NINJA = platform/macos/ninja
  export JOBS ?= $(shell sysctl -n hw.ncpu)
else ifeq ($(shell uname -s), Linux)
  HOST_PLATFORM = linux
  HOST_PLATFORM_VERSION = $(shell uname -m)
  export NINJA = ninja
  export JOBS ?= $(shell grep --count processor /proc/cpuinfo)
else
  $(error Cannot determine host platform)
endif

ifeq ($(V), 1)
  export XCPRETTY
  NINJA_ARGS ?= -v
else
  export XCPRETTY ?= | tee '$(shell pwd)/build/xcodebuild-$(shell date +"%Y-%m-%d_%H%M%S").log' | xcpretty
  NINJA_ARGS ?=
endif

.PHONY: default
default: ubit demos examples

.PHONY: ums
ums: ums

.PHONY: check
check: tests
 
BUILD_DEPS += Makefile
BUILD_DEPS += CMakeLists.txt

#### Linux targets #####################################################

ifeq ($(HOST_PLATFORM), linux)

export PATH := $(shell pwd)/platform/linux:$(PATH)
export LINUX_OUTPUT_PATH = build/linux-$(shell uname -m)/$(BUILDTYPE)
LINUX_BUILD = $(LINUX_OUTPUT_PATH)/build.ninja

$(LINUX_BUILD): $(BUILD_DEPS)
	mkdir -p $(LINUX_OUTPUT_PATH)
	(cd $(LINUX_OUTPUT_PATH) && cmake -G Ninja ../../.. \
		-DCMAKE_BUILD_TYPE=$(BUILDTYPE) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DWITH_CXX11ABI=${WITH_CXX11ABI})

.PHONY: ubit
ubit: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) ubit

.PHONY: ums
ums: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) ums

.PHONY: demos
demos: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) \
	udemos table ufinder wspace xmlparser

.PHONY: examples
examples: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) \
	example1 example2 example3 sedit4 events tables layout lists \
	tree text1 text2 glcanvas

.PHONY: ubittests
tests: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) uappli_tests ubittests

endif


#### Miscellaneous targets #####################################################

.PHONY: clean
clean:
	-rm -rf ./build

.PHONY: distclean
distclean: clean
