#pragma once
#include "Singltone.h"
#include <fstream>
#include <string>
#include <functional>
#include <stdarg.h> 
class Log
{
public:
    enum Verbosity {
      L_WARNING,
      L_ERROR,
      L_VERBOSE,
      L_TRACE
    };

    Log(Log::Verbosity max_v,const std::string& fname);
    Log(): Log(L_WARNING,"Default.log") {}
    Log(const std::string& fname): Log(L_WARNING, fname) {}
    ~Log();
    int LogW(const std::string& s);
    int LogFmtW(const char * fmt_str, ...);
    int LogE(const std::string& s);
    int LogFmtE(const char * fmt_str, ...);
    int LogV(const std::string& s);
    int LogFmtV(const char * fmt_str, ...);

    void SetCallback(std::function<void(Log::Verbosity v,const std::string& s)> callback );
private:
    std::string log_file;
    std::ofstream d_logfile_stream;
    Verbosity v;
    std::function<void (Log::Verbosity, const std::string &)> d_callback;

};
#define LOGW(...) (Singltone<Log>::GetInstance()->LogFmtW(__VA_ARGS__))
#define LOGE(...) (Singltone<Log>::GetInstance()->LogFmtE(__VA_ARGS__))
#define LOGV(...) (Singltone<Log>::GetInstance()->LogFmtV(__VA_ARGS__))