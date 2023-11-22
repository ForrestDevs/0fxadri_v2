#ifndef DATAIO_HPP
#define DATAIO_HPP

#include <vector>
#include <string>
#include "models.hpp" // Include this if your functions use types defined in models.hpp

/**
 * @brief Save the data to a binary file
 * 
 * @param data A vector of BarData structs
 * @param filename The name of the binary file to save
 * 
*/
void saveData(const std::vector<BarData> &data, const std::string &filename);

/**
 * @brief Load the data from a binary file
 * 
 * @param filename The name of the binary file to load
 * @return std::vector<BarData> A vector of BarData structs
 * 
*/
std::vector<BarData> loadData(const std::string &filename);

#endif // DATAIO_HPP