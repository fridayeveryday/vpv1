// ****************************************************************************
// table.h - ��������� ���������� ��������-��������������� �������
// ������ 2020: nvnulstu@gmail.com
// ****************************************************************************
#pragma once
#include "float.h"

#define LEN_ADDR0 20 // ������ ������ ������� ��� �������� ������� �������
#define LEN_ADDR1 10  // ������ ������ ������� ��� �������� ������ �������
#define LEN_ADDR2 8  // ������ ������ ������� ��� �������� ������ �������
#define DX_DERIV  (1.0E-1-20) // dX ���������� ������������������ (����������� ������� � �����)

typedef float ETable0;  // ��� �������� ������� ��� �������� 1-� �������
typedef struct { float a0; float a1; } ETable1;  // ��� �������� ������� ��� �������� 1-� �������
typedef struct { float a0; float a1; float a2; } ETable2;  // ��� �������� ������� ��� �������� 2-� �������

ETable0 * table0;
ETable1 * table1;
ETable2 * table2;

// ��� ��������� ������ � ����� ��������� �������� ���������� double dmathFunc
double dmathFunc(double x) {
	return sin(x) / x;
}

// ������ ����������� � ����� x ��������� ������� �� ������ ����������� ���������
double firstDerivative(double x) {
	return cos(x) / x - dmathFunc(x) / x ;
}
// ������ ����������� � ����� x ��������� �������
double secondDerivative(double x) {
	return -dmathFunc(x) - 2 * cos(x) / (x * x) + 2 * sin(x) / (x * x * x);
}

// ������� ������� ������� ����������� - ������ ��������� �����
ETable0 * genTable0() {
	int sizeTable = (1 << LEN_ADDR0); // ������ ������� = 2^lenAddr
	ETable0 * table = new ETable0[sizeTable];
	float step = (float)1./sizeTable; // sizeTable �������� ���������� ��������� �������� ��������� x in [0;1)
	for (int n = 0; n < sizeTable; n++)
		table[n] = (float)dmathFunc(step * n);
	return table;
}
float tableFunc0(float x) {
	return table0[(int)(x * (1 << LEN_ADDR0))];
}

// ������� ������ ������� a0 + a1 * x
ETable1 *genTable1() {
	int sizeTable = (1 << LEN_ADDR1); // ������ ������� = 2^lenAddr
	ETable1 * table = new ETable1[sizeTable];
	float step = (float)1. / sizeTable; // sizeTable �������� ���������� ��������� �������� ��������� x in [0;1)
	for (int n = 0; n < sizeTable; n++) {
		double x = step * n;
		table[n].a0 = (float)dmathFunc(x);
		table[n].a1 = (float)firstDerivative(x);
	}
	return table;
}
float tableFunc1(float x) {
	register int addr = (int)(x * (1 << LEN_ADDR1));
	register ETable1 * pcoef = table1 + addr;
	float dx = x - (float)addr * ((float)1./ (1 << LEN_ADDR1));
	return pcoef->a0 + pcoef->a1 * dx;
}

// ������� ������ ������� a0 + a1 * x + a2 * x^2
ETable2 * genTable2() { // ��������� �������
	int sizeTable = (1 << LEN_ADDR2); // ������ ������� = 2^lenAddr
	ETable2 * table = new ETable2[sizeTable];
	float step = (float)1. / sizeTable; // sizeTable �������� ���������� ��������� �������� ��������� x in [0;1)
	for (int n = 0; n < sizeTable; n++) {
		double x = step * n;
		table[n].a0 = (float)dmathFunc(x);
		table[n].a1 = (float)firstDerivative(x);
		table[n].a2 = (float)secondDerivative(x)/2;
	}
	return table;
}
float tableFunc2(float x) {
	register int addr = (int)(x * (1 << LEN_ADDR2));
	register ETable2 * pcoef = table2 + addr;
	float dx = x - (float)addr * ((float)1. / (1 << LEN_ADDR2));
	return (pcoef->a2 * dx + pcoef->a1) * dx + pcoef-> a0;
}


