#include "Log.h"
#include <iostream>
#include <stdarg.h> 
#include "MAssert.h"
#include "ErrorCodes.h"
#include "string_format.h"
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
       if (d_callback)
       d_callback(Log::L_WARNING,std::string("[WARN] ")+s+'\n');
    return ESUCCESS;
}
int Log::LogFmtW(const std::string& fmt_str, ...) 
{
  va_list args;
  va_start(args, fmt_str);
  int r = LogW(string_format(fmt_str,args));
  va_end(args);
  return r;
}
int Log::LogFmtE(const std::string& fmt_str, ...) 
{
  va_list args;
  va_start(args, fmt_str);
  int r = LogE(string_format(fmt_str,args));
  va_end(args);
  return r;
}
int Log::LogFmtV(const std::string& fmt_str, ...) 
{
  va_list args;
  va_start(args, fmt_str);
  int r = LogV(string_format(fmt_str,args));
  va_end(args);
  return r;
}
int Log::LogE(const std::string &s)
{
   d_logfile_stream <<  "[E]" << s << std::endl;

   if (d_callback)
       d_callback(Log::L_ERROR,std::string("[ERR] ")+s+'\n');
   /*log to std console -- remove me*/
   std::cout << s << std::endl;
   std::cout.flush();
   d_logfile_stream.flush();
   return ESUCCESS;
}

int Log::LogV(const std::string &s)
{
  if (d_callback)
    d_callback(Log::L_VERBOSE,std::string("[VERB] ")+s+'\n');
  d_logfile_stream <<  "[V]" << s << std::endl;
  return ESUCCESS;
}

void Log::SetCallback(std::function<void (Log::Verbosity, const std::string &)> callback)
{
    MASSERT(!callback);
    d_callback = callback;
}
