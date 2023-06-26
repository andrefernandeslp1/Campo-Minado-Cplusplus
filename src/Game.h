#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>

const std::string CONFIG_FILE = "./dist/config.txt";

enum class Difficulty
{
  beginner,
  intermediary,
  advanced
};

struct Cell
{
  bool is_hidden{true};
  bool has_mine{false};
  bool has_flag{false};
  short total_mines;
};

using Cells = std::vector<std::vector<Cell>>;

struct Map
{
  int height;  // altura do mapa
  int width;   // largura do mapa
  Cells cells; // pontos de que o mapa é composto
};

struct Game
{
  short total_mines;
  Difficulty level;
  Map map;
};

// reacts to user's inputs such as --difficulty flags etc
void prompt(int argc, char **argv);

// load current difficulty
Difficulty load_difficulty(const std::string config_file);

// start the game bases on current level
bool play(Difficulty level);

// end the game and stores player information to the ranking if they win.
void end_game(bool won, int seconds);

// colocar e retirar flag
void put_takeoff_flag(Game &game, int x, int y);

// revelar células vazias
void revelar(Game &game, int x, int y);

// preencher com minas nível intermediário
void fill_with_mines_intermediary(Map &map, int total_mines, int x, int y);

// preencher com minas nível avançado
void fill_with_mines_advanced(Map &map, int total_mines, int x, int y);

// posição das flags antes do primeiro reveal
std::vector <std::pair<int,int>> pos_flags_before_first_reveal (Map map);

// repor flags após primeiro reveal
void reinsert_flags (Game &game , std::vector <std::pair<int,int>> posicoes);

// hack para revelar todas as células sem minas
void win_game ( Game &game );

#endif
