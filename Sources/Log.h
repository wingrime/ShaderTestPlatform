#pragma once
#include "Singltone.h"
#include <fstream>
#include <string>
#include <functional>

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
    int LogE(const std::string& s);
    int LogV(const std::string& s);
    void SetCallback(std::function<void(Log::Verbosity v,const std::string& s)> callback );
private:
    Verbosity v;
    std::string log_file;
    std::ofstream d_logfile_stream;
    std::function<void (Log::Verbosity, const std::string &)> d_callback;

};
class MainLog :public Singltone<MainLog>  ,public Log{
public:
    MainLog() :Log ("Main.log") , Singltone(this) {}
};
#define LOGW(a) (MainLog::GetInstance()->LogW(a))
#define LOGE(a) (MainLog::GetInstance()->LogE(a))
#define LOGV(a) (MainLog::GetInstance()->LogV(a))
