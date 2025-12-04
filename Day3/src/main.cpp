#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace std::chrono;

/* Each line of input corresponds to a bank of batteries. Each digit on the line represents the charge level of a battery.
    Within each bank (line) I need to turn on exactly two batteries which produce the highest charge (i.e find the two number combo which combine to the highest sum)
    You can not rearrange batteries.
    e.g 987654321111111 would mean 9 and 8 combined to make 98
        818181911112111 would be 9 and 2 to make 92, as the higher 8 is before the 9.
        Batteries can not be split or rearranged.
        The total charge is the sum of the two chosen batteries of each bank (line) so 98 + 92 = 190*/

long long return_largest_digit(vector<long long> number)
{
    vector<long long> digits = number;                                           // copy input vector to manipulate
    vector<long long>::iterator max = max_element(number.begin(), number.end()); // find the max element in the vector
    // cout << "Max digit in bank: " << *max << endl;
    // cout << "Found at index: " << distance(number.begin(), max) << endl;

    // if the largest digit is the last element, then find the second largest in the rest of the vector
    if (distance(number.begin(), max) == number.size() - 1)
    {
        // cout << "Max digit is last element, finding second largest in rest of bank." << endl;
        digits.pop_back(); // remove last element
        vector<long long>::iterator second_max = max_element(digits.begin(), digits.end());
        // cout << "Second max digit in bank: " << *second_max << endl;
        // cout << "Found at index: " << distance(digits.begin(), second_max) << endl;
        // combine the two largest digits but the opposite way round as the largest is last
        return *second_max * 10 + *max;
    }
    else
    {
        // cout << "Max digit is not last element, finding second largest in rest of bank." << endl;
        digits.erase(digits.begin(), digits.begin() + distance(number.begin(), max) + 1); // remove all elements up to and including the max element
        vector<long long>::iterator second_max = max_element(digits.begin(), digits.end());
        // cout << "Second max digit in bank: " << *second_max << endl;
        // cout << "Found at index: " << distance(digits.begin(), second_max) << endl;
        return *max * 10 + *second_max;
    }
}

int main()
{
    // load in the file
    ifstream file("batterybank.txt");
    if (!file)
    {
        cerr << "Unable to open file batterybanktest.txt";
        return 1; // Exit with error code
    }

    auto time_start = high_resolution_clock::now();

    string line;
    long long total_charge = 0;
    while (getline(file, line))
    {
        // line is battery bank as a string of digits, convert to vector of long long for processing
        vector<long long> battery_bank; // store each battery bank line as vector of long long
        for (char c : line)
        {
            battery_bank.push_back(c - '0'); // convert char digit to long long and add to vector
        }
        long long largest_value = return_largest_digit(battery_bank); // get largest digit from battery bank
        // cout << "Largest battery combination value is: " << largest_value << endl;
        total_charge += largest_value;
    }
    cout << "Total charge from all battery banks: " << total_charge << endl;

    auto time_end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(time_end - time_start).count();
    cout << "Time taken to find max digit: " << duration << " microseconds" << endl;

    return 0;
}