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
        // Check if the completed word is in the dictionary
        if (dict.find(current) != dict.end()) {
            // Check that we used all floating characters
            if (remaining_floating.empty()) {
                results.insert(current);
            }
        }
        return;
    }
    
    // If the current position already has a fixed letter, move to the next position
    if (current[position] != '-') {
        wordle_helper(current, position + 1, remaining_floating, dict, results);
        return;
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
    
    // Case 2: Try using any of the alphabet letters if we've used all required floating letters
    // or if we still have positions to fill after using all floating letters
    if (remaining_floating.size() <= (current.size() - position - 1)) {
        // Try all lowercase letters
        for (char c = 'a'; c <= 'z'; c++) {
            // Skip this letter if it's in the remaining floating characters
            // as we don't want to accidentally use a floating character without
            // removing it from the floating list
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