#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
using namespace std;

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    //Открытие файла на чтение исходного текста
    ifstream in("sample.txt");

    if (!in.is_open()) {
        cerr << "Не удалось открыть файл для чтения!" << endl;
        return 1;
    }

    //Чтение всего файла в строку
    //C++17 позволяет легко прочитать все, даже спец-символы
    string text((istreambuf_iterator<char>(in)),
        istreambuf_iterator<char>());
    
    cout << text << endl;
    in.close();


    //Открытие файла на запись закодированного текста

    ofstream file("zip.txt");

    if (!file.is_open()) {
        cerr << "Не удалось открыть файл для записи!" << endl;
        return 1;
    }
    
    file << "Hello";
    file << 1232322222222222;
    file.close();

    return 0;
}