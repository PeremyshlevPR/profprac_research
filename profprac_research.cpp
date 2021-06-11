#include <iostream>
#include <vector>
#include <cmath>
#include "plant.h"


using namespace std;

void enter_data(vector<int>& M, int& H, int& P, double& Y, size_t& N, vector<double>& A, size_t k, double& min, double& max, double& delta, size_t& a)
{
	cerr << "Номера опрашиваемых датчиков: ";
	for (size_t i = 1; i <= k; i++)
	{
		cerr << "M[" << i << "] = "; cin >> M[i];
	}

	cerr << "Номер датчика, измеряющего особо важную величину: "; cin >> H;


	cerr << "Номер канала, по которому на объект будет подаваться управляющее воздействие: "; cin >> P;


	cerr << "Номинальное значение отклика: "; cin >> Y;


	cerr << "Число тактов выработки управляющих воздействий: "; cin >> N;

	cerr << "По какому значению будет рассматриваться зависимость A[0] или A[1]? (Введите 0 или 1)";

		bool correct = false;
		while (!correct)
		{
			cin >> a;
			switch (a)
			{
			case 0:
			{
				a = 0;
				correct = true;
				break;
			}
			case 1:
			{
				a = 1;
				correct = true;
				break;
			}
			default:
				cerr << "Некорректное значение";
			}
		}

		cerr << "Введите минимальное значение коэффицента А[" << a << "] ";
		cin >> min;

		cerr << "Введите максимальное значение коэффицента А[" << a << "] ";
		cin >> max;

		cerr << "Введите значение шага, с которым будет изменяться А[" << a << "]";
		cin >> delta;

		cerr << "Введите остальные коэффиценты: " << endl;
		if (a == 0)
		{
			A[0] = min;
			cerr << "A[1] = "; 
			cin >> A[1];
		}
		else
		{
			A[1] = min;
			cerr << "A[0] = "; 
			cin >> A[0];
		}

	for (size_t i = 2; i <= k; i++)
	{
		cerr << "A[" << i << "] = "; cin >> A[i];
	}
}

double calc_of_C(double Yn, vector<double> Y)
{
	double C = 0;
	for (size_t i = 0; i < Y.size(); i++)
	{
		C += pow((Y[i] - Yn), 2);
	}
	return C;
}



int main()
{
	setlocale(LC_ALL, "rus");

	//Инициализация ОУ
	Plant plant;
	plant_init(plant);

	//Шаг 1 ввод исходных данных
	size_t k;
	cerr << "Количество оправшиваемых датчиков: "; cin >> k;
	vector<int> M(k + 1);
	int H, P;
	double Yn;
	size_t N;
	double min, max, delta;
	vector<double> A(k + 1);
	size_t a;
	enter_data(M, H, P, Yn, N, A, k, min, max, delta, a);

	//шаг 2 опрос каналов
	vector<double> Y(N);

	cout << "  №	A[" << a << "]	C1" << endl;

	size_t cycle_count = ((max - min) / delta ) + 1;
	for (int m = 1; m <= cycle_count; m++)
	{
		
		for (int i = 0; i < N; i++)
		{

			vector<double> X(k + 1);
			for (size_t j = 1; j <= k; j++)
			{
				int in_channel = M[j];
				X[j] = plant_measure(in_channel, plant);
			}

			//шаг 3 расчет величины управляющего воздействия
			double U = A[0];
			for (size_t j = 1; j <= k; j++)
			{
				U += (A[j] * X[j]);
			}

			//шаг 4 применение коэффицента U на канал с номером Р
			int out_channel = P;
			double value = U;
			plant_control(out_channel, value, plant);

			//шаг 5 измерим значение отклика Y на канале Н
			int in_channel = H;
			Y[i] = plant_measure(in_channel, plant);
		}

		// Расчет и вывод суммы С
		double C = 0;
		C = calc_of_C(Yn, Y);
		cout << "  " << m << "\t" << A[a] << "\t" << C / N << endl;
		A[a] += delta;
	}
}