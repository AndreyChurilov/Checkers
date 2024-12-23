#pragma once
#include <random>
#include <vector>

#include "../Models/Move.h"
#include "Board.h"
#include "Config.h"

const int INF = 1e9;

class Logic
{
  public:
    Logic(Board *board, Config *config) : board(board), config(config)
    {
        optimization = (*config)("Bot", "Optimization");
    }
    //поиск лучшего хода для бота - возвращает вектор ходов или побитий
    vector<move_pos> find_best_turns(const bool color) {
        //почистить вектора 
        next_move.clear();
        next_best_state.clear();
        //заполнить вектора лучшими состояниями: 
        //мастрица, чей ход - цвет, координаты - если начинаем кем то бить, состояние, альфа границп
        find_first_best_turn(board->get_board(), color, -1, -1, 0);
        
        vector<move_pos> res;
        int state = 0; //начальное состояние
        do {
            res.push_back(next_move[state]);
            //переход в след состояние
            state = next_best_state[state]; 
        } while (state != -1 && next_move[state].x != -1); //после серии побитий нет возможных ходов
        return res;
    }

private:
    //поиск первой последовательности ходов
    double find_first_best_turn(vector<vector<POS_T>> mtx, const bool color, const POS_T x, const POS_T y, size_t state,
        double alpha = -1) {
        //начальные состояния
        next_move.emplace_back(-1, -1, -1, -1);
        next_best_state.push_back(-1);
        //найти все возможные ходы, если кого-то бьем
        if (state != 0) {
            find_turns(x, y, mtx);
        }
        //сделать копии доступных ходов и побитий
        auto now_turns = turns;
        auto now_have_beats = have_beats;
        //возможные комбинации случаев "кого то бьем" и "статус 0"
        //сделали серию побитий и нет ходов (нечего бить)
        if (!now_have_beats && state != 0) {
            //поиск наилучшего в рекурсии
            return find_best_turns_rec(mtx, 1 - color, 0, alpha); 
        }
        //наилучший результат на текущий момент
        double best_score = -1;
        //если можем кого-то побить или есть ход
        for (auto turn : now_turns) {
            //следующее состояние
            size_t new_state = next_move.size();
            //текущий результат
            double score;
            //есть кого побить
            if (now_have_beats) {
                score = find_first_best_turn(make_turn(mtx, turn), color, turn.x2, turn.y2, new_state, best_score);
            }
            //есть ход
            else {
                score = find_best_turns_rec(make_turn(mtx, turn), 1 - color, 0, best_score);
            }
            //максимизация
            if (score > best_score) {
                //нашли новый оптимум - берем первый
                best_score = score;
                next_move[state] = turn;
                next_best_state[state] = (now_have_beats ? new_state : -1);
            }

        }
        return best_score;
    }

    //поиск лучшего хода: рекурсивная фун-я просчета от хода игрока на n-ходов
    double find_best_turns_rec(vector<vector<POS_T>> mtx, const bool color, const size_t depth, double alpha = -1,
        double beta = INF + 1, const POS_T x = -1, const POS_T y = -1) {
        //проверить глубину = max - выход из рекурсии
        if (depth == Max_depth) {
            return calc_score(mtx, (depth % 2 == color));
        }
        //если есть серия побитий
        if (x != -1) {
            find_turns(x, y, mtx);
        }
        //есть цвет - найти все возможные ходы
        else {
            find_turns(color, mtx);
        }
        auto now_turns = turns;
        auto now_have_beats = have_beats;
        //елси не чему бить и была серия побитий
        if (!now_have_beats && x != 0) {
            //поиск наилучшего в рекурсии
            return find_best_turns_rec(mtx, 1 - color, depth + 1, alpha, beta);
        }
        
        //если ходов нет - тот чей ход - проиграл: 0 - мы, INF - соперник
        if (turns.empty()) {
            return (depth % 2 ? 0 : INF);
        }

        //поддержка минимума и максимума
        double min_score = INF + 1;
        double max_score = -1;
        for (auto turn : now_turns) {
            double score;
            //если есть подитие
            if (now_have_beats) {
                score = find_best_turns_rec(make_turn(mtx, turn), color, depth, alpha, beta, turn.x2, turn.y2);
            }
            //если побитий нет - переход хода (смена цвета) и меняется глубина
            else {
                score = find_best_turns_rec(make_turn(mtx, turn), 1 - color, depth + 1, alpha, beta);
            }
            //обновление min и max
            min_score = min(min_score, score);
            max_score = max(max_score, score);
            //альфа-бета отсечение
            //если ходим мы - максимизация
            if (depth % 2) {
                alpha = max(alpha, max_score);
            }
            //ход соперника - минимизация
            else {
                beta = min(beta, min_score);
            }
            //оптимизация O1 - альфа и бета строгие
            if (optimization != "O0" && alpha > beta) {
                break; 
            }
            //оптимизация O2 - альфа = бета 
            if (optimization == "O2" && alpha == beta) {
                return(depth % 2 ? max_score + 1: min_score - 1);
            }
        }

        //если depth % 2 = 1 - наш ход - максимизация, если бот - мниммизация
        return(depth % 2 ? max_score : min_score);
    }

    //производит ход на матрице и возвращает копию матрицы
    //нужна для передачи матрицы в рекурсию после хода
    vector<vector<POS_T>> make_turn(vector<vector<POS_T>> mtx, move_pos turn) const
    {
        if (turn.xb != -1)
            mtx[turn.xb][turn.yb] = 0;
        if ((mtx[turn.x][turn.y] == 1 && turn.x2 == 0) || (mtx[turn.x][turn.y] == 2 && turn.x2 == 7))
            mtx[turn.x][turn.y] += 2;
        mtx[turn.x2][turn.y2] = mtx[turn.x][turn.y];
        mtx[turn.x][turn.y] = 0;
        return mtx;
    }
    //функция оценки состояний - просчет фигур
    double calc_score(const vector<vector<POS_T>> &mtx, const bool first_bot_color) const
    {
        // color - who is max player
        double w = 0, wq = 0, b = 0, bq = 0;
        for (POS_T i = 0; i < 8; ++i)
        {
            for (POS_T j = 0; j < 8; ++j)
            {
                w += (mtx[i][j] == 1); //б пешек
                wq += (mtx[i][j] == 3); //б королев
                b += (mtx[i][j] == 2);  //ч пешек
                bq += (mtx[i][j] == 4); //ч королев
            }
        }
        if (!first_bot_color)
        {
            swap(b, w);
            swap(bq, wq);
        }
        if (w + wq == 0)
            return INF;
        if (b + bq == 0)
            return 0;
        int q_coef = 4; //параметр вес королевы важна как 4 пешки
        return (b + bq * q_coef) / (w + wq * q_coef);
    }

public:
    //метод поиска ходов
    void find_turns(const bool color)
    {
        find_turns(color, board->get_board());
    }

    void find_turns(const POS_T x, const POS_T y)
    {
        find_turns(x, y, board->get_board());
    }

private:
    //поиск возможного хода
    void find_turns(const bool color, const vector<vector<POS_T>> &mtx)
    {
        vector<move_pos> res_turns;
        bool have_beats_before = false;
        //проход по клеткам
        for (POS_T i = 0; i < 8; ++i)
        {
            for (POS_T j = 0; j < 8; ++j)
            {
                if (mtx[i][j] && mtx[i][j] % 2 != color) //если цвета совпадают
                {
                    find_turns(i, j, mtx); //от клетки пытаемся найти все возможные ходы
                    if (have_beats && !have_beats_before)
                    {
                        have_beats_before = true;
                        res_turns.clear();
                    }
                    if ((have_beats_before && have_beats) || !have_beats_before)
                    {
                        res_turns.insert(res_turns.end(), turns.begin(), turns.end());
                    }
                }
            }
        }
        turns = res_turns;
        shuffle(turns.begin(), turns.end(), rand_eng);
        have_beats = have_beats_before;
    }
    //поиск возможного хода от позиции
    void find_turns(const POS_T x, const POS_T y, const vector<vector<POS_T>> &mtx)
    {
        turns.clear();
        have_beats = false;
        POS_T type = mtx[x][y];
        // логика побитий для разных типов фигур
        switch (type)
        {
        case 1:
        case 2:
            // пешка бел или чер
            for (POS_T i = x - 2; i <= x + 2; i += 4)
            {
                for (POS_T j = y - 2; j <= y + 2; j += 4)
                {
                    if (i < 0 || i > 7 || j < 0 || j > 7)
                        continue;
                    POS_T xb = (x + i) / 2, yb = (y + j) / 2;
                    if (mtx[i][j] || !mtx[xb][yb] || mtx[xb][yb] % 2 == type % 2)
                        continue;
                    turns.emplace_back(x, y, i, j, xb, yb);
                }
            }
            break;
        default:
            // королева
            for (POS_T i = -1; i <= 1; i += 2)
            {
                for (POS_T j = -1; j <= 1; j += 2)
                {
                    POS_T xb = -1, yb = -1;
                    for (POS_T i2 = x + i, j2 = y + j; i2 != 8 && j2 != 8 && i2 != -1 && j2 != -1; i2 += i, j2 += j)
                    {
                        if (mtx[i2][j2])
                        {
                            if (mtx[i2][j2] % 2 == type % 2 || (mtx[i2][j2] % 2 != type % 2 && xb != -1))
                            {
                                break;
                            }
                            xb = i2;
                            yb = j2;
                        }
                        if (xb != -1 && xb != i2)
                        {
                            turns.emplace_back(x, y, i2, j2, xb, yb);
                        }
                    }
                }
            }
            break;
        }
        // варианты ходов для разных типов
        if (!turns.empty())
        {
            have_beats = true;
            return;
        }
        switch (type)
        {
        case 1:
        case 2:
            // пешка чер или бел
            {
                POS_T i = ((type % 2) ? x - 1 : x + 1);
                for (POS_T j = y - 1; j <= y + 1; j += 2)
                {
                    if (i < 0 || i > 7 || j < 0 || j > 7 || mtx[i][j])
                        continue;
                    turns.emplace_back(x, y, i, j);
                }
                break;
            }
        default:
            // королева
            for (POS_T i = -1; i <= 1; i += 2)
            {
                for (POS_T j = -1; j <= 1; j += 2)
                {
                    for (POS_T i2 = x + i, j2 = y + j; i2 != 8 && j2 != 8 && i2 != -1 && j2 != -1; i2 += i, j2 += j)
                    {
                        if (mtx[i2][j2])
                            break;
                        turns.emplace_back(x, y, i2, j2);
                    }
                }
            }
            break;
        }
    }

  public:
    //вектор ходов, которые находятся с помощью find_turns
    vector<move_pos> turns;
    //флаг отвечает, является ли ход побитием
    bool have_beats; 
    //максимальная глубина
    int Max_depth;

  private:
    default_random_engine rand_eng;
    //оптимизация
    string optimization; 
    //вектор ходов и вектор следующих лучших состояний
    //нужны для восстановления последовательности ходов
    vector<move_pos> next_move;
    vector<int> next_best_state;
    //указатели на классы доски и конфигурации
    Board *board;  
    Config *config; 
};
