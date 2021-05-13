//*******************************************************************************************
// Пример исследования реализации функции sin(x)/x = 1 - x^2/3! + x^4/5! - x^6/7! + ...
// sinx_divx: март 2020:: nvnulstu@gmail.com 
// Каждый из релизов верифицируется и, если релиз имеет допустимую ошибку, 
// то выполняется серия замеров времени исполнения, на основе чего вычисляются 
// минимальное, максимальное, среднее значения и средне-квадратичное отклонение
// Вызов: sinx_divx [slen:N1] [dmin:N2] [dmax:N3] [npri:N4]
// где N1 - длина серии измерений; 
//     N2, N3 - число удаляемых минимальных и максимальных результатов серии при фильтрации; 
//     N4 - число распечатываемых первых результатов серии;
// вокруг двоеточия не должно быть пробелов
//*******************************************************************************************

#include "report.h"
#include "profiler.h"
#include "float.h"
#include "fixed.h"
#include "table.h"

using namespace std;

// Устанавливая в 0 следующие константы, можно отключать времязатратные фазы, не участвующие в отладке
#define GOFLOAT 1 // включение-выключение процессов испытания функций с плавающей точкой
#define GOFIX 1   // включение-выключение процессов испытания функций с фиксированной точкой
#define GOTABLE 1 // включение-выключение процессов испытания таблично-алгоритмических функций

Report report;
Config config( 
	500,		// число замеров в серии измерений
	10,			// число максимальных результатов в серии измерений, отбрасываемых при фильтрации
	5,			// число минимальных результатов в серии измерений, отбрасываемых при фильтрации
	10,			// число первых результатов серии измерений, выводимых на консоль
	3,			// число проходов для анализа повторяемости
	&report,	// адрес отчета, куда складываются результаты измерений
	MAX_ERR,	// максимально допустимая погрешность вычислений
	X_STEP,		// шаг обхода значений аргумента при верификации float 
	0			// накладные расходы на измерение времени (оценивается в init())
);

int main(int argc, char * argv[])
{
	init(argc, argv, config);
	// Создание массива объектов тестирования
	vector <Tester * > arr = {
#if GOFLOAT != 0
	new TestFloat("flMathFunc","Float - библиотечная реализация sin(x)/x", flMathFunc, config),
	new TestFloat("flCyNoGorn","Float - цикл формулы ряда", flCyNoGorn, config),
	new TestFloat("flNoCyNoGorn", "Float - безцикловая формула ряда", flNoCyNoGorn, config),
	new TestFloat("flCycleGorn", "Float - цикл схемы Горнера", flCycleGorn, config),
	new TestFloat("flNoCyGornArr", "Float - безцикловая схема Горнера(массив коэффициентов)", flNoCyGornArr, config),
	new TestFloat("flNoCyGornConst", "Float - безцикловая схема Горнера(константы)", flNoCyGornConst, config)
#if GOFIX + GOTABLE > 0 
	, 
#endif	
#endif	
#if GOFIX != 0
	new TestFixed("fxCycleGorn", "Fixed - цикл схемы Горнера", fxCycleGorn, config),
	new TestFixed("fxNoCyGornArr", "Fixed - безцикловая схема Горнера(массив коэффициентов)", fxNoCyGornArr, config),
	new TestFixed("fxNoCyGornConst", "Fixed - безцикловая реализация схемы Горнера (константы)", fxNoCyGornConst, config),
	new TestFixed("fxNoCyGornAsm", "Fixed - безцикловая реализация схемы Горнера (asm-вставка)", fxNoCyGornAsm, config)
#if GOTABLE	
	, 
#endif
#endif	
#if GOTABLE != 0
	new TestFloat("tableFunc0", "Прямой табличный метод", tableFunc0, config),
	new TestFloat("tableFunc1", "Таблично-алгоритмический метод - полином 1 степени", tableFunc1, config),
	new TestFloat("tableFunc2", "Таблично-алгоритмический метод - полином 2 степени", tableFunc2, config)
#endif
	};
	
	cout << endl << "Верификация..." << endl;
#if GOFIX != 0
	if (!testFixOperations(config)) // Сначала проверка правильности макросов преобразования между Fixed и float
		return 1;
#endif
	for (Tester * test : arr) test->verify();
	cout << endl << "Замеры времени ..." << endl;
	for(int n = 0; n < config.pass; n++) {
		cout << endl << "Проход " << (n + 1) << endl;
		if (config.lenPrintLog > 0)
			cout << "Первые " << config.lenPrintLog << " результатов замеров времени:" << endl;
		for (Tester * test : arr) test->timeSpent();
		report.calc();
		cout <<endl << "Итоги:" << endl;
		report.print();
		report.log.clear();
	}
	return 0;

}

