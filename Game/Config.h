#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "../Models/Project_path.h"
//класс-обработчик
class Config
{
  public:
    Config()
    {
        reload();
    }
    //при создании настройки загружаютс€ из settings.json и сохран€ютс€ в config
    void reload()
    {
        std::ifstream fin(project_path + "settings.json");
        fin >> config;
        fin.close();
    }
    //оператор "()" позвол€ющий получать по setting_dir и setting_name нужную настройку
    //например макс кол-во ходов: const int Max_turns = config("Game", "MaxNumTurns"); 
    auto operator()(const string &setting_dir, const string &setting_name) const
    {
        return config[setting_dir][setting_name];
    }

  private:
    json config;
};
