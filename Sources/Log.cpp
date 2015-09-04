#include "Log.h"
#include <iostream>
#include "MAssert.h"
#include "ErrorCodes.h"
Log::~Log()
{
    d_logfile_stream.flush();
    d_logfile_stream.close();
}

Log::Log(Log::Verbosity _v, const std::string &s)
    :d_logfile_stream(s),
    v(_v)
{

}

int Log::LogW(const std::string &s)
{
    d_logfile_stream <<  "[W]" << s << std::endl;
    return ESUCCESS;
}

int Log::LogE(const std::string &s)
{
   d_logfile_stream <<  "[E]" << s << std::endl;

   if (d_callback)
       d_callback(Log::L_ERROR,s+'\n');
   /*log to std console -- remove me*/
   std::cout << s << std::endl;
   std::cout.flush();
   d_logfile_stream.flush();
   return ESUCCESS;
}

int Log::LogV(const std::string &s)
{
    d_logfile_stream <<  "[V]" << s << std::endl;
    return ESUCCESS;
}

void Log::SetCallback(std::function<void (Log::Verbosity, const std::string &)> callback)
{
    MASSERT(!callback);
    d_callback = callback;
}
