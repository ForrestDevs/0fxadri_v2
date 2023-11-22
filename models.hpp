#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>


/**
 * @brief A struct to hold the data for a single bar
 * 
 * @param timestamp The timestamp of the bar in YYYY-MM-DD HH:MM:SS format
 * @param barOpen The open price of the bar
 * @param barHigh The high price of the bar
 * @param barLow The low price of the bar
 * @param barClose The close price of the bar
 * @param barVolume The volume of the bar
*/
struct BarData
{   
    // The timestamp of the bar in YYYY-MM-DD HH:MM:SS format
    std::string timestamp;
    // The open price of the bar
    double barOpen;
    // The high price of the bar
    double barHigh;
    // The low price of the bar
    double barLow;
    // The close price of the bar
    double barClose;
    // The volume of the bar
    int barVolume;
};

enum SessionType
{
    ODR,
    RDR,
    ADR
};

enum ConfDirection
{
    LONG,
    SHORT
};


struct DRrange
{   
    // A 10% increment of the standard deviation of the defining range
    double stdUnitDR;
    // A 10% increment of the standard deviation of the initial defining range
    double stdUnitIDR;
    // A vector of the standard deviation price levels of the defining range in 0.1 increments
    std::vector<double> retLevelsDR;
    // A vector of the standard deviation price levels of the initial defining range in 0.1 increments
    std::vector<double> retLevelsIDR;
    // The opening bar of the defining range
    BarData openBar;
    // The closing bar of the defining range
    BarData closeBar;
    // The direction of the defining range, if the close price is higher than the open price then the direction is LONG, otherwise the direction is SHORT
    ConfDirection drDirection;
    // The difference between the DR high and the DR low
    double drRange;
    // The difference between the IDR high and the IDR low
    double idrRange;
    // The price level of the DR high
    double DRhigh;
    // The price level of the DR low
    double DRlow;
    // The price level of the IDR high
    double IDRhigh;
    // The price level of the IDR low
    double IDRlow;

};

struct DRsession
{
    // Did the price close outside of the defining range?
    bool confDR;
    // Which direction did price close outside of the defining range?
    ConfDirection confDirection;
    // What time did price close outside of the defining range?
    std::string confTime;

    // Did price retrace into the DR?
    bool retDR;
    // IFF price retrace into the DR, what was the max retracement?
    double maxRetDR;
    // What time did the max retracement into DR occur at HH:MM:SS format?
    std::string maxRetDRTime;
    // What is the furthest that the price retraced back into the DR, after extending at least half a standard deviation above the high DR for a Long confirmation or below low DR for a short?
    double maxRetAfter05DR;
    
    // Did price retrace into the IDR?
    bool retIDR;
    // IFF price retrace into the IDR, what was the max retracement?
    double maxRetIDR;
    // What time did the max retracement into IDR occur at HH:MM:SS format?
    std::string maxRetIDRTime;
    // What is the furthest that the price retraced back into the IDR, after extending at least half a standard deviation above the high IDR for a Long confirmation or below low IDR for a short?
    double maxRetAfter05IDR; 

    // Did the session price close outside of the defining range?
    bool outDR;

    // How many standard deviations above the high IDR did the price extend?
    double stdDevUp;
    // How many standard deviations below the low IDR did the price extend?
    double stdDevDown;

    // Opening price of the trading session
    double open;
    // High price of the trading session
    double high;
    // Low price of the trading session
    double low;
    // Close price of the trading session
    double close;
};

struct Session
{
    // DD:MM:YYYY format
    std::string date;
    // ODR, RDR, ADR
    SessionType sessionType;
    // The properties of the defining range
    DRrange drRange;
    // The properties of the trading session following the defining range
    DRsession drSession;

};

#endif // MODELS_HPP