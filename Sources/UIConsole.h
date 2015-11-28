#pragma once
#include <string>
#include <vector>
#include <memory>

#include "Command.h"
#include "e_base.h"
/* font factory compile time depedency*/
#include "UIFont.h"
#include "mat_math.h"

class RBO;


class UIConsole {
public:
    UIConsole(RectSizeInt& v, AbstractCommandHandler* cmd_h);
    ~UIConsole();
    /*Draw console*/
    void Draw();
    /*Add simple message*/ 
    void Msg(const std::string& msg);
    /*void handle input*/
    int HandleInputKey(unsigned char key_code);
    int HandleExitConsole();
    int HandlePrevHistoryCommand();
    int HandleNextHistoryCommand();
    void Cls();

private:
    int HandleCommand(const std::string& cmd);
    void RebuildOut();
    void HandleDelKey();

    const unsigned int d_con_sz =  20; /*max messages in console log*/
    UIFontFactory ft;
    RectSizeInt v_port;
    UIFont * fnt ;
    std::vector<std::string> con;
    std::vector<std::string> console_history;
    std::vector<std::string>::iterator console_history_iter;
    std::string d_console_input;
    std::string d_output_cache;

    AbstractCommandHandler* d_cmd_handler;
};


class UIConsoleErrorHandler :public AbstractErrorHandler {
public:
    UIConsoleErrorHandler(UIConsole *con) :c(con) {}
    ~UIConsoleErrorHandler(){}
    void inline MessageOut(const std::string& msg) {
        if (c) c->Msg(msg+std::string("\n"));  
    
    }
private:
    UIConsole *c; /* Link to output console*/
};
