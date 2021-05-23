#pragma once

#include "vpv-lab3.h"
#include "report.h"
#include "integral.h"
#include <sstream>
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

void cpuInfo() {
	int CPUInfo[4]; // 0:EAX, 1:EBX, 2:ECX, 3:EDX
	char CPUName[80];
	// Получение имени процессора
	memset(CPUName, 0, sizeof(CPUName));
	// Коды 0x80000002..0x80000004 позволяют получить полное имя CPU по 16 байтов
	__cpuid(CPUInfo, 0x80000002);
	memcpy(CPUName, CPUInfo, sizeof(CPUInfo));
	__cpuid(CPUInfo, 0x80000003);
	memcpy(CPUName + 16, CPUInfo, sizeof(CPUInfo));
	__cpuid(CPUInfo, 0x80000004);
	memcpy(CPUName + 32, CPUInfo, sizeof(CPUInfo));
	report.startText << endl << CPUName << endl << endl 
					 << "Число вычислительных узлов: " << omp_get_num_procs() << endl;
}
void init(int argc, char * argv[], Config &conf) {
	report.startText << "Высокопроизводительные вычисления. Лабораторная работа № 3."
		<< endl << "Студент гр. ЭВМд-31 Негода В.Н." << endl;
	TOpt::iterator it;
	setlocale(LC_CTYPE, "rus");
	
	TOpt ops = makeOpsMap(argc, argv);
	it = ops.find("slen");
	if (it != ops.end())
		conf.count = atoi(it->second.c_str());
	it = ops.find("dmax");
	if (it != ops.end())
		conf.delMax = atoi(it->second.c_str());
	conf.count = max(conf.count, conf.delMax + 3);
	conf.etalon = calcEtalon();
	cpuInfo();
	report.startText << "Размер серии: " << conf.count 
		<< ", отбрасываются " << conf.delMax << " наибольших" << '.'
		<< endl << "Разрешение часов omp_wtime: " << (int) (omp_get_wtick() * 1.E9) << " ns" << endl
		<< "Эталонное значение интеграла: " << conf.etalon
		<< endl << "Единицы измерения времени: микросекунды" << endl;
	cout << report.startText.str();
	
}
