//**************************************************************************************
// Пример исследования реализации функции sin(x)/x = 1 - x^2/3! + x^4/5! - x^6/7! + ...
// vpv-lab2: март 2020:: nvnulstu@gmail.com 
//**************************************************************************************
#pragma once
#include "stdlib.h"
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

#define LEN_RES 19  // разрядность результата функции, в младшем бите которого может быть ошибка
#define MAX_ERR (float)(1.0/(1 << LEN_RES)) // максимум погрешности равен 2^(-LEN_RES)
#define LEN_POLINOM 5 // достаточно иметь 5 членов ряда 
#define X_STEP ((float)1.0/(float)(1 << 24)) // 2^(-24) - вес младшего разряда мантиссы float

// Значения факториалов 
#define FACT3 (float)(2 * 3)
#define FACT5 (float)(2 * 3 * 4 * 5)
#define FACT7 (float)(2 * 3 * 4 * 5 * 6 * 7)
#define FACT9 (float)(2 * 3 * 4 * 5 * 6 * 7 * 8 * 9)

// Значение обратных величин факториалов (float)
#define DIV1_FACT3 (1./FACT3)
#define DIV1_FACT5 (1./FACT5)
#define DIV1_FACT7 (1./FACT7)
#define DIV1_FACT9 (1./FACT9)

typedef float(*FloatFunc)(float); // Указатель на функцию вещественных переменн

typedef long FixPoint;

// В дробной части фиксированного числа 30 бит
#define FRACT_PART 30 
// 2^31 - множитель для получения 31 бита мантиссы в long, имитирующем число с фиксированной точкой  
#define FACTOR (1 << FRACT_PART) 
#define FLOAT2FIX(x) (FixPoint)(x * FACTOR) // преобразование float в Fixed
#define FIX2FLOAT(x) ((float)x / FACTOR)   // преобразование Fixed во float
#define FIXMUL(x,y) ((FixPoint)((__int64)x * (__int64)y >> FRACT_PART))     // умножение фиксированных в С++ требует двойной точности
// Значение обратных величин факториалов (FixPointt)
#define DIV1_FACT1FP (FixPoint)FLOAT2FIX(1.0) // 1 в формате FixPoint
#define DIV1_FACT3FP (FixPoint)FLOAT2FIX(DIV1_FACT3)
#define DIV1_FACT5FP (FixPoint)FLOAT2FIX(DIV1_FACT5)
#define DIV1_FACT7FP (FixPoint)FLOAT2FIX(DIV1_FACT7)
#define DIV1_FACT9FP (FixPoint)FLOAT2FIX(DIV1_FACT9)
#define TAB "\t"

// Измерение времени в тактах по схеме "барьер - чтение TSC"
#define CPUID_RDTSC(t) \
	__asm xor eax, eax\
	__asm cpuid __asm rdtsc\
	__asm mov DWORD PTR[t], eax\
	__asm mov DWORD PTR[t + 4], edx

typedef FixPoint(*FixedFunc)(FixPoint); // Указатель на функцию вещественных переменных

vector<float> arrX; // массив аргументов для серий измерений

float flMathFunc(float);
// Результат серии измерений с поддержкой сравнительного анализа
struct Result {
	string shortname;	// короткое имя релиза вместо номера
	string name;		// Наименование релиза функции
	__int64 min, max;	// минимум и максимум числа тактов затрат времени (до фильтрации)
	double avg, dev;	// среднее и СКО
	double factor;		// отношение avg данного метода к avg самого быстрого метода
	Result(string sn, string nm, __int64 minim, __int64 maxim, double average, double deviation) :
		shortname(sn), name(nm), min(minim), max(maxim), avg(average), dev(deviation) {};
};

class Report; // отчет

struct Config {
	int count;			// Число замеров в одной серии
	int delMax, delMin;	// Число удаляемых максимальных и минимальных элементов при фильтрации результатов серии измерений
	int lenPrintLog;	// Число первых результатов серии измерений, выводимых на консоль
	int pass;			// число проходов для анализа повторяемости
	Report * rep;		// указатель на отчет
	float maxErr;		// максимально допустимая погрешность
	float xStepVerify;	// шаг обхода значений аргумента при верификации float
	__int64 overhead;	// оценка накладных расходов на измерение времени в тактах
	Config(int cnt, int maxd, int mind, int nPrint, int npass, Report * report, float err, float step, __int64 over)
		  :	count(cnt), delMax(maxd), delMin(mind), lenPrintLog(nPrint), pass(npass), rep(report), maxErr(err), 
			xStepVerify(step), overhead(over) {};
};