#ifndef __AINETWORK_H__
#define __AINETWORK_H__

#include <vector>
#include <memory>
#include <random>

// Simple neural network implementation for the AI
class AINetwork {
private:
    std::vector<std::vector<std::vector<float>>> weights;  // weights[layer][neuron][input]
    std::vector<std::vector<float>> biases;                // biases[layer][neuron]
    std::vector<int> layerSizes;
    std::mt19937 rng;
    
    // Activation functions
    float relu(float x);
    float reluDerivative(float x);
    
    // Helper methods
    std::vector<float> matrixMultiply(const std::vector<float>& input, 
                                     const std::vector<std::vector<float>>& weights, 
                                     const std::vector<float>& bias);
    
public:
    AINetwork(const std::vector<int>& layers);
    ~AINetwork() = default;
    
    // Forward pass
    std::vector<float> predict(const std::vector<float>& input);
    
    // Training
    void train(const std::vector<float>& input, const std::vector<float>& target, float learningRate);
    
    // Batch training
    void trainBatch(const std::vector<std::vector<float>>& inputs, 
                   const std::vector<std::vector<float>>& targets, 
                   float learningRate);
    
    // Save/load network
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    
    // Network info
    int getInputSize() const { return layerSizes.front(); }
    int getOutputSize() const { return layerSizes.back(); }
};

#endif
