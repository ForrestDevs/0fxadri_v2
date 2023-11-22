#include "matplotlibcpp.h"
#include <vector>

namespace plt = matplotlibcpp;

void displayCandlestickChart(const std::vector<BarData>& data, int numBars) {
    std::vector<double> dates, opens, highs, lows, closes;

    // Get the last numBars bars
    for (int i = std::max(0, static_cast<int>(data.size()) - numBars); i < data.size(); ++i) {
        const BarData& barData = data[i];
        dates.push_back(i); // Use the index as the date for simplicity
        opens.push_back(barData.open);
        highs.push_back(barData.high);
        lows.push_back(barData.low);
        closes.push_back(barData.close);
    }

    plt::figure();
    plt::plot(dates, opens, "r");
    plt::plot(dates, highs, "g");
    plt::plot(dates, lows, "b");
    plt::plot(dates, closes, "c");
    plt::title("Candlestick Chart");
    plt::show();
}