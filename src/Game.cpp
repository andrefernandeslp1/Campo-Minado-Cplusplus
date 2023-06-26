#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <time.h>
#include <iomanip>
#include <sstream>

#include "Game.h"
#include "utils/string.cpp"

const int NUM_RECORDS = 10;

// explica como usar comandos do jogo
void show_usage()
{
  std::cout << "Usage: game [option]" << std::endl;
  std::cout << "Option:" << std::endl;
  std::cout << " -h or --help                  Display this information." << std::endl;
  std::cout << " -r or --records               Display records." << std::endl;
  std::cout << " -d or --difficulty <option>   Change the game difficulty for <option>" << std::endl;
  std::cout << "                               -b or --beginner" << std::endl;
  std::cout << "                               -i or --intermediary" << std::endl;
  std::cout << "                               -a or --advanced" << std::endl;
}

// exibe mapa do jogo em terminal
void show_map(Map map)
{
  std::cout << std::endl;

  for (int i = 0; i < map.height; i++)
  {
    std::cout << std::setfill('0') << std::setw(2) << i << "   ";
    for (int j = 0; j < map.width; j++)
    {
      // celula oculta
      if (map.cells[i][j].is_hidden == true)
      {
        std::cout << "X";
      }
      else
      {
        // com bandeira (f)
        if (map.cells[i][j].has_flag == true)
        {
          std::cout << "F";
        }
        // com minas (m)
        else if (map.cells[i][j].has_mine == true)
        {
          std::cout << "M";
        }
        else
        {
          if (map.cells[i][j].total_mines > 0)
          {
            std::cout << map.cells[i][j].total_mines;
          }
          else
          {
            std::cout << " ";
          }
        }
      }
      std::cout << "  ";
    }
    std::cout << std::endl;
  }

  std::cout << std::endl
            << "    ";

  for (int i = 0; i < map.width; i++)
  {
    std::cout << std::setfill('0') << std::setw(2) << i << " ";
  }

  // linha divisoria do mapa
  std::cout << std::endl;
  std::cout << std::endl
            << "    ";
  for (int i = 0; i < map.width; i++)
  {
    std::cout << "---";
  }

  std::cout << std::endl;
}

// preenche as minas do mapa a depender do total estabelecido pelo nivel do jogo
void fill_with_mines(Map &map, int total_mines)
{
  std::srand(time(NULL));

  int count_mines = 0;

  while (count_mines < total_mines)
  {
    int random = rand() % (map.height * map.width);

    int h = random / map.height;

    int w = random % map.width;

    if (map.cells[h][w].has_mine == false)
    {
      map.cells[h][w].has_mine = true;
      count_mines++;
    }
  }
}

// checa se ponto esta dentro do mapa
bool is_inside_map(Map map, int w, int h)
{
  return (h >= 0 && h < map.height) && (w >= 0 && w < map.width);
}

// checa se ha minea em um ponto do mapa
bool has_mine(Map map, int w, int h)
{
  return map.cells[h][w].has_mine;
}

// conta quantidade de minas ao redor de um ponto
int count_nested_mines(Map map, int px, int py)
{
  int count = 0;

  for (int y = -1; y <= 1; y++)
  {
    for (int x = -1; x <= 1; x++)
    {
      int dx = px + x;
      int dy = py + y;
      if (is_inside_map(map, dx, dy) && has_mine(map, dx, dy))
      {
        count++;
      }
    }
  }

  return count;
}

// preenche quantidade de minas ao redor de cada celula do mapa inteiro
void fill_with_count_nested_mines(Map &map)
{
  for (int h = 0; h < map.height; h++)
  {
    for (int w = 0; w < map.width; w++)
    {
      map.cells[h][w].total_mines = count_nested_mines(map, w, h);
    }
  }
}

// cria mapa do jogo
Map create_map(int height, int width, int total_mines, int x, int y)
{
  Map map;
  map.height = height;
  map.width = width;

  Cell cell;

  for (int i = 0; i < map.height; i++)
  {
    std::vector<Cell> cells;

    for (int j = 0; j < map.width; j++)
    {
      cells.push_back(cell);
    }

    map.cells.push_back(cells);
  }

  if (total_mines == 10)
  {
    fill_with_mines(map, total_mines);
  }

  if (total_mines == 40)
  {
    fill_with_mines_intermediary(map, total_mines, x, y);
  }

  if (total_mines == 100)
  {
    fill_with_mines_advanced(map, total_mines, x, y);
  }
/*
  else
  {
    fill_with_mines(map, total_mines);
  }
*/
  fill_with_count_nested_mines(map);

  return map;
}

Game start_game(Difficulty level)
{
  Game game;

  int height = 10;
  int width = 10;
  int total_mines = 10;

  if (level == Difficulty::intermediary)
  {
    height = 15;
    width = 15;
    total_mines = 40;
  }

  if (level == Difficulty::advanced)
  {
    height = 30;
    width = 15;
    total_mines = 100;
  }

  game.map = create_map(height, width, total_mines, 0, 0);
  game.level = level;
  game.total_mines = total_mines;

  return game;
}

// checa se e acao de revelar
bool is_reveal_action(char action)
{
  return action == 'r';
}

// checa se e acao de colocar bandeira
bool is_flag_action(char action)
{
  return action == 'f';
}

// revela todas as celulas do map
void reveal_all_map(Map &map)
{
  for (int h = 0; h < map.height; h++)
  {
    for (int w = 0; w < map.width; w++)
    {
      map.cells[h][w].is_hidden = false;
      
      if (map.cells[h][w].has_flag == true && map.cells[h][w].has_mine == true)
      {
        map.cells[h][w].has_flag = false;
      }
    }
  }
}

// checa se o usuário venceu o jogo, ou seja, revelou todas as células que não contêm minas.
bool check_user_won(Map map)
{
  for (int h = 0; h < map.height; h++)
  {
    for (int w = 0; w < map.width; w++)
    {
      if (map.cells[h][w].has_mine && map.cells[h][w].has_flag)
        continue;
      else if (map.cells[h][w].is_hidden && !map.cells[h][w].has_mine)
        return false;
    }
  }
  return true;
}

// metodo principal para realizar logica da partida
bool play(Difficulty level)
{
  std::cout << "\nWelcome to minesweeper!" << std::endl;
  std::cout << "\nType r for revealing mines and f to put flag" << std::endl;

  char action;
  bool end = false;
  bool won = false;
  int x, y = 0;
  int jogada = 0;

  Game game = start_game(level);

  int tempo_1 = time(NULL);

  while (end == false)
  {

    std::cout << std::endl << "Time: " << time(NULL) - tempo_1 << " seconds :)" << std::endl;

    show_map(game.map);

    std::cout << std::endl;
    std::cout << "Type your action [r/f]: ";
    std::cin >> action;

    //hack para vencer jogo
    if (action == 'w')
    {
      std::cout << "\nYou cheat :/" << std::endl;
      win_game ( game );
      show_map( game.map );
      won = true;
      end = true;
    } 

    if (is_reveal_action(action) || is_flag_action(action))
    {
      // ler coordenadas
      std::cout << "Type coord x: ";
      std::cin >> x;
      std::cout << "Type coord y: ";
      std::cin >> y;

      // checar se posicao e valida. Enquanto nao for, pedir coordenadas
      while (!is_inside_map(game.map, x, y))
      {
        std::cout << "Invalid input" << std::endl;
        std::cout << "Type coord x: ";
        std::cin >> x;
        std::cout << "Type coord y: ";
        std::cin >> y;
      }

      //*** marca/descamarca com bandeira ***
      if (action == 'f')
      {
        put_takeoff_flag(game, x, y);
      }

      if (action == 'r')
      {
        if (game.map.cells[y][x].has_flag == false)
        {
          // cria mapa novo baseado nas restricoes de primeira jogada (intermediário e avançado)
          if (jogada == 0 && (game.level == Difficulty::intermediary || game.level == Difficulty::advanced))
          {
            std::vector <std::pair<int,int>> pos_flags = pos_flags_before_first_reveal ( game.map );

            game.map = create_map(game.map.height, game.map.width, game.total_mines, x, y);
            jogada++;

            reinsert_flags ( game , pos_flags);
          }

          if (has_mine(game.map, x, y))
          { 
            reveal_all_map(game.map);
            show_map(game.map);
            std::cout << std::endl << "Time: " << time(NULL) - tempo_1 << " seconds :(" << std::endl;
            won = false;
            end = true;
          }

          else
          {
            // caso seja encontrada um numero, eh exibida celula
            if (count_nested_mines(game.map, x, y) > 0)
            {
              game.map.cells[y][x].is_hidden = false;
            }
            else
            {
              //revelar células vizinhas
              revelar(game, x, y);
            }
            // ao fim, checa se usuario venceu.
            won = check_user_won(game.map);
            // Se sim, encerra o programa. Senao, continua o fluxo
            if (won)
            {
              end = true;
            }
          }
        }
        else
        {
          std::cout << std::endl <<"WARNING: Cell with flag! To show cell, take off flag first." << std::endl;
        }
      }

    }
    else
    {
      if (action != 'w') std::cout << "Oops..Invalid action!" << std::endl;
    }
  }

  return won;
}

// grava dificuldade (nivel) em arquivo de configuracao
void log_difficulty(const std::string config_file, Difficulty level)
{
  std::ofstream file;
  file.open(config_file.c_str(), std::ifstream::out);
  if (file.is_open())
  {
    switch (level)
    {
    case Difficulty::beginner:
      file << 'b';
      std::cout << "Difficulty set to begginer!" << std::endl;
      break;
    case Difficulty::intermediary:
      file << 'i';
      std::cout << "Difficulty set to intermediary!" << std::endl;
      break;
    case Difficulty::advanced:
      file << 'a';
      std::cout << "Difficulty set to advanced!" << std::endl;
      break;
    }
    file.close();
  }
}

// carrega a dificuldade (nivel) atual
Difficulty load_difficulty(const std::string config_file)
{
  Difficulty level;
  std::ifstream file;
  file.open(config_file.c_str(), std::ifstream::in);
  if (file.is_open())
  {
    char c;
    file >> c;
    switch (c)
    {
    case 'b':
      level = Difficulty::beginner;
      break;
    case 'i':
      level = Difficulty::intermediary;
      break;
    case 'a':
      level = Difficulty::advanced;
      break;
    }
    file.close();
  }
  else
  {
    log_difficulty(config_file, Difficulty::beginner);
    level = Difficulty::beginner;
  }
  return level;
}

std::string to_string(Difficulty level)
{
  std::string name;

  switch (level)
  {
  case Difficulty::intermediary:
    name = "intermediary";
  case Difficulty::advanced:
    name = "advanced";
  default:
    name = "beginner";
  }

  return name;
}

//get records file based on current level
std::string get_records_file_name() {
  Difficulty level = load_difficulty(CONFIG_FILE);
  std::string levelName = to_string(level);
  return "./dist/" + levelName + "-ranking" + ".txt";
}

// show records of players
void show_records()
{
  std::string fileName = get_records_file_name();
  std::string line;
  std::ifstream file;
  std::vector<std::vector<std::string>> players;

  file.open(fileName.c_str(), std::ifstream::in);

  std::cout << "Records: " << std::endl;

  if (file.is_open())
  {
    while (getline(file, line))
    {
      std::vector<std::string> info;
      split(line, ';', info);
      players.push_back(info);
    }
    file.close();
    players.shrink_to_fit();
    sort(players);

    //for (int i = 0; i < players.size(); i++)
    for (int i = 0; i < NUM_RECORDS ; i++)
    {
      std::cout << std::setfill('0') << std::setw(2) << i + 1 << ": ";
      std::cout << players[i][0] << " - " << players[i][1] << " seconds." << std::endl;
    }
  }
  else
  {
    std::cout << "Unexpected error when opening file " << fileName << std::endl;
  }
}

// reage a acoes do usuario via terminal. Prompt do jogo
void prompt(int argc, char **argv)
{
  std::string arg = argv[1];

  if (arg == "-h" || arg == "-help")
  {
    show_usage();
  }
  else if (arg == "-r" || arg == "--records")
  {
    show_records();
  }
  else if (arg == "-d" || arg == "--difficulty")
  {
    if (argc > 2)
    {
      std::string newlevel = argv[2];

      if (newlevel == "-b" || newlevel == "--beginner")
      {
        log_difficulty(CONFIG_FILE, Difficulty::beginner);
      }
      else if (newlevel == "-i" || newlevel == "--intermediary")
      {
        log_difficulty(CONFIG_FILE, Difficulty::intermediary);
      }
      else if (newlevel == "-a" || newlevel == "--advanced")
      {
        log_difficulty(CONFIG_FILE, Difficulty::advanced);
      }
      else
      {
        std::cout << "Unknown difficulty argument: " << newlevel << std::endl;
        show_usage();
      }
    }
    else
    {
      std::cout << "It was expected a difficulty for: " << argv[1] << std::endl;
      show_usage();
    }
  }
  else
  {
    std::cout << "Unknown argument: " << argv[1] << std::endl;
    show_usage();
  }
}

// finaliza a partida
void end_game(bool won, int seconds)
{
  if (won == false)
  {
    std::cout << "\nGame Over!!!" << std::endl;
    return;
  }

  std::cout << "Congrats! You finished the game in " << seconds << " seconds!" << std::endl;

  std::cout << "Please enter your name:" << std::endl;
  
  std::string name;
  std::cin >> name;

  std::fstream file;

  file.open(get_records_file_name(), std::fstream::app);

  if (file.is_open())
  {
    file << name << ";" << seconds << "\n";
    file.close();
  }
}

// *** função para colocar/retirar flag ***
void put_takeoff_flag(Game &game, int x, int y)
{
  if (game.map.cells[y][x].is_hidden == true && game.map.cells[y][x].has_flag == false)
  {
    game.map.cells[y][x].is_hidden = false;
    game.map.cells[y][x].has_flag = true;
  }
  else if (game.map.cells[y][x].is_hidden == false && game.map.cells[y][x].has_flag == true)
  {
    game.map.cells[y][x].is_hidden = true;
    game.map.cells[y][x].has_flag = false;
  }
}

// *** revelar celulas vazias ***
void revelar(Game &game, int x, int y)
{
  if(is_inside_map(game.map, x, y))
  {
    if (game.map.cells[y][x].is_hidden)
    {
      if (count_nested_mines(game.map, x, y) == 0 && game.map.cells[y][x].has_mine == false)
      {
        
        game.map.cells[y][x].is_hidden = false;

        for (int j = -1; j <= 1; j+=2)
        {
          for (int i = -1; i <= 1; i+=2)
          {
            int dx = x + i;
            int dy = y + j;

            if (is_inside_map(game.map, dx , dy) && count_nested_mines(game.map, dx, dy) > 0 && game.map.cells[dy][dx].has_mine == false)
            {
              game.map.cells[dy][dx].is_hidden = false;
            }
          }
        }

        revelar(game, x-1 , y  ); //N
        revelar(game, x   , y-1); //E
        revelar(game, x   , y+1); //O
        revelar(game, x+1 , y  ); //S

        //revelar(game, x-1 , y-1); //NO
        //revelar(game, x-1 , y+1); //NE
        //revelar(game, x+1 , y-1); //SO
        //revelar(game, x+1 , y+1); //SE
                
      } 
      else if(count_nested_mines(game.map, x, y) > 0 && game.map.cells[y][x].has_mine == false)
      {
        game.map.cells[y][x].is_hidden = false;
      }
    }
  }
}

// *** preencher com minas nível intermediário ***
void fill_with_mines_intermediary(Map &map, int total_mines, int x, int y)
{
  std::vector<std::pair<int, int>> positions;

  for (int j = -1; j <= 1; j++)
  {
    for (int i = -1; i <= 1; i++)
    {
      int dx = x + i;
      int dy = y + j;

      if (is_inside_map(map, dx , dy))
      {
        positions.push_back({dy,dx});
      }
    }
  }
  
  std::srand(time(NULL));

  int count_mines = 0;

  while (count_mines < total_mines)
  {
    int random = rand() % (map.height * map.width);

    int h = random / map.height;

    int w = random % map.width;

    int cont_posicoes = 0;

    for (int i = 0 ; i < positions.size() ; i++)
    {
      if (positions[i].first == h && positions[i].second == w)
      {
        cont_posicoes++;
      }
    }

    if (cont_posicoes == 0 && map.cells[h][w].has_mine == false)
    {
      map.cells[h][w].has_mine = true;
      count_mines++;
    }    
  }
}

// *** preencher com minas nível avançado ***
void fill_with_mines_advanced(Map &map, int total_mines, int x, int y)
{
  std::vector<std::pair<int, int>> positions;

  for (int j = -1; j <= 1; j++)
  {
    for (int i = -1; i <= 1; i++)
    {
      int dx = x + i;
      int dy = y + j;

      if (is_inside_map(map, dx , dy))
      {
        positions.push_back({dy,dx});
      }
    }
  }
  
  std::srand(time(NULL));

  int count_mines = 0;

  int cont_posicoes = 0;

  while (count_mines < total_mines)
  {
    int random1 = rand() % (map.height * map.height);
    int random2 = rand() % (map.width * map.width);

    int h = random1 / map.height;

    int w = random2 / map.width;

    if (cont_posicoes == 0)
    {
      for (int i = 0 ; i < positions.size() ; i++ )
      {
        if (( x != w && y != h ) && ( positions[i].first == h && positions[i].second == w ))
        {
          map.cells[h][w].has_mine = true;
          cont_posicoes++;
          count_mines++;
        }
      }
    }

    if (cont_posicoes > 0 && ( x != w && y != h ) && map.cells[h][w].has_mine == false )
    {
      map.cells[h][w].has_mine = true;
      count_mines++;
    }    
  }
}

// posição das flags antes do primeiro reveal
std::vector <std::pair<int,int>> pos_flags_before_first_reveal (Map map)
{
  std::vector<std::pair<int, int>> posicoes;

  for ( int i = 0 ; i < map.height ; i++ )
  {
    for ( int j = 0 ; j < map.width ; j++ )
    {
      if ( map.cells[i][j].has_flag == true)
      {
        posicoes.push_back({i,j});
      }
    }
  }
  return posicoes;
}

// repor flags após primeiro reveal
void reinsert_flags (Game &game , std::vector <std::pair<int,int>> posicoes)
{

  for ( int i = 0 ; i < posicoes.size() ; i++ )
  {
    int first = posicoes[i].first;
    int second = posicoes[i].second;

    game.map.cells[first][second].has_flag = true;
    game.map.cells[first][second].is_hidden = false;

  }
}

// hack para revelar todas as células sem minas
void win_game ( Game &game )
{
  for ( int i = 0 ; i < game.map.height ; i++)
  {
    for ( int j = 0 ; j < game.map.width ; j++ )
    {
      if ( game.map.cells[i][j].has_mine == false )
      {
        game.map.cells[i][j].is_hidden = false;
      }
    }
  }
}