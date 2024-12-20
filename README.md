# Checkers  
Desktop application for playing checkers with a bot / friend in C++.  
Using the SDL2 framework for rendering.  
Supports the game bot vs bot with the setting of the depth of calculation for each separately (from settings.json).  
## For developers:  
To work install SDL2 and SDL2_image(Board.h, Hand.h), nlohmann/json(Config.h) and correct path strings in Models/Project_path.h.
The calculation is made for the number of steps equal to depth + 1, where, for example, steps with multiple takes are counted as 1 step.  
State traversal uses a minimax algorithm with alpha-beta pruning heuristics.  
To calculate values in leaf states, the Logic::calc_score function is used.  
You can set your params in settings.json:  

### WindowSize
Width - Ширина окна 0 - fullscreen.  
Hight - Высота окна. 0 - fullscreen.  
### Bot
IsWhiteBot - true/false. Бот белый  
IsBlackBot - true/false. Бот черный
WhiteBotLevel - unsigned int. Уровень сложности для Белого бота + 1. (0 - 2 легко 3 - 5 средне, 6 - 12 тяжело. 6+ работаюь медленно без оптимизации).   
BlackBotLevel - unsigned int. Уровень сложности для Черного бота - аналогично Белому.  
BotScoringType - "NumberOnly" (бот учитывает только количество шашек)  or "NumberAndPotential" (бот также учитывает позиции шашек).  
BotDelayMS - unsigned int. Минимальная задержка перед ходом бота.  
NoRandom - true/false. Будет ли бот детерминированным.  
Optimization - "O0"/"O1"/"O2". Значительная оптимизация с точки зрения времени прохождения бота. O0 без оптимизации (max level 7), O1 позволяет отсекать наихудшие ветви поиска (max level 12), O2(временно недоступно) это намного быстрее, но это может повлиять на выбор хода.  
### Game
MaxNumTurns - unsigned int. Максимальное кол-во ходов.  
