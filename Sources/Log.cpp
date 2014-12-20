#include "Log.h"

Log::~Log()
{
    d_logfile_stream.flush();
    d_logfile_stream.close();
}

Log::Log(Log::Verbosity _v, const std::string &s)
    :d_logfile_stream(s)
{

}

void Log::LogW(const std::string &s)
{
    d_logfile_stream <<  "[W]" << s << std::endl;
}

void Log::LogE(const std::string &s)
{
   d_logfile_stream <<  "[E]" << s << std::endl;
}

void Log::LogV(const std::string &s)
{
    d_logfile_stream <<  "[V]" << s << std::endl;
}
