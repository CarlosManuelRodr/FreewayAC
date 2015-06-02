#include <chrono>
#include <random>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <sstream>
#include <algorithm>
#include "Aux.h"
using namespace std;

/****************************
*                           *
*   Funciones auxiliares    *
*                           *
****************************/

bool aux_string_to_bool(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}
void aux_progress_bar(double progress)
{
    const int barWidth = 35;
    cout << "[";
    int pos = (int)(barWidth * progress);
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << int(progress * 100.0) << " %\r";
    cout.flush();
    if (progress == 1.0)
        cout << endl;
}
void aux_progress_bar(double val, double min, double max, double dt)
{
	double progress;
	if (val + dt > max)
		progress = 1.0;
	else
		progress = (val-min)/(max-min);

	const int barWidth = 35;
	cout << "[";
	int pos = (int)(barWidth * progress);
	for (int i = 0; i < barWidth; ++i)
	{
		if (i < pos) cout << "=";
		else if (i == pos) cout << ">";
		else cout << " ";
	}
	cout << "] " << int(progress * 100.0) << " %\r";
	cout.flush();
	if (progress == 1.0)
		cout << endl;
}


/****************************
*                           *
*  Generador de aleatorios  *
*                           *
****************************/

RandomAlgorithm RandomGen::m_ra = LCG;
std::mt19937 RandomGen::mt;

void RandomGen::SetAlgorithm(RandomAlgorithm ra)
{
	m_ra = ra;
}
void RandomGen::Seed(int seed)
{
	if (seed == -1)
		seed = std::chrono::system_clock::now().time_since_epoch().count();

	switch (m_ra)
	{
	case MT19937:
		mt.seed(seed);
		break;
	case LCG:
		srand(seed);
		break;
	};
}
int RandomGen::GetInt(int i)
{
	switch (m_ra)
	{
	case MT19937:
		return mt() % i;
		break;
	case LCG:
		return rand() % i;
		break;
	};
	return 0;
}
double RandomGen::GetDouble()
{
	switch (m_ra)
	{
	case MT19937:
		return (double)mt()/(double)mt.max();
		break;
	case LCG:
		return (double)rand()/(double)RAND_MAX;
		break;
	};
	return 0.0;
}


/****************************
*                           *
*        Argumentos         *
*                           *
****************************/
Args::Args()
{
    m_double_args.push_back(0.0);
    m_int_args.push_back(0);
    m_bool_args.push_back(false);
}
Args::Args(initializer_list<double> d_args, std::initializer_list<int> i_args,
           initializer_list<bool> b_args)
{
    m_double_args = d_args;
    m_int_args = i_args;
    m_bool_args = b_args;
}
double Args::GetDouble(const unsigned &i)
{
    if (i < m_double_args.size())
        return m_double_args[i];
    else

    {
        cout << "Error: Argumento double faltante. Indice: " << i << "." << endl;
        return 0.0;
    }
}
int Args::GetInt(const unsigned &i)
{
    if (i < m_int_args.size())
        return m_int_args[i];
    else
    {
        cout << "Error: Argumento int faltante. Indice: " << i << "." << endl;
        return 0;
    }
}
bool Args::GetBool(const unsigned &i)
{
    if (i < m_bool_args.size())
        return m_bool_args[i];
    else
    {
        cout << "Error: Argumento bool faltante. Indice: " << i << "." << endl;
        return false;
    }
}
void Args::SetDouble(const unsigned &i, double val)
{
    if (i < m_double_args.size())
        m_double_args[i] = val;
    else
        cout << "Error: Asignacion double invalida. Indice: " << i << ", valor: " << val << "." << endl;
}
void Args::SetInt(const unsigned &i, int val)
{
    if (i < m_int_args.size())
        m_int_args[i] = val;
    else
        cout << "Error: Asignacion int invalida. Indice: " << i << ", valor: " << val << "." << endl;
}
void Args::SetBool(const unsigned &i, bool val)
{
    if (i < m_bool_args.size())
        m_bool_args[i] = val;
    else
        cout << "Error: Asignacion bool invalida. Indice: " << i << ", valor: " << val << "." << endl;
}
void Args::operator=(const Args &arg)
{
    m_double_args = arg.m_double_args;
    m_int_args = arg.m_int_args;
    m_bool_args = arg.m_bool_args;
}
