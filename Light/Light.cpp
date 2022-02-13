// Шимкин Р.Р. ПММ 2 курс 62 группа
// реализовать источники света, предусмотреть освещение зеркальных поверхностей
#include <iostream>
#include <string>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include <vector>

using namespace std;

typedef vector<pair<int, int>> VECT;

// абстрактный класс источника света:
class LightSource {
protected:
    int x, y; // координаты центра
    int height, width; // размеры поля
    VECT barr, darr; // массивы пятен яркого и тусклого света
public:
    // функция сдвига пятна:
    bool move(char g) {
        bool changed = false;
        switch (g) {
        case 'w':
        case 'W':
            if (x > 0) x--;
            changed = true;
            break;
        case 's':
        case 'S':
            if (x < height - 1) x++;
            changed = true;
            break;
        case 'd':
        case 'D':
            if (y < width - 1) y++;
            changed = true;
            break;
        case 'a':
        case 'A':
            if (y > 0) y--;
            changed = true;
            break;
        }
        return changed;
    }
    virtual VECT getBright() {
        return barr;
    }
    virtual VECT getDim() {
        return darr;
    }
    // вирт-ая ф-ия обнов-я сост-я
    virtual void update(char x) = 0 {

    }
};

// абстрактный класс Электрического источника 
class ElectricalLightSource : public LightSource {
protected:
    bool on; // поле вкл/выкл 
    int rad; // поле радиуса пятна света
public:
    VECT getBright() {
        VECT temp;
        if (on) temp = barr;
        return temp;
    }
    VECT getDim() {
        VECT temp;
        if (on) temp = darr;
        return temp;
    }
    // виртуальная функция задания пятна по координатам центра и радиуса
    virtual void setLight(bool bright = false) = 0 {}
    virtual void update(char x) = 0 {}
    // виртуальная функция выкл/вкл
    virtual bool onOff(char x) = 0 {}
};

// класс лампы
class Lamp : public ElectricalLightSource {
private:
    bool plugged; // поле розетки
public:
    // полиморфный конструктор (будет испол-ся в Фонарике и Лазере)
    Lamp(int h, int w, int r = 8) {
        plugged = on = false;
        rad = r;
        height = h;
        width = w;
        x = h / 2;
        y = w / 2;
        setLight();
    }
    void setLight(bool bright = false) {
        barr.clear();
        darr.clear();
        // циклы для формирования коорд-т пятен света
        for (int i = x - rad; i <= x + rad; i++) {
            for (int k = y - rad; k <= y + rad; k++) {
                int temp = abs(x - i) * abs(x - i) + abs(y - k) * abs(y - k);
                // точки внутри сферы яркие:
                if (temp < rad * rad) barr.push_back(make_pair(i, k));
                // точки на границе тусклые (если bright = false)
                else if (temp >= rad * rad && temp < (rad + 1) * (rad + 1)) {
                    if (!bright) darr.push_back(make_pair(i, k));
                    else barr.push_back(make_pair(i, k));
                }
            }
        }
    }
    virtual bool onOff(char x = 'f') {
        if (x == 'f' || x == 'F') {
            if (!on && plugged) on = true;
            else on = false;
            return true;
        }
        return false;
    }
    // функция включение в розетку:
    bool plug(char x) {
        if (x == 'g' || x == 'G') {
            if (plugged) {
                plugged = false;
                on = false;
            }
            else plugged = true;
            return true;
        }
        return false;
    }
    virtual void update(char x) {
        if (plugged) cout << "plugged\n";
        else cout << "unplugged\n";
        bool changed = false;
        if (!onOff(x) && !plug(x)) changed = move(x);
        if (changed) setLight();
    }
};

// класс Фонарика
class Flashlight : public Lamp {
protected:
    float battery;
    float koef; // коэф-т скорости разряда батареи
public:
    // последний параметр констр-ра будет исп-ся в Лазере
    Flashlight(int h, int w, int r = 6): Lamp(h, w, r), battery (1) {
        if (r == 6) koef = 0.0005;
        else koef = 0.002;
    }
    bool onOff(char x = 'f') {
        if (x == 'f' || x == 'F') {
            if (!on && battery >= 0.05) on = true;
            else on = false;
            return true;
        }
        return false;
    }
    void update(char x) {
        cout << "battery: " << battery << endl;
        if (on) battery -= koef;
        if (battery <= 0.05) onOff();
        bool changed = false;
        if (!onOff(x)) changed = move(x);
        if (changed) setLight();
    }
};

// класс Лазера
class Laser : public Flashlight {
public:
    Laser(int h, int w): Flashlight(h, w, 1) {}
};

// класс свечи
class Candle : protected LightSource {
protected:
    bool isLitUp; // горит или нет
    float state; // насколько сгорела
public:
    Candle(int h, int w) {
        height = h;
        width = w;
        x = h / 2;
        y = w / 2;
        isLitUp = false;
        state = 1;
        setLight();
    }
    bool onOff(char x = 'f') {
        if (x == 'f' || x == 'F') {
            if (!isLitUp && state >= 0.1) isLitUp = true;
            else isLitUp = false;
            return true;
        }
        return false;
    }
    VECT getBright() {
        VECT temp;
        if (isLitUp) temp = barr;
        return temp;
    }
    VECT getDim() {
        VECT temp;
        if (isLitUp) temp = darr;
        return temp;
    }
    virtual void setLight() {
        barr.clear();
        darr.clear();
        for (int i = x - 2; i <= x + 2; i++) {
            darr.push_back(make_pair(i, y - 2));
            darr.push_back(make_pair(i, y + 2));
            for (int k = y - 1; k <= y + 1; k++) barr.push_back(make_pair(i, k));
        }
        for (int i = y - 1; i <= y + 1; i++) {
            darr.push_back(make_pair(x - 3, i));
            darr.push_back(make_pair(x + 3, i));
        }
    }
    virtual void update(char x) {
        cout << "state: " << state << endl;
        if (isLitUp) state -= 0.0005;
        if (state <= 0.1) onOff();
        bool changed = false;
        if (!onOff(x)) changed = move(x);
        if (changed) setLight();
    }
};

// класс Спички
class Match : public Candle {
public:
    Match(int h, int w): Candle(h, w) {}
    void setLight() {
        barr.clear();
        darr.clear();
        for (int i = x - 2; i <= x + 2; i++) {
            if (i >= 0 && i < height) {
                if (y > 0) darr.push_back(make_pair(i, y - 1));
                darr.push_back(make_pair(i, y));
                if (y < width - 1) darr.push_back(make_pair(i, y + 1));
            }
        }
        barr.push_back(make_pair(x, y));
        if (x < height - 1) barr.push_back(make_pair(x + 1, y));
        if (x > 0 )barr.push_back(make_pair(x - 1, y));

    }
    void update(char x) {
        cout << "state: " << state << endl;
        if (isLitUp) state -= 0.02;
        if (state <= 0.1) onOff();
        bool changed = false;
        if (!onOff(x)) changed = move(x);
        if (changed) setLight();
    }
};

// класс Блока карты
class Block {
    char bright, dim;
    char cur;
public:
    Block() {
        cur = ' ';
    }
    void hide() {
        cur = ' ';
    }
    // блок - обычная поверхность
    void setSurface() {
        bright = 'O';
        dim = 'o';
    }
    // блок - зеркальная пов-ть
    void setMirror() {
        bright = 'X';
        dim = 'x';
    }
    void makeBright() {
        cur = bright;
    }
    void makeDim() {
        cur = dim;
    }
    void show() {
        cout << cur << " ";
    }
    bool isMirror() {
        return bright == 'X';
    }
};

// класс основного поля
class Field {
    Block** blocks; // матрица блоков
    int height, width;
public:
    Field(char** matr, int h, int w) {
        height = h;
        width = w;
        blocks = new Block * [height];
        // считывание карты из файла
        for (int i = 0; i < height; i++) {
            blocks[i] = new Block[width];
            for (int k = 0; k < width; k++)
                if (matr[i][k] == 's') blocks[i][k].setSurface();
                else blocks[i][k].setMirror();
        }
    }
    bool check(int x, int y) {
        return x < height && x >= 0 && y < width && y >= 0;
    }
    // функция отображения пятен света, переданных парам-ми:
    void show(VECT &br, VECT &dm) {
        VECT temp;
        for (int i = 0; i < dm.size(); i++) {
            int a = dm[i].first;
            int b = dm[i].second;
            // бОльшее пятно на зеркальной поверхности через массив temp:
            if (check(a, b) && blocks[a][b].isMirror()) {
                if (check(a - 1, b) && blocks[a - 1][b].isMirror()) temp.push_back(make_pair(a - 1, b));
                if (check(a + 1, b) && blocks[a + 1][b].isMirror()) temp.push_back(make_pair(a + 1, b));
                if (check(a, b - 1) && blocks[a][b - 1].isMirror()) temp.push_back(make_pair(a, b - 1));
                if (check(a, b + 1) && blocks[a][b + 1].isMirror()) temp.push_back(make_pair(a, b + 1));
                if (check(a - 1, b - 1) && blocks[a - 1][b - 1].isMirror()) temp.push_back(make_pair(a - 1, b - 1));
                if (check(a + 1, b - 1) && blocks[a + 1][b - 1].isMirror()) temp.push_back(make_pair(a + 1, b - 1));
                if (check(a + 1, b + 1) && blocks[a + 1][b + 1].isMirror()) temp.push_back(make_pair(a + 1, b + 1));
                if (check(a - 1, b + 1) && blocks[a - 1][b + 1].isMirror()) temp.push_back(make_pair(a - 1, b + 1));
            }
        }
        for (auto x : br) {
            int a = x.first;
            int b = x.second;
            // бОльшее пятно на зеркальной поверхности через массив temp:
            if (check(a, b) && blocks[a][b].isMirror()) {
                if (check(a - 1, b) && blocks[a - 1][b].isMirror()) temp.push_back(make_pair(a - 1, b));
                if (check(a + 1, b) && blocks[a + 1][b].isMirror()) temp.push_back(make_pair(a + 1, b));
                if (check(a, b - 1) && blocks[a][b - 1].isMirror()) temp.push_back(make_pair(a, b - 1));
                if (check(a, b + 1) && blocks[a][b + 1].isMirror()) temp.push_back(make_pair(a, b + 1));
                if (check(a - 1, b - 1) && blocks[a - 1][b - 1].isMirror()) temp.push_back(make_pair(a - 1, b - 1));
                if (check(a + 1, b - 1) && blocks[a + 1][b - 1].isMirror()) temp.push_back(make_pair(a + 1, b - 1));
                if (check(a + 1, b + 1) && blocks[a + 1][b + 1].isMirror()) temp.push_back(make_pair(a + 1, b + 1));
                if (check(a - 1, b + 1) && blocks[a - 1][b + 1].isMirror()) temp.push_back(make_pair(a - 1, b + 1));
            }
        }
        for (int i = 0; i < temp.size(); i++) blocks[temp[i].first][temp[i].second].makeDim();
        // настройка света блоков внутри пятен:
        for (int i = 0; i < dm.size(); i++) {
            int a = dm[i].first;
            int b = dm[i].second;
            if (check(a,b)) blocks[a][b].makeDim();
        }
        for (int i = 0; i < br.size(); i++) {
            int a = br[i].first;
            int b = br[i].second;
            if (check(a, b)) blocks[a][b].makeBright();
        }
        // печать границы поля
        for (int i = 0; i < 2 * width; i++) cout << "_";
        cout << endl;
        // печать блоков
        for (int i = 0; i < height; i++) {
            for (int k = 0; k < width; k++) blocks[i][k].show();
            cout << '|';
            cout << endl;
        }
        for (int i = 0; i < 2 * width; i++) cout << "-";
        // обнуление света на блоках
        for (int i = 0; i < temp.size(); i++) 
            if (check(temp[i].first, temp[i].second)) blocks[temp[i].first][temp[i].second].hide();
        for (int i = 0; i < br.size(); i++) 
            if (check(br[i].first, br[i].second)) blocks[br[i].first][br[i].second].hide();
        for (int i = 0; i < dm.size(); i++) 
            if (check(dm[i].first, dm[i].second))blocks[dm[i].first][dm[i].second].hide();
    }
};

// функция считывания из файла символьной матрицы
char** BuildMatrix(int &h, int &w) {
    cout << "Enter the name of the map file: " << endl;
    string name;
    cin >> name;
    ifstream file(name.c_str());
    file >> h >> w;
    char** matr = new char* [h];
    for (int i = 0; i < h; i++) {
        matr[i] = new char[w];
        for (int k = 0; k < w; k++)
            file >> matr[i][k];
    }
    return matr;
}

// основная функция 
void func() {
    int height, width;
    char** matr = BuildMatrix(height, width);
    Field field(matr, height, width);
    VECT br, dm;
    char x = 'm';
    // основной цикл
    while (x != 'q' && x != 'Q') {
        // основное меню
        cout << "Choose the object:\n";
        cout << "Matches(1), Candle(2), Lamp(3), Flashlight(4), Laser(5)\n";
        cout << "or q/Q to quit\n";
        char x = _getch();
        switch (x) {
        case '1':
        {
            Match match(height, width);
            match.setLight();
            char m = 'm';
            while (m != 'q' && m != 'Q') {
                system("cls");
                cout << "Press f/F to ignite/dim the match,\n";
                cout << "use WASD to move the match\n";
                cout << "or press q/Q to quit\n";
                m = 'm';
                if (_kbhit()) m = _getch();
                match.update(m);
                br = match.getBright();
                dm = match.getDim();
                field.show(br, dm);
                Sleep(30);
            }
        }
            break;
        case '2':
        {
            Candle candle(height, width);
            char m = 'm';
            while (m != 'q' && m != 'Q') {
                system("cls");
                cout << "Press f/F to ignite/dim the candle,\n";
                cout << "use WASD to move the candle\n";
                cout << "or press q/Q to quit\n";
                m = 'm';
                if (_kbhit()) m = _getch(); // проверка на нажатие
                candle.update(m);
                br = candle.getBright();
                dm = candle.getDim();
                field.show(br, dm);
                Sleep(30);
            }
        }
            break;
        case '3':
        {
            Lamp lamp(height, width);
            char m = 'm';
            while (m != 'q' && m != 'Q') {
                system("cls");
                cout << "Press f/F to turn on/off the lamp,\n";
                cout << "Press g/G to plug in/put the lamp,\n";
                cout << "use WASD to move the lamp\n";
                cout << "or press q/Q to quit\n";
                m = 'm';
                if (_kbhit()) m = _getch();
                lamp.update(m);
                br = lamp.getBright();
                dm = lamp.getDim();
                field.show(br, dm);
                Sleep(30);
            }
        }
            break;
        case '4':
        {
            Flashlight fl(height, width);
            char m = 'm';
            while (m != 'q' && m != 'Q') {
                system("cls");
                cout << "Press f/F to turn on/off the flashlight,\n";
                cout << "use WASD to move the flashlight\n";
                cout << "or press q/Q to quit\n";
                m = 'm';
                if (_kbhit()) m = _getch();
                fl.update(m);
                br = fl.getBright();
                dm = fl.getDim();
                field.show(br, dm);
                Sleep(30);
            }
        }
            break;
        case '5':
        {
            Laser laser(height, width);
            char m = 'm';
            while (m != 'q' && m != 'Q') {
                system("cls");
                cout << "Press f/F to turn on/off the laser,\n";
                cout << "use WASD to move the laser\n";
                cout << "or press q/Q to quit\n";
                m = 'm';
                if (_kbhit()) m = _getch();
                laser.update(m);
                br = laser.getBright();
                dm = laser.getDim();
                field.show(br, dm);
                Sleep(30);
            }
        }
            break;
        }
    }
    
}

int main()
{
    func();
}
