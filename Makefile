.DEFAULT_GOAL := all


CXX=clang++
COMPILE_FLAGS=-g3 -std=c++17
LINKER_FLAGS=-g3 -lsqlite3

SRCDIR := src
BUILDDIR := build

#CPP_FILES := main.cpp datamanager.cpp dbmanager.cpp

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

TARGET := $(BUILDDIR)/stockr

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(COMPILE_FLAGS) -c $< -o $@


$(TARGET): $(OBJS) | $(BUILDDIR)
	$(CXX) $^ $(LINKER_FLAGS) -o $@

all: $(TARGET)

.PHONY: all clean
clean:
	rm -rf $(BUILDDIR)
