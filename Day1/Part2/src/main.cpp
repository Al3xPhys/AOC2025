/* This code reads in a list of turns on a safe dial from 0 to 99, starting at 0 i.e L5 would be 95 then R10 would be 5.
The code will count the number of times that the dial lands on zero as well as times it passes 0 */

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Pass by reference to modify dial_position directly, return bool for zero check
bool calculate_dial_position(int &current_position, int &full_rotations, char direction, int amount)
{
    if (direction == 'L')
    {
        /*determine the number of full rotations (number of times passing zero) and also return current position after*/
        if (amount % 100 >= current_position && current_position != 0)
        {
            full_rotations++; // we pass zero going left
        }
        full_rotations += abs(amount / 100); // add full rotations from amount, this is how many times we pass zero
        // wrap around using modulo to stay within 0-99, this prevents undercounting when starting on 0 or moving amount less than startin position
        current_position = (current_position - amount % 100 + 100) % 100; 
    }
    else
    {
        /* Calculate postion turning right but make sure that if value goes above 99, to wrap around to 0 and continue */
        full_rotations += (current_position + amount) / 100;
        current_position = (current_position + amount) % 100;
    }
    cout << "Current Position: " << current_position << " Full Rotations: " << full_rotations << " Operation: " << direction << amount << endl;
    return current_position == 0; // return whether we're at zero
}

int main()
{
    auto start_time = high_resolution_clock::now();
    int dial_position = 50; // starting position
    int full_rotations = 0; // number of times passing zero
    int previous_position = 0;
    int zero_count = 0;
    ifstream infile("rotations.txt");

    if (!infile)
    {
        cerr << "Unable to open file rotation_test.txt";
        return 1; // Exit with error code
    }

    // cout << "Starting at position: " << dial_position << endl;
    // Read each line from the file
    string line;
    while (getline(infile, line))
    {
        // Always get the direction using the first character of the line
        char const direction = line[0]; // left or right (L or R)
        // check if direction is valid
        if (direction != 'L' && direction != 'R')
        {
            continue;
        }

        int const amount = stoi(line.substr(1)); // stoi to convert the rest of the line to an integer
        // Call function which modifies dial_position and returns if we hit zero
        if (calculate_dial_position(dial_position, full_rotations, direction, amount))
        {
            // if move lands on zero but the counter started at zero before moving, do not increment
            zero_count++;
        }
    };
    infile.close();
    auto end_time = high_resolution_clock::now();
    duration<double, std::milli> duration = end_time - start_time;
    cout << "Times dial landed on zero: " << zero_count << endl;
    cout << "Times dial passed zero: " << full_rotations << endl;
    cout << "Total (landed + passed) zero: " << full_rotations + zero_count << endl;
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    return 0;
};