@set LLVM_HOME="C:\Program Files\LLVM"
@set CXXFLAGS=-I%LLVM_HOME%\include -L%LLVM_HOME%\lib -D_CRT_SECURE_NO_WARNINGS=1
@set LDFLAGS=-llibclang

clang %CXXFLAGS% struct.c -o struct.exe %LDFLAGS%