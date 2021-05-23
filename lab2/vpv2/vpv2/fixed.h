//**************************************************************************************
// ������ ������������ ���������� ������� sin(x)/x = 1 - x^2/3! + x^4/5! - x^6/7! + ...
// sinx_divx: ���� 2020:: nvnulstu@gmail.com 
//**************************************************************************************
#pragma once

#include "vpv-lab2.h"
#include "string.h"
#include <sstream>
#include <iostream>


using namespace std;

// #define DEBUG // �����������������, ���� ����� �������� � ������� ����������� ���������� ����� �������

FixPoint fixCoef[LEN_POLINOM] = { DIV1_FACT1FP, -DIV1_FACT3FP, DIV1_FACT5FP, -DIV1_FACT7FP, DIV1_FACT9FP, -DIV1_FACT11FP };

/*void printHex(FixPoint x) {
	cout << hex << uppercase << setfill('0') << setw(8) << x;
}*/
// ������������ ������ ��������� ������������� FixPoint 32 ���� � lenFP ��������� ������� �����
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
// ������ ������������� ��� sin(x)/x: ����, ���� ��� ����� �����, 30 ��� ������� �����
// �.�  w[31]: ����, w[30]: 1, w[29]: 2^(-1), .. w[0]: 2^(-30)
bool testFixOperations(Config config) {
	char strbuf[34]; // ����� ��������� �����
	// ����������� �����: 1, 2^(-1), 2^(-2), 2^(-3), 2^(-30), -2^(-1), -2^(-2), -2^(-3), -2^(-30)
	float	flo[] = { 1., 0.5, 0.25, 0.125, (float)1. / (1 << 30), -0.5, -0.25, -0.125, -(float)1. / (1 << 30) };
	// �� �� ����� �����, ��� � ������� flo, �������������� � ������� fix � ������� FixPoint
	FixPoint fix[] = {
		0x40000000, // 01.000000..0 : 1
		0x20000000, // 00.100000..0 : 0.5 
		0x10000000, // 00.010000..0 : 0.25
		0x08000000, // 00.001000..0 : 0.125
		0x00000001, // 00.0000..001 : 2^(-30)
		// ��� ������������� ��� ��������������
		-0x20000000, // 11.100000..0 : -0.5 
		-0x10000000, // 11.110000..0 : -0.25 
		-0x08000000, // 11.111000..0 : -0.125 
		-0x00000001, // 11.1111..111 : 2^(-30)
	};
	int sz = sizeof(fix) / sizeof(FixPoint);
	// �������� FLOAT2FIX ����� ������������� fix[i] � FLOAT2FIX(flo[i]) 
	bool ok1 = true;
	for (int n = 0; n < sz; n++) {
		FixPoint fx = FLOAT2FIX(flo[n]);
		if (fx != fix[n]) { // 
			cout << "FLOAT2FIX(" << flo[n] << "):: " << fixPointToStr(fx, FRACT_PART)
				<< " != " << fixPointToStr(fix[n], FRACT_PART) << endl;
			ok1 = false;
		}
	}
	if (ok1 && config.lenPrintLog > 0)
		cout << "FLOAT2FIX(x) - OK" << endl;

	// �������� FIX2FLOAT ��� ������� fix[i] �������� float � ����������� � flo[i]
	bool ok2 = true;
	for (int n = 0; n < sz; n++) {
		float fl = FIX2FLOAT(fix[n]);
		if (fl != flo[n]) {
			cout << "FIX2FLOAT(" << fixPointToStr(fix[n], FRACT_PART) << "):: " << fl
				<< " != " << flo[n] << endl;
			ok2 = false;
		}
	}
	if (ok2 && config.lenPrintLog > 0)
		cout << "FIX2FLOAT(x) - OK" << endl;
	// �������� FIXMUL - ������������ ��������� ��������� fix ���������� � ��������� ��������� ������ ���������,
	// ���� �� �� ������ �� ������ X_STEP 
	bool ok3 = true;
	for (int n = 0; n < sz; n++) {
		for (int k = 0; k < sz; k++) {
			FixPoint mfix = FIXMUL(fix[n], fix[k]);
			float etalon = flo[n] * flo[k];
			if (fabs(FIX2FLOAT(mfix) - etalon) > X_STEP) {
				cout << "FIXMUL(" << fixPointToStr(fix[n], FRACT_PART) << ", "
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

// ���� ����� �������
// ��������� �������� ��������� DEBUG ��������� ��� ���������� ������� ����������� ����������
// ����� �������� ���������� ��������
FixPoint fxCycleGorn(FixPoint x) {
	FixPoint x2 = FIXMUL(x, x), sum = 0;
#ifdef DEBUG
	// � if ������ ���� ��������, �� ������� ����������� �������� ����������� ����������
	// ���� ��� ���������, �� �������� ������������� ����������� ����������, 
	//	��������� �������� � ����, ��� ���������� � ��������
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
	return FIXMUL(sum, x);
}

// ����������� ����� ������� (������ �������������)
FixPoint fxNoCyGornArr(FixPoint x) {
	FixPoint x2 = FIXMUL(x, x); // �� ������ ��������� x^2
	FixPoint sum = FIXMUL(fixCoef[5], x2) + fixCoef[4];
	sum = FIXMUL(sum, x2) + fixCoef[3];
	sum = FIXMUL(sum, x2) + fixCoef[2];
	sum = FIXMUL(sum, x2) + fixCoef[1];
	sum = FIXMUL(sum, x2) + fixCoef[0];
	return FIXMUL(sum, x);
}
FixPoint fxNoCyGornConst(FixPoint x) { // ����������� ����� ������� � �����������
	FixPoint x2 = FIXMUL(x, x);
	FixPoint sum = FIXMUL(-DIV1_FACT11FP, x2) + DIV1_FACT9FP;
	sum = FIXMUL(sum, x2) - DIV1_FACT7FP;
	sum = FIXMUL(sum, x2) + DIV1_FACT5FP;
	sum = FIXMUL(sum, x2) - DIV1_FACT3FP;
	sum = FIXMUL(sum, x2) + DIV1_FACT1FP;
	return FIXMUL(sum, x);
}
FixPoint fxNoCyGornAsm(FixPoint x) { // ����������� ����� ������� asm
	FixPoint sum = 0;
	if (x == 0x00B50E40)
		sum = 5;
	_asm
	{
		// ����������� ����� ������� (������ �������������)
		; FixPoint fxNoCyGornArr(FixPoint x) {
		/*	push        ebp
				mov         ebp, esp
				push        esi*/
				;	FixPoint x2 = FIXMUL(x, x); // �� ������ ��������� x^2
			mov         esi, dword ptr[x]
				mov         eax, esi
				imul        esi
				mov         ecx, eax
				shrd        ecx, edx, 1Eh
				;	FixPoint sum = FIXMUL(fixCoef[5], x2) + fixCoef[4];
			mov         edx, 0FFFFFFE6h
				mov         eax, ecx
				imul        edx
				shrd        eax, edx, 1Eh
				add         eax, 0B8Eh
				;	sum = FIXMUL(sum, x2) + fixCoef[3];
			imul        ecx
				shrd        eax, edx, 1Eh
				sub         eax, 34034h
				;	sum = FIXMUL(sum, x2) + fixCoef[2];
			imul        ecx
				shrd        eax, edx, 1Eh
				add         eax, 888888h
				;	sum = FIXMUL(sum, x2) + fixCoef[1];
			imul        ecx
				shrd        eax, edx, 1Eh
				sub         eax, 0AAAAAAAh
				;	sum = FIXMUL(sum, x2) + fixCoef[0];
			imul        ecx
				shrd        eax, edx, 1Eh
				add         eax, 40000000h
				;	FixPoint r = FIXMUL(sum, x);
			imul        esi
				;	//sum = sum + 4;
			;	//return FIXMUL(sum, x);
			;	int a = r;
			;	a += 2;
			;	return a;
			; pop         esi
				shrd        eax, edx, 1Eh
				; add         eax, 2
				; sub eax, 2
				mov sum, eax
				; pop ebp
	}
	return sum;
}

