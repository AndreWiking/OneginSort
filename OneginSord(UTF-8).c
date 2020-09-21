// Компилирую в Visual Studio
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define null ((void *)0)
#define UNIT_TESTS

typedef struct
{
    char* b; // begin
    char* e; // end
} String;

// Проверка является ли символ буквой или цифрой
char isNoun(char c) {
    if (c >= 'А' && c <= 'Я' || c >= 'а' && c <= 'я' || c >= 'A' && c <= 'Z' ||
        c >= 'a' && c <= 'z' || c >= '0' && c <= '9') return 1;
    else return 0;
}

// Сравнение строк для сортировки, игнорируя знаки препинания
// (слева на право)
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

// Сравнение строк для сортировки, игнорируя знаки препинания
// (справа на лево)
int StringCompRev(const String* ca, const String* cb) {
    int la = 0, lb = 0;
    const char* a = ca->e, * b = cb->e;
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

// Печатает текст из res в файл SortedText.txt
void PrintText(char const* res, int type) {

    FILE* fOut;
    fOut = fopen("SortedText.txt", type == 0 ? "w" : "a");

    if (type == 0)
        fputs("\n\nТекст отсортирован с лева на право\n_______________\n\n\n\n", fOut);

    else if (type == 1)
        fputs("\n\nТекст отсортирован с право на лево\n_______________\n\n\n\n", fOut);

    else if (type == 2)
        fputs("\n\nОригинальный текст\n_______________\n\n\n\n", fOut);

    fputs(res, fOut);

    fclose(fOut);
}

// Узнаём размер файла
size_t GetFileSize(FILE* pFile) {
    fseek(pFile, 0, SEEK_END);
    size_t lSize = ftell(pFile);
    rewind(pFile);
    return lSize;
}

// Возвращает указатель на считанный текст
// !!! необходимо потом почистить память по возвращаемому указателю !!!
char* ReadText(const char* FileName, size_t *lSize, size_t *result) {
    FILE* pFile;
    char* text;

    // Бинарное считывание файла
    pFile = fopen(FileName, "rb");
    if (pFile == null) { fprintf(stderr, "File error"); exit(-1); }

    *lSize = GetFileSize(pFile);

    // Выделяем память для записи данных из файла
    text = (char*)calloc(*lSize + 1, sizeof(char));
    if (text == null) { fprintf(stderr, "Memory error"); exit(-2); }

    // Записываем данные из файла
    *result = fread(text, sizeof(char), *lSize, pFile);
    fclose(pFile);
    if (*result != *lSize) { fprintf(stderr, "Reading error"); exit(-3); }
    if (*result == 0) { fprintf(stderr, "Empty file");  exit(-4); }
    *(text + *result) = '\n';

    return text;
}

// Быстрый подсчёт количества \n
int LinesCount(const char* text) {
    int sCount = 0;
    while(1)
    {
        text = strchr(text, '\n');
        if (text == null) break;
        ++text;
        ++sCount;
    }
    return sCount;
}

// Делим char* на String и возвращаем их количество 
size_t FillStrings(String* strings, char* text, size_t result) {
    size_t sn = 0; // номер текущей строки
    for (size_t i = 0; i < result + 1; ++i) {
        if (text[i] == '\n' && sn != 0) {
            strings[sn - 1].e = text + i - (text[i - 1] == '\r' ? 2 : 1);
            if (i != result) strings[sn++].b = text + i + 1;
        }
        else if (sn == 0 && text[i] != '\r') strings[sn++].b = text;
    }
    return sn; // количество строк
}

// Преобразовываем массив String в char*
void StringsToChars(char* resText, const String *strings, size_t sn) {
    int k = 0;
    for (size_t i = 0; i < sn; ++i) {
        char f = 0;
        for (char* v = strings[i].b; v <= strings[i].e; ++v) if (*v != '\r' && *v != '\n') {
            resText[k++] = *v;
            f = 1;
        }
        if (f) resText[k++] = '\n';
    } resText[k++] = '\0';
}

// Сортирует текст из файла FileName
// type == 0 сортировка с слева на право
// type == 1 сортировка с справа на лево
// type >= 2 не сортирует текст
// Возвращает указатель на отсортированный текст
// !!! необходимо потом почистить память по возвращаемому указателю !!!
char* SortText(const char* FileName, int type) {
    char* text;
    size_t lSize;
    size_t result;

    text = ReadText(FileName, &lSize, &result);

    // Узнаём максимальное количество строк
    int sCount = LinesCount(text);
    
    // Выделяем память для указателей на строки   
    String *strings = (String*)calloc(sCount, sizeof(String));
    if (strings == null) { fprintf(stderr, "Memory error"); exit(-2); }

    // Заполняем массив строк и узнаём их итоговое количество
    size_t sn = FillStrings(strings, text, result);

    // Сортировка слева на право
    if (type == 0) qsort((void*)strings, sn, sizeof(String), StringComp);
    
    // Сортировка справа на лево
    if (type == 1) qsort((void*)strings, sn, sizeof(String), StringCompRev);

    // Выделяем память для отсортированного текста
    char* resText = (char*)calloc(lSize + 2, sizeof(char));
    if (resText == null) { fprintf(stderr, "Memory error"); exit(-2); }
    
    // Записываем отсортированный текст из strings в resText для быстрого вывода
    StringsToChars(resText, strings, sn);

    // Освобождаем память
    free(text);
    free(strings);

    return resText;
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

    // Сортировка слева на право
    char* res = SortText("Onegin.txt", 0);
    PrintText(res, 0);
    free(res);

    // Сортировка справа на лево
    res = SortText("Onegin.txt", 1);
    PrintText(res, 1);
    free(res);

    // Оригинальный текст
    res = SortText("Onegin.txt", 2);
    PrintText(res, 2);
    free(res);

    printf("-Successful sort\n--See SortedText.txt");

    return 0;
}
