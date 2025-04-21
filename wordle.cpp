#ifndef RECCHECK
#include <iostream>
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
    
    // Calculate minimum word length
    int dash_count = 0;
    for(char c : current) {
        if(c == '-') dash_count++;
    }
    
    // Impossible if more floating letters than spaces
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
    // Base case
    if (position == current.size()) {
        // Check if the completed word is in the dictionary and we used all floating chars
        if (remaining_floating.empty() && dict.find(current) != dict.end()) {
            results.insert(current);
        }
        return;
    }
    
    // Count remaining dashes from current position to end
    int remaining_dashes = 0;
    for(size_t i = position; i < current.size(); i++) {
        if(current[i] == '-') remaining_dashes++;
    }
    
    // Early termination: Not enough spaces for remaining floating chars
    if(remaining_dashes < remaining_floating.size()) {
        return;
    }
    
    // If the current position already has a fixed letter, move to the next position
    if (current[position] != '-') {
        wordle_helper(current, position + 1, remaining_floating, dict, results);
        return;
    }
    
    // Case 1: Try using one of the floating characters at this position
    for (size_t i = 0; i < remaining_floating.size(); i++) {
        current[position] = remaining_floating[i];
        
        // Create new remaining floating string by removing used character
        string new_remaining = remaining_floating;
        new_remaining.erase(i, 1);
        
        wordle_helper(current, position + 1, new_remaining, dict, results);
    }
    
    // Case 2: Try other alphabet letters if we don't need all spots for floating letters
    if (remaining_floating.size() < remaining_dashes) {
        for (char c = 'a'; c <= 'z'; c++) {
            // Check if this letter is in remaining_floating
            bool skip = false;
            for (char f : remaining_floating) {
                if (c == f) {
                    skip = true;
                    break;
                }
            }
            
            if (!skip) {
                current[position] = c;
                wordle_helper(current, position + 1, remaining_floating, dict, results);
            }
        }
    }
    
    // Restore current position for backtracking
    current[position] = '-';
}