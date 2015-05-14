#include "CellularAutomata.h"
using namespace std;

/****************************
*                           *
*   Funciones auxiliares    *
*                           *
****************************/

void aux_progress_bar(double progress)
{
    int barWidth = 35;

    std::cout << "[";
    int pos = (int)(barWidth * progress);
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
    if (progress == 1.0)
        std::cout << std::endl;
}


/****************************
*                           *
*         AC Básico         *
*                           *
****************************/

CellularAutomata::CellularAutomata(const unsigned &size, const double &density, const int &vmax, const double &rand_prob)
{
    // Inicializa variables.
    m_size = size;
    m_vmax = vmax;
    m_rand_prob = rand_prob;
    m_ca.assign(size, -1);
    m_ca_temp.assign(size, -1);
    m_ca_history.clear();
    m_ca_flow_history.clear();
    unsigned vehicles = (unsigned)(((double)size)*density);

    // Asigna semilla a numeros aleatorios.
    unsigned seed = static_cast<unsigned int>(time(NULL));
    m_drand.seed(seed);
    m_irand.seed(seed);

    // Coloca autos al azar.
    for (unsigned i=0; i<vehicles; ++i)
    {
        unsigned pos;
        do
        {
            pos = m_irand() % m_ca.size();
        }
        while (m_ca[pos] != -1);
        m_ca[pos] = 1;
    }
    m_ca_history.push_back(m_ca);
}
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

            // Frenado.
            if (m_ca[i] > 0)
            {
                int nd = NextCarDist(i);
                if (nd <= m_ca[i])
                    m_ca[i] = nd - 1;
            }

            // Aleatoriedad.
            if ((m_ca[i] > 0) && (m_drand() <= m_rand_prob))
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
int &CellularAutomata::At(const unsigned &i, const CAS &ca)
{
    return At(i, 0, ca);
}


/****************************
*                           *
*        AC Circular        *
*                           *
****************************/

CircularCA::CircularCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob) 
    : CellularAutomata(size, density, vmax, rand_prob) {}
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
int CircularCA::NextCarDist(const int &pos)
{
    unsigned dist = 1;
    while (At(pos+dist) == -1)
        dist++;
    return dist;
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


/****************************
*                           *
*        AC Abierto         *
*                           *
****************************/

OpenCA::OpenCA(const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &new_car_prob) 
    : CellularAutomata(size, density, vmax, rand_prob)
{
    m_new_car_prob = new_car_prob;
}
int &OpenCA::At(const unsigned &i, const unsigned &j, const CAS &ca)
{
    if (i >= m_ca.size())
    {
        if ((ca == CA) || (ca == CA_TEMP) || (ca == CA_HISTORY))
            empty = -1;
        else
            empty = 0;
        return empty;
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
int OpenCA::NextCarDist(const int &pos)
{
    unsigned dist = 1;
    while ((At(pos+dist) == -1) && ((pos+dist) < 2*m_size))
        dist++;
    return dist;
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
    if (m_drand() < m_new_car_prob)
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
    // Selecciona autos inteligentes.
    unsigned smart_car_number = (unsigned)(((double)size)*smart_density);
    std::vector<int>::iterator it;
    for (unsigned i=0; i<smart_car_number; ++i)
    {
        unsigned pos;
        do
        {
            pos = m_irand() % m_ca.size();
        }
        while ((m_ca[pos] != -1) || aux_is_in<int>(smart_cars, pos));
        smart_cars.push_back(pos);
    }
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
            int pos = aux_find_pos<int>(smart_cars, i);
            if (pos != -1)
                smart_cars[pos] = (i+m_ca[i]) % m_size;

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
            if (aux_is_in<int>(smart_cars, i))
            {
                // Auto inteligente.
                int nc = i + NextCarDist(i);
                nc %= m_size;
                if ((m_ca[i] <= m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)) || (m_ca[nc] <= m_vmax) && (NextCarDist(nc) > (m_ca[nc] + 1)))
                    m_ca[i]++;
            }
            else
            {
                // Aceleracion.
                if ((m_ca[i] <= m_vmax) && (NextCarDist(i) > (m_ca[i] + 1)))
                    m_ca[i]++;

                // Aleatoriedad.
                if ((m_ca[i] > 0) && (m_drand() <= m_rand_prob))
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
*      Manejador de CA      *
*                           *
****************************/

CellularAutomata* cellularautomata = NULL;
CircularCA* circularca = NULL;
OpenCA* openca = NULL;
SmartCA* smartca = NULL;

CellularAutomata* create_ca(CA_TYPE ct, const unsigned &size, const double &density, const int &vmax, const double &rand_prob, const double &extra)
{
    delete_ca();
    switch(ct)
    {
    case CIRCULAR_CA:
        cellularautomata = circularca = new CircularCA(size, density, vmax, rand_prob);
        break;
    case OPEN_CA:
        cellularautomata = openca = new OpenCA(size, density, vmax, rand_prob, extra);
        break;
    case SMART_CA:
        cellularautomata = smartca = new SmartCA(size, density, vmax, rand_prob, extra);
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

    circularca = NULL;
    openca = NULL;
    smartca = NULL;
    cellularautomata = NULL;
}