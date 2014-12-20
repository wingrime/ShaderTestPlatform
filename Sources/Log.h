#pragma once
#include "Singltone.h"
#include <string>
#include <fstream>
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
    void LogW(const std::string& s);
    void LogE(const std::string& s);
    void LogV(const std::string& s);
private:
    Verbosity v;
    std::string log_file;
    std::ofstream d_logfile_stream;

};
class MainLog :public Singltone<MainLog>  ,public Log{
public:
    MainLog() :Log ("Main.log"){}
};
#define LOGW(a) (MainLog::GetInstance()->LogW(a))
#define LOGE(a) (MainLog::GetInstance()->LogE(a))
#define LOGV(a) (MainLog::GetInstance()->LogV(a))
