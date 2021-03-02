#include <iostream>
#include <locale>

typedef short int (*SecretFunc)(const short int&); // Тип для функций  преобразования

class User {
    int pass = -1;
    
public:
    User();
    ~User();
    bool checkPass(const int& _pass);
    bool reg();
};

short int secretOne(const short int &value);
short int secretTwo(const short int& value);
short int secretThree(const short int& value);

const int numbSecretFunc = 3;

// Возвращает true, если рукопожатие успешно, иначе false
bool handshake(const SecretFunc* secretArr);
// menu для удобной проверки работоспособности программы
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
    SecretFunc* secretArr = new SecretFunc[numbSecretFunc];

    secretArr[0] = secretOne;
    secretArr[1] = secretTwo;
    secretArr[2] = secretThree;

    int contin;

    std::cout << "Если хотите зарегистрироваться, введите число > 0\nДля авторизации введите 0\nДля выхода введите отрицательное число\nВаш выбор: ";
    std::cin >> contin; std::cin.ignore(32767, '\n');

    User u1;

    while (contin >= 0) {

        if (contin == 0) {// Авторизация
            int userPass;
   
            std::cout << "Введите пароль: ";
            std::cin >> userPass;
            if (!u1.checkPass(userPass))
                std::cout << "Пароль не верен\n\n";
            else
                (handshake(secretArr)) ?
                    std::cout << "Пользователь авторизован\n\n" :
                    std::cout << "Пользователь не авторизован\n\n";
            
        }
        else {// Регистрация
            u1.reg();
        }

        std::cout << "Если хотите зарегистрироваться, введите число > 0\nДля авторизации введите 0\nДля выхода введите отрицательное число\nВаш выбор: ";
        std::cin >> contin; std::cin.ignore(32767, '\n');
    }

    delete[] secretArr;
}

bool handshake(const SecretFunc* secretArr) {
    srand(static_cast <unsigned int> (time(NULL)));
    short int randValue = rand() % static_cast <unsigned short> (pow(2, sizeof(short int) * 8) / 2);
    int iFunc = rand() % 3; // Номер секретного преобразования

    std::cout << "Введите число " << secretArr[iFunc](randValue) << ": ";
    short int userRandValue;

    std::cin >> userRandValue;

    return (secretArr[iFunc](userRandValue) == randValue);
}

short int secretOne(const short int& value) {
    return (value ^ 10);
}

short int secretTwo(const short int& value) {
    return (value ^ 9);
}

short int secretThree(const short int& value) {
    return (value ^ 99);
}

bool User :: reg() {
    std::cout << "Введите пароль: ";
    std::cin >> this->pass;
    return true;
}

bool User :: checkPass(const int& _pass) {
    return (_pass == this->pass);
}

User::User() {

}

User::~User() {

}
