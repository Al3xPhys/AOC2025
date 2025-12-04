#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

/* AOC Day 4.
    - Forklift can only access rolls of paper if there are fewer than four rolls of paper in the eight adjacent positions
    - Figure out which rolls of paper the forklifts can access.
    Eg. @ - roll of paper
         . - empty space

    ..@@.@@@@.
    @@@.@.@.@@
    @@@@@.@.@@
    @.@@@@..@.
    @@.@@@@.@@
    .@@@@@@@.@
    .@.@.@.@@@
    @.@@@.@@@@
    .@@@@@@@@.
    @.@.@@@.@.

    There are 13 rolls that can be access, marked with 'x'

    ..xx.xx@x.
    x@@.@.@.@@
    @@@@@.x.@@
    @.@@@@..@.
    x@.@@@@.@x
    .@@@@@@@.@
    .@.@.@.@@@
    x.@@@.@@@@
    .@@@@@@@@.
    x.x.@@@.x.              */

size_t count_neighbours(vector<vector<char>>& grid, vector<vector<char>>& temp_grid)
{
    size_t R = grid.size();
    size_t C = grid[0].size();

    // All 8 possible directions
    vector<pair<int, int>> valid_directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    size_t count = 0;
    // Check each direction
    for (size_t r = 0; r < R; r++)
    {
        for (size_t c = 0; c < C; c++)
        {
            if (grid[r][c] == 1)
            { // if theres a roll of paper
                int num_of_neighbours = 0;
                // iterate through all 8 directions
                for (auto &d : valid_directions)
                {
                    // Calculate the next position
                    size_t next_row = r + d.first;
                    size_t next_col = c + d.second;

                    // check through each direction and see if theres a roll of paper, if so increment num_of_neighbours
                    if (next_row >= 0 && next_row < R && next_col >= 0 && next_col < C)
                    {
                        if (grid[next_row][next_col] == 1)
                        {
                            num_of_neighbours++;
                            // cout << "Roll of paper found at (" << next_row << ", " << next_col << ")\n";
                        } // if theres a roll of paper in this direction
                    }
                }
                if (num_of_neighbours < 4)
                {
                    count++;
                    // remove this roll of paper from the grid to mark it as accessible
                    // temp_grid[r][c] = 0; // mark as accessible
                }
            }
        }
    }
    if (count == 0)
    {
        return -1; // no more accessible rolls
    }
    return count;
}

int main()
{
    ifstream infile("input.txt");
    vector<vector<char>> grid;
    vector<vector<char>> temp_grid;
    size_t total_accessible = 0;
    size_t accessible_rolls = 0;

    string line;
    while (getline(infile, line))
    {
        vector<char> row;

        for (char c : line)
        {
            row.push_back(c == '@'); // 1 for roll of paper, 0 for empty space
        }

        grid.push_back(row);
    } 
    infile.close();

    accessible_rolls = count_neighbours(grid, temp_grid);
    total_accessible = accessible_rolls;

    cout << "Total number of accessible rolls of paper: " << total_accessible << endl;


    return 0;
}