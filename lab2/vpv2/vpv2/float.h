//**************************************************************************************
// Пример исследования реализации функции sin(x)/x = 1 - x^2/3! + x^4/5! - x^6/7! + x^8/9!...
// sinx_divx: март 2020:: nvnulstu@gmail.com 
//**************************************************************************************
#pragma once
#include "vpv-lab2.h"
#include <iostream>
using namespace std;
// массив коэффициентов
float flCoef[LEN_POLINOM] = { 1., -DIV1_FACT3, DIV1_FACT5, -DIV1_FACT7, DIV1_FACT9,-DIV1_FACT11 };

// Факториал для int - не может иметь n > 12
int factorial(int n) {
	int fact = n;
	while (--n > 0) {
		fact *= n;
	}
	return fact;
}

// Библиотечная реализация функции на основе библиотеки math.h для измерения
float flMathFunc(float x) {
	return (float)sin(x);
}

// Цикл формулы ряда 1 - x^2/3! + x^4/5! - x^6/7! + x^8/9! через цикл sum += x^2n/(2n+1)!
float flCyNoGorn(float x) {
	float sum = 0.0;
	for (int n = 1; n < LEN_POLINOM; n++) {
		sum += (float)pow(-1, n - 1.0) * pow(x, 2.0 * n - 1) / factorial(2 * n - 1);
	}
	return sum;
}

// Непосредственная реализация формулы 1 - x^2/3! + x^4/5! - x^6/7! + x^8/9!
float flNoCyNoGorn(float x) {
	return x - pow(x, 3) / factorial(3) + pow(x, 5) / factorial(5) - pow(x, 7) / factorial(7) + pow(x, 9) / factorial(9) - pow(x, 11) / factorial(11);
}
// Цикл схемы Горнера
float flCycleGorn(float x) {
	float x2 = x * x, sum = 0.;
	std::cout << "total sum FLOAT: \n";
	cout.precision(20);
	for (int n = LEN_POLINOM; n > 0; n--) {
		sum = sum * x2 + flCoef[n - 1];
		FixPoint fixEtalon = FLOAT2FIX(sum);
		std::cout << fixEtalon << endl;
		//cout << sum << endl;
	}
	return sum * x;
}

// Бесцикловая схема Горнера классическая sin(x)/x = (((a[4]*x^2 + a[3])*x^2 + a[2])*x^2 + a[1])*x^2 + a[0]
float flNoCyGornArr(float x) {
	float x2 = x * x; // за скобки выносится x^2
	return (((((flCoef[5] * x2 + flCoef[4]) * x2 + flCoef[3]) * x2 + flCoef[2]) * x2 + flCoef[1]) * x2 + flCoef[0]) * x;
}

// Бесцикловая схема Горнера - константы вместо элементов массива
float flNoCyGornConst(float x) {
	float x2 = x * x;
	return (((((-DIV1_FACT11 * x2 + DIV1_FACT9) * x2 - DIV1_FACT7) * x2 + DIV1_FACT5) * x2 - DIV1_FACT3) * x2 + 1.0) * x;
		//x * ((float)1.0 - x2 * (DIV1_FACT3 + x2 * (DIV1_FACT5 - x2 * (DIV1_FACT7 + x2 * (DIV1_FACT9 - x2 * DIV1_FACT11)))));
}

