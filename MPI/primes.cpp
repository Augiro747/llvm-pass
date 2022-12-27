#include <stdio.h>
#include <time.h>
#include "mpi.h"

#define RETURN return 0

void get_interval(int, int, int,int,int*,int*);
inline void print_simple_range(int, int,int);
void wait(int);

int main(int argc, char **argv)
{
	// инициализируем необходимые переменные
	int thread, thread_size, processor_name_length,r1,r2;
	int* thread_range;
	int interval[2];
	double cpu_time_start, cpu_time_fini;
	char* processor_name = new char[MPI_MAX_PROCESSOR_NAME * sizeof(char)];

	MPI_Status status;
	
	// Инициализируем работу MPI
	MPI_Init(&argc, &argv);
	
	// Получаем имя физического процессора
	MPI_Get_processor_name(processor_name, &processor_name_length);
	
	// Получаем номер конкретного процесса на котором запущена программа
	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	
	// Получаем количество запущенных процессов
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);
	
	// Если это первый процесс, то выполняем следующий участок кода
	if(thread == 0)
	{
		// Выводим информацию о запуске
		printf("----- Programm information -----\n");
		printf(">>> Processor: %s\n", processor_name);
		printf(">>> Num threads: %d\n", thread_size);
		printf(">>> Input the interval: ");

		// Просим пользователья ввести интервал на котором будут вычисления
		scanf("%d %d", &interval[0], &interval[1]);

		// Каждому процессу отправляем полученный интервал с тегом сообщения 0. 
		for (int to_thread = 1; to_thread < thread_size; to_thread++) 
   		   MPI_Send(&interval, 2, MPI_INT, to_thread, 0, MPI_COMM_WORLD);

		// Начинаем считать время выполнения
		cpu_time_start = MPI_Wtime();
	}
	// Если процесс не первый, тогда ожидаем получения данных
	else 
	{
	   MPI_Recv(&interval, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	// Все процессы запрашивают свой интервал
	get_interval(thread, thread_size, interval[0],interval[1],&r1,&r2);
	
	printf("thread:%d, intervals:%d , %d\n",thread,r1,r2);
	// После чего отправляют полученный интервал в функцию которая производит вычисления
	print_simple_range(r1, r2,thread);

	// Последний процесс фиксирует время завершения, ожидает 1 секунду и выводит результат
	if(thread == thread_size - 1)
	{
		cpu_time_fini = MPI_Wtime();
		wait(1);
		printf("CPU Time: %lf ms\n", (cpu_time_fini - cpu_time_start));
	}

	}
	MPI_Finalize();
	RETURN;
}

void get_interval(int proc, int size, int interval1, int interval2, int* r1, int* r2)
{
	// Функция для рассчета интервала каждого процесса
	int interval_size = (interval2 - interval1) / size;

	*r1= interval1 + interval_size * proc;
	*r2 = interval1 + interval_size * (proc + 1);
	*r2 = (*r2 == interval2 - 1) ? interval2 : *r2;
	
}

inline void print_simple_range(int ibeg, int iend,int thread_num)
{
	// Прострейшая реализация определения простого числа
	bool res;
	for(int i = ibeg; i <= iend; i++)
	{
		res = true;
		while(res)
		{
			res = false;
			for(int j = 2; j < i; j++) if(i % j == 0) res = true;
			if(res) break;
		}
		res = not res;
		if(res) printf("Simple value ---> %d, THREAD:%d\n", i,thread_num);

	}
}
void wait(int seconds)
 {
 	// Функция ожидающая в течение seconds секунд
	clock_t endwait;
	endwait = clock () + seconds * CLOCKS_PER_SEC ;
	while (clock() < endwait) {};
}
