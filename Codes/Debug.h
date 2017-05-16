#ifndef _Debug_h_
#define _Debug_h_

#include <string>
#include <map>
#include <chrono>

std::ostream& DbgClearStream(char const*, uint32_t);
std::ostream& DbgOstream(char const*, uint32_t);
std::ostream& ErrOstream(char const*, uint32_t);
std::wostream& DbgClearWstream(char const* funcName, uint32_t line);
std::wostream& DbgWostream(char const* funcName, uint32_t line);
std::wostream& ErrWostream(char const* funcName, uint32_t line);

#define dbgcstrm DbgClearStream(__func__, __LINE__)
#define dbgstrm DbgOstream(__func__, __LINE__)
#define errstrm ErrOstream(__func__, __LINE__)
#define dbgcwstrm DbgClearWstream(__func__, __LINE__)
#define dbgwstrm DbgWostream(__func__, __LINE__)
#define errwstrm ErrWostream(__func__, __LINE__)

/*
Example:
    DebugFlag debugFlag;
    debugFlag.SetState("Controller::handle_close", true);
*/
class DebugFlag
{
public:
    static DebugFlag& GetInstance()
    {
        static DebugFlag instance;
        return instance;
    }

    bool GetState(std::string const& funcName);
    void SetState(std::string const& funcName, bool doDebug);

private:
    DebugFlag();
    std::map<std::string, bool> flags;
};

#define Print(...) \
{   const char* func = __func__; int line = __LINE__; \
    DebugFlag flag;                     \
    if (flag.GetState(func))            \
    {                                   \
        printf("%s, %d> ", func, line); \
        printf(__VA_ARGS__);            \
    }                                   \
}

#endif /* _Debug_h_ */
