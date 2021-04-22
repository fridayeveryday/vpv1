#include <time.h>
#include <iostream>
#include <intrin.h>
#include <windows.h>
#include <profileapi.h>
#include <math.h>
#include <functional>
#include <vector>
#include <fstream>


using namespace std;

// n & 2 => x1 because 1(10) = 000010(2) 
bool myFunction(unsigned short n) {
	cout << "x1x2x3 | x2!x3x4 | x1!x2 | x1!x2x3!x4 | x3x4\n";
	//             x1                x2                x3                 
	return ((((n & 2) >> 1) && ((n & 4) >> 2) && ((n & 8) >> 3))
		//       |        x2                !x3                x4            
		|| (((n & 4) >> 2) && !((n & 8) >> 3) && ((n & 16) >> 4))
		//   |       x1                !x2 
		|| (((n & 2) >> 1) && !((n & 4) >> 2))
		//   |       x1                !x2                x3                !x4
		|| (((n & 2) >> 1) && !((n & 4) >> 2) && ((n & 8) >> 3) && !((n & 16) >> 4))
		//   |       x3                x4
		|| (((n & 8) >> 3) && ((n & 16) >> 4)));
}


unsigned long long counter = 1e6;

long fibRecursive(long n) {
	if (n < 2) return n;
	return fibRecursive(n - 1) + fibRecursive(n - 2);
}

long measureByClock(int n, std::function<long(unsigned __int64)> func) {
	// measuring by clock
	long res = 0;
	clock_t start = clock();
	for (size_t i = 0; i < counter; i++)
	{
		res = func(n);
	}
	clock_t end = clock();
	clock_t delta = end - start;
	double time = (delta * 1.0) / (CLOCKS_PER_SEC);
	time = (time * 1.0) / counter;
	time *= 1e9;
	cout.precision(0);
	cout << "Result for " << n << " is " << fixed << time << " by clock " << "\n";
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


long measureByTSC(int n, std::function<long(unsigned __int64)> func) {

	//cout << "frequancy is " << F1 << endl;
	unsigned long long start;
	unsigned long long end;
	long res = 0;
	unsigned long long frequency = getFrequencyForTSC();
	start = __rdtsc();
	for (size_t i = 0; i < counter; i++)
	{
		res = func(n);
	}
	end = __rdtsc();
	unsigned long long deltaTSC = end - start;
	double delta = (deltaTSC * 1.0) / (frequency * counter);
	delta *= 1e9;
	//cout.precision(0);
	cout << "Result for " << n << " is " << fixed << delta << " by TSC " << "\n";
	return res;
}

long measureByQPC(int n, std::function<long(unsigned __int64)> func) {
	LARGE_INTEGER t_start, t_finish, freqQPC;
	QueryPerformanceFrequency(&freqQPC); // получаем частоту
	QueryPerformanceCounter(&t_start); // засекаем время старта CODE
	long res = 0;
	for (size_t i = 0; i < counter; i++)
	{
		res = func(n);
	}
	QueryPerformanceCounter(&t_finish);
	cout << "частота:" << freqQPC.QuadPart << endl;
	auto deltaQPC = t_finish.QuadPart - t_start.QuadPart;
	double delta = (deltaQPC * 1.0) / counter;
	delta = delta / freqQPC.QuadPart;
	delta *= 1e9;
	cout.precision(0);
	cout << "Result for " << n << " is " << fixed << delta << " by QPC " << "\n";
	return res;
}

double statistic(vector<int>& arr, long long n, double& average, double& msd) {
	long totalSum = 0;
	for (size_t i = 0; i < n; i++)
	{
		totalSum += arr[i];
	}
	average = totalSum * 1.0 / n;
	double devSum = 0.0;
	for (size_t i = 0; i < n; i++)
	{
		devSum += arr[i] - average;
	}

	msd = sqrt(devSum * 1.0 / n);
	return average;
}

//int sumVector(vector<int>)
void generate(vector<int>& arr, int n) {
	for (size_t i = 0; i < n; i++)
	{
		arr[i] = i + 1;
	}
}
struct RP
{
	double time;
	long long n;
};
void clockRP(vector<int>& vec) {
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
		time *= 1e0;
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
	//time *= 1e9;
	cout << "для TSC\n" << time << " с\n";
}

double increamentVector(vector<int>& vec, int n) {
	for (size_t i = 0; i < n; i++)
	{
		vec[i]++;
	}
	return vec[n - 1] * 1.0;
}
void QPCRP(vector<int>& vec) {
	LARGE_INTEGER t_start, t_finish, freqQPC;
	vector<RP> results;
	long long n = 1;
	cout << "для QPC\n";
	while (n < vec.size()) {
		double average = 0.0;
		double msd = 0.0;
		QueryPerformanceCounter(&t_start); // засекаем время старта CODE
		//average = statistic(vec, n, average, msd);
		//average = increamentVector(vec, n);
		QueryPerformanceCounter(&t_finish);
		QueryPerformanceFrequency(&freqQPC); // получаем частоту
		auto deltaQPC = t_finish.QuadPart - t_start.QuadPart;
		cout << "частота:" << freqQPC.QuadPart << endl;
		double delta = (deltaQPC * 1.0);
		delta = delta / freqQPC.QuadPart;
		//delta *= 1e9;
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
void generateRandom4bitNum() {
	ofstream out;
	out.open(path + "randoms.txt");
	for (size_t i = 0; i < 10000; i++)
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
}

//void writeTimes2File() {
//	ofstream out;
//	out.open(path + "repeatabilityByClock");
//
//}
void repeatabilityClock() {
	vector<double> results;
	for (size_t i = 0; i < 1000; i++)
	{
		vector<int> rand4bitNum(10000);
		read4bitNum(rand4bitNum);
		clock_t start = clock();
		for (size_t i = 0; i < rand4bitNum.size(); i++)
		{
			bool res = myFunction(15);
		}
		clock_t end = clock();
		clock_t delta = end - start;
		double time = (delta * 1.0) / CLOCKS_PER_SEC;
		time /= rand4bitNum.size();
		//time *= 1e0;
		results.push_back(time);
	}



	cout.precision(5);
	cout << "повторяемость время " << time << endl;
}

void repeatabilityTSC() {
	vector<double> results;
	for (size_t i = 0; i < 1000; i++)
	{
		vector<int> rand4bitNum(10000);
		read4bitNum(rand4bitNum);

		unsigned long long start;
		unsigned long long end;
		unsigned long long frequency = getFrequencyForTSC();
		start = __rdtsc();
		for (size_t i = 0; i < rand4bitNum.size(); i++)
		{
			bool res = myFunction();
		}
		end = __rdtsc();
		unsigned long long deltaTSC = end - start;
		double time = (deltaTSC * 1.0) / frequency;
		time *= 1e9;
		results.push_back(time);

	}
}

void repeatabilityQPC() {
	vector<double> results;
	for (size_t i = 0; i < 1000; i++)
	{
		LARGE_INTEGER t_start, t_finish, freqQPC;
		QueryPerformanceFrequency(&freqQPC); // получаем частоту
		for (size_t i = 0; i < length; i++)
		{

		}
		QueryPerformanceCounter(&t_start); // засекаем время старта CODE
		bool res = myFunction(randVar);

		QueryPerformanceCounter(&t_finish);
		auto deltaQPC = t_finish.QuadPart - t_start.QuadPart;
		double time = (deltaQPC * 1.0) / freqQPC.QuadPart;
		time *= 1e9;

		results.push_back(time);

	}
}
int main()
{
	//generateRandom4bitNum();
	//vector<int> rand4bitNum(10000);
	//read4bitNum(rand4bitNum);
	setlocale(LC_ALL, "Russian");
	cout << "menu: 0 - сделать все, 1 - фибоначчи, 2 - разрешающая способность, 3 - повторяемость результатов\n";
	int menu;
	cin >> menu;
	if (menu == 1 || menu == 0) {
		int n = 10;

		measureByClock(n, fibRecursive);
		measureByTSC(n, fibRecursive);
		measureByQPC(n, fibRecursive);
		cout << endl;

		int k = 40;
		counter = 10;
		measureByClock(k, fibRecursive);
		measureByTSC(k, fibRecursive);
		measureByQPC(k, fibRecursive);
	}
	else if (menu == 2 || menu == 0) {
		cout << "разрешающая способность \n";
		const int n = 1e6;
		vector<int> vec(n);
		generate(vec, n);

		clockRP(vec);
		tscRP();
		QPCRP(vec);

	}
	else if (menu == 3 || menu == 0) {
		repeatabilityClock();
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