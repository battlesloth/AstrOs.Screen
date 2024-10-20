#include "astros_script.hpp"
#include "cJSON.h"

AstrosScript script;

AstrosScript::AstrosScript()
{
    AstrosScript::defaultScript();
}

AstrosScript::~AstrosScript() {}

void AstrosScript::LoadScript(std::string blob)
{
    scripts.clear();

    cJSON *root = cJSON_Parse(blob.c_str());

    const cJSON *pages = NULL;
    const cJSON *page = NULL;
    const cJSON *button = NULL;

    pages = cJSON_GetObjectItem(root, "pages");

    cJSON_ArrayForEach(page, pages)
    {

        std::vector<script_cmd_t> buttonPage;

        cJSON_ArrayForEach(button, page)
        {

            std::string name = cJSON_GetObjectItem(button, "name")->valuestring;
            std::string command = cJSON_GetObjectItem(button, "command")->valuestring;

            buttonPage.push_back({name, command});
        }

        if (buttonPage.size() < 9)
        {
            for (size_t i = buttonPage.size(); i < 9; i++)
            {
                buttonPage.push_back({"None", "0"});
            }
        }

        scripts.push_back(buttonPage);
    }

    cJSON_Delete(root);

    if (scripts.size() < 1)
    {
        AstrosScript::defaultScript();
    }

    page = 0;
}

void AstrosScript::IncrementPage()
{
    page++;
    if (page >= scripts.size())
    {
        page = 0;
    }
}

void AstrosScript::DecrementPage()
{
    page--;
    if (page < 0)
    {
        page = scripts.size() - 1;
    }
}

int AstrosScript::GetPageCount()
{
    return scripts.size();
}

std::string AstrosScript::GetScriptName(int button)
{
    auto val = button - 1;

    if (scripts[page].size() > button)
    {
        val = scripts[page].size() - 1;
    }
    else if (val < 0)
    {
        val = 0;
    }

    return scripts[page][val].name;
}

std::string AstrosScript::GetScriptCommand(int button)
{
    auto val = button - 1;

    if (scripts[page].size() > button)
    {
        val = scripts[page].size() - 1;
    }
    else if (val < 0)
    {
        val = 0;
    }

    return scripts[page][val].command;
}

void AstrosScript::defaultScript()
{
    scripts.clear();

    std::vector<script_cmd_t> emptyPage = {
        {"None", "0"}, {"None", "0"}, {"None", "0"}, 
        {"None", "0"}, {"None", "0"}, {"None", "0"}, 
        {"None", "0"}, {"None", "0"}, {"None", "0"}
    };

    scripts.push_back(emptyPage);

    std::vector<script_cmd_t> page2 = {
        {"Page2", "0"}, {"Page2", "0"}, {"Page2", "0"}, {"Page2", "0"}, {"Page2", "0"}, {"Page2", "0"}, {"Page2", "0"}, {"Page2", "0"}, {"Page2", "0"}};

    scripts.push_back(page2);
    std::vector<script_cmd_t> page3 = {
        {"Page3", "0"}, {"Page3", "0"}, {"Page3", "0"}, {"Page3", "0"}, {"Page3", "0"}, {"Page3", "0"}, {"Page3", "0"}, {"Page3", "0"}, {"Page3", "0"}};

    scripts.push_back(page3);

    page = 0;
}