#include <iostream>
#include <string>
#include <vector>

typedef struct oKey openKey;
typedef struct sKey secretKey;
typedef struct uKey userKey;

struct oKey {
    int e, fn;
};

struct sKey {
   unsigned long int d, n; int p, q;
};

struct uKey {
    openKey openUserKey; secretKey secretUserKey;
};

// Возвращает позицию a в моссиве vect, иначе -1
int inArr(const std::vector <int>& vect, int a);
// Разбивает сообщение на блоки
void prepareBlocks(std::vector<int>& blocks, const std::string& msg);
// Нахождение НОД
int gcd(int a, int b);
// Возвращает true, если числа a и b взаимно просты
bool isCoprimeNumbers(int a, int b);
// Возвращает true, если открытый ключ open успешно создан, иначе false
bool createOpenKey(openKey& open, const secretKey& secret);
// Возвращает true, если ключи успешно созданы, иначе false
bool createKeys(int p, int q, userKey& userPass);
// В случае успешно зашифрованного текста msg в cryptMsg возвращает true, иначе false 
bool RSA(const std::vector <int> &blocks, const std::string& msg, std::vector<unsigned long int>& cryptMsg, const userKey &usrKey);
// В случае успешно расшифрованного текста cryptMsg в msg возвращает true, иначе false 
bool decryptRSA(const std::vector <int>& blocks, std::string& msg, const std::vector<unsigned long int>& cryptMsg, const userKey& usrKey);
// Выводит в консоль сообщение msg и вектор
void outputConsole(const char msg[], const std::vector <unsigned long int>& vect) {
    std::cout << msg << "\n";
    for (unsigned long int i = 0; i < vect.size(); ++i)
        std::cout << vect[i] << " ";
    std::cout << "\n\n";
}

int main()
{
    setlocale(LC_ALL, "Rus");
    userKey uPass;
    if (!createKeys(5, 7, uPass)) {
        std::cout << "Что-то не так\n";
    }
    else {
        /*std::cout << "Открытый ключ:\n" << "fn: " <<
            uPass.openUserKey.fn << "\ne: " << uPass.openUserKey.e << "\n\n";
        std::cout << "Закрытый ключ: " << "\nd: " << uPass.secretUserKey.d <<
            "\nn: " <<uPass.secretUserKey.n <<
            "\np: " << uPass.secretUserKey.p << "\nq: " << uPass.secretUserKey.q << "\n";*/
        std::string msg, decryptedMsg; std::vector <unsigned long int> encryptedMsg;
        std::vector<int>blocks;
        
        std::cout << "Введите сообщение для сокрытия: ";
        std::getline(std::cin, msg);
        prepareBlocks(blocks, msg);
        std::cout << "Будет зашифровано: " << msg << "\n";
        if (!RSA(blocks, msg, encryptedMsg, uPass)) {
            std::cout << "Что-то пошло не так...\n"; return 0;
        }
        outputConsole("Зашифрованное сообщение:\0", encryptedMsg);
        if (!decryptRSA(blocks,decryptedMsg, encryptedMsg, uPass)) {
            std::cout << "Что-то пошло не так...\n"; return 0;
        }
        std::cout << "Расшифрованное сообщение: " << decryptedMsg << "\n\n";
    }
    std::cout << "Hello World!\n";
    system("pause");
    return 0;
}

bool decryptRSA(const std::vector <int>& blocks, std::string& msg, const std::vector<unsigned long int>& cryptMsg, const userKey& usrKey) {
    try {
        if (cryptMsg.size() < 1)
            throw "Длинна расшифровываемого сообщения равна 0\0";
        msg.resize(cryptMsg.size() + 1, ' ');
        unsigned long long int pos;
        unsigned long int n = usrKey.secretUserKey.n, d = usrKey.openUserKey.e;
        for (unsigned int i = 0; i < cryptMsg.size(); ++i) {
            pos = static_cast<unsigned long long int>(pow(cryptMsg[i], d)) % n;
            if ((pos < 0) || (pos >= blocks.size())) {
                throw "Слишком большие p и q!!! Не хватает разрядности для шифрования! Позиция в массиве блоков выходит за пределы!!!\0";
            }
            msg[i] = static_cast<char>(blocks[pos] + 'a');
        }
    }
    catch (const char e[]) {
        std::cout << "Ошибка!!! " << e << "\n\n";
        return false;
    }
    return true;
}

bool RSA(const std::vector <int> &blocks, const std::string& msg, std::vector<unsigned long int>& cryptMsg, const userKey& usrKey) {
    try {
        int e = usrKey.secretUserKey.d; unsigned long int n = usrKey.secretUserKey.n;
        for (unsigned int i = 0; i < msg.size(); ++i) {
            cryptMsg.push_back(
                static_cast<unsigned long long int>(pow((inArr(blocks, static_cast<int>(msg[i] - 'a'))), e)) % n);
        }
        if (cryptMsg.size() < 1)
            throw "Длинна зашифрованного сообщения 0\0";
    }
    catch (char* e) {
        std::cout << "Ошибка!!! " << e << "\n\n";
        return false;
     }
    return true;
}

bool createKeys(int p, int q, userKey& userPass) {
    userPass.secretUserKey.p = p; userPass.secretUserKey.q = q;
    try {
        if (!createOpenKey(userPass.openUserKey, userPass.secretUserKey)) 
            throw "Невозможно создать открытый ключ\n"; 
        int e = userPass.openUserKey.e; unsigned long int max = static_cast<int>(pow(2, sizeof(unsigned long int) * 8)), i = 2;
        userPass.secretUserKey.n = p * q;
       
        for(; i < max; ++i)
            if (((e * i) % userPass.openUserKey.fn) == 1) {
                userPass.secretUserKey.d = i; break;
            }
    }
    catch (char* e) {
        std::cout << "Ошибка!!! Ключи не созданы!!!" << e << "\n";
        return false;
    }
    return true;
}

bool createOpenKey(openKey& open, const secretKey& secret) {
    try {
        int i, res = (secret.p - 1) * (secret.q - 1), max = static_cast<int>(pow(2, (sizeof(int) * 8) / 2));
        for(i = 2; i < max; ++i)
            if (gcd(res, i) == 1) {
                open.e = i; break;
            }

        if (i == max)
            throw "Не удалось найти e взаимно простое для (p - 1) xor (q - 1)\n";

        open.fn = (secret.p - 1) * (secret.q - 1);
    }
    catch (char* e) {
        std::cout << "Открытый ключ не может быть создан! " << e << "\n\n";
        return false;
    }
    return true;
}

int gcd(int a, int b) {
    if (a == b) {
        return a;
    }

    a = std::abs(a);
    if (a < b) {
        std::swap(a, b);
    }

    int mod = a % b;
    while (mod > 0) {
        a = b;
        b = mod;
        mod = a % b;
    }

    return b;
}

void prepareBlocks(std::vector<int>& blocks, const std::string& msg) {
    int k = 0;
    for (long int i = 0; i < msg.length(); ++i)
        if(inArr(blocks, static_cast<int>(msg[i] - 'a')) == -1)
            blocks.push_back(static_cast<int>(msg[i] - 'a'));
}

int inArr(const std::vector <int>& vect, int a) {
    for (unsigned long int i = 0; i < vect.size(); ++i)
        if (vect[i] == a)
            return i;
    return -1;
}

bool isCoprimeNumbers(int a, int b) {
    if (gcd(a, b) == 1)
        return true;
    return false;
}
