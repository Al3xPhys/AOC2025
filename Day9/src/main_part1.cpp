#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std; // Use the standard namespace

pair<pair<uint64_t, uint64_t>, pair<uint64_t, uint64_t>> find_largest_rectangle(ifstream &file)
{
    /* Find largest rectangle that can be made. Find the coordinate with the smallest x and largest x regardless of y, then
        find the coordinate with smallest y and largest y regardless of x. Then calculate the area of the these two rectangles.
        Take the largest. */

    // load all coordinates into a vector
    string line;
    vector<pair<uint64_t, uint64_t>> coordinates;
    while (getline(file, line))
    { // Read the file line by line
        size_t comma_pos = line.find(',');
        if (comma_pos != string::npos)
        {
            uint64_t x = stoull(line.substr(0, comma_pos));
            uint64_t y = stoull(line.substr(comma_pos + 1));
            coordinates.push_back({x, y});
        }
    }

    pair<pair<uint64_t, uint64_t>, pair<uint64_t, uint64_t>> max_area_coords;
    uint64_t max_area = 0;
    const int n = coordinates.size();
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            uint64_t width = abs((int64_t)coordinates[i].first - (int64_t)coordinates[j].first) + 1;
            uint64_t height = abs((int64_t)coordinates[i].second - (int64_t)coordinates[j].second) + 1;
            uint64_t area = width * height;
            // cout << "Checking coordinates: (" << coordinates[i].first << "," << coordinates[i].second << ") and ("
            //      << coordinates[j].first << "," << coordinates[j].second << ") => Area: " << area << endl;

            if (area > max_area)
            {
                max_area = area;
                max_area_coords = {coordinates[i], coordinates[j]};
            }
        }
    }

    cout << "Max Area: " << max_area << endl;
    // cout << "Coordinates for Max Area: (" << max_area_coords.first.first << "," << max_area_coords.first.second << ") and ("
    //      << max_area_coords.second.first << "," << max_area_coords.second.second << ")" << endl;
    return max_area_coords;
}

int main()
{
    ifstream file("test.txt"); // Open a file named example.txt
    if (!file)
    {
        cerr << "Unable to open file input.txt";
        return 1; // Exit with error code if file cannot be opened
    }

    auto [furthest_coords1, furthest_coords2] = find_largest_rectangle(file); // Call the function to find the furthest point

    file.close(); // Close the file
}