#ifndef RECCHECK
// For debugging
#include <iostream>
// For std::remove
#include <algorithm> 
#include <map>
#include <set>
#endif

#include "wordle.h"
#include "dict-eng.h"
using namespace std;

// Helper function prototype
void wordle_helper(
    string& current,
    int position,
    string remaining_floating,
    const set<string>& dict,
    set<string>& results);

// Definition of primary wordle function
std::set<std::string> wordle(
    const std::string& in,
    const std::string& floating,
    const std::set<std::string>& dict)
{
    set<string> results;
    string current = in;
    
    // Early pruning: Calculate minimum word length needed
    int dash_count = 0;
    for(char c : current) {
        if(c == '-') dash_count++;
    }
    
    // If we have more floating letters than spaces, it's impossible
    if(floating.size() > dash_count) {
        return results; // Return empty set
    }
    
    // Use recursive helper to find all possible combinations
    wordle_helper(current, 0, floating, dict, results);
    
    return results;
}

// Recursive helper function that generates all possible words matching the criteria
void wordle_helper(
    string& current,
    int position,
    string remaining_floating,
    const set<string>& dict,
    set<string>& results)
{
    // Base case: we've reached the end of the string
    if (position == current.size()) {
        // Check if the completed word is in the dictionary and we used all floating chars
        if (remaining_floating.empty() && dict.find(current) != dict.end()) {
            results.insert(current);
        }
        return;
    }
    
    // Key optimization: Check if we still have enough positions to place all remaining floating chars
    int remaining_positions = current.size() - position;
    int remaining_dashes = 0;
    for(size_t i = position; i < current.size(); i++) {
        if(current[i] == '-') remaining_dashes++;
    }
    
    if(remaining_dashes < remaining_floating.size()) {
        return; // Not enough spaces for remaining floating chars
    }
    
    // If the current position already has a fixed letter, move to the next position
    if (current[position] != '-') {
        wordle_helper(current, position + 1, remaining_floating, dict, results);
        return;
    }
    
    // Early pruning: Check if the current prefix could lead to a valid word
    string prefix = current.substr(0, position);
    bool prefix_valid = false;
    
    // Only do expensive prefix checking for positions > 0
    if (position > 0) {
        // Check if any word in the dictionary starts with this prefix
        for (const auto& word : dict) {
            if (word.size() == current.size() && word.substr(0, position) == prefix) {
                prefix_valid = true;
                break;
            }
        }
        
        if (!prefix_valid) {
            return; // No valid words start with this prefix
        }
    }
    
    // Case 1: Try using one of the floating characters at this position
    for (size_t i = 0; i < remaining_floating.size(); i++) {
        // Place the floating character at the current position
        current[position] = remaining_floating[i];
        
        // Remove this character from remaining_floating
        string new_remaining = remaining_floating.substr(0, i) + 
                               remaining_floating.substr(i + 1);
        
        // Recursive call with updated state
        wordle_helper(current, position + 1, new_remaining, dict, results);
    }
    
    // Case 2: Try using any alphabet letter (if we don't need all remaining positions for floating letters)
    if (remaining_floating.size() < remaining_dashes) {
        for (char c = 'a'; c <= 'z'; c++) {
            // Skip this letter if it's in the remaining floating characters
            bool in_floating = false;
            for (char f : remaining_floating) {
                if (c == f) {
                    in_floating = true;
                    break;
                }
            }
            
            if (!in_floating) {
                // Place this letter at the current position
                current[position] = c;
                
                // Recursive call with the same remaining floating letters
                wordle_helper(current, position + 1, remaining_floating, dict, results);
            }
        }
    }
    
    // Restore the current position back to a dash for backtracking
    current[position] = '-';
}