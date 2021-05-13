#pragma once

#include "vpv-lab2.h"
#include "fixed.h"
#include <sstream>

using namespace std;

typedef float(*PFloatFu)(float);
typedef FixPoint(*PFixFunc)(FixPoint);

// ���������� ������, ���������� � �������������� ���������
class Log { 
public:
	vector<__int64> val;// �������� ����� ���������
	int count;			// ����� ����� ���������
	int delMax, delMin; // ����� ��������� ������������ � ����������� ���������
	int lenPrintLog;	// ����� ������ ����������� ���������, ��������� �� �����
	int xStepVerify;	// ����� ���������� �������� ����� ����� � ���������� 
	__int64 min, max;	// �������, ��������
	double	avg, dev;	// �������, ���
	void calc() {
		vector<__int64> fval = val; // ����������� ������ ����� ���������
		if (fval.size() > 0) {
			sort(fval.begin(), fval.end());
			min = *fval.begin();
			max = *(fval.end() - 1);
			// ���������� ����� �������� ������������ � ����������� �������� 
			for (auto n = delMax; n > 0 && fval.size() > 1; n--)
				fval.pop_back();
			for (auto n = delMin; n > 0 && fval.size() > 1; n--)
				fval.erase(fval.begin());
			auto sz = fval.size();
			// ���������� ����� � ��������
			__int64 sum = 0;
			for (__int64 el : fval) sum += el;
			avg = (double)sum / sz;
			// ���������� ���
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

// ������� ����� ����������� - ������ ������� 
class Tester {
public:	
	string shortname;	// �������� ��� ������ ������ ������
	string name;		// ������������� ������������ ������ ���������� �������
	float err;			// ��������� ���������� ������
	float xStep;		// ��� ������ �������� ���������� ��� ����������� �������
	Log log;			// �������� conf.repeat ����������� ��������� � ��������� �������������� ���������
	Report * report;	// ����� ������, ���� ���������� ���������
	__int64 overhead;   // ��������� ������� �� ���������
	bool proper;			// ��������� �����������
	Tester(string sn, string nm, Config & conf)
		: shortname(sn), name(nm), err(conf.maxErr),  report(conf.rep), xStep(conf.xStepVerify), overhead(conf.overhead) {
		log.count = conf.count;
		log.delMax = conf.delMax;
		log.delMin = conf.delMin;
		log.lenPrintLog = conf.lenPrintLog;
		proper = false;
	}
	void saveResult() { // ���������� ����������
		report->log.push_back(Result(shortname, name, log.min, log.max, log.avg, log.dev));
	}
	virtual string test() { return ""; }
	void verify() { cout << test() << endl; }
	virtual void measure() {}
	void timeSpent() {
		if (proper) {	// ����	�������������� ���� �������
			measure();	// ��	��������� repeat ��������� ������ ������� 
			log.print(shortname);
			log.calc();		//		���������� �������� ����� ���������
			saveResult();	//		��������� ���������� � report
			log.val.clear();
		}
	}
};

// ����� ��� float
class TestFloat : public Tester {
public:
	FloatFunc func; // ����� ����������� �������
	TestFloat(string sn, string nm, FloatFunc fu, Config & conf) :
	Tester(sn, nm, conf), func(fu) {}
	string test() { // �������� ������������ ������� ��� ���� �������� ������� ��� ������� �������
		ostringstream str;
		//str << shortname;
		for (float x = 0.; x < 1.; x += xStep) {
			float etalon = flMathFunc(x);
			float real = func(x);
			if (fabs(etalon - real) > err) {
				str <<  ": ������ func(" << x << ") = " << real << " != " << etalon;
				proper = false;
				return str.str();
			}
		}
		proper = true;
		str << shortname << " - OK";
		return str.str();
	}
	void measure() { // ������ ������� ��� repeat �������� x in [0, 1) � ����������� �����
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

// ������ ��� ������������� �����
class TestFixed : public Tester {
public:
	FixedFunc func; // ����� ����������� �������
	TestFixed(string sn, string nm, FixedFunc fu, Config & conf) :
	Tester(sn, nm, conf), func(fu) {}
	string test() { // �������� ������������ ������� ��� ���� ������������� �������� Fixed 
		ostringstream str;
		str << shortname;
		FixPoint step = FLOAT2FIX(xStep);
		for (FixPoint x = 0; x < DIV1_FACT1FP; x += step) {
			float etalon = (float)flMathFunc(FIX2FLOAT(x)); // ������ float
			FixPoint fixEtalon = FLOAT2FIX(etalon);
			FixPoint r = func(x);	// �������� fixed
			float real = FIX2FLOAT(r); // ���������� �������� � float, ����� �������
			if (fabs(etalon - real) > err) {
				str	<< ": ������ func(" << hex << uppercase << setfill('0') << setw(8) << x << "): "
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
	void measure() { // ������ ������� ��� repeat �������� x in [0, 1) � ����������� �����
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

