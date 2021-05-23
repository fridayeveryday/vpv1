//************************************************************************
// Организация серий измерений затрат времени многопоточных реализаций 
// численного метода интегрирования
// Для каждой серии формируются min, max, avg, СКО, 
// nvnulstu@gmail.com - май 2020
#pragma once

#include "vpv-lab3.h"

#include <sstream>
#include <fstream>


// Тип многопоточной функции, измеряемой в Tester.measure() 
typedef double (*FuncThreadGranul)(int threads, int granul);
// Накопление данных, фильтрация и статистическая обработка
class Log { 
public:
   vector<double> val;// протокол серии измерений
   ResultOfSeries res;
   //int thread, granularity; // число потоков и гранулярность
   int count;			// длина серии измерений
   int delMax, delMin; // число удаляемых максимальных и минимальных элементов
   void calc() {
      vector<double> fval = val; // фильтруемый массив серии измерений
      if (fval.size() > 0) {
         sort(fval.begin(), fval.end());
         // фильтрация через уд2аление максимальных и минимальных значений 
         for (auto n = delMax; n > 0 && fval.size() > 1; n--)
            fval.pop_back();
         for (auto n = delMin; n > 0 && fval.size() > 1; n--)
            fval.erase(fval.begin());
         res.min = *fval.begin();
         res.max = *(fval.end() - 1);
         auto sz = fval.size();
         // Вычисление суммы и среднего
         double sum = 0;
         for (double el: fval) 
            sum += el;
         res.avg = sum / sz;
         // Вычисление СКО
         double dev = 0.;
         for (double el : fval) {
            double diff = (double)el - res.avg;
            dev += diff * diff;
         }
         dev = sqrt(dev/sz); // СКО
         res.dev = (res.avg < 1.E-200)? 0. : (100 * dev) / res.avg; // СКО%
      }
   }
};

// Базовый класс верификации - замера времени 
class Tester {
public:	
	string name;		// Семантическое наименование метода реализации функции
	double (* calcParallel)(int, int); // Адрес тестируемой функции 
	double maxErr;		// Предельно допустимая ошибка
	Log log;			// Протокол conf.count результатов измерений с функциями статистической обработки
	int numFunc;		// Индекс функции с параллельной обработкой
	Report * report;	// Адрес отчета, куда передается результат
	__int64 overhead;   // накладные расходы на измерение
	double etalon;		// эталонное значение интеграла для верификации
	bool proper;		// Признак успешности функционального тестирования
	Tester(string nm, double (* func) (int, int), Config & conf, int nFunc)
		: name(nm), numFunc(nFunc), calcParallel(func), maxErr(conf.maxErr), report(conf.rep),  etalon(conf.etalon){
		log.count = conf.count;
		log.delMax = conf.delMax;
		log.delMin = 0;
		proper = false;
	}
	
	// Вычисление ошибки выполняется для максимально возможного числа потоков и минимальноно числа подинтервалов
	string calcError() { 
		ostringstream str;
		double err;
		str << name;
		err = calcParallel(4, 100) - etalon;
		if (fabs(err) > maxErr) {
				str << ": Ошибка = " << err;
				proper = false;
				return str.str();
			}
		proper = true;
		str << " - OK";
		return str.str();
	}
	void verify() { cout << calcError() << endl; }
	void measure() { // Замеры времени для repeat значений x in [0, 1) с равномерным шагом
		double t; // засечка времени
		double res; // результат вычисления для подсчета ошибок
		cout << endl << name;
		for (int threads = 1; threads <= 12; threads++) {
			cout << endl << setw(WIDTH_COL) << threads;
			for (int gran = 0; gran < COUNT_GRANULARITY; gran++) {
				// Серия log.count измерений
				int subIntervals = report->arrGranularity[gran];
				log.res.threads = threads; 
				log.res.granularity = subIntervals;
				log.val.clear();
				for (int n = 0; n < log.count; n++) {
					t = omp_get_wtime();
					res = calcParallel(threads, subIntervals);
					log.val.push_back(omp_get_wtime() - t);
				} // серия
				log.res.error = res - etalon;
				log.calc();
				report->cubeResults[numFunc].name = name;
				report->cubeResults[numFunc].matrix[threads-1][gran] = log.res;
				cout << setw(WIDTH_COL) << fixed << setprecision(1) << MICROSEC(log.res.min);
			} // гранулярности
		} // потоки
	}
};

