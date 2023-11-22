#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

struct StockData
{
    std::string timestamp;
    double open_price;
    double high_price;
    double low_price;
    double close_price;
    int volume;
};

struct SessionStats {
    int count = 0;
    double totalSize = 0.0;
};







std::tm londonStart = {0, 30, 3, 0, 0, 0, 0, 0, 0}; // 3:30 AM
std::tm londonEnd = {0, 30, 4, 0, 0, 0, 0, 0, 0};   // 4:30 AM

std::tm newYorkStart = {0, 30, 9, 0, 0, 0, 0, 0, 0}; // 9:30 AM
std::tm newYorkEnd = {0, 30, 10, 0, 0, 0, 0, 0, 0};  // 10:30 AM

std::tm asianStart = {0, 30, 20, 0, 0, 0, 0, 0, 0}; // 8:30 PM
std::tm asianEnd = {0, 30, 21, 0, 0, 0, 0, 0, 0};   // 9:30 PM

std::tm parseTimestamp(const std::string &timestamp)
{
    std::tm tm = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S"); // Adjust this format to match your timestamp format
    return tm;
}

std::tm parseTime(const std::string &timestamp)
{
    std::tm tm = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&tm, "%H:%M:%S"); // Adjust this format to match your timestamp format
    return tm;
}

std::string tmToString(const std::tm &time)
{
    std::ostringstream ss;
    ss << std::put_time(&time, "%H:%M:%S");
    return ss.str();
}

bool isTimeInRange(const std::string &time, const std::string &start, const std::string &end)
{
    // std::cout << "Time: " << time << '\n';
    // std::cout << "Start: " << start << '\n';
    // std::cout << "End: " << end << '\n';

    std::tm tm_time = parseTime(time);
    std::tm tm_start = parseTime(start);
    std::tm tm_end = parseTime(end);

    // Convert to minutes since midnight for easier comparison
    int minutes_time = tm_time.tm_hour * 60 + tm_time.tm_min;
    int minutes_start = tm_start.tm_hour * 60 + tm_start.tm_min;
    int minutes_end = tm_end.tm_hour * 60 + tm_end.tm_min;

    // std::cout << "Time: " << minutes_time << '\n';
    // std::cout << "Start: " << minutes_start << '\n';
    // std::cout << "End: " << minutes_end << '\n';

    return minutes_time >= minutes_start && minutes_time <= minutes_end;
}

#include <chrono>

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

// double calculateAverageRange(const std::vector<StockData>& data, int start, int end) {
//     double total = 0.0;
//     int count = 0;

//     for (int i = start; i < end; ++i) {
//         total += data[i].high - data[i].low; // Assuming 'high' and 'low' are the range of each data point
//         ++count;
//     }

//     return total / count;
// }

int countEvents(const std::vector<StockData> &data, std::tm &londonStart, std::tm &londonEnd, std::tm &newYorkStart, std::tm &newYorkEnd, std::tm &asianStart, std::tm &asianEnd)
{
    std::string londonStart_s = tmToString(londonStart);
    std::string londonEnd_s = tmToString(londonEnd);
    std::string newYorkStart_s = tmToString(newYorkStart);
    std::string newYorkEnd_s = tmToString(newYorkEnd);
    std::string asianStart_s = tmToString(asianStart);
    std::string asianEnd_s = tmToString(asianEnd);

    int londonCount = 0;
    int newYorkCount = 0;
    int asianCount = 0;

    bool londonFlag = false;
    bool newYorkFlag = false;
    bool asianFlag = false;

    int total = data.size();

    int totalRL = 0;
    int totalRN = 0;
    int totalRA = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    int sessionOpen = 0;

    for (int i = 5735000; i < total; ++i)
    {
        const auto &sd = data[i];
        std::tm timestamp = parseTimestamp(sd.timestamp);
        std::string timestamp_s = tmToString(timestamp);

        if (isTimeInRange(timestamp_s, londonStart_s, londonEnd_s))
        {
            // std::cout << "London session: " << timestamp_s << '\n';
            // std::cout << "London session start: " << londonStart_s << '\n';
            // std::cout << "London session end: " << londonEnd_s << '\n';

            if (!londonFlag)
            {   
                sessionOpen = data[i].open_price;
                ++londonCount;
                londonFlag = true;
            }
        }
        else
        {
            if(londonFlag) {
                int close = data[i-1].close_price;
                if (sessionOpen >= close) {
                    totalRL += sessionOpen - close;
                } else {
                    totalRL += close - sessionOpen;
                }
                sessionOpen = 0;
            }
            londonFlag = false;
        }

        if (isTimeInRange(timestamp_s, newYorkStart_s, newYorkEnd_s))
        {
            // std::cout << "New York session: " << timestamp_s << '\n';
            if (!newYorkFlag)
            {
                sessionOpen = data[i].open_price;
                ++newYorkCount;
                newYorkFlag = true;
            }
        }
        else
        {
            // std::cout << "Not in New York session: " << timestamp_s << '\n';
            if (newYorkFlag) {
                int close = data[i-1].close_price;
                if (sessionOpen >= close) {
                    totalRN += sessionOpen - close;
                } else {
                    totalRN += close - sessionOpen;
                }
                sessionOpen = 0;
            }
            newYorkFlag = false;
        }

        if (isTimeInRange(timestamp_s, asianStart_s, asianEnd_s))
        {
            // std::cout << "Asian session: " << timestamp_s << '\n';
            if (!asianFlag)
            {   
                sessionOpen = data[i].open_price;
                ++asianCount;
                asianFlag = true;
            }
        }
        else
        {   
            if (asianFlag) {
                int close = data[i-1].close_price;
                if (sessionOpen >= close) {
                    totalRA += sessionOpen - close;
                } else {
                    totalRA += close - sessionOpen;
                }
                sessionOpen = 0;
            }
            // std::cout << "Not in Asian session: " << timestamp_s << '\n';
            asianFlag = false;
        }

        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed_seconds = std::chrono::duration<double>(current_time - start_time).count();
        printProgressBar(i, total, elapsed_seconds);
    }

    std::cout << std::endl; // Move to the next line after the progress bar

    // for (const auto &sd : data)
    // {
    // }

    std::cout << "London session count: " << londonCount << '\n';
    std::cout << "New York session count: " << newYorkCount << '\n';
    std::cout << "Asian session count: " << asianCount << '\n';

    std::cout << "London session average range: " << totalRL / londonCount << '\n';
    std::cout << "New York session average range: " << totalRN / newYorkCount << '\n';
    std::cout << "Asian session average range: " << totalRA / asianCount << '\n';
    
    return londonCount + newYorkCount + asianCount;
}


vector<StockData> loadData(const string &filename)
{
    ifstream in(filename, std::ios::binary);
    vector<StockData> data;
    StockData sd;
    while (in.read(reinterpret_cast<char *>(&sd), sizeof(sd)))
    {
        data.push_back(sd);
    }
    return data;
}


struct SessionStats {
    int count = 0;
    double totalSize = 0.0;
};


int main()
{
    std::cout << "Loading data ... \n";
    std::vector<StockData> data = loadData("data.bin"); 



    std::cout << "Counting events ... \n";
    int count = countEvents(data, londonStart, londonEnd, newYorkStart, newYorkEnd, asianStart, asianEnd);

    return 0;
}