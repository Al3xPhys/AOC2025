#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

/* read in input file "tacyon beam manifold diagram" then count how many beam splitters there are (^)
    to determine how many times a single beam gets split */

vector<vector<char>> parse_tacyon_diagram(ifstream &infile)
{
    vector<vector<char>> diagram;
    string line;
    line.reserve(200); // reserve space for 200 characters per line

    while (getline(infile, line))
    {
        vector<char> row(line.begin(), line.end());
        diagram.push_back(row);
    }
    return diagram;
}

int64_t count_beam_splits(const vector<vector<char>> &tacyon_diagram)
{
    // find the start position 'S' on first row of the diagram
    auto start_it = find(tacyon_diagram[0].begin(), tacyon_diagram[0].end(), 'S');
    int64_t beam_start_index = distance(tacyon_diagram[0].begin(), start_it);
    cout << "Start position of beam (S) is at index: " << beam_start_index << endl;

    vector<int64_t> beam_positions;             // to track current beam positions
    beam_positions.push_back(beam_start_index); // add current beam position to a vector

    // iterate through each row of the diagram starting from the second row
    int64_t total_beam_splits = 0; // track total number of beam splits
    for (size_t row = 1; row < tacyon_diagram.size(); ++row)
    {
        // find beam splitters '^' in the current row
        vector<int64_t> new_beam_positions; // to track new beam positions after splits
        for (const auto &pos : beam_positions)
        {
            if (tacyon_diagram[row][pos] == '^')
            {
                // beam splitter found, split the beam into two new beams (left and right)
                total_beam_splits++;
                if (pos > 0)
                {                                          // ensure not out of bounds to the left
                    new_beam_positions.push_back(pos - 1); // add left beam3
                }
                if (pos < static_cast<int64_t>(tacyon_diagram[row].size()) - 1)
                {                                          // ensure not out of bounds to the right
                    new_beam_positions.push_back(pos + 1); // add right beam
                }
            }
            else
            {
                // No splitter, beam continues straight down
                new_beam_positions.push_back(pos);
            }
        }
        // remove duplicate beam positions
        sort(new_beam_positions.begin(), new_beam_positions.end());
        new_beam_positions.erase(unique(new_beam_positions.begin(), new_beam_positions.end()), new_beam_positions.end());
        beam_positions = new_beam_positions; // update beam positions for next row
    }

    cout << "Total number of beams after traversing the diagram: " << beam_positions.size() << endl;

    return total_beam_splits;
}

int main()
{
    ifstream infile("input.txt");
    if (!infile.is_open())
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    vector<vector<char>> tacyon_diagram = parse_tacyon_diagram(infile); // parse the diagram into a 2D vector
    int64_t total_beam_splits = count_beam_splits(tacyon_diagram);      // count the beam splits
    cout << "Total number of beam splits encountered: " << total_beam_splits << endl;

    infile.close();
    return 0;
}