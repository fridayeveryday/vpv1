//************************************************************************
// ����������� ����� ��������� ������ ������� ������������� ���������� 
// ���������� ������ ��������������
// ��� ������ ����� ����������� min, max, avg, ���, 
// nvnulstu@gmail.com - ��� 2020
#pragma once

#include "vpv-lab3.h"

#include <sstream>
#include <fstream>


// ��� ������������� �������, ���������� � Tester.measure() 
typedef double (*FuncThreadGranul)(int threads, int granul);
// ���������� ������, ���������� � �������������� ���������
class Log { 
public:
   vector<double> val;// �������� ����� ���������
   ResultOfSeries res;
   //int thread, granularity; // ����� ������� � �������������
   int count;			// ����� ����� ���������
   int delMax, delMin; // ����� ��������� ������������ � ����������� ���������
   void calc() {
      vector<double> fval = val; // ����������� ������ ����� ���������
      if (fval.size() > 0) {
         sort(fval.begin(), fval.end());
         // ���������� ����� ��2������ ������������ � ����������� �������� 
         for (auto n = delMax; n > 0 && fval.size() > 1; n--)
            fval.pop_back();
         for (auto n = delMin; n > 0 && fval.size() > 1; n--)
            fval.erase(fval.begin());
         res.min = *fval.begin();
         res.max = *(fval.end() - 1);
         auto sz = fval.size();
         // ���������� ����� � ��������
         double sum = 0;
         for (double el: fval) 
            sum += el;
         res.avg = sum / sz;
         // ���������� ���
         double dev = 0.;
         for (double el : fval) {
            double diff = (double)el - res.avg;
            dev += diff * diff;
         }
         dev = sqrt(dev/sz); // ���
         res.dev = (res.avg < 1.E-200)? 0. : (100 * dev) / res.avg; // ���%
      }
   }
};

// ������� ����� ����������� - ������ ������� 
class Tester {
public:	
	string name;		// ������������� ������������ ������ ���������� �������
	double (* calcParallel)(int, int); // ����� ����������� ������� 
	double maxErr;		// ��������� ���������� ������
	Log log;			// �������� conf.count ����������� ��������� � ��������� �������������� ���������
	int numFunc;		// ������ ������� � ������������ ����������
	Report * report;	// ����� ������, ���� ���������� ���������
	__int64 overhead;   // ��������� ������� �� ���������
	double etalon;		// ��������� �������� ��������� ��� �����������
	bool proper;		// ������� ���������� ��������������� ������������
	Tester(string nm, double (* func) (int, int), Config & conf, int nFunc)
		: name(nm), numFunc(nFunc), calcParallel(func), maxErr(conf.maxErr), report(conf.rep),  etalon(conf.etalon){
		log.count = conf.count;
		log.delMax = conf.delMax;
		log.delMin = 0;
		proper = false;
	}
	
	// ���������� ������ ����������� ��� ����������� ���������� ����� ������� � ������������ ����� �������������
	string calcError() { 
		ostringstream str;
		double err;
		str << name;
		err = calcParallel(4, 100) - etalon;
		if (fabs(err) > maxErr) {
				str << ": ������ = " << err;
				proper = false;
				return str.str();
			}
		proper = true;
		str << " - OK";
		return str.str();
	}
	void verify() { cout << calcError() << endl; }
	void measure() { // ������ ������� ��� repeat �������� x in [0, 1) � ����������� �����
		double t; // ������� �������
		double res; // ��������� ���������� ��� �������� ������
		cout << endl << name;
		for (int threads = 1; threads <= 12; threads++) {
			cout << endl << setw(WIDTH_COL) << threads;
			for (int gran = 0; gran < COUNT_GRANULARITY; gran++) {
				// ����� log.count ���������
				int subIntervals = report->arrGranularity[gran];
				log.res.threads = threads; 
				log.res.granularity = subIntervals;
				log.val.clear();
				for (int n = 0; n < log.count; n++) {
					t = omp_get_wtime();
					res = calcParallel(threads, subIntervals);
					log.val.push_back(omp_get_wtime() - t);
				} // �����
				log.res.error = res - etalon;
				log.calc();
				report->cubeResults[numFunc].name = name;
				report->cubeResults[numFunc].matrix[threads-1][gran] = log.res;
				cout << setw(WIDTH_COL) << fixed << setprecision(1) << MICROSEC(log.res.min);
			} // �������������
		} // ������
	}
};

