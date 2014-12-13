#pragma once

#include "viewport.h"
#include <string>
#include <vector>
#include "r_uibase.h"
#include <memory>
class AbstractCommandHandler {
public:
    virtual int HandleCommand(const std::string& cmd){return EFAIL;};
};

class UIConsole {
public:
    UIConsole(Viewport *v, std::shared_ptr<AbstractCommandHandler> cmd_h);
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
int UIConsole::HandleExitConsole() {
    d_console_input.clear();
    RebuildOut();
    return 0;
}
UIConsole::UIConsole(Viewport *v, std::shared_ptr<AbstractCommandHandler> cmd_h)
    :d_cmd_handler(cmd_h)
{

    fnt = ft.Construct(16);
    v_port = v;
}
void UIConsole::Draw() {

    float sx = (v_port->w);
    float sy = (v_port->h);
    fnt->RenderText(d_output_cache,0.0,0.1,    sx, sy);
}
int UIConsole::HandlePrevHistoryCommand() {
    if ( !console_history.empty() ) 
    {
        if (console_history_iter != console_history.end()) {
            d_console_input.clear(); //TODO better console
            d_console_input = std::string(*console_history_iter);
        }
        if (console_history_iter != console_history.begin())
            console_history_iter = std::prev(console_history_iter);
        RebuildOut();
    }
    return 0;
}
int UIConsole::HandleNextHistoryCommand() {
    if (console_history_iter != console_history.end() && !console_history.empty() )
    {
        if (console_history_iter != console_history.begin()) {
            d_console_input.clear(); //TODO better console
            d_console_input = std::string(*console_history_iter);
        }
        console_history_iter = std::next(console_history_iter);
        RebuildOut();
    }
    return 0;
}
int UIConsole::HandleInputKey(unsigned char key_code)
{
    if (key_code == 13 /*enter key*/) {
        HandleCommand(std::string(d_console_input));
        console_history.push_back(d_console_input);

        console_history_iter = std::prev(console_history.end());
        d_console_input.clear();
    }
    if (key_code == 8 /*del key*/) {
        HandleDelKey();

    }

    /*are std::isprint better?*/
    if (isprint(key_code)){
        d_console_input.push_back(key_code);
    }

    //for key detect
    //printf("key sended %d\n" , key_code);

    RebuildOut();
    return 0;
}
void UIConsole::Msg(const std::string& msg) {
    con.push_back(msg);
    if (con.size() > d_con_sz){
        con.erase(con.begin());
    }
    RebuildOut();
}
void UIConsole::Cls() {
    con.clear();
    RebuildOut();
}

int UIConsole::HandleCommand(const std::string &cmd)
{
    if (cmd.empty()) {
        return EFAIL;
    }
    Msg(cmd+"\n");
    if (!d_cmd_handler) {
        Msg("Invalid cmd handler\n");
        return EFAIL;
    }


    d_cmd_handler->HandleCommand(cmd);
    Msg(">");
    return 0;
}

void UIConsole::RebuildOut()
{
    d_output_cache.clear();
    if (!con.empty()) {

        std::vector<std::string>::iterator it;
        unsigned int n;


        for (it = con.begin(),n = 0; ((n < d_con_sz) && -(it < con.end())); n++,it++ ) {
             d_output_cache += *it;
        }
    }
    d_output_cache += d_console_input;
}

void UIConsole::HandleDelKey()
{
    if (!d_console_input.empty())
        d_console_input.pop_back();
    RebuildOut();
}



class UIConsoleErrorHandler :public AbstractErrorHandler {
public:
    UIConsoleErrorHandler(UIConsole *con) :c(con) {};
    ~UIConsoleErrorHandler(){};
    void MessageOut(const std::string& msg) {
        if (c) c->Msg(msg+std::string("\n"));  
    
    }
private:
    UIConsole *c; /* Link to output console*/
};
