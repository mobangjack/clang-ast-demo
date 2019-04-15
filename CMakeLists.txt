CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(struct)

ADD_EXECUTABLE(struct struct.c)

LIST(APPEND CLANG_LIBS
 clangFrontend 
 clangSerialization 
 clangDriver 
 clangParse 
 clangSema 
 clangAnalysis 
 clangAST 
 clangBasic 
 clangEdit 
 clangLex 
 clangTooling)

TARGET_LINK_LIBRARIES(struct ${LLVM_LIBS} ${CLANG_LIBS})
