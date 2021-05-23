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
   // ������ ������: �������, ��������, ���������, ���, ������, 
   enum Command {	
       EXIT, REPEAT, AVG, MIN, MAX, DEV, ERROR, TOFILE, SVG, COUNT_MODES
   };
   string nameMode[COUNT_MODES] = { 
      "�����", "������", 
      "�������", "��������", "���������", "���%", 
      "������", "�����", "SVG"
   };
   int wCol1; // ������ ������ ������� ������
   int wTable; // ������ ���������� �������
   ofstream svg;
public:
   Report() {
      wCol1 = 6; // ������ ������� ��� ���������
      wTable = WIDTH_COL * COUNT_GRANULARITY + wCol1 + 1;
   }
   ostringstream startText; // ��������� ����� ��� ������������ � ����
   // �������� �������� ��� methods * threads * granuls � ������������ ����� ���������
   ResultOfFunction cubeResults[COUNT_FUNCTION];
   int arrGranularity[COUNT_GRANULARITY] = { 100, 1000, 10000, 100000, 1000000 };
   void printLine() {
   cout << endl;
   for (int n = 0; n < wTable; n++)
      cout << '-';
   }
   void printTables() {
      ofstream fout("allResult.txt"); //������� ���� ��� ������
      streambuf *coutbuf = std::cout.rdbuf(); //�������� ������ �����
      cout.rdbuf(fout.rdbuf()); //� ������ ��� ����� � ���� out.txt!
      cout << startText.str();
      printLine();
      for (int mode = AVG; mode <= ERROR; mode++) {
         showTables(mode);
      }
      cout.rdbuf(coutbuf);
   } // printTables
   
   // ������ ������ ���� ������������ ������� ��� ��������� ����������
   void showTables(int mode) {
      cout << endl << endl << "\t" << nameMode[mode] << " ��� ���� ���������� " << endl;
      for(unsigned r = 0; r < COUNT_FUNCTION; r++) { // ����� ���� �������
         cout << endl << "\t" << cubeResults[r].name;
         printLine();
         cout << endl << setw(wCol1) << "������|";
         for (int g = 0; g < COUNT_GRANULARITY; g++) // ������ � �������������� 
            cout << setw(WIDTH_COL) << cubeResults[0].matrix[0][g].granularity;
         cout << endl << setw(wCol1) << "������|";
         printLine(); 
         for (int t = 0; t < MAX_THREAD; t++) { // ����� ����������� ��� ������ ����� ��������
            cout << endl << setw(wCol1-1) << cubeResults[r].matrix[t][0].threads << " |"; //  ����� ����� �������
            for (int g = 0; g < COUNT_GRANULARITY; g++) { // ����� ����� ������ ���� �������
               cout << fixed  << setw(WIDTH_COL) << setprecision(1);
               switch(mode) {
                  case MIN: cout << MICROSEC(cubeResults[r].matrix[t][g].min); break;
                  case MAX: cout << MICROSEC(cubeResults[r].matrix[t][g].max); break;
                  case AVG: cout << MICROSEC(cubeResults[r].matrix[t][g].avg); break;
                  case DEV: cout << setprecision(2) << cubeResults[r].matrix[t][g].dev; break;
                  case ERROR: cout << setprecision(WIDTH_COL - 3) << cubeResults[r].matrix[t][g].error; break;
                  case TOFILE: printTables();
               }
            } // ������
         } // �������
         printLine();
         cout << endl;
      } // �����
   } // showTables

   // ��������� ����������
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
      
      { // ������� ����������� �� ����� �������
         DataGroupSet dset(MAX_THREAD);
         for (int g = 0; g < COUNT_GRANULARITY; g++) {
            string sGran = to_string(arrGranularity[g]);
            string head = u8"������������� " + sGran;
            Histogram hist(600, head, u8"T���", "Threads");
            hist.setFileName("lab3-" + sGran + ".svg");
            hist.setLegend(legends);
            for (int t = 0; t < MAX_THREAD; t++) { // ����� ����������� ��� ������ ����� ��������
               dset[t].label = to_string(t + 1);
               vector <MinAvgMax> data;
               for (unsigned r = 0; r < COUNT_FUNCTION; r++) { // ����� ���� �������
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

      { // ����� ����������� �� ��������
         DataGroupSet dset(COUNT_FUNCTION);
         for (int g = 0; g < COUNT_GRANULARITY; g++) {
            string sGran = to_string(arrGranularity[g]);
            string head = u8"������������� " + sGran;
            Histogram hist(600, head, u8"T���", "Threads");
            hist.setFileName("lab3-" + sGran + "f.svg");
            hist.setLegend(legends);
            string labels = "1.." + to_string(MAX_THREAD);
            for (unsigned r = 0; r < COUNT_FUNCTION; r++) { // ����� ���� �������
               dset[r].label = labels;
               vector <MinAvgMax> data;
               for (int t = 0; t < MAX_THREAD; t++) { // ����� ����������� ��� ������ ����� ��������
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
	// ����� ����������� ���������� true ��� ������� ���������
	bool show() { 
		char chCmd; // ������ ������� �������
		int intCmd; // �������� ������������� �������
		do {
			cout << endl << "�������(";
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
