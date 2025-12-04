#include <iostream>
#include <string>
#include <fstream> // file input
#include <sstream> // stringstream for parsing
#include <chrono>
#include <cmath>

using namespace std;
using namespace std::chrono;

/*Code input takes a single long line of ID ranges, each ID is firstID-lastID, separated by commas.
Example input: 11-22, 223-665, 12453-1246341,... etc
Invalid IDs are IDs that have some sequence of digits that repeat twice.
    i.e 11-22 has invalid IDs '11' and '22'..
    6464 is invalid because of '64' repeating, but 6456 is valid.
    123123 is invalid because of '123' repeating, but 1234 is valid.
No IDs have leading zeros. (0101 does not exist in the input, 101 does and is valid)
Find all invalid IDs, i.e 11-22 would be two invalid IDs: 11 and 22, 95-115 would be one invalid ID: 99.
Output the sum of all invalid IDs.i.e 11+22+99 = 132 */

long long stringToLongLong(const string &num_str)
{
    long long result = 0;
    for (char c : num_str)
    {
        result = result * 10 + (c - '0');
    }
    return result;
}

// precomputed powers of 10 to remove need for pow()
static const long long powersOf10[] = {
    1LL, 
    10LL, 
    100LL, 
    1000LL, 
    10000LL, 
    100000LL, 
    1000000LL, 
    10000000LL, 
    100000000LL, 
    1000000000LL, 
    10000000000LL, 
    100000000000LL, 
    1000000000000LL,
    10000000000000LL,
    100000000000000LL,
    1000000000000000LL,
    10000000000000000LL,
    100000000000000000LL,
    1000000000000000000LL
};

inline int countDigits(long long const id) {
    /* hardcoded digit count for efficiency */
    if (id < 10) return 1;
    if (id < 100) return 2;
    if (id < 1000) return 3;
    if (id < 10000) return 4;
    if (id < 100000) return 5;
    if (id < 1000000) return 6;
    if (id < 10000000) return 7;
    if (id < 100000000) return 8;
    if (id < 1000000000) return 9;
    if (id < 10000000000LL) return 10;
    if (id < 100000000000LL) return 11;
    if (id < 1000000000000LL) return 12;
    if (id < 10000000000000LL) return 13;
    if (id < 100000000000000LL) return 14;
    if (id < 1000000000000000LL) return 15;
    if (id < 10000000000000000LL) return 16;
    if (id < 100000000000000000LL) return 17;
    if (id < 1000000000000000000LL) return 18;
    return 19; // max for signed long long
}

bool isRepeat(long long id){
    int len = countDigits(id);
    
    /* Hardcode in checks to check for repeated sequences of common digit factors, 2,3,4 and 5 */
    if (len % 2 == 0) {
        int half = len / 2;
        long long divisor = powersOf10[half];
        if (id / divisor == id % divisor) {
            return true;
        }
    }
    
    if (len % 3 == 0) {
        int third = len / 3;
        long long divisor = powersOf10[third];
        long long first = id / powersOf10[2 * third];
        long long second = (id / divisor) % divisor;
        long long third_part = id % divisor;
        if (first == second && second == third_part) {
            return true;
        }
    }
    
    // for higher factors, do a for loop to calculate chunks and compare
    for (int factor = 4; factor <= len / 2; ++factor) {
        if (len % factor != 0) continue;
        int repeat_count = len / factor;
        
        long long divisor = powersOf10[factor];
        long long sequence = id % divisor;
        bool all_match = true;
        
        long long temp = id / divisor;
        for (int j = 1; j < repeat_count; ++j) {
            if (temp % divisor != sequence) {
                all_match = false;
                break;
            }
            temp /= divisor;
        }
        if (all_match) return true;
    }
    
    return false;
}

int main()
{

    // read in file and store line as string
    ifstream file("idranges.txt");
    if (!file)
    {
        cerr << "cannot open file\n";
        return 1;
    }
    string line;
    getline(file, line);
    file.close();
    auto start_time = high_resolution_clock::now();
    // parse the line to extract id ranges using delimter ','
    stringstream ss(line);
    string id_range;
    long number_of_ranges = 0;
    long long total_invalid_id_sum = 0;
    while (getline(ss, id_range, ','))
    {
        number_of_ranges++;
        // id_range = "11-22"
        // split id range into start  and end and store in start_str and end_str
        // convert start_str and end_str to long long integers
        size_t dash_pos = id_range.find('-');
        if (dash_pos == string::npos)
        {
            continue; // invalid range, skip
        }
        long long start_id = stringToLongLong(id_range.substr(0, dash_pos)); // convert to long long
        long long end_id = stringToLongLong(id_range.substr(dash_pos + 1));  // convert to long long.
        for (long long id = start_id; id <= end_id; ++id)
        {
            bool repeats = isRepeat(id);
            // cout << "Checked ID: " << id << ", is repeat: " << (repeats ? "Yes" : "No") << "\n";
            if (repeats)
            {
                total_invalid_id_sum += id;
            }
        }

    }
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time).count();
    cout << "Total invalid ID sum: " << total_invalid_id_sum << "\n";
    cout << "Number of ranges processed: " << number_of_ranges << "\n";
    cout << "Duration: " << duration << " milliseconds\n";
    return 0;
}