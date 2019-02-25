export CXXSTD = -std=c++11
export OPTIM = -O2
export PYTHON_INCLUDE =
export PYTHON_LIB =
export NUMPY_INCLUDE =
export NUMPY_LIB =
export BOOST_INCLUDE =
export BOOST_LIB =

override CXXFLAGS := $(CXXFLAGS)
override LDFLAGS := $(LDFLAGS)

SOURCE_DIR = src
TARGET_DIR = deoverlap_2d
export TARGET = _deoverlap_croutine.so

.PHONY: build
build:
	$(MAKE) -C $(SOURCE_DIR) build

.PHONY: all
all: $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/$(TARGET): $(SOURCE_DIR)/$(TARGET)
	$(SHELL) -c 'cp $(SOURCE_DIR)/$(TARGET) $@'

$(SOURCE_DIR)/$(TARGET): build

.PHONY: clean
clean:
	$(MAKE) -C $(SOURCE_DIR) clean

.PHONY: distclean
distclean: clean
	$(RM) $(TARGET_DIR)/$(TARGET)
