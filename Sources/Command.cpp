#include "Command.h"




int ConsoleCommandHandler::HandleCommand(const std::string& str) {
    /*tokenize*/
    std::vector<std::string> elems;
    split(str, ' ', elems);
    /*exec*/
    ExecuteCommand<const std::string & ,std::vector < std::string > * >(elems[0],elems[0], &elems );
    return 0; 
}
std::vector<std::string> & ConsoleCommandHandler::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

int InputCommandHandler::HandleCommand(const std::string& str) {
    /*exec*/
    ExecuteCommand< >(str);
    return 0; 
}
