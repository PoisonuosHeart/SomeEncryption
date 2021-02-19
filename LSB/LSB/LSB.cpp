#include <iostream>
#include <fstream>
#include <locale.h>
#include <string>
#include <windows.h>

void copyFile(const std::string& fName, std::string& sName);
// Возвращает считанное слово в формате big-endian (не перевернутое)
WORD read_16(std::fstream& f);
DWORD read_32(std::fstream& f);
WORD read_16(std::ifstream& f);
DWORD read_32(std::ifstream& f);
//  Возвращает 10-значение числа из 1 бита, регулируется интервалом start end
unsigned  int get10FrombyteWithRange(char& c, int start, int end);
// Возвращает позицию матрицы цветов в файле
int getPosColorMatr(const BITMAPINFOHEADER& bih);
//  Шифрование сообщения msg методом LBS-M
void LBS_M(std::string& fName, std::string& msg);
void decryptionMessage(std::string fName, long int lengthMessage);
// Menu for encryption/decryption user message from BMP container
void menu();

int main()
{
    setlocale(LC_ALL, "rus");
    
    menu();

    std::cout << "Hello World!\n";
    system("pause");
    return 0;
}

void LBS_M(std::string& fName, std::string& msg) {
    std::string sName;
    copyFile(fName, sName);

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    std::fstream f(sName.c_str(), std::ios_base::out | std::ios_base::in |
        std::ios_base::binary);
    std::cout << "Name of encrypted file: " << sName << "\n";

    bfh.bfType = read_16(f);
    bfh.bfSize = read_32(f);
    bfh.bfReserved1 = read_16(f);
    bfh.bfReserved2 = read_16(f);
    bfh.bfOffBits = read_32(f);

    bih.biSize          = read_32(f);
    bih.biWidth         = read_32(f);
    bih.biHeight        = read_32(f);
    bih.biPlanes        = read_16(f);
    bih.biBitCount      = read_16(f);
    bih.biCompression   = read_32(f);

    long int fPtr = getPosColorMatr(bih);  // позиция с которой начинается матрица пикселей

    std::ofstream fw(sName.c_str(), std::ios_base::out | std::ios_base::in |
        std::ios_base::binary);
    
    f.seekp(fPtr);
    fw.seekp(fPtr);  // Устанавливаем корретку в начало файла с матрицей цветов
    std::cout << "№ байта с которого начинается зашифрованное сообщение: " << fPtr << "\n";
    char symb; 
    long int i = 0; std::streamoff curPos; int k1 = 7;
    while (i < msg.length()) {
        if (k1 >= 0) {
            curPos = f.tellp();
            f.read(reinterpret_cast<char*>(&symb), 1);

            if ((symb & 1) != ((msg[i] >> k1) & 1)) {
                if ((symb & 1) == 1)
                    symb = symb & 254;
                else symb = symb | 1;
            }
          
            --k1;
            fw.write(reinterpret_cast<char*>(&symb), 1);
        }
        else {
            k1 = 7; ++i;
        } 
    }
    fw.close();
    f.close();
}

void copyFile(const std::string& fName, std::string& sName) {
    std::ifstream fBmp(fName, std::ios_base::binary | std::ios_base::out);
    sName = fName; sName += "_copy.bmp\0";
    std::ofstream f(sName, std::ios_base::in | std::ios_base::out |
        std::ios_base::binary | std::ios_base::trunc); 
    char tmp;
    while (!fBmp.eof()) {
        fBmp.read(reinterpret_cast<char*>(&tmp), sizeof(tmp));
        f.write(reinterpret_cast<char*>(&tmp), sizeof(tmp));
    }
    f.close();
    fBmp.close();
}

WORD read_16(std::fstream& f) {
    char b0, b1;
    f.read(&b0, sizeof(b0));
    f.read(&b1, sizeof(b1));
  
    unsigned short int res = 0;
    
    res += get10FrombyteWithRange(b1, 0, 8);
    res += get10FrombyteWithRange(b0, 8, 16);

    return res;
}

DWORD read_32(std::fstream& f) {
    char b0, b1, b2, b3;
    f.read(&b3, sizeof(b3));
    f.read(&b2, sizeof(b2));
    f.read(&b1, sizeof(b1));
    f.read(&b0, sizeof(b0));

    unsigned int res = 0;

    res += get10FrombyteWithRange(b3, 0, 8);
    res += get10FrombyteWithRange(b2, 8, 16);
    res += get10FrombyteWithRange(b1, 16, 32);
    res += get10FrombyteWithRange(b0, 32, 64);

    return res;
}

WORD read_16(std::ifstream& f) {
    char b0, b1;
    f.read(&b0, sizeof(b0));
    f.read(&b1, sizeof(b1));

    unsigned short int res = 0;

    res += get10FrombyteWithRange(b1, 0, 8);
    res += get10FrombyteWithRange(b0, 8, 16);

    return res;
}

DWORD read_32(std::ifstream& f) {
    char b0, b1, b2, b3;
    f.read(&b3, sizeof(b3));
    f.read(&b2, sizeof(b2));
    f.read(&b1, sizeof(b1));
    f.read(&b0, sizeof(b0));

    unsigned int res = 0;

    res += get10FrombyteWithRange(b3, 0, 8);
    res += get10FrombyteWithRange(b2, 8, 16);
    res += get10FrombyteWithRange(b1, 16, 32);
    res += get10FrombyteWithRange(b0, 32, 64);

    return res;
}

unsigned int get10FrombyteWithRange(char& c, int start, int end) {
    unsigned int res = 0, k = 0;
    for (int i = start; i < end; ++i, ++k)
        if (((c >> k) & 1) == 1) {
            res += static_cast <unsigned int>(pow(2, i));
        }
    return res;
}

int getPosColorMatr(const BITMAPINFOHEADER& bih) {
    if (bih.biSize == 108) //4
        return 122;

    if (bih.biSize == 124) //5
        return 138;

    if (bih.biSize == 12)   // 1
        return 26;

    if ((bih.biSize == 40) && (bih.biCompression == 3))
        return 66;

    if ((bih.biSize == 40) && (bih.biCompression == 6))
        return 70;

    if (bih.biSize == 40) //3
        return 54;
    return -1;
}

void decryptionMessage(std::string fName, long int lengthMessage) {
    std::ifstream f(fName.c_str(), std::ios_base::in |
        std::ios_base::out | std::ios_base::binary);

    BITMAPINFOHEADER bih;
    f.seekg(14);

    bih.biSize = read_32(f);
    bih.biWidth = read_32(f);
    bih.biHeight = read_32(f);
    bih.biPlanes = read_16(f);
    bih.biBitCount = read_16(f);
    bih.biCompression = read_32(f);

    long int fPtr = getPosColorMatr(bih);

    int b = 7; std::string msg('E', lengthMessage + 1); long int i = 0;
    char bMsg = static_cast<char>(0);

    f.seekg(fPtr);

    while (i < lengthMessage) {
        if (b >= 0) {
            char iB;
            f.read(reinterpret_cast<char*> (&iB), 1);
            bMsg = bMsg | ((iB & 1) << b);
            --b;
        }
        else {
            msg[i] = bMsg;
            ++i; b = 7; bMsg = static_cast<char>(0);
        }
    }

    f.close();
    msg += '\0';
    std::cout << "decrypt message: " << msg << "\n";
}

void menu() {
    int contin; std::string fName, msg;
    std::cout << "Если хотите зашифровать сообщение, введите число > 0\nДля расшифровки введите 0\nДля выхода введите отрицательное число\nВаш выбор: ";
    std::cin >> contin; std::cin.ignore(32767, '\n');
    while (contin >= 0) {
        std::cout << "\nВведите имя BMP-контейнера с расширением .bmp: ";
        std::getline(std::cin, fName);
        std::cout << "Исходное файл: " << fName << "\n\n";
        
        if (contin > 0) {
            std::cout << "Введите сообщение для сокрытия в bmp: ";
            std::getline(std::cin, msg);
            LBS_M(fName, msg);
        }
        else {
            std::string sName = fName + "_copy.bmp\0";
            decryptionMessage(sName, msg.length());
        }

        std::cout << "\n\nЕсли хотите зашифровать сообщение, введите число > 0\nДля расшифровки введите 0\nДля выхода введите отрицательное число\nВаш выбор: ";
        std::cin >> contin; std::cin.ignore(32767, '\n');
    }
}