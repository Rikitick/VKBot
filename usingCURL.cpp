#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>

using namespace std;


int main()
{
    // Создаём переменные тип массив char и инициализируем нулями: fileex - полный путь до файла С расширением,
                                                                // ex - само расширение,
                                                                // file - полный путь до файла БЕЗ расширения
    char fileex[501]{0}, ex[5]{0}, file[501]{0};

    cout << "FULL path to file for compilation to CURL (max: 500 char): ";
    // Считываем полный путь
    cin >> fileex;

    // Выделяем расширение
    size_t len = strlen(fileex);
    ex[0] = fileex[len-4];
    ex[1] = fileex[len-3];
    ex[2] = fileex[len-2];
    ex[3] = fileex[len-1];

    if (!strcmp(ex, ".cpp")) {
        strncpy(file, fileex, len-4);
    }
    else {
        strncpy(file, fileex, len);
        strcat(fileex, ".cpp");
    }

    // Формируем команду для консоли для создания объектного файла
    char* command1 = new char[500];
    sprintf(command1, "g++ -c %s -IC:\\curl-8.9.1\\include", fileex);
    system(command1);
    delete [] command1;
    
    // Выделения имени файла
    char name[100]{0};
    char* ind = !strrchr(file, '/') ? strrchr(file, '\\') : strrchr(file, '/');
    strcpy(name, ind + 1);

    // Сохраняем путь откуда была запущена программа
    char path[500]{0};
    getcwd(path, 500);

    // Линковка
    char* command2 = new char[500];
    sprintf(command2, "g++ %s\\%s.o -o %s -LC:\\curl-8.9.1\\lib -lcurl", path, name, file);
    system(command2);
    strcat(path, "\\");
    strcat(path, name);
    strcat(path, ".o");
    remove(path);
    delete [] command2;

    return 0;
}