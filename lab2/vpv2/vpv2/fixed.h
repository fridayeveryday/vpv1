//**************************************************************************************
// Пример исследования реализации функции sin(x)/x = 1 - x^2/3! + x^4/5! - x^6/7! + ...
// sinx_divx: март 2020:: nvnulstu@gmail.com 
//**************************************************************************************
#pragma once

#include "vpv-lab2.h"
#include "string.h"
#include <sstream>

using namespace std;

// #define DEBUG // раскомментировать, если нужно бороться с ошибкми бесцикловой реализация схемы Горнера

FixPoint fixCoef[LEN_POLINOM] = { DIV1_FACT1FP, -DIV1_FACT3FP, DIV1_FACT5FP, -DIV1_FACT7FP, DIV1_FACT9FP };

/*void printHex(FixPoint x) {
	cout << hex << uppercase << setfill('0') << setw(8) << x;
}*/
// Формирование строки двочиного представления FixPoint 32 бита с lenFP разрядами дробной части
string fixPointToStr(FixPoint fx, int lenFP) {
	string str = "";
	for (int count = 0; count < 32; count++) {
		if (count == lenFP)
			str.insert(0, 1, '.');
		str.insert(0, 1, (fx & 1) ? '1' : '0');
		fx >>= 1;
	}
	return str;
}
// Формат фиксированных для sin(x)/x: знак, один бит целой части, 30 бит дробной части
// т.е  w[31]: знак, w[30]: 1, w[29]: 2^(-1), .. w[0]: 2^(-30)
bool testFixOperations(Config config) {
	char strbuf[34]; // буфер двоичного числа
	// Контрольные числа: 1, 2^(-1), 2^(-2), 2^(-3), 2^(-30), -2^(-1), -2^(-2), -2^(-3), -2^(-30)
	float	flo[] = { 1., 0.5, 0.25, 0.125, (float)1. / (1 << 30), -0.5, -0.25, -0.125, -(float)1. / (1 << 30) };
	// Те же самые числа, что в массиве flo, представляются в массиве fix в формате FixPoint
	FixPoint fix[] = {
		0x40000000, // 01.000000..0 : 1
		0x20000000, // 00.100000..0 : 0.5 
		0x10000000, // 00.010000..0 : 0.25
		0x08000000, // 00.001000..0 : 0.125
		0x00000001, // 00.0000..001 : 2^(-30)
		// для отрицательных код дополнительный
		-0x20000000, // 11.100000..0 : -0.5 
		-0x10000000, // 11.110000..0 : -0.25 
		-0x08000000, // 11.111000..0 : -0.125 
		-0x00000001, // 11.1111..111 : 2^(-30)
	};
	int sz = sizeof(fix) / sizeof(FixPoint);
	// Контроль FLOAT2FIX путем сопоставления fix[i] и FLOAT2FIX(flo[i]) 
	bool ok1 = true;
	for (int n = 0; n < sz; n++) { 
		FixPoint fx = FLOAT2FIX(flo[n]);
		if ( fx != fix[n]) { // 
			cout	<< "FLOAT2FIX(" << flo[n] << "):: " << fixPointToStr(fx, FRACT_PART) 
					<< " != " << fixPointToStr(fix[n], FRACT_PART) << endl;
			ok1 = false;
		}
	}
	if (ok1 && config.lenPrintLog > 0) 
		cout << "FLOAT2FIX(x) - OK" << endl;
	
	// Контроль FIX2FLOAT для каждого fix[i] получить float и сравнивнить с flo[i]
	bool ok2 = true;
	for (int n = 0; n < sz; n++) {
		float fl = FIX2FLOAT(fix[n]);
		if (fl != flo[n]) {
			cout	<< "FIX2FLOAT(" << fixPointToStr(fix[n], FRACT_PART) << "):: " << fl
					<< " != " << flo[n] << endl;
			ok2 = false;
		}
	}
	if (ok2 && config.lenPrintLog > 0) 
		cout << "FIX2FLOAT(x) - OK" << endl;
	// Контроль FIXMUL - всемозможные сочетания элементов fix умножаются и результат считается равным эталоному,
	// если он по модулю не больше X_STEP 
	bool ok3 = true;
	for (int n = 0; n < sz; n++) {
		for (int k = 0; k < sz; k++) {
			FixPoint mfix = FIXMUL(fix[n], fix[k]);
			float etalon = flo[n] * flo[k];
			if (fabs(FIX2FLOAT(mfix) - etalon) > X_STEP) {
				cout	<< "FIXMUL(" << fixPointToStr(fix[n], FRACT_PART) << ", "
						<< fixPointToStr(fix[k], FRACT_PART) << ") = "
						<< fixPointToStr(mfix, FRACT_PART) << " != " << etalon << endl;
				ok3 = false;
			}
		}
	}
	if (ok3 && config.lenPrintLog > 0) 
		cout << "FIXMUL(x, y) - OK" << endl;
	return ok1 && ok2 && ok3;
}

// Цикл схемы Горнера
// Директивы условной трасляции DEBUG вставлены для облегчения отладки бесцикловой реализации
// Когда цикловая реализация отлажена
FixPoint fxCycleGorn(FixPoint x) {
	FixPoint x2 = FIXMUL(x,x), sum = 0;
#ifdef DEBUG
	// В if должно быть значение, на котором спотыкается проверка бесцикловой реализации
	// Если это случиться, то придется отрессировать бесцикловую реализацию, 
	//	сравнивая значения с теми, что получаются в цикловой
	FixPoint val = 0x0014C900;
	if (x == val) {
		cout << "x2 = ";
		printHex(x2);
		cout << ":: sum("; 
		printHex(x);
		cout << "): ";
	}
#endif
	for (int n = LEN_POLINOM; n > 0; n--) {
		sum = FIXMUL(sum, x2) + fixCoef[n - 1];
#ifdef DEBUG
		if (x == val) {
			printHex(sum);
			cout << " ";
		}
#endif
	}
#ifdef DEBUG
	if (x == val) cout << endl;
#endif
	return sum;
}

// Бесцикловая схема Горнера (массив коэффициентов)
FixPoint fxNoCyGornArr(FixPoint x) {
	FixPoint x2 = FIXMUL(x, x); // за скобки выносится x^2
	FixPoint sum = FIXMUL(fixCoef[4], x2) + fixCoef[3];
	sum = FIXMUL(sum, x2) + fixCoef[2];
	sum = FIXMUL(sum, x2) + fixCoef[1];
	sum = FIXMUL(sum, x2) + fixCoef[0];
	return sum;
}
FixPoint fxNoCyGornConst(FixPoint x) { // Бесцикловая схема Горнера с константами
	FixPoint x2 = FIXMUL(x, x);
	FixPoint sum = FIXMUL(DIV1_FACT9FP, x2) - DIV1_FACT7FP;
	sum = FIXMUL(sum, x2) + DIV1_FACT5FP;
	sum = FIXMUL(sum, x2) - DIV1_FACT3FP;
	sum = FIXMUL(sum , x2) + DIV1_FACT1FP;
	return sum;
}
FixPoint fxNoCyGornAsm(FixPoint x) { // Бесцикловая схема Горнера asm
	FixPoint sum;
	if (x == 0x00B50E40)
		sum = 5;
	_asm
	{
		; eax = x * x
		MOV		EAX, x
		MOV		ECX, EAX
		IMUL	ECX
		MOV		EAX, EDX
		SAL		EAX, 2
		; ecx = x2 = x * 2
		MOV		ECX, EAX
		; EAX = a[4] * x2
		MOV		EAX, 0B8EH; DIV1_FACT9FP
		IMUL	ECX
		MOV		EAX, EDX
		SAL		EAX, 2
		; EAX = a[4] * x2 + a[3]
		ADD		EAX, 0FFFCBFCCH; -DIV1_FACT7FP
		IMUL	ECX
		MOV		EAX, EDX
		SAL		EAX, 2
		; EAX = (a[3] * x ^ 2 + a[2])
		ADD		EAX, 00888889H; DIV1_FACT5FP
		; EAX = (a[3] * x2 + a[2])*x2
		IMUL	ECX
		MOV		EAX, EDX
		SAL		EAX, 2
		; EAX = (a[3] * x2 + a[2])*x2 + a[1]
		SUB		EAX, 0AAAAAB0H; DIV1_FACT3FP
		; EAX = ((a[3] * x2 + a[2])*x2 + a[1])*x2
		IMUL	ECX
		MOV		EAX, EDX
		SAL		EAX, 2
		; EAX = ((a[3] * x2 + a[2])*x2 + a[1])*x2 + a[0
		ADD		EAX, 40000000H; DIV1_FACT1FP
		MOV		sum,	EAX
	}
	return sum;
}

