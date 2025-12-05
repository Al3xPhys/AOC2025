#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/* ranges i.e 10-20, check if ids fall within any range, all in one text file sperated by a space.
    Check how many are fresh. If ids fall within range, they are fresh. If not, they are spoiled. */

int check_fresh(vector<pair<unsigned long long, unsigned long long>> &ranges, vector<unsigned long long> &ids)
{
    int fresh_count = 0;

    for (unsigned long long &id : ids)
    {
        for (pair<unsigned long long, unsigned long long> &range : ranges)
        {
            unsigned long long lower_bound = range.first;
            unsigned long long upper_bound = range.second;

            if (id >= lower_bound && id <= upper_bound)
            {
                fresh_count++;
                break;
            }
        }
    }
    return fresh_count;
}

int main()
{
    ifstream infile("input.txt");

    vector<pair<unsigned long long, unsigned long long>> ranges; // pairs of upper and lower bound ranges
    vector<unsigned long long> ids;                              // ids to be checked

    string line;
    bool reading_ranges = true;
    while (getline(infile, line))
    {
        size_t dash = line.find('-');
        if (reading_ranges)
        {
            if (dash == string::npos)
            {
                reading_ranges = false;
                cout << "Ranges ended. Now reading tickets." << endl;
            }
            else
            {
                auto lower_bound = stoull(line.substr(0, dash));
                auto upper_bound = stoull(line.substr(dash + 1));
                ranges.push_back({lower_bound, upper_bound}); // add range to vector
            }
        }
        else
        {
            ids.push_back(stoull(line)); // add id to vector
        }
    }

    int fresh_count = check_fresh(ranges, ids);
    cout << "Number of fresh tickets: " << fresh_count << endl;

    infile.close();
    return 0;
}
