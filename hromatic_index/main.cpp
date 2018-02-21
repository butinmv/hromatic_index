#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include "button.hpp"
#include "programmObject.hpp"

using namespace std;
using namespace sf;

// Подпрограммы
void inpFile();         // считывание графа из файла
void findIndex();       // нахождение хроматического индекса

int big;                        // число независимости графа
int n;                          // число вершин
int ncycle = 0;                 // счетчик циклов
vector<vector<int> *> cycle;    // циклы все простые
vector<vector<int> *> g;        // вершины и связи
vector<int> p;                  // путь
vector<int> color;              // список посещенных
vector<int> wg;                 // рабочие вершины

struct Edge {                   // ребро
    int start;
    int end;
    
    Edge(int a, int b) {
        start = a;
        end = b;
    }
};

vector<vector <Edge *> *> colorEdges;   // окрашеные ребра, colorEdges[i] - цвет, colorEdges[i][j] - ребро i-j
vector<Edge *> bufferVertex;            // список ребер
bool q;
int maxDegree;                          // максимальная степень вершины
int count_color;                        // количество не раскрашенных ребер
int const countColor = 8;
Color myColors[countColor] = {
    Color(127, 255, 212),
    Color(0, 191, 255),
    Color(123, 104, 238),
    Color(255, 255, 0),
    Color(250, 128, 114),
    Color(255, 105, 180),
    Color(255, 160, 122),
    Color(244, 164, 96)
};

// Интерфейс
int r = 700;                            // радиус круга
//цвет интерфейса
Color white_100 = Color(255, 255, 255, 100);
Color white_255 = Color(255, 255, 255, 255);
Color background = Color(255, 255, 255);
Color line_color = Color(35, 95, 140);
vector<int> x, y;                       // координаты вершин
string str = "Hromatic index: ";


int main() {
    // настройки окна
    ContextSettings settings;
    settings.antialiasingLevel = 100;
    VideoMode videoMode;
    videoMode.width = 2000;
    videoMode.height = 2000;
    string nameWindow = "Hromatic index";
    RenderWindow window(videoMode, nameWindow, Style::Close, settings);
    
    // найтсроки шрифта
    Font font;
    if (!font.loadFromFile("cour.ttf")) {
        return EXIT_FAILURE;
    }
    Text text("", font, 35);
    text.setFillColor(Color::Red);
    text.setStyle(Text::Bold);

    int r = 30;
    CircleShape shape(r);
    shape.setOutlineColor(Color::Red);
    shape.setOutlineThickness(3);
    shape.setFillColor(Color(255, 255, 255, 200));
    
    findIndex();
    
    // настройки иконки
    Image icon;
    if (!icon.loadFromFile("icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    // найстройки кнопок
    vector<Button> button;
    big = maxDegree;
    string res = to_string(big);
    str = str + res;
    Button info(80, videoMode.height - 160, 1840, 80, Color(35, 95, 140, 200), Color(0,0,0), str, 0);
    button.push_back(info);
    
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                window.close();
            }
            
            // движение узлов
            if (Mouse::isButtonPressed(Mouse::Left))
            {
                for (int i = 0; i < n; i++)
                {
                    if (x[i] <= (Mouse::getPosition(window)).x && x[i] >= (Mouse::getPosition(window)).x - 2 * r &&
                        y[i] <= (Mouse::getPosition(window)).y && y[i] >= (Mouse::getPosition(window)).y - 2 * r)
                    {
                        x[i] = (Mouse::getPosition(window)).x - r;
                        y[i] = (Mouse::getPosition(window)).y - r;
                        break;
                    }
                }
            }
            
            window.clear(background);
            
            // вывод графа
            // ребер
            int size = bufferVertex.size();
            for (int i = 0; i < size; i++) {
                int a = bufferVertex[i]->start;
                int b = bufferVertex[i]->end;
                Vertex line[] = {
                    Vertex(Vector2f(x[a] + r, y[a] + r), white_255),
                    Vertex(Vector2f(x[b] + r, y[b] + r), white_255)
                };
                
                window.draw(line, 2, Lines);
            }
            
            // вывод окрашеных ребер
            size = colorEdges.size();
            if(countColor >= big)
                for (int i = 0; i < size; i++) {
                    int csize = colorEdges[i]->size();
                    for(int j = 0; j < csize; j++) {
                        int a = (*colorEdges[i])[j]->start;
                        int b = (*colorEdges[i])[j]->end;
                        RectangleShape line;
                        float h_ab, w_ab, long_ab, angle;
                        h_ab = x[a] - x[b];
                        w_ab = y[a] - y[b];
                        long_ab = sqrt(h_ab * h_ab + w_ab * w_ab);
                        angle = asin(h_ab / long_ab) / M_PI * 180;
                        if(h_ab < 0 && w_ab > 0)
                            angle = 180 - angle;
                        if(h_ab >= 0 && w_ab >= 0)
                            angle = 180 - angle;
                        line.setRotation(angle);
                        line.setSize(Vector2f(8, long_ab));
                        line.setFillColor(myColors[i]);
                        line.setPosition(Vector2f(x[a] + r, y[a] + r));
                        window.draw(line);
                    }
                }
            
            // вершин
            for (int i = 0; i < n; i++) {
                shape.setPosition(x[i], y[i]);
                string s = to_string(i + 1);
                text.setString(s);
                text.setPosition(x[i] + 17, y[i] + 3);
                window.draw(shape);
                window.draw(text);
            }
            
            // Фон
            int v = 40;
            int sizeBackground = videoMode.width / v + 1;
            for(int j = 0; j < sizeBackground ; j++) {
                Vertex line[] = {
                    Vertex(Vector2f(j * v, 0), line_color),
                    Vertex(Vector2f(j * v, videoMode.height), line_color)
                };
                window.draw(line, 2, Lines);
            }
            
            size = videoMode.height / v + 1;
            for(int j = 0; j < size; j++) {
                Vertex line[] = {
                    Vertex(Vector2f(0, j * v), line_color),
                    Vertex(Vector2f(videoMode.width, j * v), line_color)
                };
                window.draw(line, 2, Lines);
            }
        
            size = button.size();
            for(int i = 0; i < size; i++)
                button[i].draw(window);
            
            window.display();
        }
    }
    return 0;
}


// Считывание графа из файла
void inpFile() {
    int size = g.size(); // очистка вершин
    for (int i = 0; i < size; i++)
        g[i]->clear();
    g.clear();
    
    size = bufferVertex.size(); // очистка ребер
    for (int i = 0; i < size; i++)
        delete bufferVertex[i];
    bufferVertex.clear();
    
    // ввод вершин
    ifstream file("input.txt");
    
    if (!file.is_open())
        cout << "Error! File is not found!" << endl;
    else
        file >> n;
    
    for (int i = 0; i < n; i++) {
        p.push_back(-1);
        color.push_back(0);
        g.push_back(new vector<int>());
        x.push_back(0);
        y.push_back(0);
        wg.push_back(1);
    }
    
    // вывод по кругу
    for (int i = 0; i < n; i++) {
        int xx = 1000 - 30 + r * cos(i * 360 / n * M_PI / 180);
        int yy = 900 - 30 + r * sin(i * 360 / n * M_PI / 180);
        x[i] = xx;
        y[i] = yy;
    }
    
    // ввод ребер
    vector<Edge *> bufferVertex_1;
    int v_start, v_finish;
    bool flag = true;
    while(!file.eof())
    {
        file >> v_start;
        file >> v_finish;
        if (v_start > 0 && v_start <= n && v_finish > 0 && v_finish <= n) {
            g[v_start - 1]->push_back(v_finish - 1);
            g[v_finish - 1]->push_back(v_start - 1);
            Edge* a = new Edge(v_start - 1, v_finish - 1);
            bufferVertex_1.push_back(a);
        }
    }
    
    // ищем главную вершину (наибольшей степени)
    maxDegree = g[0]->size();
    int number = 0;
    size = g.size();
    for(int i = 1; i < size; i++) {
        int gsize = g[i]->size();
        if(gsize > maxDegree) {
            maxDegree = g[i]->size();
            number = i;
        }
    }
    
    // ребра не из главной вершины
    size = bufferVertex_1.size();
    count_color = 0;
    for(int i = 0; i < size; i++)
        if(bufferVertex_1[i]->start != number && bufferVertex_1[i]->end != number) {
            bufferVertex.push_back(bufferVertex_1[i]);
            count_color++;
        }
    
    // ребра не из главной вершины
    for(int i = 0; i < size; i++)
        if(bufferVertex_1[i]->start == number || bufferVertex_1[i]->end == number)
            bufferVertex.push_back(bufferVertex_1[i]);
    
    // вывод
    cout << "----- Все рёбра -----" << endl;
    size = bufferVertex.size();
    for(int i = 0; i < size; i++)
        cout << bufferVertex[i]->start + 1 << " - " << bufferVertex[i]->end + 1 << endl;
    cout << endl;
    
    file.close();
}

// Нахождение хроматического индекса
void findIndex() {
    inpFile();
    cout << "Максимальная степень вершины " << maxDegree << endl << count_color << endl;
    bool f;
    int size = bufferVertex.size();
    int *colors = new int[size]; // массив возможной комбинации цветов
    
    // сбрасываем цвета всех ребер
    for(int i = 0; i < size; i++)
        colors[i] = 0;
    
    // красим ребра главной вершины
    int col = 0;
    for(int i = count_color; i < size; i++) {
        colors[i] = col;
        col++;
    }
    
    // цикл поиска q раскраски, а не q + 1
    bool good = true;
    if(count_color == 0) {
        size = colorEdges.size();
        for(int i = 0; i < size; i++)
            colorEdges[i]->clear();
        colorEdges.clear();
        
        // окраска в текущую комбинация цветов
        for (int i = 0; i < maxDegree; i++) // добавление цветов
            colorEdges.push_back(new vector<Edge *>());
        size = bufferVertex.size();
        
        for (int i = 0; i < size; i++) // нужные ребра в нужный цвет
            colorEdges[colors[i]]->push_back(bufferVertex[i]);
        return;
    }
    
    colors[0] = -1;
    do {
        size = count_color;
        q = true;
        for(int i = 0; i < size; i++)
            if((colors[i] + 1) != maxDegree) {
                colors[i]++;
                for(int j = i - 1; j > -1; j--)
                    colors[j] = 0;
                break;
            }
        size = colorEdges.size();
        for(int i = 0; i < size; i++)
            colorEdges[i]->clear();
        colorEdges.clear();
        
        //Окраска в текущую комбинация цветов
        for (int i = 0; i < maxDegree; i++) // добавление цветов
            colorEdges.push_back(new vector<Edge *>());
        size = bufferVertex.size();
        
        for (int i = 0; i < size; i++) // нужные ребра в нужный цвет
            colorEdges[colors[i]]->push_back(bufferVertex[i]);
        // проверка на смежность одноцветных ребер
        size = colorEdges.size();
        f = false; // не нашли
        for (int i = 0; i < size; i++) {
            int csize = colorEdges[i]->size();
            for (int j = 0; j < csize; j++)
                for (int k = j + 1; k < csize; k++)
                    if (((*colorEdges[i])[j]->start == (*colorEdges[i])[k]->start) ||
                        ((*colorEdges[i])[j]->start == (*colorEdges[i])[k]->end) ||
                        ((*colorEdges[i])[j]->end == (*colorEdges[i])[k]->start) ||
                        ((*colorEdges[i])[j]->end == (*colorEdges[i])[k]->end))
                        f = true;
        }
        
        // вывод
        size = colorEdges.size();
        cout << endl;
        
        for (int i = 0; i < size; i++) {
            cout << "Color: " << i << endl;
            int csize = colorEdges[i]->size();
            for(int j = 0; j < csize; j++) {
                int a = (*colorEdges[i])[j]->start + 1;
                int b = (*colorEdges[i])[j]->end + 1;
                cout << a << " - " << b << "     ";
            }
            cout << endl;
        }
        
        bool a1 = f;
        //Если окрасился неправильно, проверяем не закончился ли счётчик
        if (f) {
            f = false;
            for (int i = 0; i < count_color; i++)
                if ((colors[i] + 1) != maxDegree) {
                    f = true;
                    break;
                }
            q = false;
        }
        
        if(!a1)
            return;
        
        if(good && a1 && !f) {
            good = false;
            f = true;
            maxDegree++;
            col++;
            
            // сбрасываем цвета всех ребер
            size = bufferVertex.size();
            
            for(int i = 0; i < size; i++)
                colors[i] = 0;
            
            // красим ребра главной вершины
            int col = 1;
            for(int i = count_color; i < size; i++) {
                colors[i] = col;
                col++;
            }
            colors[0] = -1;
        }
    } while(f); // пока не нашли расскраску в n цветов
}

