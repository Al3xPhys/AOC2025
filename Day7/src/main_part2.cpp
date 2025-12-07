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
    diagram.reserve(1000); // reserve space for rows
    string line;
    line.reserve(200); // reserve space for 200 characters per line

    while (getline(infile, line))
    {
        diagram.emplace_back(line.begin(), line.end()); // construct in-place
    }
    return diagram;
}

void count_beam_splits(const vector<vector<char>> &tacyon_diagram)
{
    // find the start position 'S' on first row of the diagram
    auto start_it = find(tacyon_diagram[0].begin(), tacyon_diagram[0].end(), 'S');
    int64_t beam_start_index = distance(tacyon_diagram[0].begin(), start_it);
    // cout << "Start position of beam (S) is at index: " << beam_start_index << endl;

    // map beams at each position (tracks all beams including duplicates via counts)
    unordered_map<int64_t, int64_t> beam_positions; // position -> count of beams
    unordered_map<int64_t, int64_t> new_beam_positions;
    beam_positions.reserve(tacyon_diagram[0].size()); // pre-allocate based on row width
    new_beam_positions.reserve(tacyon_diagram[0].size());
    beam_positions[beam_start_index] = 1;

    // iterate through each row of the diagram starting from the second row

    for (size_t row = 1; row < tacyon_diagram.size(); ++row)
    {
        new_beam_positions.clear();                    // reuse map instead of creating new one each iteration
        const auto &current_row = tacyon_diagram[row]; // cache row reference
        const int64_t row_size = static_cast<int64_t>(current_row.size());

        for (const auto &[pos, count] : beam_positions)
        {
            if (current_row[pos] == '^')
            {
                // beam splitter found, each of the 'count' beams at this position splits into two
                if (pos > 0)
                {
                    new_beam_positions[pos - 1] += count; // add 'count' left beams
                }
                if (pos < row_size - 1)
                {
                    new_beam_positions[pos + 1] += count; // add 'count' right beams
                }
            }
            else
            {
                // no splitter, beam continues straight down
                new_beam_positions[pos] += count;
            }
        }

        swap(beam_positions, new_beam_positions); // swap instead of move for better performance
    }

    // calculate total beams only once at the end
    int64_t total_beams = 0;
    for (const auto &[pos, count] : beam_positions)
    {
        total_beams += count;
    }
    cout << "Total number of beams after traversing the diagram: " << total_beams << endl;
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
    count_beam_splits(tacyon_diagram);      // count the beam splits

    infile.close();
    return 0;
}