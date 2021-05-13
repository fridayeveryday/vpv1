#pragma once
#include "stdlib.h"
#include "table.h"
#include <vector>
#include "vpv-lab2.h"
#include <iomanip>
#include <iostream>
#include <intrin.h>
#include <map>


typedef map<string, string> TOpt;

TOpt makeOpsMap(int argc, char *argv[]) {
	TOpt ops;
	char *pcolon;
	while (argc > 0) {
		char * parg = argv[--argc];
		if ((pcolon = strchr(parg, ':')) != NULL) {
			*pcolon = '\0'; 
			ops.insert(pair<string, string>(parg, pcolon + 1));
		}
	}
	return ops;
}
// Оценка минимума накладных расходов на измерение времени
__int64 overheadTSC() {
	volatile __int64 t1, t2, t, tmin = 100000;
	int i = 1000000; // максимальное число повторов замера накладных затрат
	while (--i > 0) 
	{ // повторяем, пока не стабилизируется текущий минимум 
		CPUID_RDTSC(t1);
		CPUID_RDTSC(t2);
		if (t2 > t1) {
			t = t2 - t1;
			if (t < tmin) {
				tmin = t;
			}
		}
	}
	return tmin;
}

void init(int argc, char * argv[], Config &conf) {
	int CPUInfo[4];
	char CPUName[80];
	TOpt::iterator it;
	setlocale(LC_CTYPE, "rus");
	// Генерация массива аргументов
	for (int n = 0; n < conf.count; n++)
		arrX.push_back(rand() * (float)0.9999999 / RAND_MAX);
	// Генерация таблиц для табличных методов
	table0 = genTable0();
	table1 = genTable1();
	table2 = genTable2();

	TOpt ops = makeOpsMap(argc, argv);
	it = ops.find("slen");
	if( it != ops.end()) 
		conf.count = atoi(it->second.c_str());
	it = ops.find("dmin");
	if (it != ops.end()) 
		conf.delMin = atoi(it->second.c_str());
	it = ops.find("dmax");
	if (it != ops.end()) 
		conf.delMax = atoi(it->second.c_str());
	it = ops.find("npri");
	if (it != ops.end()) 
		conf.lenPrintLog = atoi(it->second.c_str());
	it = ops.find("pass");
	if (it != ops.end())
		conf.pass = atoi(it->second.c_str());
	conf.count = max(conf.count, conf.delMax + conf.delMin + 100);
	conf.overhead = overheadTSC();
	memset(CPUName, 0, sizeof(CPUName));
	// Коды 0x80000002..0x80000004 позволяют получить полное имя CPU по 16 байтов
	__cpuid(CPUInfo, 0x80000002);
	memcpy(CPUName, CPUInfo, sizeof(CPUInfo));
	__cpuid(CPUInfo, 0x80000003);
	memcpy(CPUName + 16, CPUInfo, sizeof(CPUInfo));
	__cpuid(CPUInfo, 0x80000004);
	memcpy(CPUName + 32, CPUInfo, sizeof(CPUInfo));
	
	cout << "ВПВ: лабораторная работа №2. Исследование реализаций функции sin(x)/x степенными рядами"
		 << endl << "Студент группы ЭВМд-31 Негода В.Н." << endl;
	cout << CPUName << endl << endl;
	cout << "Размер серии: " << conf.count << '.'
		 << " Отбрасываются " << conf.delMin << " наименьших и " << conf.delMax << " наибольших" << '.'
		 << endl << "Длина распечатываемой части серии: " << conf.lenPrintLog << '.'
		 << "  Накладные расходы измерения: " << conf.overhead << endl;
}
class Report {
public:	
	vector <Result> log;
	
	void calc() {
		// Найти самый быстрый метод
		double min = 100000000000000;
		for (const auto & res : log)
			if (res.avg < min) min = res.avg;
		// Прописать factor
		for (auto & res : log) 
			res.factor = res.avg / min;
	}
	void printLine(int width) {
		for (int n = 0; n < width; n++)
			cout << '-';
		cout << endl;
	}
	void print() {
		// Печать протокола
		const int width = 10;
		if (log.size() == 0) return;
		printLine(16 + 6 * width);
		vector <string> head = { "Функция", " Рейтинг" , "Среднее", "СКО", "СКО%", "Минимум", "Максимум"};
		cout << left << setw(16) << head[0];
		for (unsigned n = 1; n < head.size(); n++) 
			cout << right << setw(width) << head[n];
		cout << endl;
		printLine(16 + 6 * width);
		for (unsigned n = 0; n < log.size(); n++)
			cout << left << setw(16) << log[n].shortname 
				 << right << setw(width) << setprecision(2) << log[n].factor 
				 << setw(width) << setprecision(2) << fixed << log[n].avg
				 << setw(width) << setprecision(2) << log[n].dev
				 << setw(width) << setprecision(2) << (100 * (log[n].dev / log[n].avg)) 
				 << setw(width) << log[n].min 
				 << setw(width) << log[n].max << endl;
		printLine(16 + 6 * width);
	}
};
