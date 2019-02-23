CXXSTD = -std=c++11
OPTIM = -O2
PYTHON_INCLUDE = -I/usr/local/Cellar/python/3.6.5/Frameworks/Python.framework/Versions/3.6/include/python3.6m/
PYTHON_LIB = -L/usr/local/Cellar/python/3.6.5/Frameworks/Python.framework/Versions/3.6/lib/ -lpython3.6
NUMPY_INCLUDE = -I/opt/python3-virtualenv/lib/python3.6/site-packages/numpy/core/include/
NUMPY_LIB =
BOOST_INCLUDE = -I/usr/local/Cellar/boost/1.68.0_1/include/
BOOST_LIB = -L/usr/local/Cellar/boost/1.68.0_1/lib/

override CXXFLAGS := $(CXXFLAGS)
override LDFLAGS := $(LDFLAGS)
override INCLUDES = $(PYTHON_INCLUDE) $(NUMPY_INCLUDE) $(BOOST_INCLUDE)
override LIBS = $(PYTHON_LIB) $(NUMPY_LIB) $(BOOST_LIB)

TARGET = _deoverlap_croutine.so
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDENCIES = $(SOURCES:.cpp=.d)
INSTALL_DIR = ../deoverlap_2d

.PHONY: build
build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -shared -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXSTD) $(OPTIM) $(CXXFLAGS) $(INCLUDES) -fPIC -c -MMD -MP -o $@ $<

.PHONY: all
all: $(INSTALL_DIR)/$(TARGET)

$(INSTALL_DIR)/$(TARGET): $(TARGET)
	$(SHELL) -c 'cp $(TARGET) $@'

.PHONY: clean
clean:
	$(RM) $(TARGET)
	$(RM) $(OBJECTS)
	$(RM) $(DEPENDENCIES)

-include $(DEPENDENCIES)
