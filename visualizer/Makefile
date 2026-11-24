
BUILD ?= build
BUILD_TYPE ?= Release

ifeq ($(DEBUG),yes)
BUILD_TYPE := Debug
endif

.PHONY: all clean dist

all:
	cmake -S . -B $(BUILD) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(BUILD)

clean:
	cmake --build $(BUILD) --target clean

dist: all
	cmake --install $(BUILD) --prefix ../dist/Rose
