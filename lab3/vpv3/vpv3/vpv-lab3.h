//**************************************************************************************
// Пример исследования параллельного вычисления определенного интеграла функции sin(x)/x 
// на интервале [0;1]
// vpv-lab2: март 2020:: nvnulstu@gmail.com 
//**************************************************************************************
#pragma once
#include "stdlib.h"
#include <string>
#include <algorithm>
#include <vector>
#include <intrin.h>
#include <iostream>

using namespace std;

#define MAX_THREAD 12 // максимальное число потоков
#define MIN_SUBINTERVALS 100 // минимум числа прямоугольников
#define MAX_SUBINTERVALS 1000000 // максимум числа прямоугольников
#define COUNT_GRANULARITY 5 // число гранулярностей 100, 1000, 10000, 100000, 1000000
#define COUNT_FUNCTION 4 // число испытываемых функций параллельной обработки
#define MAX_ERR 1.E-5 // максимально допустимое отклонение от эталонного значения
#define WIDTH_COL 12  // ширина колонки в таблице результатов

// преобразование секунд в микросекунды
#define MICROSEC(s) ((s*1.E6)) 

// Результат серии измерений
struct ResultOfSeries {
	int threads, granularity; // Число потоков и гранулярность, при которых проводилась серия измерений
	double min, max;	// минимум и максимум затрат времени (до фильтрации) в секундах
	double avg, dev;	// среднее и СКО
	double error; // ошибка вычисления
	ResultOfSeries() {}
	/*ResultOfSeries(int th, int gran, __int64 minim, __int64 maxim, double average, double deviation, double err) :
		threads(th), granularity(gran), min(minim), max(maxim), avg(average), dev(deviation),  error(err) {};*/
};

// Результат измерения для функции с параллельностью 
struct ResultOfFunction {
	ResultOfFunction() {}
	// Имя функции - способа распараллеливания
	string name; 
	// Матрица thread * granularity результатов обработки серий измерений
	ResultOfSeries matrix[MAX_THREAD][COUNT_GRANULARITY]; 
};

class Report; // отчет

struct Config {
	int count;			// Число замеров в одной серии
	int delMax;			// Число удаляемых максимальных элементов при фильтрации результатов серии измерений
	Report * rep;		// указатель на отчет
	double maxErr;		// максимально допустимая погрешность
	double etalon;		// эталонное значение интеграла
	Config(int cnt, int maxd, Report * report, double err, double et)
		  :	count(cnt), delMax(maxd), rep(report), maxErr(err), etalon(et) {};
};