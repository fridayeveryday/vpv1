#pragma once

#include "vpv-lab2.h"
#include "fixed.h"
#include <sstream>

using namespace std;

typedef float(*PFloatFu)(float);
typedef FixPoint(*PFixFunc)(FixPoint);

// Накопление данных, фильтрация и статистическая обработка
class Log { 
public:
	vector<__int64> val;// протокол серии измерений
	int count;			// длина серии измерений
	int delMax, delMin; // число удаляемых максимальных и минимальных элементов
	int lenPrintLog;	// число первых результатов измерения, выводимых на экран
	int xStepVerify;	// число десятичных разрядов после точки в распечатке 
	__int64 min, max;	// минимум, максимум
	double	avg, dev;	// среднее, СКО
	void calc() {
		vector<__int64> fval = val; // фильтруемый массив серии измерений
		if (fval.size() > 0) {
			sort(fval.begin(), fval.end());
			min = *fval.begin();
			max = *(fval.end() - 1);
			// фильтрация через удаление максимальных и минимальных значений 
			for (auto n = delMax; n > 0 && fval.size() > 1; n--)
				fval.pop_back();
			for (auto n = delMin; n > 0 && fval.size() > 1; n--)
				fval.erase(fval.begin());
			auto sz = fval.size();
			// Вычисление суммы и среднего
			__int64 sum = 0;
			for (__int64 el : fval) sum += el;
			avg = (double)sum / sz;
			// Вычисление СКО
			dev = 0.;
			for (__int64 el : fval) {
				double diff = (double)el - avg;
				dev += diff * diff;
			}
			dev = sqrt(dev / (sz - 1));
		}
	}
	void print(string name) {
		if (lenPrintLog <= 0) return;
		cout << setw(16) << left << name << ": ";
		cout.setf(ios::right | ios::dec);
		cout.fill(' ');
		for (int n = 0; n < lenPrintLog; n++)
			cout << setw(6) << val[n];
		cout << endl;
	}
};

// Базовый класс верификации - замера времени 
class Tester {
public:	
	string shortname;	// короткое имя релиза вместо номера
	string name;		// Семантическое наименование метода реализации функции
	float err;			// Предельно допустимая ошибка
	float xStep;		// Шаг обхода значений аргументов при верификации функций
	Log log;			// Протокол conf.repeat результатов измерений с функциями статистической обработки
	Report * report;	// Адрес отчета, куда передается результат
	__int64 overhead;   // накладные расходы на измерение
	bool proper;			// результат верификации
	Tester(string sn, string nm, Config & conf)
		: shortname(sn), name(nm), err(conf.maxErr),  report(conf.rep), xStep(conf.xStepVerify), overhead(conf.overhead) {
		log.count = conf.count;
		log.delMax = conf.delMax;
		log.delMin = conf.delMin;
		log.lenPrintLog = conf.lenPrintLog;
		proper = false;
	}
	void saveResult() { // сохранение результата
		report->log.push_back(Result(shortname, name, log.min, log.max, log.avg, log.dev));
	}
	virtual string test() { return ""; }
	void verify() { cout << test() << endl; }
	virtual void measure() {}
	void timeSpent() {
		if (proper) {	// ЕСЛИ	функциональный тест пройден
			measure();	// ТО	выполнить repeat измерений затрат времени 
			log.print(shortname);
			log.calc();		//		обработать протокол серии измерений
			saveResult();	//		сохранить результаты в report
			log.val.clear();
		}
	}
};

// Класс для float
class TestFloat : public Tester {
public:
	FloatFunc func; // Адрес тестируемой функции
	TestFloat(string sn, string nm, FloatFunc fu, Config & conf) :
	Tester(sn, nm, conf), func(fu) {}
	string test() { // Проверка правильности функции для всех значений мантисс при нулевом порядке
		ostringstream str;
		//str << shortname;
		for (float x = 0.; x < 1.; x += xStep) {
			float etalon = flMathFunc(x);
			float real = func(x);
			if (fabs(etalon - real) > err) {
				str <<  ": Ошибка func(" << x << ") = " << real << " != " << etalon;
				proper = false;
				return str.str();
			}
		}
		proper = true;
		str << shortname << " - OK";
		return str.str();
	}
	void measure() { // Замеры времени для repeat значений x in [0, 1) с равномерным шагом
		__int64 t1,t2;
		for ( unsigned n = 0; n < arrX.size(); n++) {
			float x = arrX[n];
			CPUID_RDTSC(t1);
			float fl = func(x);
			CPUID_RDTSC(t2);
			log.val.push_back(t2 - t1 - overhead);
		}
	}
};

// Тестер для фиксированной точки
class TestFixed : public Tester {
public:
	FixedFunc func; // Адрес тестируемой функции
	TestFixed(string sn, string nm, FixedFunc fu, Config & conf) :
	Tester(sn, nm, conf), func(fu) {}
	string test() { // Проверка правильности функции для всех положительных значений Fixed 
		ostringstream str;
		str << shortname;
		FixPoint step = FLOAT2FIX(xStep);
		for (FixPoint x = 0; x < DIV1_FACT1FP; x += step) {
			float etalon = (float)flMathFunc(FIX2FLOAT(x)); // эталон float
			FixPoint fixEtalon = FLOAT2FIX(etalon);
			FixPoint r = func(x);	// реальный fixed
			float real = FIX2FLOAT(r); // превращаем реальный в float, чтобы сранить
			if (fabs(etalon - real) > err) {
				str	<< ": Ошибка func(" << hex << uppercase << setfill('0') << setw(8) << x << "): "
					<< hex << uppercase << setfill('0') << setw(8) << r << " != " 
					<< hex << uppercase << setfill('0') << setw(8) << fixEtalon
					<< " == " << etalon;
				proper = false;
				return str.str();
			}
		};
		proper = true;
		str << " - OK";
		return str.str();
	}
	void measure() { // Замеры времени для repeat значений x in [0, 1) с равномерным шагом
		__int64 t1, t2;
		for ( unsigned n = 0; n < arrX.size(); n++) {
			FixPoint x = FLOAT2FIX(arrX[n]);
			CPUID_RDTSC(t1);
			FixPoint f = func(x);
			CPUID_RDTSC(t2);
			log.val.push_back(t2 - t1 - overhead);
		}
	}
};

