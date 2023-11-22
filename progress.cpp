#include <iostream>
#include "progress.hpp"

void printProgressBar(int progress, int total, double elapsed_seconds)
{
    const int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress / total;
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }
    double speed = progress / elapsed_seconds;
    std::cout << "] " << int(progress * 100.0 / total) << " %  " << speed << " it/s\r";
    std::cout.flush();
}