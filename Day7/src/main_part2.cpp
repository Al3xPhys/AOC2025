#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unordered_map>

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
    // cout << "Start position of beam (S) is at index: " << beam_start_index << endl;

    // map beams at each position (tracks all beams including duplicates via counts)
    unordered_map<int64_t, int64_t> beam_positions; // position -> count of beams
    beam_positions[beam_start_index] = 1;

    // iterate through each row of the diagram starting from the second row
    int64_t total_beam_splits = 0; // track total number of beam splits
    int64_t total_beams = 1;
    
    for (size_t row = 1; row < tacyon_diagram.size(); ++row)
    {
        // find beam splitters '^' in the current row
        unordered_map<int64_t, int64_t> new_beam_positions; // to track new beam positions after splits
        
        for (const auto &[pos, count] : beam_positions)
        {
            if (tacyon_diagram[row][pos] == '^')
            {
                // beam splitter found, each of the 'count' beams at this position splits into two
                total_beam_splits += count;
                if (pos > 0)
                {                                          // ensure not out of bounds to the left
                    new_beam_positions[pos - 1] += count; // add 'count' left beams
                }
                if (pos < static_cast<int64_t>(tacyon_diagram[row].size()) - 1)
                {                                          // ensure not out of bounds to the right
                    new_beam_positions[pos + 1] += count; // add 'count' right beams
                }
            }
            else
            {
                // no splitter, beam continues straight down
                new_beam_positions[pos] += count;
            }
        }
        
        beam_positions = move(new_beam_positions); // update beam positions for next row
        
        // calculate total beams across all positions
        total_beams = 0;
        for (const auto &[pos, count] : beam_positions)
        {
            total_beams += count;
        }
    }

    cout << "Total number of beams after traversing the diagram: " << total_beams << endl;

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

    infile.close();
    return 0;
}