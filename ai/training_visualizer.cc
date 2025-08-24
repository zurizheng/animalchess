#include "training_visualizer.h"
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <limits>

TrainingVisualizer::TrainingVisualizer() 
    : player1Wins(0), player2Wins(0), totalGames(0) {
}

void TrainingVisualizer::addGameResult(int gameNum, double p1Reward, double p2Reward, int winner) {
    gameNumbers.push_back(gameNum);
    player1Rewards.push_back(p1Reward);
    player2Rewards.push_back(p2Reward);
    winners.push_back(winner);  // Store the actual winner
    
    // Count wins based on the winner parameter
    if (winner == 0) {
        player1Wins++;
    } else if (winner == 1) {
        player2Wins++;
    }
    // winner == -1 means draw, no increment needed
    
    totalGames++;
    
    // Calculate win rate over last 100 games using actual winner data
    int start = std::max(0, static_cast<int>(gameNumbers.size()) - 100);
    int recentP1Wins = 0;
    int recentGames = 0;
    
    for (int i = start; i < gameNumbers.size(); i++) {
        recentGames++;
        // Use the stored winner data instead of reward comparison
        if (i < winners.size() && winners[i] == 0) {
            recentP1Wins++;
        }
    }
    
    double winRate = recentGames > 0 ? static_cast<double>(recentP1Wins) / recentGames : 0.0;
    winRates.push_back(winRate);
}

double TrainingVisualizer::getMovingAverage(const std::vector<double>& data, int window) const {
    if (data.empty()) return 0.0;
    
    int start = std::max(0, static_cast<int>(data.size()) - window);
    double sum = 0.0;
    for (int i = start; i < data.size(); i++) {
        sum += data[i];
    }
    return sum / (data.size() - start);
}

void TrainingVisualizer::printRewardProgress() const {
    if (gameNumbers.empty()) return;
    
    std::cout << "\nTraining Progress - Average Rewards" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    const int plotWidth = 60;
    const int plotHeight = 10;
    
    // Get recent data for plotting
    int startIdx = std::max(0, static_cast<int>(player1Rewards.size()) - 200);
    
    if (startIdx >= player1Rewards.size()) return;
    
    // Find min/max for scaling
    double minReward = std::numeric_limits<double>::max();
    double maxReward = std::numeric_limits<double>::lowest();
    
    for (int i = startIdx; i < player1Rewards.size(); i++) {
        minReward = std::min(minReward, std::min(player1Rewards[i], player2Rewards[i]));
        maxReward = std::max(maxReward, std::max(player1Rewards[i], player2Rewards[i]));
    }
    
    if (maxReward == minReward) {
        maxReward = minReward + 1.0;
    }
    
    // Simple ASCII plot
    std::cout << "Player 1 (●) vs Player 2 (○) - Last " << (player1Rewards.size() - startIdx) << " games" << std::endl;
    std::cout << "Reward range: " << std::fixed << std::setprecision(1) 
              << minReward << " to " << maxReward << std::endl;
    
    for (int row = plotHeight - 1; row >= 0; row--) {
        double threshold = minReward + (maxReward - minReward) * row / (plotHeight - 1);
        std::cout << std::setw(6) << std::fixed << std::setprecision(1) << threshold << " |";
        
        for (int col = 0; col < plotWidth; col++) {
            int dataIdx = startIdx + (col * (player1Rewards.size() - startIdx)) / plotWidth;
            if (dataIdx >= player1Rewards.size()) dataIdx = player1Rewards.size() - 1;
            
            bool p1Above = player1Rewards[dataIdx] >= threshold;
            bool p2Above = player2Rewards[dataIdx] >= threshold;
            
            if (p1Above && p2Above) std::cout << "●";
            else if (p1Above) std::cout << "●";
            else if (p2Above) std::cout << "○";
            else std::cout << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "       +" << std::string(plotWidth, '-') << std::endl;
    std::cout << "        Game " << gameNumbers[startIdx] << " → " << gameNumbers.back() << std::endl;
    
    // Print moving averages
    double p1Avg = getMovingAverage(player1Rewards, 50);
    double p2Avg = getMovingAverage(player2Rewards, 50);
    std::cout << "50-game average: Player 1: " << std::fixed << std::setprecision(2) 
              << p1Avg << ", Player 2: " << p2Avg << std::endl;
}

void TrainingVisualizer::printWinRateProgress() const {
    if (winRates.empty()) return;
    
    std::cout << "\nWin Rate Progress (Player 1)" << std::endl;
    std::cout << "===============================" << std::endl;
    
    const int plotWidth = 60;
    const int plotHeight = 8;
    
    int startIdx = std::max(0, static_cast<int>(winRates.size()) - 200);
    
    std::cout << "Win rate over last 100 games - Last " << (winRates.size() - startIdx) << " data points" << std::endl;
    
    for (int row = plotHeight - 1; row >= 0; row--) {
        double threshold = static_cast<double>(row) / (plotHeight - 1);
        std::cout << std::setw(5) << std::fixed << std::setprecision(2) << threshold << " |";
        
        for (int col = 0; col < plotWidth; col++) {
            int dataIdx = startIdx + (col * (winRates.size() - startIdx)) / plotWidth;
            if (dataIdx >= winRates.size()) dataIdx = winRates.size() - 1;
            
            if (winRates[dataIdx] >= threshold) {
                std::cout << "█";
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
    
    std::cout << "      +" << std::string(plotWidth, '-') << std::endl;
    std::cout << "       0%" << std::string(plotWidth/2 - 3, ' ') << "50%" 
              << std::string(plotWidth/2 - 3, ' ') << "100%" << std::endl;
    
    std::cout << "Current win rate: " << std::fixed << std::setprecision(1) 
              << (winRates.back() * 100) << "%" << std::endl;
}

void TrainingVisualizer::saveDataCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open " << filename << " for writing" << std::endl;
        return;
    }
    
    file << "Game,Player1_Reward,Player2_Reward,Player1_WinRate\n";
    for (size_t i = 0; i < gameNumbers.size(); i++) {
        file << gameNumbers[i] << "," 
             << player1Rewards[i] << "," 
             << player2Rewards[i] << ",";
        if (i < winRates.size()) {
            file << winRates[i];
        }
        file << "\n";
    }
    
    file.close();
    std::cout << "Training data saved to " << filename << std::endl;
    
    // Also create a version without headers for gnuplot
    std::string gnuplotFile = filename.substr(0, filename.find_last_of('.')) + "_gnuplot.csv";
    std::ofstream gFile(gnuplotFile);
    if (gFile.is_open()) {
        for (size_t i = 0; i < gameNumbers.size(); i++) {
            gFile << gameNumbers[i] << "," 
                  << player1Rewards[i] << "," 
                  << player2Rewards[i] << ",";
            if (i < winRates.size()) {
                gFile << winRates[i];
            }
            gFile << "\n";
        }
        gFile.close();
    }
}

void TrainingVisualizer::generateGnuplotScript(const std::string& scriptName) const {
    std::ofstream script(scriptName);
    if (!script.is_open()) {
        std::cerr << "Could not create gnuplot script " << scriptName << std::endl;
        return;
    }
    
    script << "set terminal png size 1200,800\n";
    script << "set output 'training_progress.png'\n";
    script << "set multiplot layout 2,1\n";
    script << "set title 'AI Training Progress - Rewards'\n";
    script << "set xlabel 'Game Number'\n";
    script << "set ylabel 'Average Reward'\n";
    script << "set grid\n";
    script << "set datafile separator ','\n";
    script << "plot 'training_data_gnuplot.csv' using 1:2 with lines title 'Player 1' lw 2, \\\n";
    script << "     'training_data_gnuplot.csv' using 1:3 with lines title 'Player 2' lw 2\n";
    script << "set title 'AI Training Progress - Win Rate'\n";
    script << "set ylabel 'Win Rate (%)'\n";
    script << "set yrange [0:100]\n";
    script << "plot 'training_data_gnuplot.csv' using 1:($4*100) with lines title 'Player 1 Win Rate' lw 2\n";
    script << "unset multiplot\n";
    
    script.close();
    std::cout << "Gnuplot script saved to " << scriptName << std::endl;
    std::cout << "Run 'gnuplot " << scriptName << "' to generate training_progress.png" << std::endl;
}

void TrainingVisualizer::printSummary() const {
    std::cout << "\nTraining Summary" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "Total games: " << totalGames << std::endl;
    
    // Use actual win data instead of reward comparison
    int actualDraws = totalGames - player1Wins - player2Wins;
    
    std::cout << "Player 1 wins: " << player1Wins << " (" 
              << (totalGames > 0 ? 100.0 * player1Wins / totalGames : 0.0) << "%)" << std::endl;
    std::cout << "Player 2 wins: " << player2Wins << " (" 
              << (totalGames > 0 ? 100.0 * player2Wins / totalGames : 0.0) << "%)" << std::endl;
    std::cout << "Draws: " << actualDraws << " (" 
              << (totalGames > 0 ? 100.0 * actualDraws / totalGames : 0.0) << "%)" << std::endl;
    
    // Also show reward-based comparison for debugging
    int rewardBasedP1Wins = 0;
    int rewardBasedP2Wins = 0;
    
    for (int i = 0; i < player1Rewards.size() && i < player2Rewards.size(); i++) {
        if (player1Rewards[i] > player2Rewards[i]) {
            rewardBasedP1Wins++;
        } else if (player2Rewards[i] > player1Rewards[i]) {
            rewardBasedP2Wins++;
        }
    }
    
    std::cout << "\nReward Comparison (for debugging):" << std::endl;
    std::cout << "Player 1 higher rewards: " << rewardBasedP1Wins << " games" << std::endl;
    std::cout << "Player 2 higher rewards: " << rewardBasedP2Wins << " games" << std::endl; 
    
    if (!player1Rewards.empty()) {
        std::cout << "Average rewards - Player 1: " << getMovingAverage(player1Rewards, player1Rewards.size())
                  << ", Player 2: " << getMovingAverage(player2Rewards, player2Rewards.size()) << std::endl;
    }
}
