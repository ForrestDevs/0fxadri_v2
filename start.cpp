#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

struct StockData
{
    std::string timestamp;
    double open_price;
    double high_price;
    double low_price;
    double close_price;
    int volume;
};

std::tm londonStart = {0, 30, 3, 0, 0, 0, 0, 0, 0}; // 3:30 AM
std::tm londonEnd = {0, 30, 4, 0, 0, 0, 0, 0, 0};   // 4:30 AM

std::tm newYorkStart = {0, 30, 9, 0, 0, 0, 0, 0, 0}; // 9:30 AM
std::tm newYorkEnd = {0, 30, 10, 0, 0, 0, 0, 0, 0};  // 10:30 AM

std::tm asianStart = {0, 30, 20, 0, 0, 0, 0, 0, 0}; // 8:30 PM
std::tm asianEnd = {0, 30, 21, 0, 0, 0, 0, 0, 0};   // 9:30 PM

#include <iomanip>

std::tm parseTimestamp(const std::string &timestamp)
{
    std::tm tm = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S"); // Adjust this format to match your timestamp format
    return tm;
}
// std::tm parseTimestamp(const std::string &timestamp)
// {
//     std::tm tm = {};
//     std::istringstream ss(timestamp.substr(11)); // Skip the date part
//     ss >> std::get_time(&tm, "%H:%M:%S"); // Adjust this format to match your timestamp format
//     return tm;
// }
#include <ctime>

bool isTimeInRange(const std::string& time, const std::string& start, const std::string& end) {
    std::tm tm_time = parseTimestamp(time);
    std::tm tm_start = parseTimestamp(start);
    std::tm tm_end = parseTimestamp(end);

    // Convert to minutes since midnight for easier comparison
    int minutes_time = tm_time.tm_hour * 60 + tm_time.tm_min;
    int minutes_start = tm_start.tm_hour * 60 + tm_start.tm_min;
    int minutes_end = tm_end.tm_hour * 60 + tm_end.tm_min;

    return minutes_time >= minutes_start && minutes_time <= minutes_end;
}

std::string tmToString(const std::tm& time)
{
    std::ostringstream ss;
    ss << std::put_time(&time, "%H:%M:%S");
    return ss.str();
}


void testTimestamps(const std::vector<StockData>& data) {
    std::time_t londonStart_t = std::mktime(&londonStart);
    std::time_t londonEnd_t = std::mktime(&londonEnd);
    std::time_t newYorkStart_t = std::mktime(&newYorkStart);
    std::time_t newYorkEnd_t = std::mktime(&newYorkEnd);
    std::time_t asianStart_t = std::mktime(&asianStart);
    std::time_t asianEnd_t = std::mktime(&asianEnd);

    std::cout << "London start: " << std::put_time(&londonStart, "%Y-%m-%d %H:%M:%S") << '\n';
    std::cout << "London end: " << std::put_time(&londonEnd, "%Y-%m-%d %H:%M:%S") << '\n';
    std::cout << "New York start: " << std::put_time(&newYorkStart, "%Y-%m-%d %H:%M:%S") << '\n';
    std::cout << "New York end: " << std::put_time(&newYorkEnd, "%Y-%m-%d %H:%M:%S") << '\n';
    std::cout << "Asian start: " << std::put_time(&asianStart, "%Y-%m-%d %H:%M:%S") << '\n';
    std::cout << "Asian end: " << std::put_time(&asianEnd, "%Y-%m-%d %H:%M:%S") << '\n';
    
    for (size_t i = 0; i < 10 && i < data.size(); ++i) {
        const auto& sd = data[i];
        std::tm timestamp = parseTimestamp(sd.timestamp);
        std::time_t timestamp_t = std::mktime(&timestamp);
        std::cout << "Timestamp: " << sd.timestamp
                  << ", std::tm: " << std::put_time(&timestamp, "%Y-%m-%d %H:%M:%S")
                  << ", std::time_t: " << timestamp_t << '\n';
    }
}

int countEvents(const std::vector<StockData> &data, std::tm &londonStart, std::tm &londonEnd, std::tm &newYorkStart, std::tm &newYorkEnd, std::tm &asianStart, std::tm &asianEnd)
{
    std::time_t londonStart_t = std::mktime(&londonStart);
    std::time_t londonEnd_t = std::mktime(&londonEnd);
    std::time_t newYorkStart_t = std::mktime(&newYorkStart);
    std::time_t newYorkEnd_t = std::mktime(&newYorkEnd);
    std::time_t asianStart_t = std::mktime(&asianStart);
    std::time_t asianEnd_t = std::mktime(&asianEnd);

    int londonCount = 0;
    int newYorkCount = 0;
    int asianCount = 0;

    bool londonFlag = false;
    bool newYorkFlag = false;
    bool asianFlag = false;

     for (int i = 900; i < 950; ++i) {
        const auto& sd = data[i];

        std::tm timestamp = parseTimestamp(sd.timestamp);
        timestamp.tm_year = londonStart.tm_year;
        timestamp.tm_mon = londonStart.tm_mon;
        timestamp.tm_mday = londonStart.tm_mday;
        std::time_t timestamp_t = std::mktime(&timestamp);

        if (timestamp_t >= londonStart_t && timestamp_t <= londonEnd_t && !londonFlag)
        {
            ++londonCount;
            londonFlag = true;

            std::cout << "Timestamp: " << sd.timestamp
                  << ", std::tm: " << std::put_time(&timestamp, "%Y-%m-%d %H:%M:%S")
                  << ", std::time_t: " << timestamp_t << '\n';
            std::cout << "London start: " << std::put_time(&londonStart, "%Y-%m-%d %H:%M:%S") << '\n';
            std::cout << "London end: " << std::put_time(&londonEnd, "%Y-%m-%d %H:%M:%S") << '\n';

        }
        else if (timestamp_t > londonEnd_t)
        {
            londonFlag = false;

            std::cout << "Timestamp: " << sd.timestamp
                  << ", std::tm: " << std::put_time(&timestamp, "%Y-%m-%d %H:%M:%S")
                  << ", std::time_t: " << timestamp_t << '\n';
        }

        if (timestamp_t >= newYorkStart_t && timestamp_t <= newYorkEnd_t && !newYorkFlag)
        {
            ++newYorkCount;
            newYorkFlag = true;

            std::cout << "Timestamp: " << sd.timestamp
                  << ", std::tm: " << std::put_time(&timestamp, "%Y-%m-%d %H:%M:%S")
                  << ", std::time_t: " << timestamp_t << '\n';
            std::cout << "New York start: " << std::put_time(&newYorkStart, "%Y-%m-%d %H:%M:%S") << '\n';
            std::cout << "New York end: " << std::put_time(&newYorkEnd, "%Y-%m-%d %H:%M:%S") << '\n';
        }
        else if (timestamp_t > newYorkEnd_t)
        {
            newYorkFlag = false;

            std::cout << "Timestamp: " << sd.timestamp
                  << ", std::tm: " << std::put_time(&timestamp, "%Y-%m-%d %H:%M:%S")
                  << ", std::time_t: " << timestamp_t << '\n';
        }

        if (timestamp_t >= asianStart_t && timestamp_t <= asianEnd_t && !asianFlag)
        {
            ++asianCount;
            asianFlag = true;

            std::cout << "Timestamp: " << sd.timestamp
                  << ", std::tm: " << std::put_time(&timestamp, "%Y-%m-%d %H:%M:%S")
                  << ", std::time_t: " << timestamp_t << '\n';
            std::cout << "Asian start: " << std::put_time(&asianStart, "%Y-%m-%d %H:%M:%S") << '\n';
            std::cout << "Asian end: " << std::put_time(&asianEnd, "%Y-%m-%d %H:%M:%S") << '\n';
        }
        else if (timestamp_t > asianEnd_t)
        {
            asianFlag = false;

            std::cout << "Timestamp: " << sd.timestamp
                  << ", std::tm: " << std::put_time(&timestamp, "%Y-%m-%d %H:%M:%S")
                  << ", std::time_t: " << timestamp_t << '\n';
        }


        // // Process sd
        // std::cout << "Timestamp: " << sd.timestamp
        //           << ", Open: " << sd.open_price
        //           << ", High: " << sd.high_price
        //           << ", Low: " << sd.low_price
        //           << ", Close: " << sd.close_price
        //           << ", Volume: " << sd.volume << '\n';
    }

    // for (const auto &sd : data)
    // {
    //     std::tm timestamp = parseTimestamp(sd.timestamp);
    //     timestamp.tm_year = londonStart.tm_year;
    //     timestamp.tm_mon = londonStart.tm_mon;
    //     timestamp.tm_mday = londonStart.tm_mday;
    //     std::time_t timestamp_t = std::mktime(&timestamp);
    

    //     if (timestamp_t >= londonStart_t && timestamp_t <= londonEnd_t && !londonFlag)
    //     {
    //         ++londonCount;
    //         londonFlag = true;
    //     }
    //     else if (timestamp_t > londonEnd_t)
    //     {
    //         londonFlag = false;
    //     }

    //     if (timestamp_t >= newYorkStart_t && timestamp_t <= newYorkEnd_t && !newYorkFlag)
    //     {
    //         ++newYorkCount;
    //         newYorkFlag = true;
    //     }
    //     else if (timestamp_t > newYorkEnd_t)
    //     {
    //         newYorkFlag = false;
    //     }

    //     if (timestamp_t >= asianStart_t && timestamp_t <= asianEnd_t && !asianFlag)
    //     {
    //         ++asianCount;
    //         asianFlag = true;
    //     }
    //     else if (timestamp_t > asianEnd_t)
    //     {
    //         asianFlag = false;
    //     }
    // }

    std::cout << "London session count: " << londonCount << '\n';
    std::cout << "New York session count: " << newYorkCount << '\n';
    std::cout << "Asian session count: " << asianCount << '\n';

    return londonCount + newYorkCount + asianCount;
}

// int countEvents(const std::vector<StockData>& data, std::tm& start, std::tm& end) {
//     int count = 0;
//     for (const auto& sd : data) {
//         std::tm timestamp = parseTimestamp(sd.timestamp); // You'll need to implement this function

//         std::time_t timestamp_t = std::mktime(&timestamp);
//         std::time_t start_t = std::mktime(&start);
//         std::time_t end_t = std::mktime(&end);
//         if (timestamp_t >= start_t && timestamp_t <= end_t) {
//             ++count;
//         }
//     }
//     return count;
// }

void saveData(const std::vector<StockData> &data, const std::string &filename)
{
    std::ofstream out(filename, std::ios::binary);
    for (const auto &sd : data)
    {
        out.write(reinterpret_cast<const char *>(&sd), sizeof(sd));
    }
}

std::vector<StockData> loadData(const std::string &filename)
{
    std::ifstream in(filename, std::ios::binary);
    std::vector<StockData> data;
    StockData sd;
    while (in.read(reinterpret_cast<char *>(&sd), sizeof(sd)))
    {
        data.push_back(sd);
    }
    return data;
}

int main()
{
    // std::ifstream file("/Volumes/LaCie/DATA/SP500/ES_continuous_adjusted_1min.txt");
    // std::string line;
    // std::vector<StockData> data;

    // std::cout << "Reading file ... \n";

    // while (std::getline(file, line)) {
    //     std::istringstream ss(line);
    //     StockData sd;
    //     std::getline(ss, sd.timestamp, ',');
    //     ss >> sd.open_price;
    //     ss.ignore(); // ignore the comma
    //     ss >> sd.high_price;
    //     ss.ignore(); // ignore the comma
    //     ss >> sd.low_price;
    //     ss.ignore(); // ignore the comma
    //     ss >> sd.close_price;
    //     ss.ignore(); // ignore the comma
    //     ss >> sd.volume;
    //     data.push_back(sd);
    // }

    // std::cout << "Saving data ... \n";

    // saveData(data, "data.bin");

    std::cout << "Loading data ... \n";
    std::vector<StockData> data = loadData("data.bin");
    // testTimestamps(data);   
    std::cout << "Counting events ... \n";
    int count = countEvents(data, londonStart, londonEnd, newYorkStart, newYorkEnd, asianStart, asianEnd);

    // for (int i = 900; i < 950; ++i) {
    //     const auto& sd = data[i];
    //     // Process sd
    //     std::cout << "Timestamp: " << sd.timestamp
    //               << ", Open: " << sd.open_price
    //               << ", High: " << sd.high_price
    //               << ", Low: " << sd.low_price
    //               << ", Close: " << sd.close_price
    //               << ", Volume: " << sd.volume << '\n';
    // }

    // Now you can process the data
    // for (const auto& sd : data) {
    //     // std::cout << "Timestamp: " << sd.timestamp
    //     //           << ", Open: " << sd.open_price
    //     //           << ", High: " << sd.high_price
    //     //           << ", Low: " << sd.low_price
    //     //           << ", Close: " << sd.close_price
    //     //           << ", Volume: " << sd.volume << '\n';
    // }

    // int londonCount = countEvents(data, londonStart, londonEnd);
    // int newYorkCount = countEvents(data, newYorkStart, newYorkEnd);
    // int asianCount = countEvents(data, asianStart, asianEnd);

    // std::cout << "London session count: " << londonCount << '\n';
    // std::cout << "New York session count: " << newYorkCount << '\n';
    // std::cout << "Asian session count: " << asianCount << '\n';

    return 0;
}