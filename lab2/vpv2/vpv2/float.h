//**************************************************************************************
// Пример исследования реализации функции sin(x)/x = 1 - x^2/3! + x^4/5! - x^6/7! + x^8/9!...
// sinx_divx: март 2020:: nvnulstu@gmail.com 
//**************************************************************************************
#pragma once
#include "vpv-lab2.h"

// массив коэффициентов
float flCoef[LEN_POLINOM] = { 1., -DIV1_FACT3, DIV1_FACT5, -DIV1_FACT7, DIV1_FACT9 };

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
	return (float)sin(x) / x;
}

// Цикл формулы ряда 1 - x^2/3! + x^4/5! - x^6/7! + x^8/9! через цикл sum += x^2n/(2n+1)!
float flCyNoGorn(float x) {
	float sum = 1.0;
	for (int n = 1; n < LEN_POLINOM; n++) {
		sum += (float)(pow(-1, n) * pow(x, n * 2) / factorial(2 * n + 1));
	}
	return sum;
}

// Непосредственная реализация формулы 1 - x^2/3! + x^4/5! - x^6/7! + x^8/9!
float flNoCyNoGorn(float x) {
	return 1 - pow(x, 2) / factorial(3) + pow(x, 4) / factorial(5) - pow(x, 6) / factorial(7) + pow(x, 8) / factorial(9);
}
// Цикл схемы Горнера
float flCycleGorn(float x) {
	float x2 = x * x, sum = 0.;
	for (int n = LEN_POLINOM; n > 0; n--)
		sum = sum * x2 + flCoef[n-1];
	return sum;
}

// Бесцикловая схема Горнера классическая sin(x)/x = (((a[4]*x^2 + a[3])*x^2 + a[2])*x^2 + a[1])*x^2 + a[0]
float flNoCyGornArr(float x) {
	float x2 = x * x; // за скобки выносится x^2
	return (((flCoef[4] * x2 + flCoef[3])*x2 + flCoef[2])*x2 + flCoef[1])*x2 + flCoef[0];
}

// Бесцикловая схема Горнера - константы вместо элементов массива
float flNoCyGornConst(float x) {
	float x2 = x * x;
	return (((x2 * DIV1_FACT9 - DIV1_FACT7) * x2 + DIV1_FACT5) * x2 - DIV1_FACT3) * x2 + (float)1.;
}

