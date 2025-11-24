#include "LSystem.h"
#include <iostream>
#include <sstream>

LSystem::LSystem() : currentIterations_(0), rng_(std::random_device{}()), dist_(0.0f, 1.0f) {
    axiom_ = "F";
    currentString_ = axiom_;
}

LSystem::~LSystem() {}

void LSystem::setAxiom(const std::string& axiom) {
    axiom_ = axiom;
    currentString_ = axiom;
    currentIterations_ = 0;
}

void LSystem::addRule(char predecessor, const std::string& successor) {
    Rule rule;
    rule.predecessor = predecessor;
    rule.productions.clear();
    rule.productions.push_back({successor, 1.0f});
    rules_[predecessor] = rule;
}

void LSystem::addStochasticRule(char predecessor, const std::string& successor, float probability) {
    if (rules_.find(predecessor) == rules_.end()) {
        Rule rule;
        rule.predecessor = predecessor;
        rules_[predecessor] = rule;
    }
    rules_[predecessor].productions.push_back({successor, probability});
}

void LSystem::clearRules() {
    rules_.clear();
}

void LSystem::reset() {
    currentString_ = axiom_;
    currentIterations_ = 0;
}

std::string LSystem::generate(int iterations) {
    reset();
    for (int i = 0; i < iterations; ++i) {
        currentString_ = applyRules(currentString_);
        currentIterations_++;
    }
    return currentString_;
}

std::string LSystem::applyRules(const std::string& input) {
    std::stringstream output;
    
    for (char symbol : input) {
        char result = applyRule(symbol);
        
        // If there's a rule, apply it; otherwise keep the symbol
        if (rules_.find(symbol) != rules_.end()) {
            const Rule& rule = rules_[symbol];
            
            if (rule.productions.size() == 1) {
                // Deterministic rule
                output << rule.productions[0].first;
            } else {
                // Stochastic rule
                float rand = dist_(rng_);
                float cumulative = 0.0f;
                bool applied = false;
                
                for (const auto& prod : rule.productions) {
                    cumulative += prod.second;
                    if (rand <= cumulative) {
                        output << prod.first;
                        applied = true;
                        break;
                    }
                }
                
                if (!applied) {
                    output << rule.productions.back().first;
                }
            }
        } else {
            output << symbol;
        }
    }
    
    return output.str();
}

char LSystem::applyRule(char symbol) {
    return symbol; // Not used in current implementation
}

void LSystem::loadPreset(const std::string& presetName) {
    clearRules();
    
    if (presetName == "Simple Branch") {
        // Simple branching structure
        setAxiom("F");
        addRule('F', "F[+F]F[-F]F");
        
    } else if (presetName == "Fractal Tree") {
        // Classic fractal tree
        setAxiom("X");
        addRule('X', "F[+X][-X]FX");
        addRule('F', "FF");
        
    } else if (presetName == "Bush") {
        // Bushy plant
        setAxiom("F");
        addRule('F', "FF+[+F-F-F]-[-F+F+F]");
        
    } else if (presetName == "Fern") {
        // Fern-like structure
        setAxiom("X");
        addRule('X', "F[+X]F[-X]+X");
        addRule('F', "FF");
        
    } else if (presetName == "3D Tree") {
        // 3D tree structure
        setAxiom("A");
        addRule('A', "F[&+A]////[&+A]////[&+A]");
        addRule('F', "FF");
        
    } else if (presetName == "Stochastic Tree") {
        // Stochastic tree with randomness
        setAxiom("F");
        addStochasticRule('F', "F[+F]F[-F]F", 0.33f);
        addStochasticRule('F', "F[+F]F", 0.33f);
        addStochasticRule('F', "F[-F]F", 0.34f);
        
    } else if (presetName == "Complex 3D Plant") {
        // Complex 3D plant
        setAxiom("F");
        addRule('F', "F[&+F][&-F][^+F][^-F]");
        
    } else if (presetName == "Leaf Pattern") {
        // Pattern with leaves
        setAxiom("F");
        addRule('F', "F[+FL][-FL]F");
        addRule('L', "L");
        
    } else {
        // Default: simple tree
        setAxiom("F");
        addRule('F', "F[+F]F[-F]F");
    }
}

std::vector<std::string> LSystem::getAvailablePresets() const {
    return {
        "Simple Branch",
        "Fractal Tree",
        "Bush",
        "Fern",
        "3D Tree",
        "Stochastic Tree",
        "Complex 3D Plant",
        "Leaf Pattern"
    };
}
