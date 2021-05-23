//**************************************************************************************
// ������ ������������ ������������� ���������� ������������� ��������� ������� sin(x)/x 
// �� ��������� [0;1]
// vpv-lab2: ���� 2020:: nvnulstu@gmail.com 
//**************************************************************************************
#pragma once
#include "stdlib.h"
#include <string>
#include <algorithm>
#include <vector>
#include <intrin.h>
#include <iostream>

using namespace std;

#define MAX_THREAD 12 // ������������ ����� �������
#define MIN_SUBINTERVALS 100 // ������� ����� ���������������
#define MAX_SUBINTERVALS 1000000 // �������� ����� ���������������
#define COUNT_GRANULARITY 5 // ����� �������������� 100, 1000, 10000, 100000, 1000000
#define COUNT_FUNCTION 4 // ����� ������������ ������� ������������ ���������
#define MAX_ERR 1.E-5 // ����������� ���������� ���������� �� ���������� ��������
#define WIDTH_COL 12  // ������ ������� � ������� �����������

// �������������� ������ � ������������
#define MICROSEC(s) ((s*1.E6)) 

// ��������� ����� ���������
struct ResultOfSeries {
	int threads, granularity; // ����� ������� � �������������, ��� ������� ����������� ����� ���������
	double min, max;	// ������� � �������� ������ ������� (�� ����������) � ��������
	double avg, dev;	// ������� � ���
	double error; // ������ ����������
	ResultOfSeries() {}
	/*ResultOfSeries(int th, int gran, __int64 minim, __int64 maxim, double average, double deviation, double err) :
		threads(th), granularity(gran), min(minim), max(maxim), avg(average), dev(deviation),  error(err) {};*/
};

// ��������� ��������� ��� ������� � ��������������� 
struct ResultOfFunction {
	ResultOfFunction() {}
	// ��� ������� - ������� �����������������
	string name; 
	// ������� thread * granularity ����������� ��������� ����� ���������
	ResultOfSeries matrix[MAX_THREAD][COUNT_GRANULARITY]; 
};

class Report; // �����

struct Config {
	int count;			// ����� ������� � ����� �����
	int delMax;			// ����� ��������� ������������ ��������� ��� ���������� ����������� ����� ���������
	Report * rep;		// ��������� �� �����
	double maxErr;		// ����������� ���������� �����������
	double etalon;		// ��������� �������� ���������
	Config(int cnt, int maxd, Report * report, double err, double et)
		  :	count(cnt), delMax(maxd), rep(report), maxErr(err), etalon(et) {};
};