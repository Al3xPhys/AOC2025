#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <deque>
#include <unordered_set>

using namespace std; // Use the standard namespace

/* The input text file contains 2d coordinates in the format "x,y" on each line. These represent red tiles (#).
    Every red tile is connected to the red tile before and after by a straight line of green tiles (X) it in the list.
    The list wraps so the first and last tiles are also connected.
    All times inside the polygon formed by the red and green tiles are also green.
    The outside tiles are empty and are never red or green (.)

    The goal is to find the largest axis-aligned rectangle that can be formed such that all tiles inside the rectangle are either red or green.
    The rectangle's corners must be on red tiles, but any other tiles must be red or green. */

vector<pair<int64_t, int64_t>> parse_coordinates_into_vector(ifstream &file)
{
    /* Parse coordinates from the input file into a vector of pairs. Each pair represents (x, y) coordinates. */
    string line;
    vector<pair<int64_t, int64_t>> coordinates;
    while (getline(file, line))
    { // Read the file line by line
        size_t comma_pos = line.find(',');
        if (comma_pos != string::npos)
        {
            int64_t x = stoll(line.substr(0, comma_pos));
            int64_t y = stoll(line.substr(comma_pos + 1));
            coordinates.push_back({x, y});
        }
    }
    return coordinates;
}

bool is_point_in_polygon(int64_t x, int64_t y, const vector<pair<int64_t, int64_t>> &polygon)
{
    /* We will use ray casting to determine if a point (i.e the corners) are within the polygon
        Cast a ray at any point outside the polygon to the point. 
            If number of boundaries crossed is odd, the point is inside the polygon
            if even, the point is outside the polygon */

    
}

int main()
{
    ifstream file("test.txt"); // Open a file named example.txt
    if (!file)
    {
        cerr << "Unable to open file input.txt";
        return 1; // Exit with error code if file cannot be opened
    }

    vector<pair<int64_t, int64_t>> coordinates = parse_coordinates_into_vector(file); // Parse coordinates from the file into a vector

    /* Iterate through all coordinates. For each coordinate, go through all other coordiantes and draw a rectangle. Check if the rectangle is valid by:
        - Checking all four corners are within the polygon
        - Checking no edges drawn between the corners passing outside the polygon

        If the rectanlge is valid, calculate it area and if its larger than the current maximum, update the maximum */

    int n = coordinates.size();
    int64_t max_area = 0; // Current maximum area found
    pair<pair<int64_t, int64_t>, pair<int64_t, int64_t>> max_coords; // Coordinates of the rectangle with the maximum area
    bool corners_within_polygon = true;
    bool edges_dont_exit_polygon = true; // Flags to track validity of rectangle
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            // First corner
            int64_t x1 = coordinates[i].first;
            int64_t y1 = coordinates[i].second;
            // Opposite corner
            int64_t x2 = coordinates[j].first;
            int64_t y2 = coordinates[j].second;

            // Check all four corners are within the polygon

        
        }
    }


    file.close(); // Close the file
}