// *******************************************************************
// Параллельные реализации численного метода вычисления интеграла
// функции sin(x)/x
// integralThreadLock - потоки создаются, сразу исполняются 
//     и локальная сумма подсуммируется к глобальной сумме с блокировкой внутри потока 
// integralThreadStLock - потоки создаются вне профилирования, затем запуск 
//     и каждый поток сам подсуммирует свою локальную сумму к глобальной с блокировкой
// integralThreadStNoLock - потоки создаются вне профилирования, затем запуск 
//     и каждый поток сам подсуммирует свою локальную сумму к глобальной с блокировкой
// integralOpenMP - потоки создаются и исполняются через библиотеку openMP директивой 
//     #pragma omp parallel for private(x) reduction(+:sum)
// nvnulstu@gmail.com - апрель 2021
// *******************************************************************
#pragma once
#include "vpv-lab3.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <thread>
#include <mutex>
#include <omp.h>


using namespace std;

// Определенный интеграл функции sin(x)/x на интервале 0..1 через длинный ряд
// f(x) = sin(x) / x = 1 - x^2/3! + x^4/5! - x^6/7! + ... + (-1)^n * x^(2n+1) / (2n+1)! ...
// Первообразная(x) = F(x) = x - x^3 / (3*3!) + x^5 / (5*5!) -x^7/(7*7!) + ... + (-1)^n * x^(2n+1) / ((2n+1)*(2n+1)!) + ... 
// Интеграл(0, 1) = F(1) - F(0) = 1 - 1/(3*3!) + 1 / (5*5!) -1/(7*7!) + ... + (-1)^n / ((2n+1)*(2n+1)!)
double calcEtalon () {
    return 1.0 - cos(1.0);
}

// Вычисление интеграла через библиотеку thread 
double globSum; // глобальная сумма высот прямоугольников от всех потоков
double arrSum[MAX_THREAD]; // cуммы от отдельных потоков 

mutex mu; // мьютекс для управления доступом к globSum
// Функция суммирования высот прямоугольников,  с шириной wRect 
// Сумма добавляется к глобальной с блокировкой
void fuSumLock(int nRect, int thRect, double wRect) {
   double x, sum = 0.0;
   thRect += nRect;
   // цикл суммирования высот средних прямоугольников
   do {
      x = nRect * wRect + 0.5 * wRect;
      sum += sin(x);
   } while (++nRect < thRect);
   mu.lock();
   globSum = globSum + sum;
   mu.unlock();
};

// Функция суммирования высот прямоугольников,  с шириной wRect 
// сумма пишется по адресу pResult
void fuSumNoLock(int nRect, int thRect, double wRect, double* pResult) {
   double x, sum = 0.0;
   thRect += nRect;
   // цикл суммирования высот средних прямоугольников
   do {
      x = nRect * wRect + 0.5 * wRect;
      sum += sin(x);
   } while (++nRect < thRect);
   *pResult = sum;
};

// Создание и исполнение nTreads потоков с раскладкой по ним allRect прямоугольников
// Каждый поток подсуммирует свой результат к глобальной сумме globSum
double integralThreadLock(int nThreads, int allRect) {	
    double wRect = 1.0 / (double)allRect; // ширина прямоугольника численного метода
    // число прямоугольников для одного потока
    int thRect = (allRect - 1) / nThreads + 1; 
    // корректировка числа потоков в случае, когда их больше, чем allRect
    nThreads = (allRect - 1) / thRect + 1; 
    int restRect = allRect - (nThreads - 1) * thRect;
    if (restRect == 0) restRect = thRect; // когда allRect кратно nThreads
    globSum = 0.0; 
    // Создаем nThreads потоков
    thread * threads = new thread[nThreads];
    for (int n = 0; n < nThreads; n++)
       threads[n] = thread(fuSumLock, n * thRect, n < nThreads - 1 ? thRect : restRect, wRect);
    for (int n = 0; n < nThreads; n++) 
       threads[n].join();
    return globSum * wRect; // возврат площади всех средних прямоугольников
}

// Использование уже созданных nTreads потоков с раскладкой по ним allRect прямоугольников
thread threadsStatic[MAX_THREAD];

double integralThreadStNoLock(int nThreads, int allRect) {
    double wRect = 1.0 / (double)allRect; // ширина прямоугольника численного метода
// число прямоугольников для одного потока
    int thRect = (allRect - 1) / nThreads + 1;
    // корректировка числа потоков в случае, когда их больше, чем allRect
    nThreads = (allRect - 1) / thRect + 1;
    int restRect = allRect - (nThreads - 1) * thRect;
    if (restRect == 0) restRect = thRect; // когда allRect кратно nThreads
    globSum = 0.0;
    // Создаем nThreads потоков
    for (int n = 0; n < nThreads; n++)
       threadsStatic[n] = thread(fuSumNoLock, n * thRect, 
          n < nThreads - 1 ? thRect : restRect, wRect, arrSum + n); 
    for (int n = 0; n < nThreads; n++) 
        threadsStatic[n].join();
    globSum = 0.0;
    for (int n = 0; n < nThreads; n++) 
       globSum += arrSum[n];
    return globSum * wRect; // возврат площади всех средних прямоугольников
}

double integralThreadStLock(int nThreads, int allRect) {
	double wRect = 1.0 / (double)allRect; // ширина прямоугольника численного метода
// число прямоугольников для одного потока
	int thRect = (allRect - 1) / nThreads + 1;
	// корректировка числа потоков в случае, когда их больше, чем allRect
	nThreads = (allRect - 1) / thRect + 1;
	int restRect = allRect - (nThreads - 1) * thRect;
	if (restRect == 0) restRect = thRect; // когда allRect кратно nThreads
	globSum = 0.0;
	// Создаем nThreads потоков
	for (int n = 0; n < nThreads; n++)
		threadsStatic[n] = thread(fuSumLock, n * thRect, n < nThreads - 1 ? thRect : restRect, wRect);
	for (int n = 0; n < nThreads; n++) 
        threadsStatic[n].join();
	return globSum * wRect; // возврат площади всех средних прямоугольников
}

// Вычисление интеграла через библиотеку openMP 
//   numTreads потоками с разбиением интервала 0..1 на numSubIntervals подинтервалов
double integralOpenMP(int numTreads, int numSubIntervals) {
	int i;
	double x;
	double sum = 0.0;
	double wRect = 1.0 / (double)numSubIntervals;
	omp_set_num_threads(numTreads);
#pragma omp parallel for private(x) reduction(+:sum)   
	for (i = 0; i < numSubIntervals; i++)
	{	x = i * wRect + wRect / 2;
		sum += sin(x);
	}
	return sum * wRect;
}

