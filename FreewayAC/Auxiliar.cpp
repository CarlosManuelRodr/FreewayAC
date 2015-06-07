#include <chrono>
#include <random>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <sstream>
#include <algorithm>
#include <fstream>
#include "Auxiliar.h"

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

#elif defined(_WIN32)
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#include <direct.h>
#include <Shlobj.h>
#include <windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

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
void aux_create_directory(string directory_name)
{
    string path = df_get_app_folder() + f_separator + directory_name;
    if (!df_directory_exist(path))
        s_mkdir(path);
}

/*****************************
*                            *
*   Directorios y archivos   *
*                            *
*****************************/

bool df_directory_exist(string path)
{
    const char* pzPath = path.c_str();
#if defined(__linux__) || defined(__APPLE__)
    if ( pzPath == NULL) return false;

    DIR *pDir;
    bool bExists = false;

    pDir = opendir (pzPath);

    if (pDir != NULL)
    {
        bExists = true;
        (void) closedir (pDir);
    }

    return bExists;
#elif defined(_WIN32)
    DWORD ftyp = GetFileAttributesA(pzPath);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;
    return false;
#endif
}
bool df_file_exist(string file_name)
{
    ifstream infile(file_name.c_str());
    bool res = infile.is_open();
    if (res)
        infile.close();
    return res;
}
string df_get_app_folder()
{
#if defined(__linux__)
    string path = "";
    pid_t pid = getpid();
    char buf[20] = {0};
    sprintf(buf,"%d",pid);
    string _link = "/proc/";
    _link.append( buf );
    _link.append( "/exe");
    char proc[512];
    int ch = readlink(_link.c_str(), proc, 512);
    if (ch != -1)
    {
        proc[ch] = 0;
        path = proc;
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
    }
    return path;
#elif defined(_WIN32)
    wchar_t ownPth[MAX_PATH];
    char mbOwnPth[MAX_PATH];

    HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
		PathRemoveFileSpec(ownPth);
		wcstombs(mbOwnPth, ownPth, MAX_PATH);
		return string(mbOwnPth);
	}
	else
		return string("");
#endif
}

/****************************
*                           *
*    Llamadas al sistema    *
*                           *
****************************/

#if defined(__linux__) || defined(__APPLE__)
int s_mkpath(std::string s, mode_t mode)
{
    size_t pre=0,pos;
    std::string dir;
    int mdret;

    if (s[s.size()-1]!='/')
        s += '/';

    while ((pos = s.find_first_of('/',pre)) != std::string::npos)
    {
        dir=s.substr(0, pos++);
        pre=pos;
        if (dir.size() == 0)
            continue;

        if ((mdret = mkdir(dir.c_str(),mode)) && errno != EEXIST)
            return mdret;
    }
    return mdret;
}
#endif

void s_replace_all(string &s, const string &search, const string &replace)
{
	for (size_t pos = 0; ; pos += replace.length())
	{
		// Locate the substring to replace
		pos = s.find(search, pos);
		if (pos == string::npos) break;
		// Replace by erasing and inserting
		s.erase(pos, search.length());
		s.insert(pos, replace);
}
}

string s_format_path(const string in, bool support_long_path = false)
{
#if defined(__linux__) || defined(__APPLE__)
	return string(in);
#elif defined(_WIN32)
	string out(in);
	s_replace_all(out, "/", "\\");
	if (support_long_path)
		out = string("\\\\?\\") + out;
	return out;
#endif
}

void s_mkdir(const string arg)
{
#if defined(__linux__) || defined(__APPLE__)
    s_mkpath(arg, 0755);
#elif defined(_WIN32)
	string in = s_format_path(arg);
    wstring w_arg = wstring(in.begin(), in.end());
    CreateDirectory(w_arg.c_str(), NULL);
#endif
}
void s_rm(const string arg)
{
#if defined(__linux__) || defined(__APPLE__)
    ::remove(arg.c_str());
#elif defined(_WIN32)
    std::ifstream infile(arg);
    if (infile.is_open())
    {
        infile.close();
		string in = s_format_path(arg);
        wstring w_arg = wstring(in.begin(), in.end());
        DeleteFile(w_arg.c_str());
    }
#endif
}

/****************************
*                           *
*  Generador de aleatorios  *
*                           *
****************************/

RandomAlgorithm RandomGen::m_ra = MT19937;
std::mt19937 RandomGen::mt;
std::ranlux24 RandomGen::rl24;
std::ranlux48 RandomGen::rl48;

void RandomGen::SetAlgorithm(RandomAlgorithm ra)
{
    m_ra = ra;
}
void RandomGen::Seed(int seed)
{
    if (seed == -1)
        seed = static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count());

    switch (m_ra)
    {
    case MT19937:
        mt.seed(seed);
        break;
    case LCG:
        srand(seed);
        break;
    case RANLUX24:
        rl24.seed(seed);
        break;
    case RANLUX48:
        rl48.seed(seed);
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
    case RANLUX24:
        return rl24() % i;
        break;
    case RANLUX48:
        return rl48() % i;
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
    case RANLUX24:
        return (double)rl24()/(double)rl24.max();
        break;
    case RANLUX48:
        return (double)rl48()/(double)rl48.max();
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
