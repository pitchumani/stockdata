.DEFAULT_GOAL := all

CXX=clang++
CXXFLAGS=-g3 -std=c++17
LDFLAGS=-g3 -lsqlite3 -L /usr/local/lib/
TEST_FLAGS=-pthread --coverage -I /usr/local/include/

COVERAGE_FLAGS=-fprofile-instr-generate -fcoverage-mapping

SRCDIR := src
BUILDDIR := build

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

TESTDIR := $(SRCDIR)/test
TEST_SRCS := $(wildcard $(TESTDIR)/*.cpp)
TEST_OBJS := $(patsubst $(TESTDIR)/%.cpp,$(BUILDDIR)/%.test.o,$(TEST_SRCS))

TARGET := $(BUILDDIR)/stockr
TARGET_UNITTEST := $(BUILDDIR)/stockr-unittest
TARGET_COVERAGE := $(BUILDDIR)/stockr-coverage

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(BUILDDIR)/%.test.o: $(TESTDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) -I$(SRCDIR) -c $< -o $@

# Remove build/main.o from OBJS when building TEST_TARGET
FILTERED_OBJS := $(filter-out $(BUILDDIR)/main.o, $(OBJS))

$(TARGET): $(OBJS) | $(BUILDDIR)
	$(CXX) $^ $(LDFLAGS) -o $@

$(TARGET_UNITTEST): $(TEST_OBJS) $(FILTERED_OBJS) | $(BUILDDIR)
	echo $(TEST_OBJS)
	echo $(FILTERED_OBJS)
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) $^ -lgtest $(LDFLAGS) -o $@

# Coverage build: recompile everything with coverage flags
coverage_build: CXXFLAGS += $(COVERAGE_FLAGS)
coverage_build: LDFLAGS += $(COVERAGE_FLAGS)
coverage_build: clean $(BUILDDIR) $(TEST_OBJS) $(FILTERED_OBJS)
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) $(TEST_OBJS) $(FILTERED_OBJS) -lgtest $(LDFLAGS) -o $(TARGET_COVERAGE)

.PHONY: all
all: $(TARGET) $(TARGET_UNITTEST)

.PHONY: test
test: $(TARGET_UNITTEST)
	$(TARGET_UNITTEST)

.PHONY: coverage
coverage: coverage_build
	cd $(BUILDDIR); \
	rm -f *.profraw *.profdata; \
	./stockr-coverage; \
	llvm-profdata merge -sparse default.profraw -o default.profdata; \
	llvm-cov show ./stockr-coverage -ignore-filename-regex=".*(googletest|gtest).*"  -instr-profile=default.profdata -format=html -output-dir=coverage_report
	@echo "Find the coverage report in $(BUILDDIR)/coverage_report/"

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

