#include <chrono>

/**
 * @brief calculo de duracao do programa
 *
 */
int calc_duration(std::chrono::_V2::system_clock::time_point begin)
{
    auto diff = std::chrono::high_resolution_clock::now() - begin;
    int seconds = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
    return seconds;
}
