#pragma once
#include <memory>
#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>
#include "ErrorCodes.h"
#include "Log.h"

class AbstractCommandHandler {
public:
    virtual int HandleCommand(const std::string& cmd){return EFAIL;};
};
class BaseCommand
{
public:
    virtual ~BaseCommand() {}
};

template <class... ArgTypes>
class Command : public BaseCommand
{
  typedef std::function<void(ArgTypes...)> FuncType;
  FuncType f_;
  public:
    Command() {}
    Command(FuncType f) : f_(f) {}
    void operator()(ArgTypes... args) { if(f_) f_(args...); }
};

class BaseCommandHandler :public AbstractCommandHandler {
public:
    
    
    template <class... ArgTypes> int ExecuteCommand(const std::string& cmd_n, ArgTypes... args);
    template <class T> int AddCommand(const std::string& cmd_n, const T& cmd);
private: 
    typedef std::unordered_map< std::string , std::shared_ptr< BaseCommand >  > CmdMapType;
    
     CmdMapType d_cmd_map;
};




class ConsoleCommandHandler :public BaseCommandHandler{
public:
    typedef Command< const std::string &, std::vector < std::string > * > StrCommand;
    int HandleCommand(const std::string& str);
private:
    std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems);
};

class InputCommandHandler :public BaseCommandHandler {
public:
    typedef Command< > InputCommand;
    int HandleCommand(const std::string& str);
    template <typename  T>
    int HandleInputKey(T key_code);
    template <typename  T>
    int BindKey(T key_code, const std::string& name );

private:
    /*vs map*/
    std::unordered_map< int , std::string > d_key_map;
};


template < typename  T>
int InputCommandHandler::BindKey(T key_code, const std::string& name ) {
    d_key_map[static_cast<int>(key_code)] = name;
    return 0;
}
template <typename  T>
int InputCommandHandler::HandleInputKey(T key_code) {
    const std::string & command = d_key_map [ static_cast<int>(key_code)];
    /*exec*/
    HandleCommand (command);
    return 0;
}

template <class... ArgTypes> 
int BaseCommandHandler::ExecuteCommand(const std::string& cmd_n, ArgTypes... args){
    CmdMapType::const_iterator it  = d_cmd_map.find(cmd_n);
    if (it != d_cmd_map.end() ) {
        auto &cmd =   it->second;
        /*EXXXX static_cast??? or dynamic??*/
        Command<ArgTypes...> * c = static_cast<  Command < ArgTypes...>  *   >(cmd.get());
        if (c) {
            (*c)(args...);
        }// else
            //EMSGS("Invalid cmd type!\n");
    } else
         LOGE("No cmd: " + cmd_n);
    return 0;
}
template <class T> 
int BaseCommandHandler::AddCommand(const std::string& cmd_n, const T& cmd) {
    d_cmd_map[cmd_n] = std::shared_ptr< BaseCommand > (new T(cmd));
    return 0;
}
