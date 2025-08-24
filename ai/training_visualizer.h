#ifndef TRAINING_VISUALIZER_H
#define TRAINING_VISUALIZER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class TrainingVisualizer {
private:
    std::vector<double> player1Rewards;
    std::vector<double> player2Rewards;
    std::vector<int> gameNumbers;
    std::vector<double> winRates;
    std::vector<int> winners;  // Track who won each game: 0=P1, 1=P2, -1=draw
    int player1Wins;
    int player2Wins;
    int totalGames;
    
public:
    TrainingVisualizer();
    
    // Add data points
    void addGameResult(int gameNum, double p1Reward, double p2Reward, int winner);
    
    // Calculate moving average
    double getMovingAverage(const std::vector<double>& data, int window = 50) const;
    
    // Generate simple ASCII plots
    void printRewardProgress() const;
    void printWinRateProgress() const;
    
    // Save data to CSV for external plotting
    void saveDataCSV(const std::string& filename) const;
    
    // Generate simple gnuplot script
    void generateGnuplotScript(const std::string& scriptName) const;
    
    // Print summary statistics
    void printSummary() const;
};

#endif
