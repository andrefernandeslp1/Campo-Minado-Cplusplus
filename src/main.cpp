#include <chrono>
#include "./utils/time.cpp"
#include "Game.h"

int main(int argc, char **argv)
{
  if (argc > 1)
  {
    prompt(argc, argv);
  }
  else
  {
    Difficulty current_level = load_difficulty(CONFIG_FILE);

    std::chrono::_V2::system_clock::time_point begin = std::chrono::high_resolution_clock::now();

    bool result = play(current_level);

    end_game(result, calc_duration(begin));
  }
  return 0;
}