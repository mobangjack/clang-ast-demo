#include <stdio.h>
#include <stdbool.h>
#include <clang-c/Index.h>
#include <stdlib.h>
#include <string.h>
#include "utlist.h"
#include "uthash.h"

#define STRUCT_PREFIX "struct "
#define STRUCT_PREFIX_LENGTH (sizeof(STRUCT_PREFIX) - 1)
#define LOG(...) printf(__VA_ARGS__)

static inline char *stralloc(const char *s)
{
    int len = strlen(s);
    char *p = (char *) malloc(len + 1);
    if (p)
    {
        strncpy(p, s, len);
        p[len] = '\0';
    }
    return p;
}

struct struct_member
{
    struct struct_member *prev, *next;
    char *struct_name;
    char *type;
    int size;
    int offset;
};

struct struct_decl
{
    char *struct_name;
    struct struct_member *members;
    int num_members;
    UT_hash_handle hh;
};

static struct struct_decl *s_struct_decls;

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

const char *get_struct_name(const char *canonical_name)
{
    if (strncmp(canonical_name, STRUCT_PREFIX, STRUCT_PREFIX_LENGTH) == 0)
    {
        return canonical_name + STRUCT_PREFIX_LENGTH;
    }
    return canonical_name;
}

enum CXChildVisitResult visitor(CXCursor cursor, CXCursor parent,
                                CXClientData client_data)
{
    const char *curSpelling = clang_getCString(clang_getCursorSpelling(cursor));
    CXType curType = clang_getCursorType(cursor);
    enum CXCursorKind curKind = clang_getCursorKind(cursor);
    const char *curKindSpelling = clang_getCString(
                                  clang_getCursorKindSpelling(curKind));

    struct struct_decl *decl = NULL;
    const char *struct_name = NULL;

    if (clang_Cursor_isAnonymous(cursor) || strcmp(curSpelling, "") == 0)
    {
        return CXChildVisit_Recurse;
    }

    if (curKind == CXCursor_StructDecl)
    {
        struct_name = get_struct_name(curSpelling);
        HASH_FIND_STR(s_struct_decls, struct_name, decl);
        if (!decl)
        {
            decl = (struct struct_decl *) calloc(1, sizeof(struct struct_decl));
            if (!decl)
            {
                LOG("out of memory!\n");
                return CXChildVisit_Break;
            }
            decl->struct_name = stralloc(struct_name);
            HASH_ADD_STR(s_struct_decls, struct_name, decl);
        }
    }
    else if (curKind == CXCursor_FieldDecl)
    {
         CXCursor semanticParent = clang_getCursorSemanticParent(cursor);
        const char *parentSpelling = clang_getCString(
            clang_getCursorSpelling(semanticParent));
        if (strcmp(parentSpelling, "") == 0)
        {
            return CXChildVisit_Recurse;
        }
        struct_name = get_struct_name(parentSpelling);
        HASH_FIND_STR(s_struct_decls, struct_name, decl);
        if (!decl)
        {
            LOG("can not find parent struct %s for field %s!\n",
                struct_name, curSpelling);
            return CXChildVisit_Recurse;
        }
        struct struct_member *member = NULL;
        LL_FOREACH(decl->members, member)
        {
            if (strcmp(member->struct_name, curSpelling) == 0)
            {
                return CXChildVisit_Recurse;
            }
        }
        member = (struct struct_member *) calloc(1, sizeof(struct struct_member));
        if (!member)
        {
            LOG("out of memory!\n");
            return CXChildVisit_Break;
        }
        member->struct_name = stralloc(curSpelling);
        CXType canonicalType = clang_getCanonicalType(curType);
        const char *typeSpelling = clang_getCString(
            clang_getTypeSpelling(canonicalType));
        member->type = stralloc(get_struct_name(typeSpelling));
        member->size = clang_Type_getSizeOf(clang_getCursorType(cursor));
        member->offset = clang_Cursor_getOffsetOfField(cursor);
        LL_APPEND(decl->members, member);
    }

    return CXChildVisit_Recurse;
}

int post_process(struct struct_decl *decls)
{
    struct struct_decl *entry, *temp;
    struct struct_member *member;
    HASH_ITER(hh, decls, entry, temp)
    {
        LOG("%s {\n", entry->struct_name);
        LL_FOREACH(entry->members, member)
        {
            LOG("\t%s {\n", member->struct_name);
            LOG("\t\ttype: %s\n", member->type);
            LOG("\t\tsize: %d\n", member->size);
            LOG("\t\toffset: %d\n", member->offset);
            LOG("\t}\n");
        }
        LOG("}\n");
    }
    return 0;
}

int main(int argc, const char *argv[]) {

    CXIndex Index = clang_createIndex(0,0);
    CXTranslationUnit TU = clang_parseTranslationUnit(Index, 0, argv, argc,
                                                      0, 0,
                                                      CXTranslationUnit_None);
    CXCursor C = clang_getTranslationUnitCursor(TU);
    clang_visitChildren(C, visitor, NULL);

    post_process(s_struct_decls);

    clang_disposeTranslationUnit(TU);
    clang_disposeIndex(Index);
    return 0;
}
