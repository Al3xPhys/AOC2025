/* This code reads in a list of turns on a safe dial from 0 to 99, starting at 0 i.e L5 would be 95 then R10 would be 5. 
The code will count the number of times that the dial lands on zero */

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Pass by reference to modify dial_position directly, return bool for zero check
bool calculate_dial_position(int& current_position, char direction, int amount) {
    if (direction == 'L') {
        /* Calculate postion turning left but make sure that if value goes below 0, to wrap around to 99 and continue */
        current_position = (current_position - amount % 100 + 100) % 100;
    }
    else {
        /* Calculate postion turning right but make sure that if value goes above 99, to wrap around to 0 and continue */
        current_position = (current_position + amount) % 100;
    }
    return current_position == 0; // return whether we're at zero
}

int main() {
    auto start_time = high_resolution_clock::now();
    int dial_position = 50; // starting position
    int previous_position = 0;
    int zero_count = 0;
    ifstream infile("rotations.txt");

    if (!infile) {
        cerr << "Unable to open file rotations.txt";
        return 1; // Exit with error code
    }

    // cout << "Starting at position: " << dial_position << endl;
    // Read each line from the file
    string line;
    while (getline(infile, line)) {
        // Always get the direction using the first character of the line
        char const direction = line[0]; // left or right (L or R)
        // check if direction is valid
        if (direction != 'L' && direction != 'R') {
            continue;
        }
        
        int const amount = stoi(line.substr(1)); // stoi to convert the rest of the line to an integer
        // Call function which modifies dial_position and returns if we hit zero
        if (calculate_dial_position(dial_position, direction, amount)) {
            // if move lands on zero but the counter started at zero before moving, do not increment
            zero_count++;
        }
    };
    infile.close();
    auto end_time = high_resolution_clock::now();
    duration<double, std::milli> duration = end_time - start_time;
    cout << "Total times dial landed on zero: " << zero_count << endl;  
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    return 0;
};