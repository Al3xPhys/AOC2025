#include <iostream>
#include <string>
#include <fstream> // file input
#include <sstream> // stringstream for parsing
#include <regex>   // regex for parsing
#include <chrono>

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

void convert_range_str_to_start_end_strings(const string &range_str, string &start_str, string &end_str)
{
    stringstream id_range_ss(range_str);
    getline(id_range_ss, start_str, '-'); // get start ID range and store in reference
    getline(id_range_ss, end_str, '-');   // get end ID range and store in reference
}

long long string_to_long_long(const string &num_str)
{
    long long result = 0;
    for (char c : num_str)
    {
        result = result * 10 + (c - '0');
    }
    return result;
}

bool isDoubleRepeat(long long id)
{
    long long temp = id;
    int len = 0;
    while (temp > 0)
    {
        len++;
        temp /= 10;
    } // count number of digits

    if (len % 2 != 0)
    {
        return false; // odd length cannot have double repeat
    }
    int half = len / 2;
    long long divisor = 1;
    for (int i = 0; i < half; ++i)
    {
        divisor *= 10;
    }
    long long first_half = id / divisor;
    long long second_half = id % divisor;
    return first_half == second_half;
}

vector<long long> findRepeats(long long start, long long end)
{
    vector<long long> invalid_ids;
    for (long long id = start; id <= end; ++id)
    {
        if (isDoubleRepeat(id))
        {
            invalid_ids.push_back(id);
        }
    }
    return invalid_ids;
}

long long sumInvalidIDsInRange(long long start, long long end)
{
    long long sum = 0;
    for (long long id = start; id <= end; ++id)
    {
        if (isDoubleRepeat(id))
        {
            sum += id;
        }
    }
    return sum;
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
        long long start_id = string_to_long_long(id_range.substr(0, dash_pos)); // convert to long long
        long long end_id = string_to_long_long(id_range.substr(dash_pos + 1));  // convert to long long
        vector<long long> repeats = findRepeats(start_id, end_id);
        // cout << "Processing ID range: " << id_range << " Start ID: " << start_id << " End ID: " << end_id << "\n";
        for (long long invalid_id : repeats)
        {
            // cout << "  Found invalid ID: " << invalid_id << "\n";
            total_invalid_id_sum += sumInvalidIDsInRange(start_id, end_id);
        }
    }
    auto end_time = high_resolution_clock::now();
    duration<double, std::milli> duration = end_time - start_time;
    cout << "sum of invalid ids: " << total_invalid_id_sum << "\n";
    cout << "Number of ranges processed: " << number_of_ranges << "\n";
    cout << "Execution time (ms): " << duration.count() << "\n";

    return 0;
}