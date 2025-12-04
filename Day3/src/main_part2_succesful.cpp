#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <iostream>

using namespace std;
using namespace std::chrono;

vector<long long> greedy_alogorithm(const vector<long long> &number, int const combination_length)
{
    // copy input vector to manipulate
    vector<long long> stack;
    vector<long long> result = vector<long long>(combination_length, -1); // store result digits here, initialized to -1

    for (int i = 0; i < number.size(); ++i)
    {
        long long current_digit = number[i];
        // while stack is not empty, the last digit in stack is less than current digit and there are enough remaining digits to fill result
        while (!stack.empty() && stack.back() < current_digit && (stack.size() + (number.size() - i) - 1) >= combination_length)
        {
            stack.pop_back(); // remove last digit from stack
        }

        if (stack.size() < combination_length)
        {
            stack.push_back(current_digit); // add current digit to stack
        }
    }
    return stack;
}

int main()
{
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
        vector<long long> battery_bank; // store each battery bank line as vector of long long
        for (char c : line)
        {
            battery_bank.push_back(c - '0'); // convert char digit to long long and add to vector
        }
        vector<long long> result = greedy_alogorithm(battery_bank, 12);
        long long largest_value = 0;
        for (long long digit : result)
        {
            largest_value = largest_value * 10 + digit;
        }
        total_charge += largest_value; // sum up total charge from each bank
        // cout << "Largest value from battery bank: " << largest_value << endl;
    }
    auto time_end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(time_end - time_start).count();

    cout << "Total charge from all battery banks: " << total_charge << endl;
    cout << "Execution time: " << duration << " ms" << endl;
    file.close();
    return 0;
}