#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ranges>

using namespace std;

/* ranges i.e 10-20, check if ids fall within any range, all in one text file sperated by a space.
    Check how many are fresh. If ids fall within range, they are fresh. If not, they are spoiled. 
    
    
    {3-5}, {10-14}, {16-20}, {12-18}
    {3-5}, {10-20} sorted by lower bound

    or 

    3,5,10,14,16,20,12,18
    3,5,10,12,14,16,18,20 sorted by lower bound

    3,6,3,3,3,3,3 = 18

    */

uint64_t check_fresh_in_range(vector<pair<uint64_t, uint64_t>> &ranges)
{
    // sort ranges by lower bound

    sort(ranges.begin(), ranges.end(), [](const pair<uint64_t, uint64_t> &a, const pair<uint64_t, uint64_t> &b) {
        return a.first < b.first;
    });

    uint64_t fresh_count = 0;

    // calcualte overlap of ranges
    uint64_t lower_bound_nm1 = ranges[0].first;
    uint64_t upper_bound_nm1 = ranges[0].second;
    for (pair<uint64_t, uint64_t> &range : ranges | views::drop(1)) //skips first element?
    {
        uint64_t lower_bound_n = range.first;
        uint64_t upper_bound_n = range.second;

        /* {a,b} {c,d}  if c > b, {b-a},*/
        if (lower_bound_n > upper_bound_nm1){
            fresh_count += upper_bound_nm1 - lower_bound_nm1 + 1;
            lower_bound_nm1 = lower_bound_n;
            upper_bound_nm1 = upper_bound_n;
        }
        else{
            upper_bound_nm1 = max(upper_bound_n, upper_bound_nm1);

        }

    }
    fresh_count += upper_bound_nm1 - lower_bound_nm1 + 1;
    return fresh_count;
}

int main()
{
    ifstream infile("input.txt");

    vector<pair<uint64_t, uint64_t>> ranges; // pairs of upper and lower bound ranges

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
                break;
            }
            else
            {
                auto lower_bound = stoull(line.substr(0, dash));
                auto upper_bound = stoull(line.substr(dash + 1));
                ranges.push_back({lower_bound, upper_bound}); // add range to vector
            }
        }
    }


    uint64_t fresh_count = check_fresh_in_range(ranges);
    cout << "Number of fresh ids: " << fresh_count << endl;

    infile.close();
    return 0;
}
