#pragma once
#include "stdlib.h"
#include <string>
#include <vector>
#include "vpv-lab3.h"
#include <iomanip>
#include <omp.h>
#include <sstream>
#include <fstream>
#include "histogram.h"

class Report {
   // Режимы вывода: среднее, минимумы, максимумы, СКО, ошибки, 
   enum Command {	
       EXIT, REPEAT, AVG, MIN, MAX, DEV, ERROR, TOFILE, SVG, COUNT_MODES
   };
   string nameMode[COUNT_MODES] = { 
      "Выход", "Повтор", 
      "Средние", "Минимумы", "Максимумы", "СКО%", 
      "Ошибки", "Вывод", "SVG"
   };
   int wCol1; // размер первой колонки таблиц
   int wTable; // ширина распечатки таблицы
   ofstream svg;
public:
   Report() {
      wCol1 = 6; // первая колонка уже остальных
      wTable = WIDTH_COL * COUNT_GRANULARITY + wCol1 + 1;
   }
   ostringstream startText; // Стартовый текст для дублирования в файл
   // Протокол содержит куб methods * threads * granuls с результатами серий измерений
   ResultOfFunction cubeResults[COUNT_FUNCTION];
   int arrGranularity[COUNT_GRANULARITY] = { 100, 1000, 10000, 100000, 1000000 };
   void printLine() {
   cout << endl;
   for (int n = 0; n < wTable; n++)
      cout << '-';
   }
   void printTables() {
      ofstream fout("allResult.txt"); //откроем файл для вывода
      streambuf *coutbuf = std::cout.rdbuf(); //запомним старый буфер
      cout.rdbuf(fout.rdbuf()); //и теперь все будет в файл out.txt!
      cout << startText.str();
      printLine();
      for (int mode = AVG; mode <= ERROR; mode++) {
         showTables(mode);
      }
      cout.rdbuf(coutbuf);
   } // printTables
   
   // Печать таблиц всех параллельных релизов для заданного результата
   void showTables(int mode) {
      cout << endl << endl << "\t" << nameMode[mode] << " для всех реализаций " << endl;
      for(unsigned r = 0; r < COUNT_FUNCTION; r++) { // обход всех релизов
         cout << endl << "\t" << cubeResults[r].name;
         printLine();
         cout << endl << setw(wCol1) << "Гранул|";
         for (int g = 0; g < COUNT_GRANULARITY; g++) // строка с гранулярностью 
            cout << setw(WIDTH_COL) << cubeResults[0].matrix[0][g].granularity;
         cout << endl << setw(wCol1) << "Потоки|";
         printLine(); 
         for (int t = 0; t < MAX_THREAD; t++) { // обход результатов при разном числе потоколв
            cout << endl << setw(wCol1-1) << cubeResults[r].matrix[t][0].threads << " |"; //  вывод числа потоков
            for (int g = 0; g < COUNT_GRANULARITY; g++) { // вывод одной строки ряда таблицы
               cout << fixed  << setw(WIDTH_COL) << setprecision(1);
               switch(mode) {
                  case MIN: cout << MICROSEC(cubeResults[r].matrix[t][g].min); break;
                  case MAX: cout << MICROSEC(cubeResults[r].matrix[t][g].max); break;
                  case AVG: cout << MICROSEC(cubeResults[r].matrix[t][g].avg); break;
                  case DEV: cout << setprecision(2) << cubeResults[r].matrix[t][g].dev; break;
                  case ERROR: cout << setprecision(WIDTH_COL - 3) << cubeResults[r].matrix[t][g].error; break;
                  case TOFILE: printTables();
               }
            } // строка
         } // таблица
         printLine();
         cout << endl;
      } // релиз
   } // showTables

   // Генерация гистограмм
   void makeSVG() {
      //ostringstream head;
      string colors[COUNT_FUNCTION] = {
         "cadetblue","brown","darkorchid","red"
      };
      GroupLegendSet legends(COUNT_FUNCTION);
      for (int i = 0; i < COUNT_FUNCTION; i++) {
         legends[i].name = cubeResults[i].name;
         legends[i].color = colors[i];
      };
      
      { // Сначала группировка по числу потоков
         DataGroupSet dset(MAX_THREAD);
         for (int g = 0; g < COUNT_GRANULARITY; g++) {
            string sGran = to_string(arrGranularity[g]);
            string head = u8"Гранулярность " + sGran;
            Histogram hist(600, head, u8"Tмкс", "Threads");
            hist.setFileName("lab3-" + sGran + ".svg");
            hist.setLegend(legends);
            for (int t = 0; t < MAX_THREAD; t++) { // обход результатов при разном числе потоколв
               dset[t].label = to_string(t + 1);
               vector <MinAvgMax> data;
               for (unsigned r = 0; r < COUNT_FUNCTION; r++) { // обход всех релизов
                  data.push_back({
                     MICROSEC(cubeResults[r].matrix[t][g].min),
                     MICROSEC(cubeResults[r].matrix[t][g].avg),
                     MICROSEC(cubeResults[r].matrix[t][g].max),
                     legends[r].color
                     });
               }
               dset[t].data = data;
            }
            hist.setData(dset);
            hist.calc();
            hist.make();
         }
      }

      { // Затем группировка по функциям
         DataGroupSet dset(COUNT_FUNCTION);
         for (int g = 0; g < COUNT_GRANULARITY; g++) {
            string sGran = to_string(arrGranularity[g]);
            string head = u8"Гранулярность " + sGran;
            Histogram hist(600, head, u8"Tмкс", "Threads");
            hist.setFileName("lab3-" + sGran + "f.svg");
            hist.setLegend(legends);
            string labels = "1.." + to_string(MAX_THREAD);
            for (unsigned r = 0; r < COUNT_FUNCTION; r++) { // обход всех релизов
               dset[r].label = labels;
               vector <MinAvgMax> data;
               for (int t = 0; t < MAX_THREAD; t++) { // обход результатов при разном числе потоколв
                  data.push_back({
                     MICROSEC(cubeResults[r].matrix[t][g].min),
                     MICROSEC(cubeResults[r].matrix[t][g].avg),
                     MICROSEC(cubeResults[r].matrix[t][g].max),
                     legends[r].color
                     });
               }
               dset[r].data = data;
            }
            hist.setData(dset);
            hist.calc();
            hist.make();
         }
      }
	}
	// Показ результатов возвращает true для повтора измерения
	bool show() { 
		char chCmd; // Символ команды диалога
		int intCmd; // Числовое представление команды
		do {
			cout << endl << "Команда(";
			for (int m = EXIT; m < COUNT_MODES; m++) {
				cout << m << "-" << nameMode[m];
				if (m == COUNT_MODES - 1)
					cout << "):";
				else cout << ", ";
			}
			cin >> chCmd;
			intCmd = chCmd - '0';
			if (intCmd < 0 || intCmd >= COUNT_MODES)
				continue;
			switch (intCmd) {
				case EXIT: exit(0);
				case REPEAT: return true;
				case AVG: 
				case MIN: 
				case MAX: 
				case DEV: 
				case ERROR: showTables(intCmd); break;
				case TOFILE: printTables(); break;
				case SVG: makeSVG();
			}
		} while (intCmd > 1);
		return false;
	}
} report;
