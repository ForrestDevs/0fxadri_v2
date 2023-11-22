#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include "models.hpp"
#include "dataIO.hpp"

void saveData(const std::vector<BarData> &data, const std::string &filename)
{
    std::ofstream out(filename, std::ios::binary);
    for (const auto &sd : data)
    {
        out.write(reinterpret_cast<const char *>(&sd), sizeof(sd));
    }
};


std::vector<BarData> loadData(const std::string &filename)
{
    std::ifstream in(filename, std::ios::binary);
    std::vector<BarData> data;
    BarData sd;
    while (in.read(reinterpret_cast<char *>(&sd), sizeof(sd)))
    {
        data.push_back(sd);
    }
    return data;
};

// std::vector<Session> loadSessions(const std::string &filename)
// {
//     std::ifstream in(filename, std::ios::binary);
//     std::vector<Session> data;
//     Session sd;
//     while (in.read(reinterpret_cast<char *>(&sd), sizeof(sd)))
//     {
//         data.push_back(sd);
//     }
//     return data;
// };

// void saveSessions(const std::vector<Session> &data, const std::string &filename)
// {
//     std::ofstream out(filename, std::ios::binary);
//     for (const auto &sd : data)
//     {
//         out.write(reinterpret_cast<const char *>(&sd), sizeof(sd));
//     }
// };

// std::vector<DRrange> loadDRranges(const std::string &filename)
// {
//     std::ifstream in(filename, std::ios::binary);
//     std::vector<DRrange> data;
//     DRrange sd;
//     while (in.read(reinterpret_cast<char *>(&sd), sizeof(sd)))
//     {
//         data.push_back(sd);
//     }
//     return data;
// };

// void saveDRranges(const std::vector<DRrange> &data, const std::string &filename)
// {
//     std::ofstream out(filename, std::ios::binary);
//     for (const auto &sd : data)
//     {
//         out.write(reinterpret_cast<const char *>(&sd), sizeof(sd));
//     }
// };

// std::vector<DRsession> loadDRsessions(const std::string &filename)
// {
//     std::ifstream in(filename, std::ios::binary);
//     std::vector<DRsession> data;
//     DRsession sd;
//     while (in.read(reinterpret_cast<char *>(&sd), sizeof(sd)))
//     {
//         data.push_back(sd);
//     }
//     return data;
// };

// void saveDRsessions(const std::vector<DRsession> &data, const std::string &filename)
// {
//     std::ofstream out(filename, std::ios::binary);
//     for (const auto &sd : data)
//     {
//         out.write(reinterpret_cast<const char *>(&sd), sizeof(sd));
//     }
// };



