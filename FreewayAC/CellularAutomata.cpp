#include <limits>
#include <algorithm>
#include <cctype>
#include <vector>
#include "CellularAutomata.h"
using namespace std;


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

CellularAutomata::CellularAutomata(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const int &init_vel)
{
    // Inicializa variables.
    m_test = false;
    m_size = size;
    m_vmax = vmax;
    m_rand_prob = rand_prob;
    m_init_vel = init_vel;
    m_ca.assign(size, -1);
    m_ca_temp.assign(size, -1);
    m_ca_flow_temp.assign(size, 0);
    m_ca_history.clear();
    m_ca_flow_history.clear();
    m_connect = nullptr;
    m_connect_pos = -1;


    // Verifica argumentos.
    double l_density = density;
    if (l_density < 0.0 || l_density > 1.0)
    {
        l_density = 0.1;
        cout << "Aviso: Densidad invalida " << density << ". Cambiando a density=" << l_density << "." << endl;
    }
    if (m_vmax < 1)
    {
        m_vmax = 1;
        cout << "Aviso: Velocidad limite invalida. Cambiando a vmax=" << m_vmax << "." << endl;
    }
    if (m_rand_prob < 0.0 || m_rand_prob > 1.0)
    {
        m_rand_prob = 0.0;
        cout << "Aviso: Probabilidad de frenado invalida. Cambiando a rand_prob=" << m_rand_prob << "." << endl;
    }
    unsigned vehicles = (unsigned)(((double)size)*l_density);

    // Coloca autos al azar.
    vector<unsigned> car_positions;
    for (unsigned i = 0; i < m_size; ++i)
        car_positions.push_back(i);

    random_shuffle(car_positions.begin(), car_positions.end(), RandomGen::GetInt);
    for (unsigned i = 0; i < vehicles; ++i)
        m_ca[car_positions[i]] = 1;

    m_ca_history.push_back(m_ca);
    m_ca_flow_history.push_back(m_ca_flow_temp);
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
    m_connect = NULL;
    m_connect_pos = -1;
    m_init_vel = 1;
}
CellularAutomata::~CellularAutomata() {}
void CellularAutomata::Print()
{
    // Imprime valores del automata celular en la terminal.
    for (unsigned i = 0; i < m_ca.size(); ++i)
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
    for (unsigned i = 0; i < m_ca_history.size(); ++i)
    {
        for (unsigned j = 0; j < m_size; ++j)
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
int CellularAutomata::DrawHistory(string path, string out_file_name)
{
    if (out_file_name == "")
        out_file_name = path + "ca.bmp";
    else
        out_file_name = path + out_file_name;

    unsigned height = m_ca_history.size();
    unsigned width = m_size;
    BMPWriter writer(out_file_name.c_str(), width, height);
    if (writer.IsOpen())
    {
        BMPPixel* bmpData = new BMPPixel[width];
        for (int i = height-1; i >= 0; --i)     // Los archivos BMP se escriben de abajo a arriba.
        {
            for (unsigned j = 0; j < width; ++j)
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
        return 0;
    }
    else
        return 1;
}
int CellularAutomata::DrawFlowHistory(string path, string out_file_name)
{
    if (out_file_name == "")
        out_file_name = path + "ca_flow.bmp";
    else
        out_file_name = path + out_file_name;

    unsigned height = m_ca_flow_history.size();
    unsigned width = m_size;
    BMPWriter writer(out_file_name.c_str(), width, height);
    if (writer.IsOpen())
    {
        BMPPixel* bmpData = new BMPPixel[width];
        for (int i = height-1; i >= 0; --i)
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
        return 0;
    }
    else
        return 1;
}
void CellularAutomata::Step()
{
    // Iterar sobre AC hasta encotrar vehiculo.
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Aceleracion.
            if ((m_ca[i] < m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)))
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
void CellularAutomata::Move()
{
    m_ca_flow_temp.assign(m_size, 0);
    m_ca_temp.assign(m_size, -1);
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Cambia las posiciones de los autos en AC.
            if ((i + m_ca[i] >= m_size) && m_connect != NULL)
                At(i + m_ca[i]) = m_ca[i];
            else
                At(i + m_ca[i], CA_TEMP) = m_ca[i];

            // Marca las casillas donde hay flujo de autos.
            for (unsigned j = i; j < i + m_ca[i]; ++j)
                At(j, CA_FLOW_TEMP) = 1;
        }
    }
    m_ca_flow_history.push_back(m_ca_flow_temp);
    m_ca.assign(m_ca_temp.begin(), m_ca_temp.end());
}
void CellularAutomata::Evolve(const unsigned &iter)
{
    for (unsigned i = 0; i < iter; ++i)
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
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
            count++;
    }
    return count;
}
bool CellularAutomata::IsFluxHalted()
{
    bool halted = true;
    if (m_ca_flow_history.size() > 1)
    {
        for (unsigned i = 1; i < m_ca_flow_history.size() && halted; ++i)
        {
            for (unsigned j = 0; j < m_size; ++j)
            {
                if (m_ca_flow_history[i][j] != 0)
                {
                    halted = false;
                    break;
                }
            }
        }
        return halted;
    }
    else
        return false;
}
bool CellularAutomata::Randomization(const double &prob)
{
    double l_prob;
    if (prob < 0)
        l_prob = m_rand_prob;
    else
        l_prob = prob;

    // Si está en modo de prueba toma los valores aleatorios de la lista.
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
        if (RandomGen::GetDouble() <= l_prob)
            return true;
        else
            return false;
    }
}
int &CellularAutomata::At(const int &i, const CAS &ca)
{
    if (m_connect != nullptr && (unsigned)i >= m_size)
        return m_connect->At(i- m_size + m_connect_pos, ca);
    else
        return At(i, 0, ca);
}
int CellularAutomata::GetAt(const unsigned &i, const CAS &ca)
{
    return At(i, ca);
}
int CellularAutomata::GetAt(const unsigned &i, const unsigned &j, const CAS &ca)
{
    return At(i, j, ca);
}
void CellularAutomata::Connect(CellularAutomata* connect, unsigned connect_pos)
{
    m_connect = connect;
    m_connect_pos = connect_pos;
    if (m_connect_pos > m_size)
    {
        m_connect_pos = m_size / 2;
        cout << "Valor incorrecto de posicion de conexion. Cambiando a " << m_connect_pos << endl;
    }
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

CircularCA::CircularCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const int &init_vel)
    : CellularAutomata(size, density, vmax, rand_prob, init_vel) {}
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
void CircularCA::Evolve(const unsigned &iter)
{
    unsigned cars = CountCars();
    for (unsigned i = 0; i < iter; ++i)
        Step();

    if (cars != CountCars())
        cout << "Error: La cantidad de autos no se conserva." << endl;
}


/****************************
*                           *
*        AC Abierto         *
*                           *
****************************/

OpenCA::OpenCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const int &init_vel,
               const double &new_car_prob, const int &new_car_speed)
    : CellularAutomata(size, density, vmax, rand_prob, init_vel)
{
    m_new_car_prob = new_car_prob;
    m_new_car_speed = new_car_speed;
    m_empty = -1;

    // Verifica argumento.
    if (m_new_car_prob < 0 || m_new_car_prob > 1)
    {
        m_new_car_prob = 0.5;
        cout << "Aviso: Probabilidad de nuevo auto invalida. Cambiando a new_car_prob=" << m_new_car_prob << "." << endl;
    }
}
OpenCA::OpenCA(const vector<int> &ca, const vector<bool> &rand_values, const int &vmax, 
               const int &new_car_speed)
    : CellularAutomata(ca, rand_values, vmax)
{
    m_new_car_prob = -1.0;
    m_new_car_speed = new_car_speed;
    m_empty = -1;
}
int &OpenCA::At(const unsigned &i, const unsigned &j, const CAS &ca)
{
    if ((unsigned)i >= m_ca.size())
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
void OpenCA::Step()
{
    // Iterar sobre AC hasta encotrar vehiculo.
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Aceleracion.
            if ((m_ca[i] < m_vmax) && (NextCarDist(i) >(m_ca[i] + 1)))
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

    // Añade coche con probabilidad aleatoria.
    if (m_ca[0] == -1 && Randomization(m_new_car_prob))
        m_ca[0] = m_new_car_speed;

    m_ca_history.push_back(m_ca);
}


/****************************
*                           *
*  AC Coches inteligentes   *
*                           *
****************************/

AutonomousCA::AutonomousCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob,
                           const int &init_vel, const double &aut_density)
    : CircularCA(size, density, vmax, rand_prob, init_vel)
{
    // Verifica argumento.
    double l_aut_density = aut_density;
    if (l_aut_density < 0 || l_aut_density > 1)
    {
        l_aut_density = 0.5;
        cout << "Densidad de autos inteligentes invalida. Cambiando a smart_density=" << l_aut_density << "." << endl;
    }

    // Selecciona autos inteligentes.
    unsigned aut_car_number = (unsigned)(((double)size)*l_aut_density);
    vector<int> aut_car_positions;
    for (unsigned i = 0; i < m_size; ++i)
    {
        if (m_ca[i] != -1)
            aut_car_positions.push_back(i);
    }

    random_shuffle(aut_car_positions.begin(), aut_car_positions.end(), RandomGen::GetInt);
    for (unsigned i = 0; i < aut_car_positions.size() && i < aut_car_number; ++i)
        m_aut_cars.push_back(aut_car_positions[i]);
}
AutonomousCA::AutonomousCA(const vector<int> &ca, vector<int> smart_cars, const vector<bool> &rand_values,
                 const int &vmax)
                 : CircularCA(ca, rand_values, vmax)
{
    m_aut_cars = smart_cars;
}
void AutonomousCA::Move()
{
    m_ca_flow_temp.assign(m_size, 0);
    m_ca_temp.assign(m_size, -1);
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            int pos = aux_find_pos<int>(m_aut_cars, i);
            if (pos != -1)
                m_aut_cars[pos] = (i+m_ca[i]) % m_size;

            // Cambia las posiciones de los autos en AC.
            At(i+m_ca[i], CA_TEMP) = m_ca[i];

            // Marca las casillas donde hay flujo de autos.
            for (unsigned j = i; j < i+m_ca[i]; ++j)
                At(j, CA_FLOW_TEMP) = 1;
        }
    }
    m_ca_flow_history.push_back(m_ca_flow_temp);
    m_ca.assign(m_ca_temp.begin(), m_ca_temp.end());
}
void AutonomousCA::Step()
{
    // Iterar sobre AC hasta encotrar vehiculo.
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {

            bool smart = aux_is_in<int>(m_aut_cars, i);
            if (smart)
            {
                // Auto inteligente.
                int nc = i + NextCarDist(i);
                nc %= m_size;
                if ((m_ca[nc] < m_vmax) && (NextCarDist(nc) > (m_ca[nc] + 1)) && (NextCarDist(i) <= m_ca[i]))
                {
                    if ((m_ca[i] < m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)))
                        m_ca[i]++;
                    else
                    {
                        // Frenado.
                        if (m_ca[i] > 0)
                        {
                            int nd = NextCarDist(i);
                            if (m_ca[nc] >= m_ca[i])
                            {
                                if (nd <= m_ca[i]-1)
                                    m_ca[i] = nd - 1;
                            }
                            else
                            {
                                if (nd <= m_ca[i])
                                    m_ca[i] = nd - 1;
                            }
                        }
                    }
                }
                else
                {
                    // Aceleracion.
                    if ((m_ca[i] < m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)))
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
                }
            }
            else
            {
                // Aceleracion.
                if ((m_ca[i] < m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)))
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
            }

            // Aleatoriedad.
            if (!smart)
            {
                bool rnd = Randomization();
                if ((m_ca[i] > 0) && rnd)
                    m_ca[i]--;
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
                           const double &rand_prob, const int &init_vel, const double &stop_density)
                           : CircularCA(size, density, vmax, rand_prob, init_vel)
{
    unsigned stops = (unsigned)(((double)size)*stop_density);
    if (stops >= size)
    {
        stops = size - 2;
        cout << "Aviso: Densidad de topes invalida. Se usaran " << stops << " topes." << endl;
    }

    // Coloca topes.
    vector<unsigned> stop_positions;
    for (unsigned i = 0; i < m_size; ++i)
        stop_positions.push_back(i);

    random_shuffle(stop_positions.begin(), stop_positions.end(), RandomGen::GetInt);
    for (unsigned i = 0; i < stops; ++i)
        m_stop_pos.push_back(stop_positions[i]);
}
void StreetStopCA::Step()
{
    // Iterar sobre AC hasta encotrar vehiculo.
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Aceleracion.
            if ((m_ca[i] < m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)))
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
    if (!m_stop_pos.empty())
    {
        vector<unsigned> dist;
        for (unsigned i = 0; i < m_stop_pos.size(); ++i)
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
int StreetStopCA::DrawHistory(string path, string out_file_name)
{
    if (out_file_name == "")
        out_file_name = path + "ca.bmp";
    else
        out_file_name = path + out_file_name;

    unsigned height = m_ca_history.size();
    unsigned width = m_size;
    BMPWriter writer(out_file_name.c_str(), width, height);
    if (writer.IsOpen())
    {
        BMPPixel* bmpData = new BMPPixel[width];
        for (int i = height-1; i >= 0; i--)
        {
            for (unsigned j = 0; j < width; ++j)
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
        return 0;
    }
    else
        return 1;
}

/****************************
*                           *
*        AC Semáforo        *
*                           *
****************************/

SemaphoreCA::SemaphoreCA(const unsigned &size, const double &density, const int &vmax, 
                         const double &rand_prob, const int &init_vel, const double &semaphore_density,
                         const bool &random_semaphores)
                       : CircularCA(size, density, vmax, rand_prob, init_vel)
{
    m_semaphore_init = 100;
    m_semaphore_open = 50;
    unsigned semaphores = (unsigned)(((double)size)*semaphore_density);
    if (semaphores >= size)
    {
        semaphores = size - 2;
        cout << "Aviso: Densidad de topes invalida. Se usaran " << semaphores << " topes." << endl;
    }

    // Coloca semáforo.
    if (random_semaphores)
    {
        vector<unsigned> semaphore_positions;
        for (unsigned i = 0; i < m_size; ++i)
            semaphore_positions.push_back(i);

        random_shuffle(semaphore_positions.begin(), semaphore_positions.end(), RandomGen::GetInt);
        for (unsigned i = 0; i < semaphores; ++i)
        {
            m_semaphore_pos.push_back(semaphore_positions[i]);
            m_semaphore_value.push_back(RandomGen::GetInt(m_semaphore_init));
        }
    }
    else
    {
        if (semaphores != 0)
        {
            int semaphore_dt = size/semaphores;
            for (unsigned i = 0; i < size; i += semaphore_dt)
            {
                m_semaphore_pos.push_back(i);
                m_semaphore_value.push_back(RandomGen::GetInt(m_semaphore_init));
            }
        }
    }
    m_semaphore_val_hist.push_back(m_semaphore_value);
}
void SemaphoreCA::Step()
{
    // Iterar sobre AC hasta encotrar vehiculo.
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        if (m_ca[i] != -1)
        {
            // Aceleracion.
            if ((m_ca[i] < m_vmax) && ((NextCarDist(i) > (m_ca[i] + 1)) && (NextSemaphoreDist(i) > (m_ca[i] + 1))))
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
    for (unsigned i = 0; i < m_semaphore_value.size(); i++)
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
        for (unsigned i = 0; i < m_semaphore_pos.size(); ++i)
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
int SemaphoreCA::DrawHistory(string path, string out_file_name)
{
    if (out_file_name == "")
        out_file_name = path + "ca.bmp";
    else
        out_file_name = path + out_file_name;

    unsigned height = m_ca_history.size();
    unsigned width = m_size;
    BMPWriter writer(out_file_name.c_str(), width, height);
    if (writer.IsOpen())
    {
        BMPPixel* bmpData = new BMPPixel[width];
        for (int i = height-1; i >= 0; i--)
        {
            for (unsigned j = 0; j < width; ++j)
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
        return 0;
    }
    else
        return 1;
}

/****************************
*                           *
*  AC Intersección simple   *
*                           *
****************************/

SimpleJunctionCA::SimpleJunctionCA(const unsigned &size, const double &density, const int &vmax, 
                                   const double &rand_prob, const int &init_vel, const double &new_car_prob,
                                   const int &new_car_speed, const int &target_lane)
                                 : OpenCA(size, density, vmax, rand_prob, init_vel, new_car_prob, new_car_speed)
{
    m_source = new OpenCA(size, density, vmax, rand_prob, init_vel, new_car_prob, new_car_speed);
    m_source->Connect(this, size / 2);
    m_target_lane = target_lane;

    if (m_target_lane < 0 || m_target_lane > 1)
    {
        m_target_lane = 1;
        cout << "Aviso: Carril objetivo invalido. Se usara el carril " << m_target_lane << "." << endl;
    }
}
SimpleJunctionCA::~SimpleJunctionCA()
{
    delete m_source;
}
void SimpleJunctionCA::Evolve(const unsigned &iter)
{
    for (unsigned i = 0; i < iter; ++i)
    {
        m_source->Step();
        this->Step();
    }
}
int SimpleJunctionCA::DrawHistory(string path, string out_file_name)
{
    if (out_file_name == "")
        out_file_name = path + "ca_junction.bmp";
    else
        out_file_name = path + out_file_name;

    m_source->DrawHistory(path);
    unsigned height = m_ca_history.size();
    unsigned width = m_size;
    BMPWriter writer(out_file_name.c_str(), width, height);
    if (writer.IsOpen())
    {
        BMPPixel* bmpData = new BMPPixel[width];
        for (int i = height - 1; i >= 0; i--)
        {
            for (unsigned j = 0; j<width; ++j)
            {
                BMPPixel color;
                if (j == m_size/2)
                {
                    if (m_ca_history[i][j] == -1)
                        color = BMPPixel((char)0, (char)255, (char)0);
                    else
                        color = BMPPixel((char)0, (char)255, (char)(255.0*(double)m_ca_history[i][j] / (double)m_vmax));
                }
                else
                {
                    if (m_ca_history[i][j] == -1)
                        color = BMPPixel((char)255, (char)255, (char)255);
                    else
                        color = BMPPixel((char)0, (char)0, (char)(255.0*(double)m_ca_history[i][j] / (double)m_vmax));
                }
                bmpData[j] = color;
            }
            writer.WriteLine(bmpData);
        }
        writer.CloseBMP();
        delete[] bmpData;
        return 0;
    }
    else
        return 1;
}
int SimpleJunctionCA::GetAt(const unsigned &i, const unsigned &j, const CAS &ca)
{
    if (m_target_lane == 0)
        return m_source->At(i, j, ca);
    else
        return this->At(i, j, ca);
}


/****************************
*                           *
*      Manejador de CA      *
*                           *
****************************/

CaHandler::CaHandler()
{
    cellularautomata = nullptr;
    circularca = nullptr;
    openca = nullptr;
    smartca = nullptr;
    streetstopca = nullptr;
    semaphoreca = nullptr;
    simplejunctionca = nullptr;
}
CaHandler::CaHandler(CA_TYPE ca, const unsigned &size, const double &density, const int &vmax,
                     const double &rand_prob, const int &init_vel, Args args, const int &custom_random_seed)
{
    cellularautomata = nullptr;
    circularca = nullptr;
    openca = nullptr;
    smartca = nullptr;
    streetstopca = nullptr;
    semaphoreca = nullptr;
    simplejunctionca = nullptr;
    CreateCa(ca, size, density, vmax, rand_prob, init_vel, args, custom_random_seed);
}
CaHandler::~CaHandler()
{
    DeleteCa();
}
void CaHandler::CreateCa(CA_TYPE ca, const unsigned &size, const double &density, const int &vmax,
                         const double &rand_prob, const int &init_vel, Args args, const int &custom_random_seed)
{
    DeleteCa();
    if (custom_random_seed != -1)
        RandomGen::Seed(custom_random_seed);
    else
        RandomGen::Seed();

    try
    {
        switch (ca)
        {
        case CIRCULAR_CA:
            cellularautomata = circularca = new CircularCA(size, density, vmax, rand_prob, init_vel);
            break;
        case OPEN_CA:
            cellularautomata = openca = new OpenCA(size, density, vmax, rand_prob, init_vel, args.GetDouble(),
                                                   args.GetInt());
            break;
        case AUTONOMOUS_CA:
            cellularautomata = smartca = new AutonomousCA(size, density, vmax, rand_prob, init_vel, args.GetDouble());
            break;
        case STOP_CA:
            cellularautomata = streetstopca = new StreetStopCA(size, density, vmax, rand_prob, init_vel, args.GetDouble());
            break;
        case SEMAPHORE_CA:
            cellularautomata = semaphoreca = new SemaphoreCA(size, density, vmax, rand_prob, init_vel, args.GetDouble(),
                args.GetBool());
            break;
        case SIMPLE_JUNCTION_CA:
            cellularautomata = simplejunctionca = new SimpleJunctionCA(size, density, vmax, rand_prob, init_vel,
                                                                       args.GetDouble(), args.GetInt(0), args.GetInt(1));
            break;
        default:
            cout << "Error: No se puede crear AC especificado en create_ca." << endl;
            break;
        };
    }
    catch (std::bad_alloc&)
    {
        cout << "Fatal: Memoria insuficiente." << endl;
        cellularautomata = nullptr;
    }
}
void CaHandler::DeleteCa()
{
    if (circularca)
        delete circularca;
    if (openca)
        delete openca;
    if (smartca)
        delete smartca;
    if (streetstopca)
        delete streetstopca;
    if (semaphoreca)
        delete semaphoreca;
    if (simplejunctionca)
        delete simplejunctionca;

    circularca = nullptr;
    openca = nullptr;
    smartca = nullptr;
    streetstopca = nullptr;
    cellularautomata = nullptr;
    semaphoreca = nullptr;
    simplejunctionca = nullptr;
}
int CaHandler::Status()
{
    if (cellularautomata != nullptr)
        return 0;
    else
        return 1;
}
void CaHandler::Evolve(const unsigned &iter)
{
    return cellularautomata->Evolve(iter);
}
int CaHandler::NextCarDist(const int &pos)
{
    return cellularautomata->NextCarDist(pos);
}
bool CaHandler::Randomization(const double &prob)
{
    return cellularautomata->Randomization(prob);
}
int &CaHandler::At(const int &i, const CAS &ca)
{
    return cellularautomata->At(i, ca);
}
int &CaHandler::At(const unsigned &i, const unsigned &j, const CAS &ca)
{
    return cellularautomata->At(i, j, ca);
}
int CaHandler::GetAt(const unsigned &i, const CAS &ca)
{
    return cellularautomata->GetAt(i, ca);
}
int CaHandler::GetAt(const unsigned &i, const unsigned &j, const CAS &ca)
{
    return cellularautomata->GetAt(i, j, ca);
}
void CaHandler::Connect(CellularAutomata* connect, unsigned connect_pos)
{
    return cellularautomata->Connect(connect, connect_pos);
}
int CaHandler::DrawHistory(std::string path, std::string out_file_name)
{
    return cellularautomata->DrawHistory(path, out_file_name);
}
int CaHandler::DrawFlowHistory(std::string path, std::string out_file_name)
{
    return cellularautomata->DrawFlowHistory(path, out_file_name);
}
void CaHandler::Print()
{
    return cellularautomata->Print();
}
unsigned CaHandler::GetSize()
{
    return cellularautomata->GetSize();
}
unsigned CaHandler::GetHistorySize()
{
    return cellularautomata->GetHistorySize();
}
unsigned CaHandler::CountCars()
{
    return cellularautomata->CountCars();
}
bool CaHandler::IsFluxHalted()
{
    return cellularautomata->IsFluxHalted();
}
void CaHandler::PrintHistory()
{
    return cellularautomata->PrintHistory();
}
void CaHandler::Step()
{
    return cellularautomata->Step();
}
void CaHandler::Move()
{
    return cellularautomata->Move();
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
    this->assign(lanes, def_val);
}

CellularAutomataML::CellularAutomataML(const unsigned &size, const unsigned &lanes, const double &density, 
                                       const int &vmax, const double &rand_prob, const int &init_vel)
{
    // Inicializa variables.
    m_test = false;
    m_size = size;
    m_lanes = lanes;
    m_vmax = vmax;
    m_rand_prob = rand_prob;
    m_init_vel = init_vel;
    m_ca.assign(size, CAElement(lanes));
    m_ca_temp.assign(size, CAElement(lanes));
    m_ca_history.clear();
    m_ca_flow_history.clear();

    m_connect = nullptr;
    m_connect_pos = -1;

    // Verifica argumentos.
    double l_density = density;
    if (l_density < 0.0 || l_density > 1.0)
    {
        l_density = 0.1;
        cout << "Aviso: Densidad invalida. Cambiando a density=" << l_density << "." << endl;
    }
    if (m_vmax < 1)
    {
        m_vmax = 1;
        cout << "Aviso: Velocidad limite invalida. Cambiando a vmax=" << m_vmax << "." << endl;
    }
    if (m_rand_prob < 0.0 || m_rand_prob > 1.0)
    {
        m_rand_prob = 0.0;
        cout << "Aviso: Probabilidad de frenado invalida. Cambiando a rand_prob=" << m_rand_prob << "." << endl;
    }
    unsigned vehicles = (unsigned)(((double)size)*density);

    // Coloca autos al azar.
    for (unsigned i = 0; i < m_lanes; ++i)
    {
        vector<unsigned> car_positions;
        for (unsigned j = 0; j < m_size; ++j)
            car_positions.push_back(j);

        random_shuffle(car_positions.begin(), car_positions.end(), RandomGen::GetInt);
        for (unsigned j = 0; j < vehicles; ++j)
            m_ca[car_positions[j]][i] = 1;
    }

    m_ca_history.push_back(m_ca);
    //m_ca_flow_history.push_back(m_ca_flow_temp);
}
CellularAutomataML::CellularAutomataML(const vector<CAElement> &ca, const vector<bool> &rand_values,
                                       const int &vmax)
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
    m_connect = NULL;
    m_connect_pos = -1;
    m_init_vel = 1;

    if (m_ca.size() == 0)
    {
        cout << "Error: El AC no contiene ningun elemento." << endl;
        m_lanes = 0;
    }
    else
        m_lanes = m_ca[0].size();
}
CellularAutomataML::~CellularAutomataML() {}
void CellularAutomataML::Print()
{
    // Imprime valores del automata celular en la terminal.
    for (unsigned i = 0; i < m_lanes; ++i)
    {
        for (unsigned j = 0; j < m_ca.size(); ++j)
        {
            if (m_ca[j][i] == -1)
                cout << "-";
            else
                cout << m_ca[j][i];
        }
        cout << endl;
    }
    for (unsigned i = 0; i < m_ca.size(); ++i)
        cout << ".";
    cout << endl;
}
int CellularAutomataML::DrawHistory(string path, string out_file_name)
{
    if (out_file_name == "")
        out_file_name = path + "ca.bmp";
    else
        out_file_name = path + out_file_name;

    unsigned bmp_height;
    if (m_lanes == 1)
        bmp_height =  m_ca_history.size();
    else
        bmp_height =  m_lanes*m_ca_history.size() + m_lanes;

    unsigned height =  m_ca_history.size();
    unsigned width = m_size;
    BMPWriter writer(out_file_name.c_str(), width, bmp_height);
    if (writer.IsOpen())
    {
        BMPPixel* bmpData = new BMPPixel[width];
        BMPPixel color;

        for (int i = height-1; i >= 0; i--)
        {
            for (unsigned l = 0; l < m_lanes; ++l)
            {
                for (unsigned j = 0; j < width; ++j)
                {
                    if (m_ca_history[i][j][l] == -1)
                        color = BMPPixel((char)255, (char)255, (char)255);
                    else
                        color = BMPPixel(0, 0, (char)(255.0*(double)m_ca_history[i][j][l]/(double)m_vmax));
                    bmpData[j] = color;
                }
                writer.WriteLine(bmpData);
            }

            if (m_lanes > 1)
            {
                for (unsigned j = 0; j < width; ++j)
                {
                    color = BMPPixel((char)0, (char)182, (char)255);
                    bmpData[j] = color;
                }
                writer.WriteLine(bmpData);
            }
        }
        writer.CloseBMP();
        delete[] bmpData;
        return 0;
    }
    else
        return 1;
}
int CellularAutomataML::DrawFlowHistory(string path, string out_file_name)
{
    if (out_file_name == "")
        out_file_name = path + "ca_flow.bmp";
    else
        out_file_name = path + out_file_name;

    unsigned bmp_height;
    if (m_lanes == 1)
        bmp_height =  m_ca_flow_history.size();
    else
        bmp_height =  m_lanes*m_ca_flow_history.size() + m_lanes;

    unsigned height =  m_ca_flow_history.size();
    unsigned width = m_size;
    BMPWriter writer(out_file_name.c_str(), width, bmp_height);
    if (writer.IsOpen())
    {
        BMPPixel* bmpData = new BMPPixel[width];
        BMPPixel color;

        for (int i = height-1; i >= 0; i--)
        {
            for (unsigned l = 0; l < m_lanes; ++l)
            {
                for (unsigned j = 0; j < width; ++j)
                {
                    if (m_ca_flow_history[i][j][l] == 0)
                        color = BMPPixel((char)255, (char)255, (char)255);
                    else
                        color = BMPPixel(0, 0, (char)(255.0*(double)m_ca_flow_history[i][j][l]/(double)m_vmax));
                    bmpData[j] = color;
                }
                writer.WriteLine(bmpData);
            }

            if (m_lanes > 1)
            {
                for (unsigned j = 0; j < width; ++j)
                {
                    color = BMPPixel((char)0, (char)182, (char)255);
                    bmpData[j] = color;
                }
                writer.WriteLine(bmpData);
            }
        }
        writer.CloseBMP();
        delete[] bmpData;
        return 0;
    }
    else
        return 1;
}
bool CellularAutomataML::Randomization(const double &prob)
{
    double l_prob;
    if (prob < 0)
        l_prob = m_rand_prob;
    else
        l_prob = prob;

    // Si está en modo de prueba toma los valores aleatorios de la lista.
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
        if (RandomGen::GetDouble() <= l_prob)
            return true;
        else
            return false;
    }
}
void CellularAutomataML::ChangeLanes()
{
    // Primero hace los cambios de carril.
    if (m_lanes > 1)
    {
        for (unsigned i = 0; i < m_ca.size(); ++i)
        {
            for (unsigned j = 0; j < m_lanes; ++j)
            {
                // Si no puede acelerar
                if (!((At(i, j) < m_vmax) && (NextCarDist(i, j) > (At(i, j) + 1))))
                {
                    // Intenta cambiar de carril.
                    bool left = false, right = false;
                    for (int k = (int)j-1; k <= (int)j+1 && k < (int)m_lanes; k++)
                    {
                        // Evita carril inexistente, el mismo y carril ocupado.
                        if (k<0 || (unsigned)k==j || At(i, k) != -1) continue;

                        // Busca auto anterior y verifica si puede cambiarse sin chocar.
                        int v, s=0;
                        for (int l = i-1; s <= m_vmax; ++s, l--)
                        {
                            // Verifica posibilidad de choque.
                            v = At(l, k);
                            if ((v != -1) && (v-s <= 0))
                            {
                                // Ahora busca si en este nuevo carril puede acelerar.
                                if (NextCarDist(i, k) > (At(i, j) + 1))
                                {
                                    // Marca carril como disponible.
                                    if ((unsigned)k == j-1) left = true;
                                    else right = true;
                                }
                                if (left && right)
                                    break;
                            }
                        }
                        if (s == m_vmax)
                        {
                            if (NextCarDist(i, k) > (At(i, j) + 1))
                            {
                                // Marca carril como disponible.
                                if ((unsigned)k == j-1) left = true;
                                else right = true;
                            }
                            if (left && right)
                                break;
                        }
                    }

                    // Si el carril está disponible adelenta.
                    if (left || right)
                    {
                        // Si ambos están disponibles apaga uno al azar.
                        if (left && right)
                        {
                            int select = RandomGen::GetInt(2);
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
void CellularAutomataML::Step()
{
    // Primero hace los cambios de carril.
    ChangeLanes();

    // Iterar sobre AC.
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        for (unsigned j = 0; j < m_lanes; ++j)
        {
            // Encuentra vehículo.
            if (At(i, j) != -1)
            {
                // Aceleracion.
                if ((At(i, j) < m_vmax) && (NextCarDist(i, j) > (At(i, j) + 1)))
                    At(i, j)++;
                else
                {
                    // Frenado.
                    if (At(i, j) > 0)
                    {
                        int nd = NextCarDist(i, j);
                        if (nd <= At(i, j))
                            At(i, j) = nd - 1;
                    }
                }

                // Aleatoriedad.
                bool rnd = Randomization();
                if ((At(i, j) > 0) && rnd)
                    At(i, j)--;
            }
        }
    }

    // Aplicar cambios.
    Move();
    m_ca_history.push_back(m_ca);
}
void CellularAutomataML::Evolve(const unsigned &iter)
{
    for (unsigned i = 0; i < iter; ++i)
    {
        Step();
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
unsigned CellularAutomataML::GetLanes()
{
    return m_lanes;
}
unsigned CellularAutomataML::CountCars()
{
    unsigned count = 0;
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        for (unsigned j = 0; j < m_lanes; ++j)
        {
            if (m_ca[i][j] != -1)
                count++;
        }
    }
    return count;
}
int &CellularAutomataML::At(const int &i, const unsigned &lane, const CAS &ca)
{
    if (m_connect != nullptr && i >= (int)m_size)
        return m_connect->At(i- m_size + m_connect_pos, 0, ca);
    else
        return At(i, lane, 0, ca);
}
int CellularAutomataML::GetAt(const unsigned &i, const unsigned &lane, const CAS &ca)
{
    return At(i, lane, ca);
}
int CellularAutomataML::GetAt(const unsigned &i, const unsigned &lane, const unsigned &j, const CAS &ca)
{
    return At(i, lane, j, ca);
}
void CellularAutomataML::Connect(CellularAutomataML* connect, unsigned connect_pos)
{
    m_connect = connect;
    m_connect_pos = connect_pos;
    if (m_connect_pos > m_size)
    {
        m_connect_pos = m_size / 2;
        cout << "Valor incorrecto de posicion de conexion. Cambiando a " << m_connect_pos << endl;
    }
}
int CellularAutomataML::NextCarDist(const int &pos, const unsigned &lane)
{
    int dist = 1;
    while ((At(pos+dist, lane) == -1) && (dist < 2*(int)m_size))
        dist++;
    return dist;
}
void CellularAutomataML::Move()
{
    m_ca_flow_temp.assign(m_size, CAElement(m_lanes, 0));
    for (unsigned i = 0; i < m_ca_temp.size(); ++i)
    {
        for (unsigned j = 0; j < m_lanes; ++j)
            m_ca_temp[i][j] = -1;
    }

    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        for (unsigned j = 0; j < m_lanes; ++j)
        {
            if (m_ca[i][j] != -1)
            {
                // Cambia las posiciones de los autos en AC.
                At(i+m_ca[i][j], j, CA_TEMP) = m_ca[i][j];

                // Marca las casillas donde hay flujo de autos.
                for (unsigned k = i; k < i+m_ca[i][j]; ++k)
                    At(k, j, CA_FLOW_TEMP) = 1;
            }
        }
    }
    m_ca_flow_history.push_back(m_ca_flow_temp);
    m_ca.assign(m_ca_temp.begin(), m_ca_temp.end());
}


/****************************
*                           *
*      AC Circular ML       *
*                           *
****************************/

CircularCAML::CircularCAML(const unsigned &size, const unsigned &lanes, const double &density,
                           const int &vmax, const double &rand_prob, const int &init_vel)
                         : CellularAutomataML(size, lanes, density, vmax, rand_prob, init_vel) {}
CircularCAML::CircularCAML(const vector<CAElement> &ca, const vector<bool> &rand_values, const int &vmax)
                         : CellularAutomataML(ca, rand_values, vmax) {}
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
void CircularCAML::Evolve(const unsigned &iter)
{
    unsigned cars = CountCars();
    for (unsigned i = 0; i < iter; ++i)
        Step();

    if (cars != CountCars())
        cout << "Error: La cantidad de autos no se conserva." << endl;
}


/****************************
*                           *
*       AC Abierto ML       *
*                           *
****************************/

OpenCAML::OpenCAML(const unsigned &size, const unsigned int &lanes, const double &density, const int &vmax,
                   const double &rand_prob, const int &init_vel, const double &new_car_prob, const int &new_car_speed)
                 : CellularAutomataML(size, lanes, density, vmax, rand_prob, init_vel)
{
    m_new_car_prob = new_car_prob;
    m_new_car_speed = new_car_speed;
    m_empty = -1;

    // Verifica argumento.
    if (m_new_car_prob < 0 || m_new_car_prob > 1)
    {
        m_new_car_prob = 0.5;
        cout << "Aviso: Probabilidad de nuevo auto invalida. Cambiando a new_car_prob=" << m_new_car_prob << "." << endl;
    }
}
OpenCAML::OpenCAML(const vector<CAElement> &ca, const vector<bool> &rand_values, const int &vmax,
                   const int &new_car_speed)
                 : CellularAutomataML(ca, rand_values, vmax)
{
    m_new_car_prob = -1.0;
    m_new_car_speed = new_car_speed;
    m_empty = -1;
}
int &OpenCAML::At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca)
{
    if (i < 0 || (unsigned)i >= m_ca.size() || j >= m_lanes)
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
            return m_ca_temp[i][lane];
            break;
        case CA_FLOW_TEMP:
            return m_ca_flow_temp[i][lane];
            break;
        case CA_HISTORY:
            return m_ca_history[j][i][lane];
            break;
        case CA_FLOW_HISTORY:
            return m_ca_flow_history[j][i][lane];
            break;
        default:
            return m_ca[i][lane];
            break;
        };
    }
}
void OpenCAML::Step()
{
    // Primero hace los cambios de carril.
    ChangeLanes();

    // Iterar sobre AC.
    for (unsigned i = 0; i < m_ca.size(); ++i)
    {
        for (unsigned j = 0; j < m_lanes; ++j)
        {
            // Encuentra vehículo.
            if (At(i, j) != -1)
            {
                // Aceleracion.
                if ((At(i, j) < m_vmax) && (NextCarDist(i, j) > (At(i, j) + 1)))
                    At(i, j)++;
                else
                {
                    // Frenado.
                    if (At(i, j) > 0)
                    {
                        int nd = NextCarDist(i, j);
                        if (nd <= At(i, j))
                            At(i, j) = nd - 1;
                    }
                }

                // Aleatoriedad.
                bool rnd = Randomization();
                if ((At(i, j) > 0) && rnd)
                    At(i, j)--;
            }
        }
    }

    // Añade coche con probabilidad aleatoria.
    for (unsigned i = 0; i < m_lanes; ++i)
    {
        if (m_ca[0][i] == -1 && Randomization(m_new_car_prob))
            m_ca[0][i] = m_new_car_speed;
    }

    // Aplicar cambios.
    Move();
    m_ca_history.push_back(m_ca);
}

/********************************
*                               *
*  Manejador de CA Multicarril  *
*                               *
********************************/

CaHandlerML::CaHandlerML()
{
    cellularautomataml = nullptr;
    circularcaml = nullptr;
    opencaml = nullptr;
}
CaHandlerML::CaHandlerML(CA_TYPE ca, const unsigned &size, const unsigned &lanes, const double &density, const int &vmax,
                         const double &rand_prob, const int &init_vel, Args args, const int &custom_random_seed)
{
    CreateCa(ca, size, lanes, density, vmax, rand_prob, init_vel, args, custom_random_seed);
}
CaHandlerML::~CaHandlerML()
{
    DeleteCa();
}
void CaHandlerML::CreateCa(CA_TYPE ca, const unsigned &size, const unsigned &lanes, const double &density,
                           const int &vmax, const double &rand_prob, const int &init_vel, Args args, 
                           const int &custom_random_seed)
{
    DeleteCa();
    if (custom_random_seed != -1)
        RandomGen::Seed(custom_random_seed);
    else
        RandomGen::Seed();

    try
    {
        switch (ca)
        {
        case CIRCULAR_MULTILANE_CA:
            cellularautomataml = circularcaml = new CircularCAML(size, lanes, density, vmax, rand_prob, init_vel);
            break;
        case OPEN_MULTILANE_CA:
            cellularautomataml = opencaml = new OpenCAML(size, lanes, density, vmax, rand_prob, init_vel, args.GetDouble(),
                                                         args.GetInt());
            break;
        default:
            cout << "Error: No se puede crear AC especificado en create_multilane_ca." << endl;
            break;
        };
    }
    catch (std::bad_alloc&)
    {
        cout << "Fatal: Memoria insuficiente." << endl;
        cellularautomataml = nullptr;
    }
}
void CaHandlerML::DeleteCa()
{
    if (circularcaml)
        delete circularcaml;
    if (opencaml)
        delete opencaml;

    circularcaml = nullptr;
    opencaml = nullptr;
}
int CaHandlerML::Status()
{
    if (cellularautomataml != nullptr)
        return 0;
    else
        return 1;
}
void CaHandlerML::Evolve(const unsigned &iter)
{
    return cellularautomataml->Evolve(iter);
}
int CaHandlerML::NextCarDist(const int &pos, const unsigned &lane)
{
    return cellularautomataml->NextCarDist(pos, lane);
}
bool CaHandlerML::Randomization(const double &prob)
{
    return cellularautomataml->Randomization(prob);
}
int &CaHandlerML::At(const int &i, const unsigned &lane, const CAS &ca)
{
    return cellularautomataml->At(i, lane, ca);
}
int &CaHandlerML::At(const int &i, const unsigned &lane, const unsigned &j, const CAS &ca)
{
    return cellularautomataml->At(i, lane, j, ca);
}
int CaHandlerML::GetAt(const unsigned &i, const unsigned &lane, const CAS &ca)
{
    return cellularautomataml->GetAt(i, lane, ca);
}
int CaHandlerML::GetAt(const unsigned &i, const unsigned &lane, const unsigned &j, const CAS &ca)
{
    return cellularautomataml->GetAt(i, lane, j, ca);
}
int CaHandlerML::DrawHistory(std::string path, std::string out_file_name)
{
    return cellularautomataml->DrawHistory(path, out_file_name);
}
int CaHandlerML::DrawFlowHistory(std::string path, std::string out_file_name)
{
    return cellularautomataml->DrawFlowHistory(path, out_file_name);
}
void CaHandlerML::Print()
{
    return cellularautomataml->Print();
}
unsigned CaHandlerML::GetSize()
{
    return cellularautomataml->GetSize();
}
unsigned CaHandlerML::GetHistorySize()
{
    return cellularautomataml->GetHistorySize();
}
unsigned CaHandlerML::GetLanes()
{
    return cellularautomataml->GetLanes();
}
unsigned CaHandlerML::CountCars()
{
    return cellularautomataml->CountCars();
}
void CaHandlerML::Step()
{
    return cellularautomataml->Step();
}
void CaHandlerML::Move()
{
    return cellularautomataml->Move();
}
