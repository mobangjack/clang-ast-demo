#include <stdio.h>
#include <stdbool.h>
#include <clang-c/Index.h>


bool printKindSpelling(CXCursor cursor) {
    enum CXCursorKind curKind = clang_getCursorKind(cursor);
    const char *curkindSpelling = clang_getCString(
                                  clang_getCursorKindSpelling(curKind));
    printf("The AST node kind spelling is:%s\n", curkindSpelling);
    return true;
}

bool printSpelling(CXCursor cursor) {
    const char *astSpelling = clang_getCString(clang_getCursorSpelling(cursor));
    int size = clang_Type_getSizeOf(clang_getCursorType(cursor));
    int offset = clang_Cursor_getOffsetOfField(cursor);
    printf("field: %s, size: %d, offset=%d\n", astSpelling, size, offset);
    return true;
}

bool printLocation(CXCursor cursor) {
    CXSourceRange range = clang_getCursorExtent(cursor);
    CXSourceLocation startLocation = clang_getRangeStart(range);
    CXSourceLocation endLocation = clang_getRangeEnd(range);

    CXFile file;
    unsigned int line, column, offset;
    clang_getInstantiationLocation(startLocation, &file, &line, &column, &offset);
    printf("Start: Line: %u Column: %u Offset: %u\n", line, column, offset);
    clang_getInstantiationLocation(endLocation, &file, &line, &column, &offset);
    printf("End: Line: %u Column: %u Offset: %u\n", line, column, offset);

    return true;
}

enum CXChildVisitResult printVisitor(CXCursor cursor, CXCursor parent,
                                     CXClientData client_data) {
    const char *astSpelling = clang_getCString(clang_getCursorSpelling(cursor));
    CXType childType = clang_getCursorType(cursor);
    CXType parentType = clang_getCursorType(parent);
    if (!clang_equalTypes(childType, parentType)) {
    	printSpelling(cursor);
    }
//    printSpelling(cursor);
//    printKindSpelling(cursor);
//    printLocation(cursor);

    return CXChildVisit_Recurse;
}

int main(int argc, const char *argv[]) {

    CXIndex Index = clang_createIndex(0,0);
    CXTranslationUnit TU = clang_parseTranslationUnit(Index, 0, argv, argc,
                                                      0, 0,
                                                      CXTranslationUnit_None);
    CXCursor C = clang_getTranslationUnitCursor(TU);
#if 0
    printSpelling(C);
    printKindSpelling(C);
    printLocation(C);
#endif
    clang_visitChildren(C, printVisitor, NULL);

    clang_disposeTranslationUnit(TU);
    clang_disposeIndex(Index);
    return 0;
}
