#include <iostream>
#include <locale>

class PointElepticCurve {
    int x = -1, y = -1;
public:
    PointElepticCurve();
    PointElepticCurve(const int &vl);
    PointElepticCurve(const int& vl1, const int& vl2);
    PointElepticCurve(const PointElepticCurve& _point);
    ~PointElepticCurve();
    
    bool operator ==(const PointElepticCurve& _point) {
        return((this->x == _point.x) && (this->y == _point.y));
    }

    void show();
    void getXY(int& _x, int& _y);
    int getInverseModulo(const int& value, const int& p);
    void addPoint(const PointElepticCurve& point, const int& p);
    void in2P(const int& p, const int& a); // Удвоить точку
    // Алгоритм скалярного умножения числа k на точку реализовать
    void mulK(const int& _k, const int& p, const int& a); //
};

// Возвращает true, если число простое
bool simpleNumber(int number);
// Возвращает true, если ключи совпали (k1 * k2 * G(x, y) = k2 * k1 * G(x, y)) 
bool isSuccsessDiffy(const int& p, const int& a, const int& k1, const int& k2, const PointElepticCurve& G);
// Возвращает true, если параметры элиптической кривой действительно задают элептическую кривую, иначе false 
bool isElepticalCurve(const int &a, const int& b, const int& p);
// Возвращает true, если для варианта 5 алгоритм Диффи-Хеллмана завершился успехом, иначе false
bool task5();

int main()
{
    setlocale(LC_ALL, "rus");
    (task5()) ?
        std::cout << "Алгоритм Диффи-Хеллмана завершен успешно!" << "\n\n" :
        std::cout << "Алгоритм Диффи-Хеллмана закончился неудачей!"<< "\n\n";
    std::cout << "Hello World!\n";
    system("pause");
    return 0;
}

PointElepticCurve::PointElepticCurve()
{
}

PointElepticCurve::PointElepticCurve(const int& vl)
{
    this->x = this->y = vl;
}

PointElepticCurve::PointElepticCurve(const int& vl1, const int& vl2)
{
    this->x = vl1; this->y = vl2;
}

PointElepticCurve::PointElepticCurve(const PointElepticCurve& _point)
{
    this->x = _point.x; this->y = _point.y;
}

PointElepticCurve::~PointElepticCurve()
{
}

void PointElepticCurve::show()
{
    std::cout << "Координаты точки - (x: " << this->x << "; y: " << this->y << ")\n\n";
}

void PointElepticCurve::getXY(int& _x, int& _y)
{
    _x = this->x; _y = this->y;
}

int PointElepticCurve::getInverseModulo(const int& value, const int& p)
{
    for (int i = 0; i < p; ++i)
        if (((i * value) % p) == 1)
            return i;
    return 0;
}

void PointElepticCurve::addPoint(const PointElepticCurve& point, const int& p)
{
    try {
        if (this->x == point.x) {// Конечная точка на той же прямой?
            if ((-this->y) == point.y) {// Должна получится бесконечно удаленная
                this->x = 0; this->y = 0;
            }else throw "Деление на 0\0";
        }
        else    
        if ((this->x == 0) && (this->y == 0)) {// Бесконечно удаленная
            this->x = point.x; this->y = point.y;
        }
        else {
            int tmpX = this->x, lyambda;
            lyambda = static_cast<int>((point.y - this->y) * getInverseModulo((this->x - point.x), p)) % p;
            this->x = (lyambda * lyambda - this->x - point.x) % p;
            this->y = (lyambda * (tmpX - point.x) - this->y) % p;

        }
    }
    catch (const char e[]) {
        std::cout << "Ошибка при сложении точек!!! " << e << "\n\n";
    }
}

void PointElepticCurve::in2P(const int& p, const int& a)
{
    if (this->y != 0) {
        int tmpX = this->x, lyambda = static_cast <int>((3 * this->x * this->x + a) * getInverseModulo((2 * this->y), p)) % p;
        this->x = static_cast<int>(lyambda * lyambda - 2 * tmpX) % p;
        this->y = (lyambda * (tmpX - this->x) - this->y) % p;
    }
}

void PointElepticCurve::mulK(const int& _k, const int& p, const int &a)
{
    PointElepticCurve Q(0, 0);
    int tmpK = _k;
    do {
        Q.in2P(p, a); // Удвоили точку
        if ((tmpK & 1) == 1)
            Q.addPoint(*this, p);
        tmpK >>= 1;
    } while (tmpK > 0);

    this->x = Q.x; this->y = Q.y; 
}

bool simpleNumber(int number) {
    //number = abs(number);
    if (number <= 1)
        return false;
    if (number == 2)
        return true;
    int max = static_cast <int> (sqrt(number)) + 2;

    for (int i = 2; i < max; ++i)
        if ((number % i) == 0)
            return false;
    return true;
}

bool isSuccsessDiffy(const int &p, const int &a, const int& k1, const int& k2, const PointElepticCurve& G)
{
    PointElepticCurve Gu1(G), Gu2(G);
    Gu1.mulK(k2, p, a); Gu2.mulK(k1, p, a);
    Gu1.mulK(k1, p, a); Gu2.mulK(k2, p, a); //Gu1 = k1 * k2 * G(x, y); Gu2 = k2 * k1 * G(x, y);
    std::cout << "K первого юзера: "; Gu1.show();
    std::cout << "K второго юзера: "; Gu2.show();
    return (Gu1 == Gu2);
}

bool isElepticalCurve(const int& a, const int& b, const int& p)
{
    return ((static_cast<int>(4 * pow(a, 3) + 27 * pow(b, 2)) % p) != 0);
}

bool task5()
{
    /*Подготовка исходных данных*/
    int a = 3, b = 5, p = 17, k1 = 8, k2 = 11; 
    PointElepticCurve G(1, 3);
    std::cout << "Исходные данные:\na: " << a << "; b: " << b << "\np: " <<
        p << "; k1: " << k1 << "; k2: " << k2 << "\n";
    G.show();
    //----------------------------
    try {
        if (!isElepticalCurve(a, b, p))
            throw "Элиптическая кривая не корректна\0";
        if (isSuccsessDiffy(p, a, k1, k2, G)) 
            return true;
        return false;
    }
    catch (const char e[]) {
        std::cout << "Ошибка!!! " << e << "\n\n";
        return false;
    }
}
