// ���������� � Visual Studio
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define null ((void *)0)
#define UNIT_TESTS

typedef struct
{
    char* b;
    char* e;
} String;

// �������� �������� �� ������ ������ ��� ������
char isNoun(char c) {
    if (c >= '�' && c <= '�' || c >= '�' && c <= '�' || c >= 'A' && c <= 'Z' ||
        c >= 'a' && c <= 'z' || c >= '0' && c <= '9') return 1;
    else return 0;
}

// ��������� ����� ��� ����������, ��������� ����� ����������
// (����� �� �����)
int StringComp(const String* ca, const String* cb) {
    int la = 0, lb = 0;
    char* a = ca->b, * b = cb->b;
    while (1)
    {
        if (a > ca->e && b > cb->e) return la < lb ? -1 : la > lb ? 1 : 0;
        else if (a > ca->e) return -1;
        else if (b > cb->e) return 1;

        if (!isNoun(*a)) { ++a, ++la; continue; }
        if (!isNoun(*b)) { ++b, ++lb; continue; }
        if (*a < *b) return -1;
        else if (*a > * b) return 1;
        ++a, ++b, ++la, ++lb;
    }
} 

// ��������� ����� ��� ����������, ��������� ����� ����������
// (������ �� ����)
int StringCompRev(const String* ca, const String* cb) {
    int la = 0, lb = 0;
    char* a = ca->e, * b = cb->e;
    while (1)
    {
        if (a < ca->b && b < cb->b) return la < lb ? -1 : la > lb ? 1 : 0;
        else if (a < ca->b) return -1;
        else if (b < cb->b) return 1;

        if (!isNoun(*a)) { --a, ++la; continue; }
        if (!isNoun(*b)) { --b, ++lb; continue; }
        if (*a < *b) return -1;
        else if (*a > * b) return 1;
        --a, --b, ++la, ++lb;
    }
}

// �������� ����� �� res � ���� SortedText.txt
void PrintText(char *res, int type) {

    FILE* fOut;
    fOut = fopen("SortedText.txt", type == 0 ? "w" : "a");

    if (type == 0)
        fputs("\n\n����� ������������ � ���� �� �����\n_______________\n\n\n\n", fOut);

    else if (type == 1)
        fputs("\n\n����� ������������ � ����� �� ����\n_______________\n\n\n\n", fOut);

    else if (type == 2)
        fputs("\n\n������������ �����\n_______________\n\n\n\n", fOut);

    fputs(res, fOut);

    fclose(fOut);
}

// ��������� ����� �� ����� FileName
// type == 0 ���������� � ����� �� �����
// type == 1 ���������� � ������ �� ����
// type >= 2 �� ��������� �����
// ���������� ��������� �� ��������������� �����
// !!! ���������� ����� ��������� ������ �� ������������� ��������� !!!
char* SortText(char* FileName, int type) {
    char* text;
    FILE* pFile;
    size_t lSize;
    size_t result;

    // �������� ���������� �����
    pFile = fopen(FileName, "rb");
    if (pFile == null) { fprintf(stderr, "File error"); exit(1); }

    // ����� ������ �����
    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);

    // �������� ������ ��� ������ ������ �� �����
    text = (char*)malloc(sizeof(char) * (lSize + 1));
    if (text == null) { fprintf(stderr, "Memory error"); exit(2); }

    // ���������� ������ �� �����
    result = fread(text, sizeof(char), lSize, pFile);
    fclose(pFile);
    if (result != lSize) { fprintf(stderr, "Reading error"); exit(3); }
    if (result == 0) { fprintf(stderr, "Empty file");  exit(4); }
    *(text + result) = '\n';

    // ����� ������������ ����������� �����
    int sCount = 0;
    for (size_t i = 0; i < result + 1; ++i)
        if (text[i] == '\n') ++sCount;
    
    // �������� ������ ��� ���������� �� ������   
    String *st = (String*)malloc(sizeof(String) * sCount);
    if (st == null) { fprintf(stderr, "Memory error"); exit(2); }
    size_t sn = 0;
    for (size_t i = 0; i < result + 1; ++i) {
        if (text[i] == '\n' && sn != 0) {
            st[sn - 1].e = text + i - (text[i - 1] == '\r' ? 2 : 1);
            if (i != result) st[sn++].b = text + i + 1;
        }
        else if (sn == 0 && text[i] != '\r') st[sn++].b = text;
    }

    // ���������� ����� �� �����
    if (type == 0) qsort((void*)st, sn, sizeof(String), StringComp);
    
    // ���������� ������ �� ����
    if (type == 1) qsort((void*)st, sn, sizeof(String), StringCompRev);

    // �������� ������ ��� ���������������� ������
    char* res = (char*)malloc(sizeof(char) * (lSize + 2)); 
    if (res == null) { fprintf(stderr, "Memory error"); exit(-2); }
    int k = 0;
    for (size_t i = 0; i < sn; ++i) {
        char f = 0;
        for (char* v = st[i].b; v <= st[i].e; ++v) if (*v != '\r' && *v != '\n') {
            res[k++] = *v;
            f = 1;
        }
        if (f) res[k++] = '\n';
    } res[k++] = '\0';

    // ����������� ������
    free(text);
    free(st);
    return res;
}

int main() {

#ifdef UNIT_TESTS

    char* test;

    // Unit test 1.1
    test = SortText("UnitTests/test1.txt", 0);
    
    if (strcmp(test, "ads\nhh\ny\n") != 0)
        fprintf(stderr, "Sort Faile (Unit Test 1.1)\n");
    free(test);

    // Unit test 1.2
    test = SortText("UnitTests/test1.txt", 1);

    if (strcmp(test, "hh\nads\ny\n") != 0)
        fprintf(stderr, "Sort Faile (Unit Test 1.2)\n");
    free(test);

    // Unit test 1.3
    test = SortText("UnitTests/test1.txt", 2);

    if (strcmp(test, "y\nads\nhh\n") != 0)
        fprintf(stderr, "Sort Faile (Unit Test 1.3)\n");
    free(test);

    // Unit test 2.1
    test = SortText("UnitTests/test2.txt", 0);

    if (strcmp(test, "&\n") != 0)
        fprintf(stderr, "Sort Faile (Unit Test 2.1)\n");
    free(test);

    // Unit test 3.1
    test = SortText("UnitTests/test3.txt", 0);
    
    if (strcmp(test, "123\n45sz\n.h__g\n&%*-y43\nzzz\n") != 0)
        fprintf(stderr, "Sort Faile (Unit Test 3.1)\n");
    free(test);

    // Unit test 3.2
    test = SortText("UnitTests/test3.txt", 1);

    if (strcmp(test, "123\n&%*-y43\n.h__g\n45sz\nzzz\n") != 0)
        fprintf(stderr, "Sort Faile (Unit Test 3.2)\n");
    free(test);

#endif // UNIT_TESTS

    // ���������� ����� �� �����
    char* res = SortText("Onegin.txt", 0);
    PrintText(res, 0);
    free(res);

    // ���������� ������ �� ����
    res = SortText("Onegin.txt", 1);
    PrintText(res, 1);
    free(res);

    // ������������ �����
    res = SortText("Onegin.txt", 2);
    PrintText(res, 2);
    free(res);

    printf("-Successful sort\n--See SortedText.txt");

    return 0;
}