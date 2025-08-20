#include "ainetwork.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>

AINetwork::AINetwork(const std::vector<int>& layers) 
    : layerSizes(layers), rng(std::random_device{}()) {
    
    // Initialize weights and biases
    weights.resize(layers.size() - 1);
    biases.resize(layers.size() - 1);
    
    std::normal_distribution<float> weightDist(0.0f, 0.1f);
    
    for (size_t i = 0; i < weights.size(); ++i) {
        int inputSize = layers[i];
        int outputSize = layers[i + 1];
        
        weights[i].resize(outputSize);
        biases[i].resize(outputSize);
        
        for (int j = 0; j < outputSize; ++j) {
            weights[i][j].resize(inputSize);
            biases[i][j] = weightDist(rng);
            
            for (int k = 0; k < inputSize; ++k) {
                weights[i][j][k] = weightDist(rng);
            }
        }
    }
}

float AINetwork::relu(float x) {
    return std::max(0.0f, x);
}

float AINetwork::reluDerivative(float x) {
    return x > 0 ? 1.0f : 0.0f;
}

std::vector<float> AINetwork::matrixMultiply(const std::vector<float>& input,
                                           const std::vector<std::vector<float>>& weights,
                                           const std::vector<float>& bias) {
    std::vector<float> output(weights.size());
    
    for (size_t i = 0; i < weights.size(); ++i) {
        output[i] = bias[i];
        for (size_t j = 0; j < input.size(); ++j) {
            output[i] += input[j] * weights[i][j];
        }
    }
    
    return output;
}

std::vector<float> AINetwork::predict(const std::vector<float>& input) {
    std::vector<float> currentLayer = input;
    
    // Forward pass through all layers
    for (size_t i = 0; i < weights.size(); ++i) {
        std::vector<float> nextLayer = matrixMultiply(currentLayer, weights[i], biases[i]);
        
        // Apply activation function (ReLU for hidden layers, linear for output)
        if (i < weights.size() - 1) {  // Hidden layers
            for (float& val : nextLayer) {
                val = relu(val);
            }
        }
        // Output layer uses linear activation for Q-values
        
        currentLayer = std::move(nextLayer);
    }
    
    return currentLayer;
}

void AINetwork::train(const std::vector<float>& input, const std::vector<float>& target, float learningRate) {
    // Simple backpropagation implementation
    std::vector<std::vector<float>> layerOutputs;
    layerOutputs.push_back(input);
    
    // Forward pass and store intermediate outputs
    std::vector<float> currentLayer = input;
    for (size_t i = 0; i < weights.size(); ++i) {
        std::vector<float> nextLayer = matrixMultiply(currentLayer, weights[i], biases[i]);
        
        if (i < weights.size() - 1) {  // Hidden layers
            for (float& val : nextLayer) {
                val = relu(val);
            }
        }
        
        layerOutputs.push_back(nextLayer);
        currentLayer = nextLayer;
    }
    
    // Backward pass
    std::vector<std::vector<float>> deltas(weights.size());
    
    // Output layer error
    deltas[weights.size() - 1].resize(target.size());
    for (size_t i = 0; i < target.size(); ++i) {
        deltas[weights.size() - 1][i] = target[i] - layerOutputs.back()[i];
    }
    
    // Hidden layers error (backpropagate)
    for (int layer = weights.size() - 2; layer >= 0; --layer) {
        deltas[layer].resize(layerSizes[layer + 1]);
        
        for (int i = 0; i < layerSizes[layer + 1]; ++i) {
            float error = 0.0f;
            for (int j = 0; j < layerSizes[layer + 2]; ++j) {
                error += deltas[layer + 1][j] * weights[layer + 1][j][i];
            }
            deltas[layer][i] = error * reluDerivative(layerOutputs[layer + 1][i]);
        }
    }
    
    // Update weights and biases
    for (size_t layer = 0; layer < weights.size(); ++layer) {
        for (size_t i = 0; i < weights[layer].size(); ++i) {
            // Update bias
            biases[layer][i] += learningRate * deltas[layer][i];
            
            // Update weights
            for (size_t j = 0; j < weights[layer][i].size(); ++j) {
                weights[layer][i][j] += learningRate * deltas[layer][i] * layerOutputs[layer][j];
            }
        }
    }
}

void AINetwork::trainBatch(const std::vector<std::vector<float>>& inputs,
                          const std::vector<std::vector<float>>& targets,
                          float learningRate) {
    for (size_t i = 0; i < inputs.size(); ++i) {
        train(inputs[i], targets[i], learningRate);
    }
}

void AINetwork::saveToFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not save network to " << filename << std::endl;
        return;
    }
    
    // Save layer sizes
    size_t numLayers = layerSizes.size();
    file.write(reinterpret_cast<const char*>(&numLayers), sizeof(numLayers));
    file.write(reinterpret_cast<const char*>(layerSizes.data()), numLayers * sizeof(int));
    
    // Save weights and biases
    for (size_t layer = 0; layer < weights.size(); ++layer) {
        for (size_t i = 0; i < weights[layer].size(); ++i) {
            size_t weightSize = weights[layer][i].size();
            file.write(reinterpret_cast<const char*>(&weightSize), sizeof(weightSize));
            file.write(reinterpret_cast<const char*>(weights[layer][i].data()), weightSize * sizeof(float));
        }
        
        size_t biasSize = biases[layer].size();
        file.write(reinterpret_cast<const char*>(&biasSize), sizeof(biasSize));
        file.write(reinterpret_cast<const char*>(biases[layer].data()), biasSize * sizeof(float));
    }
    
    file.close();
    std::cout << "Network saved to " << filename << std::endl;
}

void AINetwork::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Could not load network from " << filename << " (file may not exist)" << std::endl;
        return;
    }
    
    // Load layer sizes
    size_t numLayers;
    file.read(reinterpret_cast<char*>(&numLayers), sizeof(numLayers));
    
    layerSizes.resize(numLayers);
    file.read(reinterpret_cast<char*>(layerSizes.data()), numLayers * sizeof(int));
    
    // Reinitialize network structure
    weights.resize(layerSizes.size() - 1);
    biases.resize(layerSizes.size() - 1);
    
    // Load weights and biases
    for (size_t layer = 0; layer < weights.size(); ++layer) {
        weights[layer].resize(layerSizes[layer + 1]);
        
        for (size_t i = 0; i < weights[layer].size(); ++i) {
            size_t weightSize;
            file.read(reinterpret_cast<char*>(&weightSize), sizeof(weightSize));
            
            weights[layer][i].resize(weightSize);
            file.read(reinterpret_cast<char*>(weights[layer][i].data()), weightSize * sizeof(float));
        }
        
        size_t biasSize;
        file.read(reinterpret_cast<char*>(&biasSize), sizeof(biasSize));
        
        biases[layer].resize(biasSize);
        file.read(reinterpret_cast<char*>(biases[layer].data()), biasSize * sizeof(float));
    }
    
    file.close();
    std::cout << "Network loaded from " << filename << std::endl;
}
