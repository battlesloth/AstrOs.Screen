#include "astros_script.hpp"
#include "cJSON.h"
#include <esp_log.h>

AstrosScript script;

AstrosScript::AstrosScript()
{
    page = 0;
    this->scripts = std::vector<std::vector<script_cmd_t>>();
}

AstrosScript::~AstrosScript() {}

void AstrosScript::LoadScript(std::string blob)
{
    this->scripts.clear();

    ESP_LOGI("AstrosScript", "Loading script: %s", blob.c_str());

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

        this->scripts.push_back(buttonPage);
    }

    cJSON_Delete(root);

    if (this->scripts.size() < 1)
    {
        ESP_LOGW("AstrosScript", "No scripts found, loading default script");
        AstrosScript::DefaultScript();
    }

    this->page = 0;
}

void AstrosScript::IncrementPage()
{
    this->page++;
    if (this->page >= scripts.size())
    {
        this->page = 0;
    }
}

void AstrosScript::DecrementPage()
{
    this->page--;
    if (this->page < 0)
    {
        this->page = scripts.size() - 1;
    }
}

int AstrosScript::GetPageCount()
{
    return scripts.size();
}

std::string AstrosScript::GetScriptName(int button)
{
    auto val = button - 1;

    if (scripts[this->page].size() < button)
    {
        return "error";
    }
    else if (val < 0)
    {
        return "error";
    }

    return scripts[this->page][val].name;
}

std::string AstrosScript::GetScriptCommand(int button)
{
    auto val = button - 1;

    if (scripts[this->page].size() < button)
    {
        return "error";
    }
    else if (val < 0)
    {
        return "error";
    }

    return scripts[this->page][val].command;
}

void AstrosScript::DefaultScript()
{
    scripts.clear();

    std::vector<script_cmd_t> emptyPage = {
        {"None", "0"}, {"None", "0"}, {"None", "0"}, 
        {"None", "0"}, {"None", "0"}, {"None", "0"}, 
        {"None", "0"}, {"None", "0"}, {"None", "0"}
    };

    scripts.push_back(emptyPage);
    this->page = 0;
}