#include "UIConsole.h"
#include "mat_math.h"
#include "RBO.h"
int UIConsole::HandleExitConsole() {
    d_console_input.clear();
    RebuildOut();
    return 0;
}
UIConsole::UIConsole(RectSizeInt &v, std::shared_ptr<AbstractCommandHandler> cmd_h)
    :d_cmd_handler(cmd_h)
{

    fnt = ft.Construct(20);
    v_port = v;
}

UIConsole::~UIConsole()
{
    ft.Release(fnt);
}
void UIConsole::Draw() {
    fnt->RenderText(d_output_cache,0.0,0.05,    v_port.w, v_port.h);
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

        std::vector<std::string>::const_iterator it;
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
