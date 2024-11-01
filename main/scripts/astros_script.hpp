#ifndef __ASTROS_SCRIPT_HPP
#define __ASTROS_SCRIPT_HPP

#include <vector>
#include <string>

struct script_cmd_t
{
    std::string name;
    std::string command;
    script_cmd_t(std::string n, std::string c) : name(n), command(c) {}
};

class AstrosScript
{
    private:
        std::vector<std::vector<script_cmd_t>> scripts;
        int page;
    public:
        AstrosScript();
        ~AstrosScript();
        void DefaultScript();
        void LoadScript(std::string blob);
        void IncrementPage();
        void DecrementPage();
        int GetPageCount();
        std::string GetScriptName(int button);
        std::string GetScriptCommand(int button);
};

extern AstrosScript script;

#endif