.DEFAULT_GOAL := all

CXX=clang++
CXXFLAGS=-g3 -std=c++17
LDFLAGS=-g3 -lsqlite3 -L /usr/local/lib/
TEST_FLAGS=-I /usr/local/include/

COVERAGE_FLAGS=-fprofile-instr-generate -fcoverage-mapping

SRCDIR := src
BUILDDIR := build
COV_BUILDDIR := build/coverage

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

TESTDIR := $(SRCDIR)/test
TEST_SRCS := $(wildcard $(TESTDIR)/*.cpp)
TEST_OBJS := $(patsubst $(TESTDIR)/%.cpp,$(BUILDDIR)/%.test.o,$(TEST_SRCS))

COVERAGE_OBJS := $(patsubst $(SRCDIR)/%.cpp,$(COV_BUILDDIR)/%.o,$(SRCS))
COVERAGE_TEST_OBJS := $(patsubst $(TESTDIR)/%.cpp,$(COV_BUILDDIR)/%.test.o,$(TEST_SRCS))

# define target names
TARGET := stockr
TARGET_UNITTEST := stockr-unittest
TARGET_COVERAGE := stockr-coverage

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(COV_BUILDDIR):
	mkdir -p $(COV_BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.test.o: $(TESTDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) -I$(SRCDIR) -c $< -o $@

# CXXFLAGS are updated for coverage
$(COV_BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(COV_BUILDDIR)
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

$(COV_BUILDDIR)/%.test.o: $(TESTDIR)/%.cpp | $(COV_BUILDDIR)
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) -I$(SRCDIR) -c $< -o $@

# Remove build/main.o from OBJS when building test and coverage targets
FILTERED_OBJS := $(filter-out $(BUILDDIR)/main.o, $(OBJS))
FILTERED_COVERAGE_OBJS := $(filter-out $(COV_BUILDDIR)/main.o, $(COVERAGE_OBJS))

$(BUILDDIR)/$(TARGET): $(OBJS) | $(BUILDDIR)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BUILDDIR)/$(TARGET_UNITTEST): $(TEST_OBJS) $(FILTERED_OBJS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) $^ -lgtest $(LDFLAGS) -o $@

$(COV_BUILDDIR)/$(TARGET_COVERAGE): $(COVERAGE_TEST_OBJS) $(FILTERED_COVERAGE_OBJS) | $(COV_BUILDDIR)
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) $(TEST_FLAGS) $^ -lgtest $(LDFLAGS) -o $@

.PHONY: all build test coverage clean

# all - build
all: $(BUILDDIR)/$(TARGET)

# build the unittest target and run
test: $(BUILDDIR)/$(TARGET_UNITTEST)
	$(BUILDDIR)/$(TARGET_UNITTEST)

# build the coverage target
coverage: $(COV_BUILDDIR)/$(TARGET_COVERAGE)
	cd $(COV_BUILDDIR) && rm -f *.profraw *.profdata
	@echo "Run the coverage testing"
	cd $(COV_BUILDDIR) && ./$(TARGET_COVERAGE)
	@echo "List the files"
	cd $(COV_BUILDDIR) && pwd && ls
	@echo "Merge coverage data"
	cd $(COV_BUILDDIR) && llvm-profdata merge -sparse default.profraw -o default.profdata; \
	pwd && ls
	@echo "Generate coverage report"
	cd $(COV_BUILDDIR); \
	llvm-cov show ./$(TARGET_COVERAGE) -ignore-filename-regex=".*(googletest|gtest).*" -instr-profile=default.profdata -format=html -output-dir=coverage_report
	@echo "Find the coverage report in $(COV_BUILDDIR)/coverage_report/"

clean:
	rm -rf $(BUILDDIR) $(COV_BUILDDIR)

