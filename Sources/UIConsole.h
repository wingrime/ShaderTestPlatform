#pragma once

#include "viewport.h"
#include <string>
#include <vector>
#include "UIFont.h"
#include <memory>
class AbstractCommandHandler {
public:
    virtual int HandleCommand(const std::string& cmd){return EFAIL;};
};

class UIConsole {
public:
    UIConsole(Viewport *v, std::shared_ptr<AbstractCommandHandler> cmd_h);
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
    Viewport *v_port;
    UIFont * fnt ;
    std::vector<std::string> con;
    std::vector<std::string> console_history;
    std::vector<std::string>::iterator console_history_iter;
    std::string d_console_input;
    std::string d_output_cache;

    std::shared_ptr<AbstractCommandHandler> d_cmd_handler;
};


class UIConsoleErrorHandler :public AbstractErrorHandler {
public:
    UIConsoleErrorHandler(UIConsole *con) :c(con) {};
    ~UIConsoleErrorHandler(){};
    void inline MessageOut(const std::string& msg) {
        if (c) c->Msg(msg+std::string("\n"));  
    
    }
private:
    UIConsole *c; /* Link to output console*/
};
