#include "models.hpp"
#include "dataIO.hpp"
#include "progress.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <regex>
#include <map>
#include <cmath>

using namespace std;

bool isTimeInODRrange(const std::tm &time)
{
    int minutesSinceMidnight = time.tm_hour * 60 + time.tm_min;
    return minutesSinceMidnight >= 180 && minutesSinceMidnight <= 240; // 03:00:00 - 04:00:00
}
bool isTimeInODRsession(const std::tm &time)
{
    int minutesSinceMidnight = time.tm_hour * 60 + time.tm_min;
    return minutesSinceMidnight >= 240 && minutesSinceMidnight <= 510; // 04:00:00 - 08:30:00
}

bool isTimeInRDRrange(const std::tm &time)
{
    int minutesSinceMidnight = time.tm_hour * 60 + time.tm_min;
    return minutesSinceMidnight >= 570 && minutesSinceMidnight <= 630; // 09:30:00 - 10:30:00
}
bool isTimeInRDRsession(const std::tm &time)
{
    int minutesSinceMidnight = time.tm_hour * 60 + time.tm_min;
    return minutesSinceMidnight >= 630 && minutesSinceMidnight <= 960; // 10:30:00 - 16:00:00
}

bool isTimeInADRrange(const std::tm &time)
{
    int minutesSinceMidnight = time.tm_hour * 60 + time.tm_min;
    return minutesSinceMidnight >= 1170 && minutesSinceMidnight <= 1230; // 19:30:00 - 20:30:00
}
bool isTimeInADRsession(const std::tm &time)
{
    int minutesSinceMidnight = time.tm_hour * 60 + time.tm_min;
    return (minutesSinceMidnight >= 1230 && minutesSinceMidnight <= 1440) || // 20:30:00 - 23:59:59
           (minutesSinceMidnight >= 0 && minutesSinceMidnight <= 120);       // 00:00:00 - 02:00:00
}

map<string, DRrange> calculateDRranges(const vector<BarData> &rangeData)
{
    map<string, DRrange> drRanges;
    string currentDate = "";
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < rangeData.size(); ++i) {
        const auto &barData = rangeData[i];
        string date = barData.timestamp.substr(0, 10); // Extract the date (YYYY-MM-DD) from the timestamp

        if (date != currentDate)
        {
            // The date has changed, so create a new DRrange
            DRrange drRange;
            drRange.openBar = barData;
            drRange.closeBar = barData;

            drRange.DRhigh = barData.barHigh;
            drRange.DRlow = barData.barLow;

            if (barData.barClose > barData.barOpen)
            {
                drRange.IDRhigh = barData.barClose;
                drRange.IDRlow = barData.barOpen;
            }
            else
            {
                drRange.IDRhigh = barData.barClose;
                drRange.IDRlow = barData.barOpen;
            }

            drRange.drRange = drRange.DRhigh - drRange.DRlow;
            drRange.idrRange = drRange.IDRhigh - drRange.IDRlow;

            drRanges[date] = drRange;
            currentDate = date;
        }
        else
        {
            // Update the last DRrange in the vector
            DRrange &drRange = drRanges[date];
            drRange.closeBar = barData;

            if (barData.barHigh > drRange.DRhigh)
            {
                drRange.DRhigh = barData.barHigh;
            }

            if (barData.barLow < drRange.DRlow)
            {
                drRange.DRlow = barData.barLow;
            }

            if (barData.barClose > drRange.IDRhigh)
            {
                drRange.IDRhigh = barData.barClose;
            }

            if (barData.barClose < drRange.IDRlow)
            {
                drRange.IDRlow = barData.barClose;
            }

            drRange.drRange = drRange.DRhigh - drRange.DRlow;

            if (barData.barClose > drRange.openBar.barOpen)
            {
                drRange.idrRange = drRange.IDRhigh - drRange.IDRlow;
            }
            else
            {
                drRange.idrRange = drRange.IDRlow - drRange.IDRhigh;
            }

            drRange.drDirection = (barData.barClose > drRange.openBar.barOpen) ? ConfDirection::LONG : ConfDirection::SHORT;

            for (double i = 0.1; i <= 1.0; i += 0.1)
            {
                double incrementDR = drRange.drRange * i;
                double incrementIDR = drRange.idrRange * i;

                if (drRange.drDirection == ConfDirection::LONG)
                {
                    drRange.retLevelsDR.push_back(drRange.DRlow + incrementDR);
                    drRange.retLevelsIDR.push_back(drRange.IDRlow + incrementIDR);
                }
                else
                {
                    drRange.retLevelsDR.push_back(drRange.DRhigh - incrementDR);
                    drRange.retLevelsIDR.push_back(drRange.IDRhigh - incrementIDR);
                }
            }

            drRange.stdUnitDR = drRange.drRange * 0.1;
            drRange.stdUnitIDR = drRange.idrRange * 0.1;
        }
        
        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed_seconds = std::chrono::duration<double>(current_time - start_time).count();
        printProgressBar(i, rangeData.size(), elapsed_seconds);
    
    }

    return drRanges;
}

map<string, DRsession> calculateDRsession(const vector<BarData> &sessionData, const map<string, DRrange> &drRanges)
{
    map<string, DRsession> drSessions;
    string currentDate = "";

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < sessionData.size(); ++i)
    {   
        const auto &barData = sessionData[i];
        string date = barData.timestamp.substr(0, 10); // Extract the date (YYYY-MM-DD) from the timestamp

        // Check if the date exists in drRanges
        if (drRanges.find(date) == drRanges.end()) {
            cout << "Date not found: " << date << "\n";
            continue; // Skip this iteration if the date is not found
        }

        // Check if the date is the second last date in the keys
        auto secondLastDateIt = std::prev(drRanges.end(), 2);
        if (date >= secondLastDateIt->first) {
            cout << "Second last date, stopping early: " << date << "\n";
            break; // Stop the loop if the date is the second last date or later
        }


        DRrange drRange = drRanges.at(date);
        cout << "Date found: " << date << "\n";
    
        if (date != currentDate)
        {
            // The date has changed, so create a new DRsession
            DRsession drSession;

            if (barData.barClose > drRange.DRhigh || barData.barClose < drRange.DRlow)
            {
                drSession.confDR = true;

                if (barData.barClose > drRange.DRhigh)
                {
                    drSession.confDirection = ConfDirection::LONG;
                }
                else if (barData.barClose < drRange.DRlow)
                {
                    drSession.confDirection = ConfDirection::SHORT;
                }

                drSession.confTime = barData.timestamp;
            }
            else
            {
                drSession.confDR = false;
            }

            if (drSession.confDR)
            {
                // Check if price retraced into the DR
                if (barData.barClose <= drRange.DRhigh && barData.barClose >= drRange.DRlow)
                {
                    drSession.retDR = true;

                    // Update max retracement if current bar's close is a new max
                    if (barData.barClose > drSession.maxRetDR)
                    {
                        drSession.maxRetDR = barData.barClose;
                        drSession.maxRetDRTime = barData.timestamp;
                    }

                    // Check if price retraced back into the DR after extending at least half a standard deviation above/below the DR
                    if ((drSession.confDirection == ConfDirection::LONG && barData.barClose < drRange.DRhigh - 0.5 * drRange.stdUnitDR) ||
                        (drSession.confDirection == ConfDirection::SHORT && barData.barClose > drRange.DRlow + 0.5 * drRange.stdUnitDR))
                    {
                        drSession.maxRetAfter05DR = barData.barClose;
                    }
                }
                else
                {
                    drSession.retDR = false;
                }
            }

            if (drSession.confDR)
            {
                // Check if price retraced into the IDR
                if (barData.barClose <= drRange.IDRhigh && barData.barClose >= drRange.IDRlow)
                {
                    drSession.retIDR = true;

                    // Update max retracement if current bar's close is a new max
                    if (barData.barClose > drSession.maxRetIDR)
                    {
                        drSession.maxRetIDR = barData.barClose;
                        drSession.maxRetIDRTime = barData.timestamp;
                    }

                    // Check if price retraced back into the IDR after extending at least half a standard deviation above/below the IDR
                    if ((drSession.confDirection == ConfDirection::LONG && barData.barClose < drRange.IDRhigh - 0.5 * drRange.stdUnitIDR) ||
                        (drSession.confDirection == ConfDirection::SHORT && barData.barClose > drRange.IDRlow + 0.5 * drRange.stdUnitIDR))
                    {
                        drSession.maxRetAfter05IDR = barData.barClose;
                    }
                }
                else
                {
                    drSession.retIDR = false;
                }
            }

            // Check if the session price closed outside of the defining range
            if (barData.barClose > drRange.DRhigh || barData.barClose < drRange.DRlow)
            {
                drSession.outDR = true;
            }
            else
            {
                drSession.outDR = false;
            }

            // Calculate how many standard deviations above/below the IDR the price extended
            drSession.stdDevUp = (barData.barHigh - drRange.IDRhigh) / drRange.stdUnitIDR;
            drSession.stdDevDown = (drRange.IDRlow - barData.barLow) / drRange.stdUnitIDR;

            // Set the opening, high, low, and close prices of the trading session
            drSession.open = barData.barOpen;
            drSession.high = barData.barHigh;
            drSession.low = barData.barLow;
            drSession.close = barData.barClose;

            drSessions[date] = drSession;
            currentDate = date;
        }
        else
        {
            // Update the last DRsession in the vector
            DRsession &drSession = drSessions[date];

            if (!drSession.confDR && (barData.barClose > drRange.DRhigh || barData.barClose < drRange.DRlow))
            {
                drSession.confDR = true;

                if (barData.barClose > drRange.DRhigh)
                {
                    drSession.confDirection = ConfDirection::LONG;
                }
                else if (barData.barClose < drRange.DRlow)
                {
                    drSession.confDirection = ConfDirection::SHORT;
                }

                drSession.confTime = barData.timestamp;
            }

            if (drSession.confDR)
            {
                // Check if price retraced into the DR
                if (barData.barClose <= drRange.DRhigh && barData.barClose >= drRange.DRlow)
                {
                    drSession.retDR = true;

                    // Update max retracement if current bar's close is a new max
                    if (barData.barClose > drSession.maxRetDR)
                    {
                        drSession.maxRetDR = barData.barClose;
                        drSession.maxRetDRTime = barData.timestamp;
                    }

                    // Check if price retraced back into the DR after extending at least half a standard deviation above/below the DR
                    if ((drSession.confDirection == ConfDirection::LONG && barData.barClose < drRange.DRhigh - 0.5 * drRange.stdUnitDR) ||
                        (drSession.confDirection == ConfDirection::SHORT && barData.barClose > drRange.DRlow + 0.5 * drRange.stdUnitDR))
                    {
                        drSession.maxRetAfter05DR = std::max(drSession.maxRetAfter05DR, barData.barClose);
                    }
                }
                else
                {
                    drSession.retDR = false;
                }
            }

            if (drSession.confDR)
            {
                // Check if price retraced into the IDR
                if (barData.barClose <= drRange.IDRhigh && barData.barClose >= drRange.IDRlow)
                {
                    drSession.retIDR = true;

                    // Update max retracement if current bar's close is a new max
                    if (barData.barClose > drSession.maxRetIDR)
                    {
                        drSession.maxRetIDR = barData.barClose;
                        drSession.maxRetIDRTime = barData.timestamp;
                    }

                    // Check if price retraced back into the IDR after extending at least half a standard deviation above/below the IDR
                    if ((drSession.confDirection == ConfDirection::LONG && barData.barClose < drRange.IDRhigh - 0.5 * drRange.stdUnitIDR) ||
                        (drSession.confDirection == ConfDirection::SHORT && barData.barClose > drRange.IDRlow + 0.5 * drRange.stdUnitIDR))
                    {
                        drSession.maxRetAfter05IDR = std::max(drSession.maxRetAfter05IDR, barData.barClose);
                    }
                }
                else
                {
                    drSession.retIDR = false;
                }
            }

            // Check if the session price closed outside of the defining range
            if (barData.barClose > drRange.DRhigh || barData.barClose < drRange.DRlow)
            {
                drSession.outDR = true;
            }
            else
            {
                drSession.outDR = false;
            }

            // Calculate how many standard deviations above/below the IDR the price extended
            drSession.stdDevUp = std::max(drSession.stdDevUp, (barData.barHigh - drRange.IDRhigh) / drRange.stdUnitIDR);
            drSession.stdDevDown = std::max(drSession.stdDevDown, (drRange.IDRlow - barData.barLow) / drRange.stdUnitIDR);

            // Set the high and low prices of the trading session if they are greater or lower than the current values
            drSession.high = std::max(drSession.high, barData.barHigh);
            drSession.low = std::min(drSession.low, barData.barLow);

            drSession.close = barData.barClose;
        };
    
        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed_seconds = std::chrono::duration<double>(current_time - start_time).count();
        printProgressBar(i, sessionData.size(), elapsed_seconds);
    
    }
    return drSessions;
};

void saveSessions(const vector<Session> &sessions, const string &filename)
{
    ofstream output(filename, ios::binary);
    for (const Session &session : sessions)
    {
        output.write(reinterpret_cast<const char *>(&session), sizeof(Session));
    }
    output.close();
    // return 0;
}

vector<Session> calculateSessionStats(const vector<BarData> &rangeData, const vector<BarData> &sessionData, SessionType sessionType)
{
    // Calculate DR ranges
    cout << "Calculating DR ranges ... \n";
    map<string, DRrange> drRanges = calculateDRranges(rangeData);

    // Calculate DR sessions using the calculated DR ranges
    cout << "Calculating DR sessions ... \n";
    map<string, DRsession> drSessions = calculateDRsession(sessionData, drRanges);

    cout << "Creating sessions ... \n";
    vector<Session> sessions;
    
    cout << "Creating iterators ... \n";
    auto it1 = drRanges.begin();
    auto it2 = drSessions.begin();

    cout << "Calculating session stats ... \n";
    while (it1 != drRanges.end() && it2 != drSessions.end())
    {
        // Access the keys using it1->first and it2->first
        std::string key1 = it1->first;
        std::string key2 = it2->first;

        Session session;
        session.date = key1;
        session.sessionType = sessionType;

        if (drRanges.find(key1) != drRanges.end() && drSessions.find(key2) != drSessions.end()) {
            session.drRange = it1->second; // Use the value from the iterator directly
            session.drSession = it2->second; // Use the value from the iterator directly
            sessions.push_back(session);
        }
        ++it1;
        ++it2;
    }

    return sessions;
}

void displaySessionStats(const vector<Session>& sessions) {
    int totalSessions = sessions.size();
    int confDRTrueCount = 0;
    int outDRTrueCount = 0;

    for (const Session& session : sessions) {
        if (session.drSession.confDR) {
            confDRTrueCount++;
        }
        if (session.drSession.outDR) {
            outDRTrueCount++;
        }
    }

    double confDRPercentage = (double)confDRTrueCount / totalSessions * 100;
    double outDRPercentage = (double)outDRTrueCount / totalSessions * 100;

    cout << "Total Sessions: " << totalSessions << "\n";
    cout << "confDR True Count: " << confDRTrueCount << " (" << confDRPercentage << "%)\n";
    cout << "outDR True Count: " << outDRTrueCount << " (" << outDRPercentage << "%)\n";
}


int main()
{

    cout << "Loading data ... \n";
    vector<BarData> data = loadData("data.bin");

    vector<BarData> odrRangeData;
    vector<BarData> odrSessionData;
    vector<BarData> rdrRangeData;
    vector<BarData> rdrSessionData;
    vector<BarData> adrRangeData;
    vector<BarData> adrSessionData;

    vector<Session> odrSessions;
    vector<Session> rdrSessions;
    vector<Session> adrSessions;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < data.size(); ++i) {
        const auto &barData = data[i];

        std::istringstream ss(barData.timestamp);
        std::tm tm;
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (isTimeInODRrange(tm)) {
            odrRangeData.push_back(barData);
        } else if (isTimeInODRsession(tm)) {
            odrSessionData.push_back(barData);
        } else if (isTimeInRDRrange(tm)) {
            rdrRangeData.push_back(barData);
        } else if (isTimeInRDRsession(tm)) {
            rdrSessionData.push_back(barData);
        } else if (isTimeInADRrange(tm)) {
            adrRangeData.push_back(barData);
        } else if (isTimeInADRsession(tm)) {
            adrSessionData.push_back(barData);
        }

        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed_seconds = std::chrono::duration<double>(current_time - start_time).count();
        printProgressBar(i, data.size(), elapsed_seconds);
    }

    // Save the filtered data to respective output files
    cout << "Saving ODR range data ... \n";
    saveData(odrRangeData, "ODR_range.bin");
    cout << "Saving ODR session data ... \n";
    saveData(odrSessionData, "ODR_session.bin");
    cout << "Saving RDR range data ... \n";
    saveData(rdrRangeData, "RDR_range.bin");
    cout << "Saving RDR session data ... \n";
    saveData(rdrSessionData, "RDR_session.bin");
    cout << "Saving ADR range data ... \n";
    saveData(adrRangeData, "ADR_range.bin");
    cout << "Saving ADR session data ... \n";
    saveData(adrSessionData, "ADR_session.bin");

   
    cout << "Loading ODR range Data ... \n";
    vector<BarData> odrRangeData1 = loadData("ODR_range.bin");
    cout << "Loading ODR session Data ... \n";
    vector<BarData> odrSessionData1 = loadData("ODR_session.bin");

    cout << "Loading RDR ranges ... \n";
    vector<BarData> rdrRangeData1 = loadData("RDR_range.bin");
    cout << "Loadign RDR sessions ... \n";
    vector<BarData> rdrSessionData1 = loadData("RDR_session.bin");

    cout << "Loading ADR ranges ... \n";
    vector<BarData> adrRangeData1 = loadData("ADR_range.bin");
    cout << "Loading ADR sessions ... \n";
    vector<BarData> adrSessionData1 = loadData("ADR_session.bin");

    // Calculate sessions and save them
    cout << "Calculating ODR sessions and saving to ODR_stats.bin ... \n";
    vector<Session> odrSessions1 = calculateSessionStats(odrRangeData1, odrSessionData1, SessionType::ODR);
    saveSessions(odrSessions1, "ODR_stats.bin");

    cout << "Calculating RDR sessions and saving to RDR_stats.bin ... \n";
    vector<Session> rdrSessions1 = calculateSessionStats(rdrRangeData1, rdrSessionData1, SessionType::RDR);
    saveSessions(rdrSessions1, "RDR_stats.bin");

    cout << "Calculating ADR sessions and saving to ADR_stats.bin ... \n";
    vector<Session> adrSessions1 = calculateSessionStats(adrRangeData1, adrSessionData1, SessionType::ADR);
    saveSessions(adrSessions1, "ADR_stats.bin");

    
    cout << "ODR Session Stats:\n";
    displaySessionStats(odrSessions1);

    cout << "RDR Session Stats:\n";
    displaySessionStats(rdrSessions1);

    cout << "ADR Session Stats:\n";
    displaySessionStats(adrSessions1);


    return 0;
}