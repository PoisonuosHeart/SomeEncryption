#include <iostream>
#include <string>
#include <vector>
#include <locale>

typedef struct noneSecret nSecret;
typedef struct openKey oKey;
typedef struct secretKey sKey;
typedef struct messageSignature msgSign;

struct messageSignature {
    unsigned long int r, s;
};

struct noneSecret {
    unsigned long int p, q, a; // Не секретные данные для абонентов
};

struct openKey {
    unsigned  long int y;
};

struct secretKey {
    unsigned  long int x;
};

//bool getPQ(unsigned long int& p, unsigned long int& q);
bool getPQGost(unsigned long int& p, unsigned long int& q);
// Возвращает true, если число простое
bool simpleNumber(unsigned long int number);
int get10FrombyteWithRange(const char& c, int start, int end);  // Получить 10-е число из байта
short int get10FrombyteWithRange(const char& c, short int start, short int end);
// Считывает в 10 формате число из 16 бит
short int get16ten(const char &high, const char &low);
// Возвращает 16-битный хеш сообщения msg
short int hashFunction(const std::string& msg);
// Приготовить данные для ключей
bool prepareKeys(nSecret& ns, oKey& ok, sKey& sk);
// Получает подпись сообщения в ms
bool getMsgSign(msgSign& ms, const sKey& sk, const nSecret& ns, std::string& msg);
// Возвращает true, если подпись корректна, иначе false
bool isCorrectSign(const msgSign &ms, const nSecret &ns, const oKey &ok, const std::string& msg);
// Выводит в консоль данные ключей
void consoleOutput(const nSecret& ns, const oKey& ok, const sKey& sk);
// Изменить значение подписи
void changeSign(msgSign& ms);
// Удобное меню для проверки работы программы
void menu();

int main()
{
    setlocale(LC_ALL, "rus");
    
    menu();

    std::cout << "Hello World!\n";
    system("pause");
    return 0;
}

void menu() {
    int contin; std::string msg;
    std::cout << "Если хотите подписать сообщение, введите число > 1\nДля проверки подписи введите 0\nДля изменения подписи введите 1\nДля выхода введите отрицательное число\nВаш выбор: ";
    std::cin >> contin; std::cin.ignore(32767, '\n');

    nSecret ns; oKey ok; sKey sk; msgSign ms;

    while (contin >= 0) {

        if (contin > 1) { // Подпись сообщения
            std::cout << "\nВведите сообщение: ";
            std::getline(std::cin, msg);

            std::cout << "Ваше сообщение: " << msg << "\n\n";
            if (prepareKeys(ns, ok, sk))
                consoleOutput(ns, ok, sk); // Вывод на экран сгенерированных ключей
            else std::cout << "Ошибка, ключи не сгенерированы\n\n";
            if (getMsgSign(ms, sk, ns, msg))
                std::cout << "Сообщение успешно подписано\nr: " << ms.r << "\ns: " << ms.s << "\n\n";
            else std::cout << "Подпись не сгенерирована.\n";
        }
        else if (contin == 1) { // Внеси изменения в подпись
            changeSign(ms);
        }
        else { // Проверить подпись
            std::cout << "Проверка подписи сообщения: " << msg << "\n\n";
            consoleOutput(ns, ok, sk);
            (isCorrectSign(ms, ns, ok, msg)) ? std::cout << "Подпись верна\n" : std::cout << "Не верная подпись\n";
        }

        std::cout << "Если хотите подписать сообщение, введите число > 1\nДля проверки подписи введите 0\nДля изменения подписи введите 1\nДля выхода введите отрицательное число\nВаш выбор: ";
        std::cin >> contin; std::cin.ignore(32767, '\n');
    }
}

void changeSign(msgSign& ms) {
    std::cout << "r: "; std::cin >> ms.r;
    std::cout << "s: "; std::cin >> ms.s;
}

void consoleOutput(const nSecret& ns, const oKey& ok, const sKey& sk) {
        std::cout << "Данные успешно сгенерированы\np: " << ns.p <<
        "\nq: " << ns.q << "\na: " << ns.a <<
        "\ny: " << ok.y << "\nx: " << sk.x <<
        "\n\n";
}

bool isCorrectSign(const msgSign& ms, const nSecret& ns, const oKey &ok, const std::string& msg) {
    try {
        if (((ms.r > 0) && (ms.r < ns.q)) && ((ms.s > 0) && (ms.s < ns.q))) {
            /*------Хеш-----*/
            short int hashValue;
            ((hashFunction(msg) % ns.q) == 0) ? hashValue = 1 : hashValue = 2;
            unsigned long int v =  (fmodl(pow(hashValue, ns.q - 2), ns.q)),
                z1 = (ms.s * v) % ns.q, z2 = ((ns.q - ms.r) * v) % ns.q, 
                u = (static_cast<unsigned long int>(static_cast<unsigned long int>(pow(ns.a, z1))
                * static_cast<unsigned long int>(pow(ok.y, z2))) % ns.p) % ns.q;

            return (u == ms.r);
        }
        else return false;
    }
    catch (const char e[]) {
        std::cout << "Ошибка при проверке подписи!!! " << e << "\n\n";
        return false;
    }
}

bool getMsgSign(msgSign& ms, const sKey& sk, const nSecret& ns, std::string& msg) {
    try {
        if (msg.empty())
            throw "Пустое сообщение\0";
        short int hashValue; // Значение хеша
        ((hashFunction(msg) % ns.q) == 0) ? hashValue = 1 : hashValue = 2;
        srand(time(NULL));
        unsigned long int k;
        bool wrongK = true;
        do {
            k = ((rand() % ns.q) % 10) + 1; // Выбирается k из [1; q]
            ms.r = (static_cast<unsigned long int> (fmodl(pow(ns.a, k), ns.p))) % ns.q;

            if (ms.r == 0)
                continue; // Пропустить все что дальше

            ms.s = ((sk.x * ms.r) + (hashValue * k)) % ns.q;

            if (ms.s != 0)
                wrongK = false;

        } while (wrongK);

        return true;
    }
    catch (const char e[]) {
        std::cout << "Ошибка!!! При генерации подписи возникла ошибка! " << e << "\n\n";
        return false;
    }
    return false;
}

bool prepareKeys(nSecret& ns, oKey& ok, sKey& sk) {
    try {
        unsigned long int i = 1; bool isPQexist, k = true;
        ns.p = 23;  isPQexist = getPQGost(ns.p, ns.q);
        
        while ((isPQexist) && k) {
            
            for (i = 2; k && (i < (ns.p - 1)); ++i)
                if ((static_cast<unsigned long int>(pow(i, ns.q)) % ns.p) == 1) {
                    ns.a = i; k = false; 
                }

            if (k)
                isPQexist = getPQGost(ns.p, ns.q);
        }

        if (!isPQexist)
            throw "Не удалось найти а для p и q";

        // Успешно получены p, q, a в структуре ns
        srand(time(NULL));
        sk.x = (rand() % (ns.q - 1) % 2) + 1; // Получаем закрытый ключ (тут mod 2 из-за того, что у меня unsigned long int и в случае, когда идет проверка подписи, число не влезет в диапазон (я про открытый ключ), обнаружено при z1 и z2 = 10)
        ok.y = static_cast<unsigned long long int>(pow(ns.a, sk.x)) % ns.p;

        return true;
    }
    catch (const char e[]) {
        std::cout << "Ошибка! Данные не сгенерированы! " << e << "\n\n";
        return false;
    }
}

short int hashFunction(const std::string& msg) {
    try {
        
        if (msg.length() <= 0)
            throw "Длинна сообщения меньше допустимой\0";
        std::vector <short int> _blocks; bool evenSize;
        ((msg.length() % 2) == 0) ? evenSize = true : evenSize = false;
        unsigned long int max = msg.length(); 
        (evenSize) ? max : --max;
        for (unsigned long int i = 0; i < max; i += 2)
            _blocks.push_back(get16ten(msg[i], msg[i + 1]));

        if (!evenSize) {    // Если длинна нечетной была
            _blocks.push_back(get16ten(msg[msg.length() - 1], static_cast<char>(0)));
        }

        // Операция хеширования над 16-битными блоками сообщения сохраненного в _blocks
        for (unsigned int i = 1; i < _blocks.size(); ++i)
            _blocks[0] = (_blocks[0] ^ _blocks[i]) >> 1;
        return _blocks[0];
    }
    catch (const char e[]) {
        std::cout << "Ошибка при получении последовательности блоков длинной 16 бит! " << e << "\n\n";
        return -1;
    }
    return -1;
}

int get10FrombyteWithRange(const char& c, int start, int end) {
    int res = 0, k = 0;
    for (int i = start; i < end; ++i, ++k)
        if (((c >> k) & 1) == 1) {
            res += static_cast <unsigned int>(pow(2, i));
        }
    return res;
}

bool getPQGost(unsigned long int& p, unsigned long int& q) {
    try {
        // Находим простое число p
        unsigned long int max = static_cast<unsigned long int>(pow(2, sizeof(unsigned long int) * 8)), i;
        bool k = true;
        
        for(i = p; (k) && (i < max); ++i)
            if (simpleNumber(i)) {
                p = i; k = false;
            }

        if (k)
            throw "Не удалось найти p\0";

        k = true;

        for(i = 3; k && (i < p); ++i)
            if ((simpleNumber(i)) && (((p - 1) % i) == 0)) {
                q = i; k = false;
            }

        if (k)
            throw "Не удалось найти q\0";

        return true;
    }
    catch (const char e[]) {
        std::cout << "Ошибка получения данных!! " << e << "\n\n";
        return false;
    }
}

bool simpleNumber(unsigned long int number) {
    //number = abs(number);
    if (number <= 1)
        return false;
    if (number == 2)
        return true;
    unsigned long int max = static_cast <unsigned long int> (sqrt(number)) + 2;
    
    for (unsigned long int i = 2; i < max; ++i)
        if ((number % i) == 0)
            return false;
    return true;
}

short int get16ten(const char& high, const char& low) {
    short int res = 0;

    res += get10FrombyteWithRange(low, 0, 8);
    res += get10FrombyteWithRange(high, 8, 16);

    return res;
}

short int get10FrombyteWithRange(const char& c, short int start, short int end) {
    short int res = 0, k = 0;
    for (short int i = start; i < end; ++i, ++k)
        if (((c >> k) & 1) == 1) {
            res += static_cast <short int>(pow(2, i));
        }
    return res;
}

//int read_16(const char& b0, const char& b1); // 2 из 4 бйтов int хранят число из 2 байтов char
//int read_16(const char& b0, const char& b1) {
//
//    int res = 0;
//
//    res += get10FrombyteWithRange(b1, 0, 8);
//    res += get10FrombyteWithRange(b0, 8, 16);
//
//    return res;
//}
//bool getPQ(unsigned long int& p, unsigned long int& q) {
//    try {
//        // находим простое число q
//        unsigned long int max = static_cast<unsigned long int>(pow(2, sizeof(unsigned long int) * 8)), i;
//        bool k = true;
//
//        for (i = q + 1; k && (i < max); ++i)  // от 256 т.к. значение функции 0 < h < q
//            if (simpleNumber(i)) {
//                q = i;  k = false;
//            }
//        
//        if (k) {
//
//            throw "Не удалось сгенерировать q\0";
//        }
//
//
//        k = true;
//
//        for (i = 1; k && (i < max); ++i) {
//            if (((i * q + 1) < max) && (simpleNumber(i * q + 1))) {
//                p = i * q + 1; k = false;
//            }
//        }
//
//        if (k) {
// 
//            throw "Не удалось сгенерировать p\0";
//        }
//        return true;
//    }
//    catch (const char e[]) {
//        std::cout << "Ошибка получения данных!! " << e << "\n\n";
//        return false;
//    }
//}