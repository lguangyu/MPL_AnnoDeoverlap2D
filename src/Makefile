CXXSTD = -std=c++11
OPTIM = -O2
PYTHON_INCLUDE =
PYTHON_LIB =
NUMPY_INCLUDE =
NUMPY_LIB =
BOOST_INCLUDE =
BOOST_LIB =

override CXXFLAGS := $(CXXFLAGS)
override LDFLAGS := $(LDFLAGS)
override INCLUDES = $(PYTHON_INCLUDE) $(NUMPY_INCLUDE) $(BOOST_INCLUDE)
override LIBS = $(PYTHON_LIB) $(NUMPY_LIB) $(BOOST_LIB)

TARGET = _deoverlap_croutine.so
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDENCIES = $(SOURCES:.cpp=.d)

.PHONY: build
build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -shared -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXSTD) $(OPTIM) $(CXXFLAGS) $(INCLUDES) -fPIC -c -MMD -MP -o $@ $<

.PHONY: clean
clean:
	$(RM) $(TARGET)
	$(RM) $(OBJECTS)
	$(RM) $(DEPENDENCIES)

-include $(DEPENDENCIES)
