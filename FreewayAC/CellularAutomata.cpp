#include "CellularAutomata.h"
#include <limits>
#include <algorithm>
using namespace std;

/****************************
*                           *
*   Funciones auxiliares    *
*                           *
****************************/

MTRand m_drand;            // Generador de aleatorios (flotantes) entre 0 y 1.
MTRand_int32 m_irand;      // Generador de enteros aleatorios.

void aux_random_seed()
{
    // Asigna semilla a numeros aleatorios.
    unsigned seed = static_cast<unsigned int>(time(NULL));
    m_drand.seed(seed);
    m_irand.seed(seed);
}
int aux_random(int &i)
{
    return m_irand() % i;
}
void aux_progress_bar(double progress)
{
    int barWidth = 35;

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

////////////////////////////////////
//                                //
//        Autómatas celulares     //
//        de carril único.        //
//                                //
////////////////////////////////////


/****************************
*                           *
*         AC Básico         *
*                           *
****************************/

CellularAutomata::CellularAutomata(const unsigned &size, const double &density, const int &vmax, const double &rand_prob)
{
    // Inicializa variables.
    m_test = false;
    m_size = size;
    m_vmax = vmax;
    m_rand_prob = rand_prob;
    m_ca.assign(size, -1);
    m_ca_temp.assign(size, -1);
    m_ca_history.clear();
    m_ca_flow_history.clear();


    // Verifica argumentos.
    double l_density = density;
    if (l_density < 0.0 || l_density > 1.0)
    {
        l_density = 0.1;
        cout << "Densidad invalida. Cambiando a density=" << l_density << "." << endl;
    }
    if (m_vmax < 1)
    {
        m_vmax = 1;
        cout << "Velocidad limite invalida. Cambiando a vmax=" << m_vmax << "." << endl;
    }
    if (m_rand_prob < 0.0 || m_rand_prob > 1.0)
    {
        m_rand_prob = 0.0;
        cout << "Probabilidad de frenado invalida. Cambiando a rand_prob=" << m_rand_prob << "." << endl;
    }
    unsigned vehicles = (unsigned)(((double)size)*l_density);

    // Asigna semilla a numeros aleatorios.
    aux_random_seed();

    // Coloca autos al azar.
    vector<unsigned> car_positions;
    for (unsigned i=0; i<m_size; ++i)
        car_positions.push_back(i);

    random_shuffle(car_positions.begin(), car_positions.end(), aux_random);
    for (unsigned i=0; i<vehicles; ++i)
        m_ca[car_positions[i]] = 1;

    m_ca_history.push_back(m_ca);
}
CellularAutomata::CellularAutomata(const vector<int> &ca, const vector<bool> &rand_values, const int &vmax)
{
    // Inicializa variables.
    m_test = true;
    m_ca = ca;
    m_rand_values = rand_values;
    m_size = m_ca.size();
    m_vmax = vmax;
    m_rand_prob = 0;
    m_ca_temp.assign(m_size, -1);
    m_ca_history.clear();
    m_ca_flow_history.clear();
    m_ca_history.push_back(m_ca);
}
CellularAutomata::~CellularAutomata() {}
void CellularAutomata::Print()
{
    // Imprime valores del automata celular en la terminal.
    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        if (m_ca[i] == -1)
            cout << "-";
        else
            cout << m_ca[i];
    }
    cout << endl;
}
void CellularAutomata::PrintHistory()
{
	for (unsigned i=0; i<m_ca_history.size(); i++)
	{
		for (unsigned j=0; j<m_size; j++)
		{
			if (j != m_size -1)
			{
				if (m_ca_history[i][j] == -1)
					cout << " , ";
				else
					cout << m_ca_history[i][j] << ", ";
			}
			else
			{
				if (m_ca_history[i][j] == -1)
					cout << " ";
				else
					cout << m_ca_history[i][j];
			}
		}
		cout << endl;
	}
}
void CellularAutomata::DrawHistory()
{
    unsigned height = m_ca_history.size();
    unsigned width = m_size;
    BMPWriter writer("ca.bmp", width, height);
    BMPPixel* bmpData = new BMPPixel[width];
    for (int i=height-1; i>=0; i--)
    {
        for (unsigned j=0; j<width; ++j)
        {
            BMPPixel color;
            if (m_ca_history[i][j] == -1)
                color = BMPPixel((char)255, (char)255, (char)255);
            else
                color = BMPPixel(0, 0, (char)(255.0*(double)m_ca_history[i][j]/(double)m_vmax));
            bmpData[j] = color;
        }
        writer.WriteLine(bmpData);
    }
    writer.CloseBMP();
    delete[] bmpData;
}
void CellularAutomata::DrawFlowHistory()
{
    unsigned height = m_ca_flow_history.size();
    unsigned width = m_size;
    BMPWriter writer("ca_flow.bmp", width, height);
    BMPPixel* bmpData = new BMPPixel[width];
    for (int i=height-1; i>=0; i--)
    {
        for (unsigned j=0; j<width; ++j)
        {
            BMPPixel color;
            if (m_ca_flow_history[i][j] == 0)
                color = BMPPixel((char)255, (char)255, (char)255);
            else
                color = BMPPixel(0, 0, (char)(255.0*(double)m_ca_flow_history[i][j]/(double)m_vmax));
            bmpData[j] = color;
        }
        writer.WriteLine(bmpData);
    }
    writer.CloseBMP();
    delete[] bmpData;
}
void CellularAutomata::Step()
{
    // Iterar sobre AC hasta encotrar vehiculo.
    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Aceleracion.
            if ((m_ca[i] <= m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)))
                m_ca[i]++;
            else
            {
                // Frenado.
                if (m_ca[i] > 0)
                {
                    int nd = NextCarDist(i);
                    if (nd <= m_ca[i])
                        m_ca[i] = nd - 1;
                }
            }

            // Aleatoriedad.
			bool rnd = Randomization();
            if ((m_ca[i] > 0) && rnd)
                m_ca[i]--;
        }
    }

    // Aplicar cambios.
    Move();
    m_ca_history.push_back(m_ca);
}
void CellularAutomata::Evolve(const unsigned &iter)
{
    for (unsigned i=0; i<iter; ++i)
        Step();
}
unsigned CellularAutomata::GetSize()
{
    return m_size;
}
unsigned CellularAutomata::GetHistorySize()
{
    return m_ca_history.size();
}
unsigned CellularAutomata::CountCars()
{
    unsigned count = 0;
    for (unsigned i=0; i<m_ca.size(); i++)
    {
        if (m_ca[i] != -1)
            count++;
    }
    return count;
}
bool CellularAutomata::Randomization(const double &prob)
{
	double l_prob;
	if (prob < 0)
		l_prob = m_rand_prob;
	else
		l_prob = prob;

    if (m_test)
    {
        if (m_rand_values.size() != 0)
        {
            bool ret = m_rand_values[0];
			m_rand_values.erase(m_rand_values.begin());
            return ret;
        }
        else
            return false;
    }
    else
    {
        if (m_drand() <= l_prob)
            return true;
        else
            return false;
    }
}
int &CellularAutomata::At(const unsigned &i, const CAS &ca)
{
    return At(i, 0, ca);
}
int CellularAutomata::NextCarDist(const int &pos)
{
    int dist = 1;
    while ((At(pos+dist) == -1) && (dist < 2*(int)m_size))
        dist++;
    return dist;
}


/****************************
*                           *
*        AC Circular        *
*                           *
****************************/

CircularCA::CircularCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob) 
    : CellularAutomata(size, density, vmax, rand_prob) {}
CircularCA::CircularCA(const vector<int> &ca, const vector<bool> &rand_values, const int &vmax)
    : CellularAutomata(ca, rand_values, vmax) {}
int &CircularCA::At(const unsigned &i, const unsigned &j, const CAS &ca)
{
    // Devuelve elemento de AC suponiendo una pista ciclica.
    unsigned int pos = i % m_ca.size();
    switch (ca)
    {
    case CA_TEMP:
        return m_ca_temp[pos];
        break;
    case CA_FLOW_TEMP:
        return m_ca_flow_temp[pos];
        break;
    case CA_HISTORY:
        return m_ca_history[j][pos];
        break;
    case CA_FLOW_HISTORY:
        return m_ca_flow_history[j][pos];
        break;
    default:
        return m_ca[pos];
        break;
    };
}
void CircularCA::Move()
{
    m_ca_flow_temp.assign(m_size, 0);
    for (unsigned i=0; i<m_ca_temp.size(); ++i)
        m_ca_temp[i] = -1;
    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Cambia las posiciones de los autos en AC.
            At(i+m_ca[i], CA_TEMP) = m_ca[i];

            // Marca las casillas donde hay flujo de autos.
            for (unsigned j=i; j<i+m_ca[i]; ++j)
                At(j, CA_FLOW_TEMP) = 1;
        }
    }
    m_ca_flow_history.push_back(m_ca_flow_temp);
    m_ca.assign(m_ca_temp.begin(), m_ca_temp.end());
}
void CircularCA::Evolve(const unsigned &iter)
{
    unsigned cars = CountCars();
    for (unsigned i=0; i<iter; ++i)
        Step();

    if (cars != CountCars())
        cout << "Error: La cantidad de autos no se conserva." << endl;
}


/****************************
*                           *
*        AC Abierto         *
*                           *
****************************/

OpenCA::OpenCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &new_car_prob) 
    : CellularAutomata(size, density, vmax, rand_prob)
{
    m_new_car_prob = new_car_prob;
    m_empty = -1;

    // Verifica argumento.
    if (m_new_car_prob < 0 || m_new_car_prob > 1)
    {
        m_new_car_prob = 0.5;
        cout << "Probabilidad de nuevo auto invalida. Cambiando a new_car_prob=" << m_new_car_prob << "." << endl;
    }
}
OpenCA::OpenCA(const vector<int> &ca, const vector<bool> &rand_values, const int &vmax)
	: CellularAutomata(ca, rand_values, vmax)
{
	m_new_car_prob = -1.0;
	m_empty = -1;
}
int &OpenCA::At(const unsigned &i, const unsigned &j, const CAS &ca)
{
    if (i >= m_ca.size())
    {
        if ((ca == CA) || (ca == CA_TEMP) || (ca == CA_HISTORY))
            m_empty = -1;
        else
            m_empty = 0;
        return m_empty;
    }
    else
    {
        switch (ca)
        {
        case CA_TEMP:
            return m_ca_temp[i];
            break;
        case CA_FLOW_TEMP:
            return m_ca_flow_temp[i];
            break;
        case CA_HISTORY:
            return m_ca_history[j][i];
            break;
        case CA_FLOW_HISTORY:
            return m_ca_flow_history[j][i];
            break;
        default:
            return m_ca[i];
            break;
        };
    }
}
void OpenCA::Move()
{
    m_ca_flow_temp.assign(m_size, 0);
    for (unsigned i=0; i<m_ca_temp.size(); ++i)
        m_ca_temp[i] = -1;
    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Cambia las posiciones de los autos en AC.
            At(i+m_ca[i], CA_TEMP) = m_ca[i];

            // Marca las casillas donde hay flujo de autos.
            for (unsigned j=i; j<i+m_ca[i]; ++j)
                At(j, CA_FLOW_TEMP) = 1;
        }
    }
    m_ca_flow_history.push_back(m_ca_flow_temp);
    m_ca.assign(m_ca_temp.begin(), m_ca_temp.end());
    
    // Añade coche con probabilidad aleatoria.
    if (m_ca[0] == -1 && Randomization(m_new_car_prob))
        m_ca[0] = 1;
}


/****************************
*                           *
*  AC Coches inteligentes   *
*                           *
****************************/

SmartCA::SmartCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &smart_density) 
    : CircularCA(size, density, vmax, rand_prob)
{
    // Verifica argumento.
    double l_smart_density = smart_density;
    if (l_smart_density < 0 || l_smart_density > 1)
    {
        l_smart_density = 0.5;
        cout << "Densidad de autos inteligentes invalida. Cambiando a smart_density=" << l_smart_density << "." << endl;
    }

    // Selecciona autos inteligentes.
    unsigned smart_car_number = (unsigned)(((double)size)*l_smart_density);
    vector<int> smart_car_positions;
    for (unsigned i=0; i<m_size; ++i)
	{
		if (m_ca[i] != -1)
			smart_car_positions.push_back(i);
	}

    random_shuffle(smart_car_positions.begin(), smart_car_positions.end(), aux_random);
	for (unsigned i=0; i<smart_car_positions.size() && i<smart_car_number; ++i)
        m_smart_cars.push_back(smart_car_positions[i]);
}
void SmartCA::Move()
{
    m_ca_flow_temp.assign(m_size, 0);
    for (unsigned i=0; i<m_ca_temp.size(); ++i)
        m_ca_temp[i] = -1;
    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            int pos = aux_find_pos<int>(m_smart_cars, i);
            if (pos != -1)
                m_smart_cars[pos] = (i+m_ca[i]) % m_size;

            // Cambia las posiciones de los autos en AC.
            At(i+m_ca[i], CA_TEMP) = m_ca[i];

            // Marca las casillas donde hay flujo de autos.
            for (unsigned j=i; j<i+m_ca[i]; ++j)
                At(j, CA_FLOW_TEMP) = 1;
        }
    }
    m_ca_flow_history.push_back(m_ca_flow_temp);
    m_ca.assign(m_ca_temp.begin(), m_ca_temp.end());
}
void SmartCA::Step()
{
    // Iterar sobre AC hasta encotrar vehiculo.
    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            if (aux_is_in<int>(m_smart_cars, i))
            {
                // Auto inteligente.
                int nc = i + NextCarDist(i);
                nc %= m_size;
                if (((m_ca[i] <= m_vmax) && (NextCarDist(i) > (m_ca[i] + 1))) || ((m_ca[nc] <= m_vmax) && (NextCarDist(nc) > (m_ca[nc] + 1))))
                    m_ca[i]++;
            }
            else
            {
                // Aceleracion.
                if ((m_ca[i] <= m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)))
                    m_ca[i]++;

                // Aleatoriedad.
                if ((m_ca[i] > 0) && Randomization())
                    m_ca[i]--;
            }

            // Frenado.
            if (m_ca[i] > 0)
            {
                int nd = NextCarDist(i);
                if (nd <= m_ca[i])
                    m_ca[i] = nd - 1;
            }
        }
    }

    // Aplicar cambios.
    Move();
    m_ca_history.push_back(m_ca);
}

/****************************
*                           *
*          AC Tope          *
*                           *
****************************/

StreetStopCA::StreetStopCA(const unsigned &size, const double &density, const int &vmax, 
                           const double &rand_prob, const double &stop_density)
                           : CircularCA(size, density, vmax, rand_prob)
{
    unsigned stops = (unsigned)(((double)size)*stop_density);
    if (stops >= size)
    {
        stops = size - 2;
        cout << "Densidad de topes invalida. Se usaran " << stops << " topes." << endl;
    }

    // Coloca topes.
    vector<unsigned> stop_positions;
    for (unsigned i=0; i<m_size; ++i)
        stop_positions.push_back(i);

    random_shuffle(stop_positions.begin(), stop_positions.end(), aux_random);
    for (unsigned i=0; i<stops; ++i)
        m_stop_pos.push_back(stop_positions[i]);
}
void StreetStopCA::Step()
{
    // Iterar sobre AC hasta encotrar vehiculo.
    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Aceleracion.
            if ((m_ca[i] <= m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)))
                m_ca[i]++;
            else
            {
                // Frenado.
                if (m_ca[i] > 0)
                {
                    int nd = NextCarDist(i);
                    if (nd <= m_ca[i])
                        m_ca[i] = nd - 1;
                }
            }

            // Aleatoriedad.
            if ((m_ca[i] > 0) && Randomization())
                m_ca[i]--;

            // Tope.
            if ((NextStopDist(i) < 2*m_vmax) && (m_ca[i] > 1))
                m_ca[i]--;
        }
    }

    // Aplicar cambios.
    Move();
    m_ca_history.push_back(m_ca);
}
int StreetStopCA::NextStopDist(const int &pos)
{
    if (m_stop_pos.size() != 0)
    {
        vector<unsigned> dist;
        for (unsigned i=0; i<m_stop_pos.size(); ++i)
        {
            int tmp_dist = pos - m_stop_pos[i];
            if (tmp_dist < 0)
                dist.push_back(abs(tmp_dist));
            else
                dist.push_back(m_size - abs(tmp_dist));
        }
        return *min_element(dist.begin(), dist.end());
    }
    else
        return numeric_limits<int>::max();
}
void StreetStopCA::DrawHistory()
{
        unsigned height = m_ca_history.size();
        unsigned width = m_size;
        BMPWriter writer("ca.bmp", width, height);
        BMPPixel* bmpData = new BMPPixel[width];
        for (int i=height-1; i>=0; i--)
        {
            for (unsigned j=0; j<width; ++j)
            {
                BMPPixel color;
                if (aux_is_in(m_stop_pos, j))
                {
                    if (m_ca_history[i][j] == -1)
                        color = BMPPixel((char)0, (char)255, (char)0);
                    else
                        color = BMPPixel(0, (char)255, (char)(255.0*(double)m_ca_history[i][j]/(double)m_vmax));
                }
                else
                {
                    if (m_ca_history[i][j] == -1)
                        color = BMPPixel((char)255, (char)255, (char)255);
                    else
                        color = BMPPixel(0, 0, (char)(255.0*(double)m_ca_history[i][j]/(double)m_vmax));
                }
                bmpData[j] = color;
            }
            writer.WriteLine(bmpData);
        }
        writer.CloseBMP();
        delete[] bmpData;
}

/****************************
*                           *
*        AC Semáforo        *
*                           *
****************************/

SemaphoreCA::SemaphoreCA(const unsigned &size, const double &density, const int &vmax, 
                         const double &rand_prob, const double &semaphore_density, 
						 const bool &random_semaphores)
                       : CircularCA(size, density, vmax, rand_prob)
{
	m_semaphore_init = 100;
	m_semaphore_open = 50;
    unsigned semaphores = (unsigned)(((double)size)*semaphore_density);
    if (semaphores >= size)
    {
        semaphores = size - 2;
        cout << "Densidad de topes invalida. Se usaran " << semaphores << " topes." << endl;
    }

    // Coloca semáforo.
	if (random_semaphores)
	{
		vector<unsigned> semaphore_positions;
		for (unsigned i=0; i<m_size; ++i)
			semaphore_positions.push_back(i);

		random_shuffle(semaphore_positions.begin(), semaphore_positions.end(), aux_random);
		for (unsigned i=0; i<semaphores; ++i)
		{
			m_semaphore_pos.push_back(semaphore_positions[i]);
			m_semaphore_value.push_back(aux_random(m_semaphore_init));
		}
	}
	else
	{
		if (semaphores != 0)
		{
			int semaphore_dt = size/semaphores;
			for (unsigned i=0; i<size; i += semaphore_dt)
			{
				m_semaphore_pos.push_back(i);
				m_semaphore_value.push_back(aux_random(m_semaphore_init));
			}
		}
	}
	m_semaphore_val_hist.push_back(m_semaphore_value);
}
void SemaphoreCA::Step()
{
    // Iterar sobre AC hasta encotrar vehiculo.
    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Aceleracion.
            if ((m_ca[i] <= m_vmax) && ((NextCarDist(i) > (m_ca[i] + 1)) && (NextSemaphoreDist(i) > (m_ca[i] + 1))))
                m_ca[i]++;
            else
            {
                // Frenado.
                if (m_ca[i] > 0)
                {
                    int ncd = NextCarDist(i);
					int nsd = NextSemaphoreDist(i);
					int nd = (ncd < nsd) ? ncd : nsd;
                    if (nd <= m_ca[i])
                        m_ca[i] = nd - 1;
                }
            }

            // Aleatoriedad.
            if ((m_ca[i] > 0) && Randomization())
                m_ca[i]--;
        }
    }

	// Itera semáforos.
	for (unsigned i=0; i<m_semaphore_value.size(); i++)
	{
		m_semaphore_value[i]--;
		if (m_semaphore_value[i] < 0)
			m_semaphore_value[i] = m_semaphore_init;
	}
	m_semaphore_val_hist.push_back(m_semaphore_value);

    // Aplicar cambios.
    Move();
    m_ca_history.push_back(m_ca);
}
int SemaphoreCA::NextSemaphoreDist(const int &pos)
{
	if (!m_semaphore_pos.empty())
    {
        vector<unsigned> dist;
        for (unsigned i=0; i<m_semaphore_pos.size(); ++i)
        {
			if (m_semaphore_value[i] >= m_semaphore_open)
			{
				int tmp_dist = pos - m_semaphore_pos[i];
				if (tmp_dist < 0)
					dist.push_back(abs(tmp_dist));
				else
					dist.push_back(m_size - abs(tmp_dist));
			}
        }
		if (!dist.empty())
			return *min_element(dist.begin(), dist.end());
		else 
			return numeric_limits<int>::max();
    }
    else
        return numeric_limits<int>::max();
}
void SemaphoreCA::DrawHistory()
{
        unsigned height = m_ca_history.size();
        unsigned width = m_size;
        BMPWriter writer("ca.bmp", width, height);
        BMPPixel* bmpData = new BMPPixel[width];
        for (int i=height-1; i>=0; i--)
        {
            for (unsigned j=0; j<width; ++j)
            {
                BMPPixel color;
				int s_pos = aux_find_pos(m_semaphore_pos, j);
                if (s_pos != -1)
                {
					if (m_semaphore_val_hist[i][s_pos] < m_semaphore_open)
					{
						if (m_ca_history[i][j] == -1)
							color = BMPPixel((char)0, (char)255, (char)0);
						else
							color = BMPPixel((char)0, (char)255, (char)(255.0*(double)m_ca_history[i][j]/(double)m_vmax));
					}
					else
					{
						if (m_ca_history[i][j] == -1)
							color = BMPPixel((char)255, (char)0, (char)0);
						else
							color = BMPPixel((char)255, (char)0, (char)(255.0*(double)m_ca_history[i][j]/(double)m_vmax));
					}
                }
                else
                {
                    if (m_ca_history[i][j] == -1)
                        color = BMPPixel((char)255, (char)255, (char)255);
                    else
                        color = BMPPixel((char)0, (char)0, (char)(255.0*(double)m_ca_history[i][j]/(double)m_vmax));
                }
                bmpData[j] = color;
            }
            writer.WriteLine(bmpData);
        }
        writer.CloseBMP();
        delete[] bmpData;
}


/****************************
*                           *
*      Manejador de CA      *
*                           *
****************************/

CellularAutomata* cellularautomata = NULL;
CircularCA* circularca = NULL;
OpenCA* openca = NULL;
SmartCA* smartca = NULL;
StreetStopCA* streetstopca = NULL;
SemaphoreCA* semaphoreca = NULL;

CellularAutomata* create_ca(CA_TYPE ca, const unsigned &size, const double &density, const int &vmax, 
	                        const double &rand_prob, const double &extra1, const bool &extra2)
{
    delete_ca();
    switch(ca)
    {
    case CIRCULAR_CA:
        cellularautomata = circularca = new CircularCA(size, density, vmax, rand_prob);
        break;
    case OPEN_CA:
        cellularautomata = openca = new OpenCA(size, density, vmax, rand_prob, extra1);
        break;
    case SMART_CA:
        cellularautomata = smartca = new SmartCA(size, density, vmax, rand_prob, extra1);
        break;
    case STOP_CA:
        cellularautomata = streetstopca = new StreetStopCA(size, density, vmax, rand_prob, extra1);
		break;
	case SEMAPHORE_CA:
		cellularautomata = semaphoreca = new SemaphoreCA(size, density, vmax, rand_prob, extra1, extra2);
		break;
    };
    return cellularautomata;
}
void delete_ca()
{
    if (circularca != NULL)
        delete circularca;
    if (openca != NULL)
        delete openca;
    if (smartca != NULL)
        delete smartca;
    if (streetstopca != NULL)
        delete streetstopca;
	if (semaphoreca != NULL)
		delete semaphoreca;

    circularca = NULL;
    openca = NULL;
    smartca = NULL;
    streetstopca = NULL;
    cellularautomata = NULL;
	semaphoreca = NULL;
}


////////////////////////////////////
//                                //
//      Autómatas celulares       //
//      de varios carriles.       //
//                                //
////////////////////////////////////

/****************************
*                           *
*       AC Básico ML        *
*                           *
****************************/

CAElement::CAElement(const int lanes, const int def_val)
{
    m_lane.assign(lanes, def_val);
}
int &CAElement::operator[](const unsigned &iter)
{
    return m_lane[iter];
}

CellularAutomataML::CellularAutomataML(const unsigned &size, const unsigned &lanes, const double &density, 
                                       const int &vmax, const double &rand_prob)
{
    // Inicializa variables.
    m_size = size;
    m_lanes = lanes;
    m_vmax = vmax;
    m_rand_prob = rand_prob;

    m_ca.assign(size, CAElement(lanes));
    m_ca_temp.assign(size, CAElement(lanes));
    m_ca_history.clear();
    m_ca_flow_history.clear();


    // Verifica argumentos.
    double l_density = density;
    if (l_density <= 0.0 || l_density >= 1.0)
    {
        l_density = 0.1;
        cout << "Densidad invalida. Cambiando a density=" << l_density << "." << endl;
    }
    if (m_vmax < 1)
    {
        m_vmax = 1;
        cout << "Velocidad limite invalida. Cambiando a vmax=" << m_vmax << "." << endl;
    }
    if (m_rand_prob < 0.0 || m_rand_prob >= 1.0)
    {
        m_rand_prob = 0.0;
        cout << "Probabilidad de frenado invalida. Cambiando a rand_prob=" << m_rand_prob << "." << endl;
    }
    unsigned vehicles = (unsigned)(((double)size)*density);

    // Asigna semilla a numeros aleatorios.
    unsigned seed = static_cast<unsigned int>(time(NULL));
    m_drand.seed(seed);
    m_irand.seed(seed);

    // Coloca autos al azar.
    for (unsigned i=0; i<vehicles; ++i)
    {
        unsigned pos, lane;
        do
        {
            pos = m_irand() % m_ca.size();
            lane = m_irand() % 2;
        }
        while (m_ca[pos][lane] != -1);
        m_ca[pos][lane] = 1;
    }
    m_ca_history.push_back(m_ca);
}
CellularAutomataML::~CellularAutomataML() {}
void CellularAutomataML::Print()
{
    // Imprime valores del automata celular en la terminal.
    for (unsigned i=0; i<m_lanes; ++i)
    {
        for (unsigned j=0; j<m_ca.size(); ++j)
        {
            if (m_ca[j][i] == -1)
                cout << "-";
            else
                cout << m_ca[j][i];
        }
        cout << endl;
    }
    for (unsigned i=0; i<m_ca.size(); ++i)
        cout << ".";
    cout << endl;
}
void CellularAutomataML::DrawHistory()
{
    unsigned bmp_height;
    if (m_lanes == 1)
        bmp_height =  m_ca_history.size();
    else
        bmp_height =  m_lanes*m_ca_history.size() + m_lanes;

    unsigned height =  m_ca_history.size();
    unsigned width = m_size;
    BMPWriter writer("ca.bmp", width, bmp_height);
    BMPPixel* bmpData = new BMPPixel[width];
    BMPPixel color;

    for (int i=height-1; i>=0; i--)
    {
        for (unsigned l=0; l<m_lanes; ++l)
        {
            for (unsigned j=0; j<width; ++j)
            {
                if (m_ca_history[i][j][l] == -1)
                    color = BMPPixel((char)255, (char)255, (char)255);
                else
                    color = BMPPixel(0, 0, (char)(255.0*(double)m_ca_history[i][j][l]/(double)m_vmax));
                bmpData[j] = color;
            }
            writer.WriteLine(bmpData);
        }

        for (unsigned j=0; j<width; ++j)
        {
            color = BMPPixel((char)0, (char)255, (char)0);
            bmpData[j] = color;
        }
        writer.WriteLine(bmpData);
    }
    writer.CloseBMP();
    delete[] bmpData;
}
void CellularAutomataML::DrawFlowHistory()
{
    unsigned bmp_height;
    if (m_lanes == 1)
        bmp_height =  m_ca_flow_history.size();
    else
        bmp_height =  m_lanes*m_ca_flow_history.size() + m_lanes;

    unsigned height =  m_ca_flow_history.size();
    unsigned width = m_size;
    BMPWriter writer("ca_flow.bmp", width, bmp_height);
    BMPPixel* bmpData = new BMPPixel[width];
    BMPPixel color;

    for (int i=height-1; i>=0; i--)
    {
        for (unsigned l=0; l<m_lanes; ++l)
        {
            for (unsigned j=0; j<width; ++j)
            {
                if (m_ca_flow_history[i][j][l] == -1)
                    color = BMPPixel((char)255, (char)255, (char)255);
                else
                    color = BMPPixel(0, 0, (char)(255.0*(double)m_ca_flow_history[i][j][l]/(double)m_vmax));
                bmpData[j] = color;
            }
            writer.WriteLine(bmpData);
        }

        for (unsigned j=0; j<width; ++j)
        {
            color = BMPPixel((char)0, (char)255, (char)0);
            bmpData[j] = color;
        }
        writer.WriteLine(bmpData);
    }
    writer.CloseBMP();
    delete[] bmpData;
}
void CellularAutomataML::Step()
{
    // Primero hace los cambios de carril.
    if (m_lanes > 1)
    {
        for (unsigned i=0; i<m_ca.size(); ++i)
        {
            for (unsigned j=0; j<m_lanes; ++j)
            {
                // Si no puede acelerar
                if (!((At(i, j) <= m_vmax) && (NextCarDist(i, j) > (At(i, j) + 1))))
                {
                    if (NextCarDist(i, j) <= At(i, j))
                    {
                        // Intenta cambiar de carril.
                        bool left = false, right = false;
                        for (int k=(int)j-1; k<=(int)j+1 && k<(int)m_lanes; k++)
                        {
                            if (k<0 || (unsigned)k==j || At(i, k) != -1) continue;

                            // Busca auto anterior y verifica si puede cambiarse sin chocar.
                            int v, s = 0;
                            for (int l=i-1; s<=m_vmax; ++s, l--)
                            {
                                v = At(l, k);
                                if ((v != -1) && (v-s < 0))
                                {
                                    // Ahora busca si en este nuevo carril puede acelerar.
                                    if ((At(i, k) <= m_vmax) && (NextCarDist(i, k) > (At(i, k) + 1)))
                                    {
                                        // Marca carril como disponible.
                                        if ((unsigned)k == j-1) left = true;
                                        else right = true;
                                    }
                                    break;
                                }
                            }
                            if (s == m_vmax)
                            {
                                if ((At(i, k) <= m_vmax) && (NextCarDist(i, k) > (At(i, k) + 1)))
                                {
                                    // Marca carril como disponible.
                                    if ((unsigned)k == j-1) left = true;
                                    else right = true;
                                }
                            }
                        }

                        // Si el carril está disponible adelenta.
                        if (left || right)
                        {
                            // Si ambos están disponibles apaga uno al azar.
                            if (left && right)
                            {
                                int select = m_irand() % 2;
                                if (select == 1) right = false;
                                else left = false;
                            }
                            unsigned new_lane;
                            if (left)
                                new_lane = j-1;
                            else
                                new_lane = j+1;

                            At(i, new_lane) = At(i, j);
                            At(i, j) = -1;
                        }
                    }
                }
            }
        }
    }

    // Iterar sobre AC.
    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        for (unsigned j=0; j<m_lanes; ++j)
        {
            // Encuentra vehículo.
            if (At(i, j) != -1)
            {
                // Aceleracion.
                if ((At(i, j) <= m_vmax) && (NextCarDist(i, j) > (At(i, j) + 1)))
                    At(i, j)++;
                else
                {
                    // Frenado.
                    if (At(i, j) > 0)
                    {
                        int nd = NextCarDist(i, j);
                        if (nd <= m_ca[i][j])
                            At(i, j) = nd - 1;
                    }
                }

                // Aleatoriedad.
                if ((At(i, j) > 0) && (m_drand() <= m_rand_prob))
                    At(i, j)--;
            }
        }
    }

    // Aplicar cambios.
    Move();
    m_ca_history.push_back(m_ca);
}
void CellularAutomataML::Evolve(const unsigned &iter, const bool &print)
{
    // Escribe condiciones iniciales.
    if (print)
        Print();

    for (unsigned i=0; i<iter; ++i)
    {
        Step();
        if (print)
            Print();
    }
}
unsigned CellularAutomataML::GetSize()
{
    return m_size;
}
unsigned CellularAutomataML::GetHistorySize()
{
    return m_ca_history.size();
}
int &CellularAutomataML::At(const int &i, const unsigned &lane, const CAS &ca)
{
    return At(i, lane, 0, ca);
}


/****************************
*                           *
*      AC Circular ML       *
*                           *
****************************/

CircularCAML::CircularCAML(const unsigned size, const unsigned lanes, const double density, 
                           const int vmax, const double rand_prob) 
                         : CellularAutomataML(size, lanes, density, vmax, rand_prob) {}
int &CircularCAML::At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca)
{
    // Devuelve elemento de AC suponiendo una pista ciclica.
    unsigned int pos;
    if (i<0)
    {
        pos = abs(i) % m_ca.size();
        pos = m_ca.size() - pos;
    }
    else
        pos = i % m_ca.size();

    switch (ca)
    {
    case CA_TEMP:
        return m_ca_temp[pos][lane];
        break;
    case CA_FLOW_TEMP:
        return m_ca_flow_temp[pos][lane];
        break;
    case CA_HISTORY:
        return m_ca_history[j][pos][lane];
        break;
    case CA_FLOW_HISTORY:
        return m_ca_flow_history[j][pos][lane];
        break;
    default:
        return m_ca[pos][lane];
        break;
    };
}
int CircularCAML::NextCarDist(const int &pos, const unsigned &lane)
{
    unsigned dist = 1;
    while (At(pos+dist, lane) == -1)
        dist++;
    return dist;
}
void CircularCAML::Move()
{
    m_ca_flow_temp.assign(m_size, CAElement(m_lanes, 0));
    for (unsigned i=0; i<m_ca_temp.size(); ++i)
    {
        for (unsigned j=0; j<m_lanes; ++j)
            m_ca_temp[i][j] = -1;
    }

    for (unsigned i=0; i<m_ca.size(); ++i)
    {
        for (unsigned j=0; j<m_lanes; ++j)
        {
            if (m_ca[i][j] != -1)
            {
                // Cambia las posiciones de los autos en AC.
                At(i+m_ca[i][j], j, CA_TEMP) = m_ca[i][j];

                // Marca las casillas donde hay flujo de autos.
                for (unsigned k=i; k<i+m_ca[i][j]; ++k)
                    At(k, j, CA_FLOW_TEMP) = 1;
            }
        }
    }
    m_ca_flow_history.push_back(m_ca_flow_temp);
    m_ca.assign(m_ca_temp.begin(), m_ca_temp.end());
}
