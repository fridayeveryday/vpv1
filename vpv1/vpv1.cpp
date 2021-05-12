#define NOMINMAX
#include <time.h>
#include <iostream>
#include <intrin.h>
#include <windows.h>
#include <profileapi.h>
#include <math.h>
#include <functional>
#include <vector>
#include <fstream>
#include <algorithm>



using namespace std;

// n & 2 => x1 because 1(10) = 000010(2) 
bool myFunction(unsigned short n) {
	//cout << "x1x2x3 | x2!x3x4 | x1!x2 | x1!x2x3!x4 | x3x4\n";
	//     x1            x2            x3                 
	return (((n & 2) && (n & 4) && (n & 8))
		// |        x2            !x3           x4            
		|| ((n & 4) && !(n & 8) && (n & 16))
		//   |  x1      !x2 
		|| ((n & 2) && !(n & 4))
		//   |  x1         !x2        x3         !x4
		|| ((n & 2) && !(n & 4) && (n & 8) && !(n & 16))
		//   |  x3        x4
		|| ((n & 8) && (n & 16)));
}


unsigned long long counter = 1e6;

long fibRecursive(long n) {
	if (n < 2) return n;
	return fibRecursive(n - 1) + fibRecursive(n - 2);
}

long measureByClock(int n) {
	// measuring by clock
	long res = 0;
	clock_t start = clock();
	for (size_t i = 0; i < counter; i++)
	{
		res = fibRecursive(n);
	}
	clock_t end = clock();
	clock_t delta = end - start;
	double time = (delta * 1.0) / (CLOCKS_PER_SEC);
	time = (time * 1.0) / counter;
	time *= 1e9;
	std::cout.precision(0);
	std::cout << "Result for " << n << " is " << fixed << time << " by clock " << "\n";
	return res;
}

unsigned long long getFrequencyForTSC() {
	clock_t tclock = clock();
	while (clock() < tclock + 1); // ожидание конца начавшегося такта

	tclock = clock();
	unsigned long long  tsc = __rdtsc();
	while (clock() < tclock + 1); // ожидание конца начавшегося такта
	unsigned long long tscEnd = __rdtsc();
	unsigned long long tscDelta = tscEnd - tsc;// сколько тактов TSC прошло за один такт clock
	unsigned long long F1 = (tscDelta * CLOCKS_PER_SEC); // частота процессора

	tclock = clock();
	tsc = __rdtsc();
	while (clock() < tclock + 1); // ожидание конца начавшегося такта
	tscEnd = __rdtsc();
	tscDelta = tscEnd - tsc;// сколько тактов TSC прошло за один такт clock
	unsigned long long F2 = (tscDelta * CLOCKS_PER_SEC); // частота процессора


	return min(F1, F2);
}


long measureByTSC(int n) {

	//cout << "frequancy is " << F1 << endl;
	unsigned long long start;
	unsigned long long end;
	long res = 0;
	unsigned long long frequency = getFrequencyForTSC();
	start = __rdtsc();
	/*for (size_t i = 0; i < counter; i++)
	{*/
	res = fibRecursive(n);
	//}
	end = __rdtsc();
	unsigned long long deltaTSC = end - start;
	double delta = (deltaTSC * 1.0) / (frequency);
	delta *= 1e9;
	//cout.precision(0);
	cout << "Result for " << n << " is " << fixed << delta << " by TSC " << "\n";
	return res;
}

long measureByQPC(int n) {
	LARGE_INTEGER t_start, t_finish, freqQPC;
	QueryPerformanceFrequency(&freqQPC); // получаем частоту
	QueryPerformanceCounter(&t_start); // засекаем время старта CODE
	long res = 0;
	for (size_t i = 0; i < counter; i++)
	{
		res = fibRecursive(n);
	}
	QueryPerformanceCounter(&t_finish);
	//cout << "частота:" << freqQPC.QuadPart << endl;
	auto deltaQPC = t_finish.QuadPart - t_start.QuadPart;
	double delta = (deltaQPC * 1.0) / counter;
	delta = delta / freqQPC.QuadPart;
	delta *= 1e9;
	cout.precision(0);
	cout << "Result for " << n << " is " << fixed << delta << " by QPC " << "\n";
	return res;
}

void generateVectorValues(vector<double>& arr, int n) {
	for (size_t i = 0; i < n; i++)
	{
		arr[i] = i + 1;
	}
}

double statistic(vector<double>& arr, long long n, double& average, double& msd) {
	long totalSum = 0;
	for (size_t i = 0; i < n; i++)
	{
		totalSum += arr[i];
	}
	average = totalSum * 1.0 / n;
	double devSumSq = 0.0;
	for (size_t i = 0; i < n; i++)
	{
		devSumSq += (arr[i] - average) * (arr[i] - average);
	}

	msd = sqrt(devSumSq * 1.0 / n);
	return average;
}

//int sumVector(vector<int>)

struct RP
{
	double time;
	long long n;
};
void clockRP(vector<double>& vec) {
	vector<RP> results;
	long long n = 100;
	cout << "для clock\n";
	while (n < vec.size()) {
		double average = 0.0;
		double msd = 0.0;
		clock_t start = clock();
		average = statistic(vec, n, average, msd);
		clock_t end = clock();
		clock_t delta = end - start;
		double time = (delta * 1.0) / (CLOCKS_PER_SEC);
		cout.precision(5);
		RP rp;
		rp.n = n;
		rp.time = time;
		results.push_back(rp);
		n += 1;
		if (results.size() > 1 && results[results.size() - 1].time > 0 && results[results.size() - 2].time > 0) {
			cout << "n-1 = " << results[results.size() - 2].n << " time = " << results[results.size() - 2].time << " с";
			cout << endl;
			cout << "n   = " << results[results.size() - 1].n << " time = " << results[results.size() - 1].time << " с";
			cout << endl;
			return;
		}
	}

}

void tscRP() {
	unsigned long long start;
	unsigned long long end;
	unsigned long long frequency = getFrequencyForTSC();
	start = __rdtsc();
	end = __rdtsc();
	unsigned long long delta = end - start;
	double time = (delta * 1.0) / frequency;
	cout << "для TSC\n" << time << " с\n";
}

void QPCRP(vector<double>& vec) {
	LARGE_INTEGER t_start, t_finish, freqQPC;
	vector<RP> results;
	long long n = 1;
	cout << "для QPC\n";
	while (n < vec.size()) {
		double average = 0.0;
		double msd = 0.0;
		QueryPerformanceCounter(&t_start); // засекаем время старта CODE
		int a = 0;
		for (size_t i = 0; i < n; i++)
		{
			a++;
		}
		QueryPerformanceCounter(&t_finish);
		QueryPerformanceFrequency(&freqQPC); // получаем частоту
		auto deltaQPC = t_finish.QuadPart - t_start.QuadPart;
		double delta = (deltaQPC * 1.0);
		delta = delta / freqQPC.QuadPart;
		double time = delta;
		cout.precision(5);
		RP rp;
		rp.n = n;
		rp.time = time;
		results.push_back(rp);
		n += 1;
		if (results.size() > 1 && results[results.size() - 1].time > 0 && results[results.size() - 2].time > 0) {
			cout << "n-1 = " << results[results.size() - 2].n << " time = " << results[results.size() - 2].time << " с";
			cout << endl;
			cout << "n   = " << results[results.size() - 1].n << " time = " << results[results.size() - 1].time << " с";
			cout << endl;

			return;
		}
	}
}
string path = "E:\\programProjects\\C++\\vpv1\\";
void generateRandom4bitNum(int size) {
	ofstream out;
	out.open(path + "randoms.txt");
	for (size_t i = 0; i < size; i++)
	{
		out << rand() % 16 << endl;
	}
	out.close();
}

void read4bitNum(vector<int>& vec) {
	ifstream in;
	in.open(path + "randoms.txt");

	for (size_t i = 0; i < vec.size(); i++)
	{
		in >> vec[i];
	}
	in.close();
}


int rand4BitSize = 1e5;
void repeatabilityClock() {
	vector<double> results;
	vector<int> rand4bitNum(rand4BitSize);
	read4bitNum(rand4bitNum);
	int border = 1e5;
	while (true)
	{
		int ind = rand();
		clock_t start = clock();
		for (size_t j = 0, i = 0; j < border; j++, i++)
		{
			bool res = myFunction(rand4bitNum[ind]);
		}
		clock_t end = clock();
		if (end - start >= 200) {
			break;
		}
		border += 1e5;
	}
	for (size_t i = 0; i < 1000; i++)
	{
		clock_t start = clock();
		for (size_t j = 0; j < border; j++)
		{
			bool res = myFunction(rand4bitNum[i]);
		}
		clock_t end = clock();
		clock_t delta = end - start;
		results.push_back(delta);
	}
	cout << "граница для clock " << border << endl;
	double average = 0;
	double msd = 0;
	statistic(results, results.size(), average, msd);
	auto min_max = minmax_element(results.begin(), results.end());

	cout.precision(0);
	std::cout << fixed << "Повторяемость для clock:\n среднее  = " << average << " стандартное отклонение = " << msd
		<< " max = " << *min_max.second << " min = " << *min_max.first << endl << " процент отколнения среднеквадратичного от среднего " << (msd * 100 / average) << endl;

	ofstream out;
	out.open(path + "repClock.txt");
	out.precision(0);
	for (size_t i = 0; i < results.size(); i++)
	{
		out << fixed << results[i] << "\n";
	}
	out.close();
}

void repeatabilityTSC() {
	vector<double> results;
	vector<int> rand4bitNum(rand4BitSize);
	read4bitNum(rand4bitNum);
	int border = 1;
	while (true)
	{
		int ind = rand();
		unsigned long long start, end;
		start = __rdtsc();
		for (size_t j = 0; j < border; j++)
		{
			bool res = myFunction(rand4bitNum[ind]);
		}
		end = __rdtsc();
		unsigned long long delta = end - start;
		if (delta >= 200) {
			break;
		}
		border++;
	}
	cout << "граница для tsc " << border << endl;

	for (size_t i = 0; i < 1000; i++)
	{
		unsigned long long start;
		unsigned long long end;
		start = __rdtsc();
		for (size_t i = 0; i < border; i++)
		{
			bool res = myFunction(rand4bitNum[i]);
		}
		end = __rdtsc();
		unsigned long long deltaTSC = end - start;
		double time = deltaTSC * 1.0;
		results.push_back(time);
	}
	double average = 0;
	double msd = 0;
	statistic(results, results.size(), average, msd);
	auto min_max = minmax_element(results.begin(), results.end());
	cout.precision(0);
	std::cout << fixed << "Повторяемость для TSC:\n среднее = " << average << " стандартное отклонение = " << msd
		<< " max = " << *min_max.second << " min = " << *min_max.first << endl << " процент отколнения среднеквадратичного от среднего " << (msd * 100 / average) << endl;


	ofstream out;
	out.open(path + "repTSC.txt");
	out.precision(0);
	for (size_t i = 0; i < results.size(); i++)
	{
		out << fixed << results[i] << "\n";
	}
	out.close();
}

void repeatabilityQPC() {
	vector<double> results;
	vector<int> rand4bitNum(rand4BitSize);
	read4bitNum(rand4bitNum);
	int border = 10;
	while (true)
	{
		LARGE_INTEGER t_start, t_finish;
		int ind = rand();
		QueryPerformanceCounter(&t_start); // засекаем время старта CODE
		for (size_t j = 0; j < border; j++)
		{
			bool res = myFunction(rand4bitNum[ind]);
		}
		QueryPerformanceCounter(&t_finish);
		auto delta = t_finish.QuadPart - t_start.QuadPart;
		if (delta >= 200) {
			break;
		}
		border += 10;
	}
	cout << "граница для QPC " << border << endl;


	for (size_t i = 0; i < 1000; i++)
	{
		LARGE_INTEGER t_start, t_finish;
		QueryPerformanceCounter(&t_start); // засекаем время старта CODE
		for (size_t i = 0; i < border; i++)
		{
			bool res = myFunction(rand4bitNum[i]);
		}
		QueryPerformanceCounter(&t_finish);
		auto deltaQPC = t_finish.QuadPart - t_start.QuadPart;
		results.push_back(deltaQPC);
	}
	double average = 0;
	double msd = 0;
	statistic(results, results.size(), average, msd);
	auto min_max = minmax_element(results.begin(), results.end());

	cout.precision(0);
	cout << fixed << "Повторяемость для QPC:\n среднее = " << average << " стандартное отклонение = " << msd
		<< " max = " << *min_max.second << " min = " << *min_max.first << endl << " процент отколнения среднеквадратичного от среднего " << (msd * 100 / average) << endl;


	ofstream out;
	out.open(path + "repQPC.txt");
	out.precision(0);
	for (size_t i = 0; i < results.size(); i++)
	{
		out << fixed << results[i] << "\n";
	}
	out.close();
}

//void getApprox(vector<vector<double>>& x, double& a, double& b, int n) {
//	double sumx = 0;
//	double sumy = 0;
//	double sumx2 = 0;
//	double sumxy = 0;
//	for (int i = 0; i < n; i++) {
//		sumx += x[i][0];
//		sumy += x[i][1];
//		sumx2 += x[i][0] * x[i][0];
//		sumxy += x[i][0] * x[i][1];
//	}
//	a = (n * sumxy - (sumx * sumy)) / (n * sumx2 - sumx * sumx);
//	b = (sumy - a * sumx) / n;
//	return;
//}

int empiricalDelta = 41000;
void empiricalClock() {
	vector<vector<double>> timesAndPayloads;
	vector<double> times;
	vector<int> rand4bitNum(1e7);
	read4bitNum(rand4bitNum);
	for (size_t i = 1; i < 1001; i++)
	{
		clock_t start = clock();
		for (size_t j = 0; j < i * empiricalDelta; j++)
		{
			bool res = myFunction(rand4bitNum[i]);
		}
		clock_t end = clock();
		clock_t delta = end - start;
		double time = (delta * 1.0) / CLOCKS_PER_SEC;
		time *= 1e6;
		if (!time) {
			cout << "alert\n";
		}
		times.push_back(time);
		timesAndPayloads.push_back({ i * 1.0, time });
	}
	cout << times[times.size() - 1] << endl;
	double average, msd;
	statistic(times, times.size(), average, msd);
	auto min_max = minmax_element(times.begin(), times.end());
	cout.precision(0);
	cout << fixed << "Аппроксимация для clock:\n среднее время = " << average << " стандартное отклонение = " << msd
		<< " max = " << *min_max.second << " min = " << *min_max.first << endl << " процент отколнения среднеквадратичного от среднего " << (msd * 100 / average) << endl;

	double k = times[times.size() - 1] / times.size();
	cout << " Коэффициент k = " << k << endl;
	//vector<double> deltas;
	//for (size_t i = 0; i < timesAndPayloads.size(); i++)
	//{
	//	deltas.push_back(abs(timesAndPayloads[i][1] - i * k * empiricalDelta));
	//}
	//statistic(deltas, deltas.size(), average, msd);
	//min_max = minmax_element(deltas.begin(), deltas.end());
	//cout.precision(0);
	//cout << fixed << "Аппроксимация для clock:\n среднее ряда отклонений = " << average << " стандартное отклонение РО = " << msd
	//	<< " max = " << *min_max.second << " min = " << *min_max.first << endl << " процент отколнения среднеквадратичного от среднего " << (msd * 100 / average) << endl;

	ofstream out;
	out.open(path + "empClock.txt");
	out.precision(0);
	for (size_t i = 0; i < times.size(); i++)
	{
		out << fixed << times[i] << "\n";
	}
	out.close();
}

void empirircalTSC() {
	vector<double> times;
	vector<int> rand4bitNum(1e7);
	read4bitNum(rand4bitNum);
	//empiricalDelta = 100;
	for (size_t i = 1; i < 1001; i++)
	{
		unsigned long long start = __rdtsc();
		for (size_t j = 0; j < i * empiricalDelta; j++)
		{
			bool res = myFunction(rand4bitNum[i]);
		}
		unsigned long long end = __rdtsc();
		unsigned long long frequency = getFrequencyForTSC();
		unsigned long long deltaTSC = end - start;
		double time = (deltaTSC * 1.0) / frequency;
		time *= 1e9;
		times.push_back(time);
	}

	cout << times[times.size() - 1] << endl;
	double average, msd;
	statistic(times, times.size(), average, msd);
	auto min_max = minmax_element(times.begin(), times.end());
	cout.precision(0);
	cout << fixed << "Аппроксимация для TSC:\n среднее время = " << average << " стандартное отклонение = " << msd
		<< " max = " << *min_max.second << " min = " << *min_max.first << endl << " процент отколнения среднеквадратичного от среднего " << (msd * 100 / average) << endl;

	double k = times[times.size() - 1] / times.size();
	cout << " Коэффициент k = " << k << endl;

	ofstream out;
	out.open(path + "empTSC.txt");
	out.precision(0);
	for (size_t i = 0; i < times.size(); i++)
	{
		out << fixed << times[i] << "\n";
	}
	out << "k = " << k;
	out.close();
}

void empiricalQPC() {
	vector<double> times;
	vector<int> rand4bitNum(1e7);
	read4bitNum(rand4bitNum);
	LARGE_INTEGER t_start, t_finish, freqQPC;
	QueryPerformanceFrequency(&freqQPC); // получаем частоту
	for (size_t i = 1; i < 1001; i++)
	{
		QueryPerformanceCounter(&t_start); // засекаем время старта CODE
		for (size_t j = 0; j < i * empiricalDelta; j++)
		{
			bool res = myFunction(rand4bitNum[i]);
		}
		QueryPerformanceCounter(&t_finish);
		auto deltaQPC = t_finish.QuadPart - t_start.QuadPart;
		double time = (deltaQPC * 1.0) / freqQPC.QuadPart;
		time *= 1e9;
		times.push_back(time);
	}

	cout << times[times.size() - 1] << endl;
	double average, msd;
	statistic(times, times.size(), average, msd);
	auto min_max = minmax_element(times.begin(), times.end());
	cout.precision(0);
	cout << "Аппроксимация для QPC:\n среднее время = " << average << " стандартное отклонение = " << msd
		<< " max = " << *min_max.second << " min = " << *min_max.first << endl << " процент отколнения среднеквадратичного от среднего " << (msd * 100 / average) << endl;

	double k = times[times.size() - 1] / times.size();
	cout << " Коэффициент k = " << k << endl;

	ofstream out;
	out.open(path + "empQPC.txt");
	out.precision(0);
	//out.setf(ios::fixed);
	//out.setf(ios::showpoint);
	for (size_t i = 0; i < times.size(); i++)
	{
		out << fixed << times[i] << "\n";
	}
	out << "k = " << k;
	out.close();

}
int main()
{
	//generateRandom4bitNum(rand4BitSize);

	setlocale(LC_ALL, "Russian");
	cout << "menu: 0 - сделать все, 1 - фибоначчи, 2 - разрешающая способность, 3 - повторяемость результатов, 4 - эмпирическая аппроксимация\n";
	int menu;
	cin >> menu;
	if (menu == 1 || menu == 0) {
		int n = 10;
		measureByClock(n);
		measureByTSC(n);
		measureByQPC(n);
		cout << endl;

		int k = 40;
		counter = 10;
		measureByClock(k);
		measureByTSC(k);
		measureByQPC(k);
	}
	else if (menu == 2 || menu == 0) {
		cout << "разрешающая способность \n";
		const int n = 1e6;
		vector<double> vec(n);
		generateVectorValues(vec, n);

		clockRP(vec);
		tscRP();
		QPCRP(vec);

	}
	else if (menu == 3 || menu == 0) {
		cout << "повторяемость \n";
		repeatabilityTSC();
		repeatabilityClock();
		
		repeatabilityQPC();


	}
	else if (menu == 4 || menu == 0) {
		empiricalClock();
		empirircalTSC();
		empiricalQPC();
	}


	//vector<int> payload(1e6);



}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.k