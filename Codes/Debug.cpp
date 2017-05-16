#include "SystemInclude.h"

/* Foundation */
#include "Debug.h"
using namespace std;

DebugFlag::DebugFlag()
{}

bool DebugFlag::GetState(string const& funcName)
{
    map<string, bool>::iterator iter = flags.find(funcName);
    if (iter != flags.end())
    {
        return iter->second;
    }

    return false;
}

void DebugFlag::SetState(string const& funcName, bool doDebug)
{
    map<string, bool>::iterator iter = flags.find(funcName);
    if (iter == flags.end())
    {
        flags.insert(make_pair(funcName, doDebug));
        return;
    }

    iter->second = doDebug;
}

ostream& DbgClearStream(char const* funcName, uint32_t line)
{
    static ostream nullStream(0);
    static bool first = true;
    if (first)
    {
        first = false;
        nullStream.clear(ios::eofbit);
    }

    DebugFlag& debugFlag = DebugFlag::GetInstance();
    ostream& os = debugFlag.GetState(funcName) ? cout : nullStream;

    return os;
}

ostream& DbgOstream(char const* funcName, uint32_t line)
{
    ostream& os = DbgClearStream(funcName, line);

    os << funcName << ", " << line << "> ";
    return os;
}

ostream& ErrOstream(char const* funcName, uint32_t line)
{
    cerr << funcName << ", " << line << "> ";
    return cerr;
}

wostream& DbgClearWstream(char const* funcName, uint32_t line)
{
    static wostream nullStream(0);
    static wostream& wcout = std::wcout;
    static bool first = true;
    if (first)
    {
        first = false;
        nullStream.clear(ios::eofbit);
        wcout.imbue(locale("chs"));
    }

    DebugFlag& debugFlag = DebugFlag::GetInstance();
    wostream& wos = debugFlag.GetState(funcName) ? wcout : nullStream;

    return wos;
}

wostream& DbgWostream(char const* funcName, uint32_t line)
{
    wostream& wos = DbgClearWstream(funcName, line);

    wos << funcName << L", " << line << L"> ";
    return wos;
}

wostream& ErrWostream(char const* funcName, uint32_t line)
{
    wcerr << funcName << ", " << line << "> ";
    return wcerr;
}

