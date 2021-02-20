#include <iostream>
#include <string>
#include <vector>
#include <locale>

/*@
* Конвертировать ключ и сообщение в 2 int массива (int = 4 байта)
* Сделать алгоритм и перевести два int массива в string
* Нужна функция соответствующая
* Для извлечения инфы - read_32 из 6
* Другую написать самому
@*/

// Возвращает часть числа из символа
int get10FrombyteWithRange(char& c, int start, int end);
// Вохвращает число в 10 системе со знаком из последовательности 32 бит
int get10FromChars(char b4, char b3, char b2, char b1);
// Получить исходные данные и ключ
void getUserData(std::string &msg, std::string &key);
// Получить исходные данные в виде строки
void getStrFromArr(const std::vector <int>& arr, std::string& str);
// Преобразовать данные в удобный вид
void convertUserData(std::vector <int>& msgArr, std::vector <int> &keyArr, const std::string& msg, const std::string& key);
// Образующая функция
int generatingFunc(const std::vector <int> &key, int i);
int generatingFuncdec(const std::vector <int>& key, int i);
// Сдвиг на 1 элемент для раунда
void shiftByOneEl(std::vector<int>&msgArr);
void shiftByOneEldec(std::vector<int>& msgArr);
// Один раунд из алгоритма Feistel
void raoundFeistel(std::vector <int>& msgArr, std::vector <int> &key, int i);
void raoundFeisteldec(std::vector <int>& msgArr, std::vector <int>& key, int i);
// Шифрует сообщение алгоритмом Фестеля
void Feistel(std::vector <int>& msgArr, std::vector <int> &key, const int &rounds);
// Дешифрует сообщение
void decryptionFeistel(std::vector <int>& msgArr, std::vector <int> &key, const int &rounds);

int main()
{
    setlocale(LC_ALL, "rus");

    std::string msg, key, decryptMes; std::vector <int> msgArr, keyArr;
    getUserData(msg, key);
    convertUserData(msgArr, keyArr, msg, key);

    getStrFromArr(msgArr, decryptMes);
    std::cout << "Исходное сообщение: " << decryptMes << "\n";

    Feistel(msgArr, keyArr, 10);    // Кодирование сообщения

    getStrFromArr(msgArr, decryptMes);
    std::cout << "Зашифрованное сообщение: " << decryptMes << "\n";

    decryptionFeistel(msgArr, keyArr, 10);  // Раскодирование сообщения

    getStrFromArr(msgArr, decryptMes);
    std::cout << "Расшифрованное сообщение: " << decryptMes << "\n";

    std::cout << "Hello World!\n";
    system("pause");
    return 0;
}

int generatingFunc(const std::vector <int>  &key, int i) {
    return ((key[0] << i) ^ (key[1] >> i));
}

int generatingFuncdec(const std::vector <int>& key, int i) {
    return ((key[0] >> i) ^ (key[1] << i));
}

int get10FrombyteWithRange(char& c, int start, int end) {
    int res = 0, k = 0;
    for (int i = start; i < end; ++i, ++k)
        if (((c >> k) & 1) == 1) {
            res += static_cast <int>(pow(2, i));
        }
    return res;
}

int get10FromChars(char b4, char b3, char b2, char b1) {
    int res = get10FrombyteWithRange(b4, 24, 32);
    res += get10FrombyteWithRange(b3, 16, 24);
    res += get10FrombyteWithRange(b2, 8, 16);
    res += get10FrombyteWithRange(b1, 0, 8);
    return res;
}

void getUserData(std::string& msg, std::string& key) {
    std::cout << "Введите сообщение для шифрования, длинна ровно 16 символов: ";
    std::getline(std::cin, msg); 
    std::cout << "Введите ключ для шифрования, длинна ровно 8 символов: ";
    std::getline(std::cin, key); 
}

void convertUserData(std::vector <int>& msgArr, std::vector <int> &keyArr, const std::string& msg, const std::string& key) {
    // Получение данных ключа
    keyArr.push_back(get10FromChars(key[3], key[2], key[1], key[0])); // Младшая часть
    keyArr.push_back(get10FromChars(key[7], key[6], key[5], key[4])); // Старшая часть
    // Получение сообщения (X1 - первые 4 байта сообщения)
    msgArr.push_back(get10FromChars(msg[3], msg[2], msg[1], msg[0])); // X1
    msgArr.push_back(get10FromChars(msg[7], msg[6], msg[5], msg[4])); // X2
    msgArr.push_back(get10FromChars(msg[11], msg[10], msg[9], msg[8])); // X3
    msgArr.push_back(get10FromChars(msg[15], msg[14], msg[13], msg[12])); // X4
}

void getStrFromArr(const std::vector <int>& arr, std::string& str) {
    str.resize(arr.size() * 4 * 8 + 1); unsigned long int k = 0;
    for (unsigned long int i = 0; i < arr.size(); ++i) {
        str[k] = static_cast<char>(arr[i] & 255); ++k;
        str[k] = static_cast<char>((arr[i] >> 8) & 255); ++k;
        str[k] = static_cast<char>((arr[i] >> 16) & 255); ++k;
        str[k] = static_cast<char>((arr[i] >> 24) & 255); ++k;
    }

    str[arr.size() * 4 * 8] = '\0';
}

void shiftByOneEl(std::vector<int>& msgArr) {
    int tmp = msgArr[0]; long int i = 0;
    for (; i < msgArr.size() - 1; ++i)
        msgArr[i] = msgArr[i + 1];
    msgArr[i] = tmp;
}

void shiftByOneEldec(std::vector<int>& msgArr) {
    int tmp = msgArr[msgArr.size() - 1]; 
    for (int i = msgArr.size() - 1; i > 0; --i)
        msgArr[i] = msgArr[i - 1];
    msgArr[0] = tmp;
}

void raoundFeistel(std::vector <int>& msgArr, std::vector <int> &key, int i) {
    msgArr[0] ^= generatingFunc(key, i);
    shiftByOneEl(msgArr);
}

void raoundFeisteldec(std::vector <int>& msgArr, std::vector <int>& key, int i) {
    shiftByOneEldec(msgArr);
    msgArr[0] ^= generatingFunc(key, i);
}

void Feistel(std::vector <int>& msgArr, std::vector <int> &key, const int &rounds) {
    for (int i = 0; i < rounds; ++i)
        raoundFeistel(msgArr, key, i);
}

void decryptionFeistel(std::vector <int>& msgArr, std::vector <int> &key, const int& rounds) {
    for (int i = rounds - 1; i >= 0; --i)
        raoundFeisteldec(msgArr, key, i); 
}
//// Выполняет арифметический сдвиг двух байтов
//void arithSubR(char& l, char& r, int i);
//
//void arithSubR(char& l, char& r, int i) {
//    if (i == 0)
//        return; // нечего сдвигать
//
//    if (((l) >> 7 & 1) == 1) { // Отрицательное число
//        if (i == 8) {// Старший бит 255, а младший равен значению старшего до сдвига
//            r = (r & 0) | l;
//            l = (l & 0) | 255;
//            return;
//        }
//
//        if (i >= 16) {// Все биты заполнены битом знака
//            l = (l & 0) | 255;
//            r = (r & 0) | 255;
//            return;
//        }
//
//        int _i = i;
//        char mask = static_cast <char>(0);
//
//        if ((i > 8) && (i < 16)) {// Найти маску для младшего байта
//            _i %= 8; int bCount = 7;
//            while (_i > 0) {
//                mask = mask | (1 << bCount);
//                --_i; --bCount;
//            }
//            _i %= 8;
//            r = (l >> _i) | mask;
//            l |= 255;
//            return;
//        }
//        // i <= 7
//        while (_i > 0) {// Получаем маску для обнуления первых бит старшего байта
//            mask = mask | (1 << _i);
//            --_i;
//        }
//        r = ((l & (mask >> 1)) << (8 - i)) | ((r & (~mask)) >> i);
//        l = (l >> i) | mask;
//        return;
//    }
//
//    // число положительное
//    if (i == 8) {
//        r = l;
//        l = static_cast <char>(0);
//        return;
//    }
//
//    if (i >= 16) {
//        l = static_cast <char>(0);
//        r = static_cast <char>(0);
//        return;
//    }
//
//    int _i = i; char mask = static_cast <char>(0);
//    if ((i > 8) && (i < 16)) {
//        _i %= 8;
//        r = l >> _i;
//        l &= 0;
//        return;
//    }
//    // i <= 7
//    while (_i > 0) {// Получаем маску для обнуления первых бит старшего байта
//        mask = mask | (1 << _i);
//        --_i;
//    }
//
//    mask >>= 1;
//    r = ((r & (~mask)) >> i) | ((l & mask) << (8 - i));
//    l >>= i;
//}