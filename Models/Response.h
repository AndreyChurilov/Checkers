#pragma once

enum class Response //enum class
{
    OK,      //ход завершен
    BACK,    //шаг назад
    REPLAY,  //начать игру заново
    QUIT,    //выход
    CELL     //клик по клетке   
};
