#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <string>
#include <map>
#include <vector>
#include <random>

// Structure to represent a production rule
struct Rule {
    char predecessor;
    std::vector<std::pair<std::string, float>> productions; // (successor, probability)
    
    Rule() = default;
    Rule(char pred, const std::string& succ) : predecessor(pred) {
        productions.push_back({succ, 1.0f});
    }
};

// L-System class for procedural plant generation
class LSystem {
public:
    LSystem();
    ~LSystem();
    
    // Setup methods
    void setAxiom(const std::string& axiom);
    void addRule(char predecessor, const std::string& successor);
    void addStochasticRule(char predecessor, const std::string& successor, float probability);
    void clearRules();
    
    // Generation
    std::string generate(int iterations);
    void reset();
    
    // Getters
    std::string getAxiom() const { return axiom_; }
    std::string getCurrentString() const { return currentString_; }
    int getIterations() const { return currentIterations_; }
    
    // Predefined plant presets
    void loadPreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
private:
    std::string axiom_;
    std::string currentString_;
    std::map<char, Rule> rules_;
    int currentIterations_;
    
    std::mt19937 rng_;
    std::uniform_real_distribution<float> dist_;
    
    std::string applyRules(const std::string& input);
    char applyRule(char symbol);
};

#endif // LSYSTEM_H
