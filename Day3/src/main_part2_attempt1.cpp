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

size_t countRemainingDigits(const vector<long long> &number, int combination_length)
{

    // find first real digit (not -1)
    auto first_real = find_if(number.begin(), number.end(), [](long long digit)
                              { return digit != -1; }); // find first occurrence of not -1

    // if no real digits found, all digits are -1
    if (first_real == number.end())
    {
        cout << "All digits are -1." << endl;
        return number.size(); // all digits filled
    }

    // leading -1s before first real digit
    if (first_real != number.begin())
    {
        cout << "Leading -1s found from index: 0 to " << distance(number.begin(), first_real) - 1 << endl;
        return static_cast<size_t>(distance(number.begin(), first_real));
    }

    // find second real digit
    auto second_real = find_if(first_real + 1, number.end(), [](long long digit)
                               { return digit == -1; }); // find first occurrence of -1 after first real digit

    // if no -1 found after first real digit, all remaining digits are real
    if (second_real == number.end())
    {
        cout << "All remaining digits are real." << endl;
        cout << "Counting -1s from index: " << distance(number.begin(), first_real + 1) << " to end." << endl;
        return count(first_real + 1, number.end(), -1);
    }

    // count number of -1s from second real digit to end
    cout << "Counting -1s from index: " << distance(number.begin(), second_real) << " to end." << endl;
    return count(first_real, second_real, -1);
}

vector<long long> return_largest_digit_combination(vector<long long> number, int const combination_length = 12)
{
    vector<long long> digits = number;                                    // copy input vector to manipulate
    vector<long long> result = vector<long long>(combination_length, -1); // store result digits here, initialized to -1

    const int shiftedCombinationLength = combination_length - 1; // n-1 for calculation purposes
    size_t offset_in_original = 0;                               // track how many elements we've removed from the beginning of digits

    /* if the distance to the end is shorter than the combination length, take all digits from max element onwards but put them at positions (combination_length-1)-distance_to_end in the result.
        ie. if combination length = 4, then (4-1)-distance_to_end. If say the largest digit was found 2 from the end, then (4-1)-2 = 1. You put the max element onwards into spaces 1 from the end of result onwards.
        so given 82561291, the max element being 9, 1 distance from the end. So for a 4 digit combiation, it would be _ _ 9 1. The repeat the search from start of the line again. */
    size_t remaining_digits_from_left = countRemainingDigits(result, combination_length); // count how many digits are left to fill in result
    while (remaining_digits_from_left > 0)
    {
        vector<long long>::iterator max = max_element(digits.begin(), digits.end()); // find the max element in the vector

        // first check if the max element onwards is already exactly the number of digits we need to fill
        if (static_cast<size_t>(distance(max, digits.end())) == remaining_digits_from_left)
        {
            return vector<long long>(max, digits.end()); // can just take all digits from the max element onwards as they can be no higher combination
        }

        // if the distance to the end is longer than the combination length, then take the max element found, put it in the first available position in result. Erase all elements from start of digits up to and including max element and repeat
        if (static_cast<size_t>(distance(max, digits.end()) + 1) > remaining_digits_from_left)
        {
            cout << "distance to end greater than remaining digits to fill." << endl;
            cout << "Max digit in bank: " << *max << endl;
            auto result_pos = find(result.begin(), result.end(), -1); // find first available position in result
            // auto result_pos = result.begin() + (combination_length - remaining_digits_from_left); // calculate first available position in result based on remaining digits from the left to fill
            *result_pos = *max; // insert max element into first available position
            auto elements_to_erase = distance(digits.begin(), max) + 1;
            digits.erase(digits.begin(), digits.begin() + elements_to_erase); // remove all elements up to and including the max element from digits vector to repeat search
            offset_in_original += elements_to_erase;                          // update offset to track position in original vector
            remaining_digits_from_left--;                                     // decrement remaining digits count
        }

        // if the distance to the end is shorter than the combination length, we need to insert into results at calculated position from the end and then remove those elements from digits to repeat search
        else if (static_cast<size_t>(distance(max, digits.end()) + 1) <= remaining_digits_from_left)
        {
            cout << "distance to end less than remaining digits to fill." << endl;
            cout << "Max digit in bank: " << *max << endl;
            auto max_index_in_digits = distance(digits.begin(), max);                                                 // get index of max element in digits vector
            auto max_index_in_original = offset_in_original + max_index_in_digits;                                    // calculate index in original number vector
            size_t start_index = combination_length - distance(number.begin() + max_index_in_original, number.end()); // calculate where to start inserting digits into result vector using formula (combination_length-1)-distance_to_end using the original number vector at the same index position
            copy(max, digits.end(), result.begin() + start_index);                                                    // copy digits from max element to end into result vector starting at calculated start index
            digits.erase(digits.begin() + max_index_in_digits, digits.end());                                         // remove all elements from max to end from digits vector, to repeat search in remaining digits
            remaining_digits_from_left = countRemainingDigits(result, combination_length);                            // recount remaining digits to fill in result
        }

        cout << "Current result: ";
        for (long long digit : result)
        {
            cout << digit;
        }
        cout << "remaining digits to fill: " << remaining_digits_from_left << endl;
    }
    return result;
}

int main()
{
    // load in the file
    ifstream file("batterybanktest.txt");
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
        vector<long long> largest_combination = return_largest_digit_combination(battery_bank); // get largest two digit combination from battery bank
        // turn vector of digits into a single long long number
        long long largest_value = 0;
        for (long long digit : largest_combination)
        {
            largest_value = largest_value * 10 + digit;
        }
        cout << "Largest battery combination value is: " << largest_value << endl;
        total_charge += largest_value;
    }
    cout << "Total charge from all battery banks: " << total_charge << endl;

    auto time_end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(time_end - time_start).count();
    cout << "Time taken to find max digit: " << duration << " microseconds" << endl;

    return 0;
}