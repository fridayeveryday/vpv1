//*******************************************************************
// Построение и запуск в Windows svg-гистограммы 
// nvnulstu@gmail.com - июнь 2020
//*******************************************************************
#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;

class Render {
   ofstream svg; // продукт рендеринка сразу выводится в файл
   char * fileName; 
public:
   Render() {}
   void create(string fileName, int width, int height) {
      svg.open(fileName); // svg-файл в корневой директории проекта
      // Заголовочная часть 
      svg << "<?xml version = \"1.0\" encoding = \"utf-8\" ?>" << endl;
      svg << "<svg " << endl
         << "xmlns = \"http://www.w3.org/2000/svg\"" << endl
         << "width = \"" << width << "\"" << endl
         << "height = \"" << height << "\"" << endl
         << "version = \"1.1\">" << endl;
   }
   void close () {
      svg << "</svg>";
      svg.close();
   }
   void show() {
      system(fileName);
   }
   void line(int x1, int y1, int x2, int y2, string stroke) {
      svg << "<line x1=\"" << x1 << "\"" 
          << " y1=\"" << y1 << "\"" 
          << " x2=\"" << x2 << "\"" 
          << " y2=\"" << y2 << "\"" 
          << " stroke=\""   << stroke << "\"/>" << endl;
   }
   void bar(int x, int y, int w, int h, string fill) {
      svg << "<rect x=\"" << x << "\""
         << " y=\"" << y << "\""
         << " width=\"" << w << "\" height=\"" << h << "\""
         << " fill=\"" << fill << "\"/>" << endl;
   }
   void text(int x, int y, string text, int size, string weight, string color) {
      svg << "<text x=\"" << x << "\""
          << " y=\""    << y << "\""
          << " font-family = \"monospace\""
          << " font-size=\"" << size << "px\""
          << " font-weight=\"" << weight << "\""
          << " fill=\"" << color << "\">"
          << text<< "</text>" << endl;
   }
   void rect(int x, int y, int w, int h, string stroke, string fill) {
      svg << "<rect x=\"" << x << "\""
         << " y=\"" << y << "\""
         << " width=\"" << w << "\" height=\"" << h << "\""
         << " stroke=\"" << stroke << "\""
         << " fill=\"" << fill << "\"/>" << endl;
   }

};
// **********************************************
// Общая организация структур данных
// DataGroupSet[] = {{dataGroup_0.label, {{min,avg.max}_0, {min,avg.max}_1, ... }}_0,
//                   {dataGroup_1.label, {{min,avg.max}_0, {min,avg.max}_1, ... }}_1,
//                  }
struct MinAvgMax { // набор данных для полосы гистограммы 
   double min, avg, max;
   string color;
   MinAvgMax(double min, double avg, double max, string color) : 
      min(min), avg(avg), max(max), color(color) {};
};
struct DataGroup { 
   string label; // метка у группы данных на оси X
   vector <MinAvgMax> data; // набор данных в группе 
};
struct GroupLegend { // легенды под гистограммой
   string name; // имя набора данных
   string color; // Цвет полосы
};
typedef vector <DataGroup> DataGroupSet;
typedef vector <GroupLegend> GroupLegendSet;
class Histogram {
   DataGroupSet dataSet;
   GroupLegendSet legendSet;
   Render render;
   string head, // заголовок сверху гистограммы
          unitY, unitX, // названия единицы измерения 
          fileName; // имя svg-файла с расширением
   int width, height; // ширина и высота поля svg
   int nDigit;  // максимальное число цифр в разметке оси Y
   double step; // шаг разметки оси ординат в значениях амплитуд
   double yScale; // число пикселей в одной единице амплитуды
   int hHead; // Высота зоны оглавления
   int xLeft; // левая граница начала полос гистограммы
   int nLabel; // число горизонтальных линий с метками на оси ординат
   int barWidth = 5; // ширина полосы
   int sizeFont = 10; // шрифт надписей на осях
   int yBottom; // положение оси абсцисс
public:
   Histogram (int height, string head, string unitY, string unitX ) : 
       height(height), head(head), unitY(unitY), unitX(unitX) {
      hHead = 2 * sizeFont + 4;
   }

   void setData(DataGroupSet d) {
      dataSet = d;
   }
   
   void setLegend (GroupLegendSet legend) { 
      legendSet = legend;
   }
   // Размер в пикселах, соответствующий величине val 
   inline int pxValue(double val) { 
      return static_cast<int>(val * yScale);
   }
   void setFileName(string fn) {
      fileName = fn;
   }
   
   // Длина текста в пикселах, 
   inline int pxTextLen(string text, int size) {
      return text.size() * size * 55 / 100;
   }

   // Вычисление базовых параметров гистограммы
   void calc() { 
      double maxMax = 0.0; // максимум среди всех максимумов
      for (DataGroup dSet : dataSet) { // обход всех групп данных 
         for (MinAvgMax d : dSet.data) { // обход данных внутри группы
            double tMax = d.max;
            if (tMax > maxMax)
               maxMax = tMax;
         }
      }
      // Порядок максимума 
      int yPow = static_cast<int>(log10(maxMax)); // порядок максимума
      // Число цифр метки оси Y на единицу больше порядка
      nDigit = yPow + 1;
      // Коэффициент приведения к диапазону единиц [1..10)
      double powReduct = pow(10, yPow);
      // Приведение absMax к диапазону единиц 
      double reductMax = maxMax / powReduct; 
      // Шаг сетки ординат, приведенных к [1..10] изначально берем 0.1
      double reductStep = 0.1;
      if (reductMax > 5.0) { // увеличиваем, чтобы число линий было из [11..20]
         reductStep = 0.5; 
      }
      else if (reductMax > 2.0) { // число линий загоняем в [11..25]
         reductStep = 0.2;
      }
      // Число линий сетки координаты Y
      nLabel = static_cast<int>(ceil(reductMax / reductStep));
      // Получаем шаг сетки в исходной шкале
      step = reductStep * powReduct;
      // Ниже гистограммы должно быть 2 строки с метками и легендами
      yBottom = height - 2 * sizeFont - 5; 
      // Число пикселей в единице амплитуды
      yScale = static_cast<double>(yBottom - hHead) / (step * nLabel);
      xLeft = nDigit * sizeFont * 55 / 100 + 10;
      // Ширина гистограммы
      width = ((2 * dataSet[0].data.size() + 1) * barWidth) * dataSet.size() + xLeft + 3 * barWidth;
   } // calc
   
     // Рисование сетки
   void axis() {
      // Рамка вокруг поля гистограммы
      render.rect(xLeft, hHead, width - xLeft, yBottom - hHead, "black", "white");
      // Левая ось ординат с метками
      int x = xLeft;
      double mStep = step / 10; // расстояние между короткими метками
      // Все линии сетки и метки на оси Y
      int countLines = static_cast<int>(ceil(step * nLabel / mStep));
      for (int nLine = 0; nLine <= countLines; nLine++) {
         double y = nLine * mStep;
         int yInt = static_cast<int>(round(y));
         int pxY = yBottom - pxValue(y);
         if (nLine % 10 == 0) {
            int lenNumber = static_cast<int>(log10(static_cast<double>(yInt))) + 1;
            if(nLine != 0 && nLine != countLines)
               render.line(xLeft+1, pxY, width-1, pxY, "silver");
            if (nLine != 0) {
               string label = to_string(yInt);
               render.text(xLeft - pxTextLen(label, sizeFont) - 5, pxY + sizeFont / 2 - 1, label, sizeFont, "normal", "black");
            }
         }
         else
            render.line(xLeft+1, pxY, xLeft + ((nLine % 5 == 0) ? 5 : 3), pxY, "silver");
      }
   } // axis

   // Создание и формирование svg-файла гистограммы
   void make() {
      // Создание svg и заголовок
      render.create(fileName, width, height);
      int yHead = hHead - 8; // Отступаем от верха гистораммы
      render.text(xLeft + 150, yHead, head, yHead, "bold", "brown");
      // Имена величин на осях
      render.text(xLeft - 6, yHead, unitY, sizeFont, "normal", "black");
      render.text(10, yBottom + sizeFont, unitX, sizeFont, "normal", "black");
      
      axis(); // Координатная сетка

      // Рисование полос диаграмм
      int x = xLeft + barWidth;
      for (DataGroup dSet : dataSet) { // обход всех групп данных 
         x += barWidth;
         for (unsigned i = 0; i < dSet.data.size(); i++ ) { // обход данных внутри группы
            MinAvgMax d = dSet.data[i];
            string color = dSet.data[i].color;
            int yMin = yBottom - pxValue(d.min);
            int yAvg = yBottom - pxValue(d.avg);
            int yMax = yBottom - pxValue(d.max);
            render.bar(x, yMin, barWidth, yBottom - yMin, color);
            render.bar(x, yAvg, barWidth - 1, yMin - yAvg, color);
            render.bar(x, yMax, barWidth - 2, yAvg - yMax, color);
            x += 2 * barWidth;
         }

         // Метки под группами
         int wGroup = (2 * dSet.data.size() + 2) * barWidth; // шириная группы гистограмм
         render.text(x - (wGroup + pxTextLen(dSet.label, sizeFont))/2, yBottom + sizeFont,
            dSet.label, sizeFont, "normal", "black");
      }

      // Легенды
      int yL = height - 2; 
      x = xLeft;
      for (auto legend : legendSet) {
         render.bar(x, yL - sizeFont + 2, sizeFont, sizeFont - 2, legend.color);
         x += sizeFont + 3;
         render.text(x, yL, legend.name, sizeFont, "normal", "black");
         x += pxTextLen(legend.name, sizeFont) + sizeFont;
      }

      render.close();
      render.show();
   } // make
};