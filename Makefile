.DEFAULT_GOAL := all


CXX=clang++
COMPILE_FLAGS=-g3 -std=c++17 -I /usr/local/include
LINKER_FLAGS=-g3 -lsqlite3
TEST_FLAGS=-pthread --coverage -L /usr/local/lib

LLVM_COV_FLAGS = -fprofile-instr-generate -fcoverage-mapping
#GCOV_FLAGS = -fprofile-arcs -ftest-coverage
COV_FLAGS = $(LLVM_COV_FLAGS)

COV_TOOL = llvm-cov

SRCDIR := src
BUILDDIR := build

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

TESTDIR := $(SRCDIR)/test
TEST_SRCS := $(wildcard $(TESTDIR)/*.cpp)
TEST_OBJS := $(patsubst $(TESTDIR)/%.cpp,$(BUILDDIR)/%.test.o,$(TEST_SRCS))
TEST_TARGET := $(BUILDDIR)/unit_tests

TARGET := $(BUILDDIR)/stockr

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(COMPILE_FLAGS) $(COV_FLAGS) -c $< -o $@


$(BUILDDIR)/%.test.o: $(TESTDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(COMPILE_FLAGS) $(TEST_FLAGS) -I$(SRCDIR) -c $< -o $@

$(TARGET): $(OBJS) | $(BUILDDIR)
	$(CXX) $^ $(LINKER_FLAGS) $(TEST_FLAGS) -o $@

# Remove build/main.o from OBJS when building TEST_TARGET
FILTERED_OBJS := $(filter-out $(BUILDDIR)/main.o, $(OBJS))

$(TEST_TARGET): $(TEST_OBJS) $(FILTERED_OBJS) | $(BUILDDIR)
	echo $(TEST_OBJS)
	echo $(FILTERED_OBJS)
	$(CXX) $(COMPILE_FLAGS) $(TEST_FLAGS) $^ -lgtest $(LINKER_FLAGS) -o $@

all: $(TARGET)

.PHONY: test
test: $(TEST_TARGET)
	$(TEST_TARGET)

.PHONY: coverage
coverage: $(TEST_TARGET)
	cd $(BUILDDIR)
	@echo "Cleanup the coverage data files..."
	rm -f *.gcda *.gcno *.profraw *.profdata *.gcov
	@echo "Running tests to generate coverage data..."
	./$(TEST_TARGET) # Run test executable to generate .gcda/.profraw files
	@echo "Generating coverage report using $(COV_TOOL)..."
	llvm-profdata merge -sparse default.profraw -o default.profdata
	$(COV_TOOL) show $(TEST_TARGET) -instr-profile=default.profdata -format=html -output-dir=coverage_report
	@echo "Coverage report generated with $(COV_TOOL)."
	@echo "Find the coverage report in $(BUILDDIR)/coverage_report/"

.PHONY: all clean
clean:
	rm -rf $(BUILDDIR)
	#rm -f *.gcda *.gcno *.profraw *.profdata *.gcov
