//**************************************************************************************
// ������ ������������ ���������� ������� sin(x)/x = 1 - x^2/3! + x^4/5! - x^6/7! + ...
// vpv-lab2: ���� 2020:: nvnulstu@gmail.com 
//**************************************************************************************
#pragma once
#include "stdlib.h"
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

#define LEN_RES 19  // ����������� ���������� �������, � ������� ���� �������� ����� ���� ������
#define MAX_ERR (float)(1.0/(1 << LEN_RES)) // �������� ����������� ����� 2^(-LEN_RES)
#define LEN_POLINOM 5 // ���������� ����� 5 ������ ���� 
#define X_STEP ((float)1.0/(float)(1 << 24)) // 2^(-24) - ��� �������� ������� �������� float

// �������� ����������� 
#define FACT3 (float)(2 * 3)
#define FACT5 (float)(2 * 3 * 4 * 5)
#define FACT7 (float)(2 * 3 * 4 * 5 * 6 * 7)
#define FACT9 (float)(2 * 3 * 4 * 5 * 6 * 7 * 8 * 9)

// �������� �������� ������� ����������� (float)
#define DIV1_FACT3 (1./FACT3)
#define DIV1_FACT5 (1./FACT5)
#define DIV1_FACT7 (1./FACT7)
#define DIV1_FACT9 (1./FACT9)

typedef float(*FloatFunc)(float); // ��������� �� ������� ������������ ��������

typedef long FixPoint;

// � ������� ����� �������������� ����� 30 ���
#define FRACT_PART 30 
// 2^31 - ��������� ��� ��������� 31 ���� �������� � long, ����������� ����� � ������������� ������  
#define FACTOR (1 << FRACT_PART) 
#define FLOAT2FIX(x) (FixPoint)(x * FACTOR) // �������������� float � Fixed
#define FIX2FLOAT(x) ((float)x / FACTOR)   // �������������� Fixed �� float
#define FIXMUL(x,y) ((FixPoint)((__int64)x * (__int64)y >> FRACT_PART))     // ��������� ������������� � �++ ������� ������� ��������
// �������� �������� ������� ����������� (FixPointt)
#define DIV1_FACT1FP (FixPoint)FLOAT2FIX(1.0) // 1 � ������� FixPoint
#define DIV1_FACT3FP (FixPoint)FLOAT2FIX(DIV1_FACT3)
#define DIV1_FACT5FP (FixPoint)FLOAT2FIX(DIV1_FACT5)
#define DIV1_FACT7FP (FixPoint)FLOAT2FIX(DIV1_FACT7)
#define DIV1_FACT9FP (FixPoint)FLOAT2FIX(DIV1_FACT9)
#define TAB "\t"

// ��������� ������� � ������ �� ����� "������ - ������ TSC"
#define CPUID_RDTSC(t) \
	__asm xor eax, eax\
	__asm cpuid __asm rdtsc\
	__asm mov DWORD PTR[t], eax\
	__asm mov DWORD PTR[t + 4], edx

typedef FixPoint(*FixedFunc)(FixPoint); // ��������� �� ������� ������������ ����������

vector<float> arrX; // ������ ���������� ��� ����� ���������

float flMathFunc(float);
// ��������� ����� ��������� � ���������� �������������� �������
struct Result {
	string shortname;	// �������� ��� ������ ������ ������
	string name;		// ������������ ������ �������
	__int64 min, max;	// ������� � �������� ����� ������ ������ ������� (�� ����������)
	double avg, dev;	// ������� � ���
	double factor;		// ��������� avg ������� ������ � avg ������ �������� ������
	Result(string sn, string nm, __int64 minim, __int64 maxim, double average, double deviation) :
		shortname(sn), name(nm), min(minim), max(maxim), avg(average), dev(deviation) {};
};

class Report; // �����

struct Config {
	int count;			// ����� ������� � ����� �����
	int delMax, delMin;	// ����� ��������� ������������ � ����������� ��������� ��� ���������� ����������� ����� ���������
	int lenPrintLog;	// ����� ������ ����������� ����� ���������, ��������� �� �������
	int pass;			// ����� �������� ��� ������� �������������
	Report * rep;		// ��������� �� �����
	float maxErr;		// ����������� ���������� �����������
	float xStepVerify;	// ��� ������ �������� ��������� ��� ����������� float
	__int64 overhead;	// ������ ��������� �������� �� ��������� ������� � ������
	Config(int cnt, int maxd, int mind, int nPrint, int npass, Report * report, float err, float step, __int64 over)
		  :	count(cnt), delMax(maxd), delMin(mind), lenPrintLog(nPrint), pass(npass), rep(report), maxErr(err), 
			xStepVerify(step), overhead(over) {};
};