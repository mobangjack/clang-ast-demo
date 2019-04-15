CXX := g++
LLVMCOMPONENTS := #cppbackend
RTTIFLAG := -fno-rtti
LLVMCONFIG := ../../usr/bin/llvm-config

CXXFLAGS := -I$(shell $(LLVMCONFIG) --src-root)/tools/clang/include -I$(shell $(LLVMCONFIG) --obj-root)/tools/clang/include $(shell $(LLVMCONFIG) --cxxflags) $(RTTIFLAG)
LLVMLDFLAGS := $(shell $(LLVMCONFIG) --ldflags --libs $(LLVMCOMPONENTS))

SOURCES = struct.c

OBJECTS = $(SOURCES:.c=.o)
EXES = $(OBJECTS:.o=)
CLANGLIBS =     -lclangTooling\
				-lclangFrontendTool\
				-lclangFrontend\
				-lclangDriver\
				-lclangSerialization\
				-lclangCodeGen\
				-lclangParse\
				-lclangSema\
				-lclangStaticAnalyzerFrontend\
				-lclangStaticAnalyzerCheckers\
				-lclangStaticAnalyzerCore\
				-lclangAnalysis\
				-lclangARCMigrate\
				-lclangRewriteFrontend\
				-lclangRewrite\
				-lclangEdit\
				-lclangAST\
				-lclangLex\
				-lclangBasic\
				-lclang \
				-lclangASTMatchers\
				$(shell $(LLVMCONFIG) --libs)\
                -lcurses
			   
all: $(OBJECTS) $(EXES)

%: %.o
	$(CXX) -o $@ $<  $(CLANGLIBS) -I/usr/include  $(LLVMLDFLAGS) 

clean:
	rm -f $(EXES) $(OBJECTS) *~
