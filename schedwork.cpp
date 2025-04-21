#ifndef RECCHECK
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#endif

#include "schedwork.h"

using namespace std;

// Indicates an INVALID worker ID if that is useful to your implementation.
static const Worker_T INVALID_ID = (unsigned int)-1;

// Helper function prototypes
bool scheduleHelper(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched,
    vector<size_t>& workerShifts,
    size_t day,
    size_t slot);

bool isValid(
    const AvailabilityMatrix& avail,
    const size_t maxShifts,
    const DailySchedule& sched,
    const vector<size_t>& workerShifts,
    size_t day,
    size_t slot,
    Worker_T worker);

bool schedule(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched
)
{
    if(avail.size() == 0U){
        return false;
    }
    
    sched.clear();
    
    // No. of days
    size_t n = avail.size();
    
    // N0. of workers
    size_t k = avail[0].size();
    
    // Initialize schedule with INVALID_ID
    sched = DailySchedule(n, vector<Worker_T>(dailyNeed, INVALID_ID));
    
    // Keep track of how many shifts each worker has been assigned
    vector<size_t> workerShifts(k, 0);
    
    // Call helper function to recursively fill the schedule
    return scheduleHelper(avail, dailyNeed, maxShifts, sched, workerShifts, 0, 0);
}

/**
 * Helper function for recursive backtracking to fill the schedule
 * 
 * @param avail Availability matrix
 * @param dailyNeed Number of workers needed per day
 * @param maxShifts Maximum shifts any worker can take
 * @param sched Current schedule being built
 * @param workerShifts Current count of shifts for each worker
 * @param day Current day we're scheduling
 * @param slot Current slot in the day we're filling (0 to dailyNeed-1)
 * @return true if a valid schedule is found
 */
bool scheduleHelper(
    const AvailabilityMatrix& avail,
    const size_t dailyNeed,
    const size_t maxShifts,
    DailySchedule& sched,
    vector<size_t>& workerShifts,
    size_t day,
    size_t slot)
{
    size_t n = avail.size();
    size_t k = avail[0].size();
    
    // Base case: if we've filled all days, we're done
    if(day >= n) {
        return true;
    }
    
    // If we've filled all slots for the current day, move to next day
    if(slot >= dailyNeed) {
        return scheduleHelper(avail, dailyNeed, maxShifts, sched, workerShifts, day + 1, 0);
    }
    
    // Try assigning each worker to the current slot
    for(Worker_T worker = 0; worker < k; worker++) {
        // Check if this worker can be assigned to this slot
        if(isValid(avail, maxShifts, sched, workerShifts, day, slot, worker)) {
            // Assign the worker
            sched[day][slot] = worker;
            workerShifts[worker]++;
            
            // Recursively fill the rest of the schedule
            if(scheduleHelper(avail, dailyNeed, maxShifts, sched, workerShifts, day, slot + 1)) {
                return true;
            }
            
            // If we get here, this assignment didn't work out, so backtrack
            sched[day][slot] = INVALID_ID;
            workerShifts[worker]--;
        }
    }
    
    // If we've tried all workers and none worked, return false
    return false;
}

/**
 * Helper function to check if a worker can be assigned to a specific slot
 * 
 * @param avail Availability matrix
 * @param maxShifts Maximum shifts any worker can take
 * @param sched Current schedule being built
 * @param workerShifts Current count of shifts for each worker
 * @param day Current day we're scheduling
 * @param slot Current slot in the day we're filling
 * @param worker Worker ID we're considering
 * @return true if the worker can be assigned to this slot
 */
bool isValid(
    const AvailabilityMatrix& avail,
    const size_t maxShifts,
    const DailySchedule& sched,
    const vector<size_t>& workerShifts,
    size_t day,
    size_t slot,
    Worker_T worker)
{
    // Check if worker is available on this day
    if(!avail[day][worker]) {
        return false;
    }
    
    // Check if worker would exceed maximum shifts
    if(workerShifts[worker] >= maxShifts) {
        return false;
    }
    
    // Check if worker is already scheduled for this day
    for(size_t s = 0; s < slot; s++) {
        if(sched[day][s] == worker) {
            return false;
        }
    }
    
    // All checks passed
    return true;
}