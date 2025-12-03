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

vector<int> getFactors(long long const num)
{
    // Get all factors of num ignoring itself
    vector<int> factors;
    // cout << "Getting factors for number: " << num << "\n";
    for (int i = 1; i <= num - 1; ++i)
    {
        if (num % i == 0)
        {
            factors.push_back(i);
        }
    }
    return factors;
}

bool isRepeat(long long id){
    /* this will check if there are any repeats of digits that repeat at least twice
     i.2 12341234 has 1234 twice, 1212121212 has 12 five times etc. */
    long long temp = id;
    int len = 0;
    while (temp > 0)
    {
        len++;
        temp /= 10;
    } // count number of digits

    vector<int> factors = getFactors(len); // get all factors of number of digits
    for (int factor : factors)
    {
        int repeat_count = len / factor; // how many times the sequence would repeat
        long long divisor = 1; // to extract the repeating sequence
        for (int i = 0; i < factor; ++i)
        {
            divisor *= 10; // 10^factor
        }
        long long sequence = id % divisor; // get the last 'factor' digits as the sequence
        bool all_match = true;
        for (int j = 1; j < repeat_count; ++j) // check all repeats
        {
            long long next_sequence = (id / static_cast<long long>(pow(10, j * factor))) % divisor; // get the next sequence
            if (next_sequence != sequence) // if not match, break
            {
                all_match = false;
                break;
            }
        }
        if (all_match) // if all chunks match, it's a repeat
        {
            // cout << "ID " << id << " has repeating sequence " << sequence << " repeated " << repeat_count << " times.\n";
            return true;
        }

    }
    return false; // no repeating pattern found
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
        long long end_id = string_to_long_long(id_range.substr(dash_pos + 1));  // convert to long long.
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
    cout << "Total invalid ID sum: " << total_invalid_id_sum << "\n";
    cout << "Number of ranges processed: " << number_of_ranges << "\n";
    return 0;
}