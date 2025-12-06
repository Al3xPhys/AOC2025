#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

/* AOC Day 6 Part 1:
 */

// FUNCTIONS TO READ AND DISPLAY DATA FROM INPUT FILE //

vector<char> readOperations(ifstream &infile, int ops_row = 5)
{
    string line;
    getline(infile, line);

    for (int i = 0; i < ops_row; i++)
    {
        getline(infile, line);
    }
    vector<char> operation;
    for (char ch : line)
    {
        if (ch != ' ')
        {
            operation.push_back(ch);
        }
    }

    return operation;
}

// FUNCTION TO PROCESS DATA AND OBTAIN SOLUTION //

uint64_t solve_grid(vector<vector<vector<int64_t>>> &blockVectors, const vector<char> &operations)
{
    /* Read each block, read each column top to bottom (ignoring -1) to make a number.
    Then perform the corresponding operation on these new numbers.*/

    uint64_t total_result = 0;
    // for each block
    for (int blockIdx = 0; blockIdx < blockVectors.size(); blockIdx++)
    {
        uint64_t block_result = 0;
        // cout << "Processing Block " << blockIdx << ":\n";
        // for each column in block
        for (int col = 0; col < blockVectors[blockIdx][0].size(); col++)
        {
            // read down column to form number
            int64_t number = 0;
            for (int row = 0; row < blockVectors[blockIdx].size(); row++)
            {
                int64_t val = blockVectors[blockIdx][row][col];
                if (val != -1)
                {
                    number = number * 10 + val; // append digit
                }
            }
            if (number != 0)
            {
                // cout << " Formed number from column " << col << ": " << number << "\n";
                // after forming number, perform operation if applicable
                char op = operations[blockIdx]; // assuming operations size matches number of blocks
                if (op == '+')
                {
                    // cout << "  Adding " << number << " to total.\n";
                    block_result += number;
                    // cout << "  Intermediate block result: " << block_result << "\n";
                }
                else if (op == '*')
                {
                    // cout << "  Multiplying total by " << number << ".\n";
                    if (block_result == 0)
                    {
                        block_result = 1;
                    }
                    block_result *= number;
                    // cout << "  Intermediate block result: " << block_result << "\n";
                }
            }
        }
        // cout << " Result for Block " << blockIdx << ": " << block_result << "\n";
        total_result += block_result;
    }

    return total_result;
}

vector<vector<vector<int64_t>>> parse_grid(ifstream &infile)
{
    vector<string> lines;
    string line;
    while (getline(infile, line))
    {
        lines.push_back(line);
    }

    // filter out lines that contain operators
    vector<string> numberLines;

    for (const string &l : lines)
    {
        // check if line contains any non-digit non-space characters
        bool hasOperators = false;
        for (char ch : l)
        {
            if (ch != ' ' && !isdigit(ch))
            {
                hasOperators = true;
                break;
            }
        }

        if (!hasOperators && !l.empty())
        {
            numberLines.push_back(l);
        }
    }

    if (numberLines.empty())
    {
        return {};
    }

    size_t numRows = numberLines.size();
    size_t maxWidth = 0;
    for (const string &l : numberLines)
    {
        if (l.size() > maxWidth)
        {
            maxWidth = l.size();
        }
    }

    // find block boundaries by detecting vertical whitespace columns
    vector<pair<int, int>> blockRanges; // start and end column index for each block
    bool inBlock = false;
    int blockStart = 0;

    for (int col = 0; col < maxWidth; col++)
    {
        // Check if this column has any non-space characters
        bool hasContent = false;
        for (int row = 0; row < numRows; row++)
        {
            if (col < numberLines[row].size() && numberLines[row][col] != ' ')
            {
                hasContent = true;
                break;
            }
        }

        if (hasContent && !inBlock)
        {
            blockStart = col;
            inBlock = true;
        }
        else if (!hasContent && inBlock)
        {
            blockRanges.push_back({blockStart, col - 1});
            inBlock = false;
        }
    }

    //  case where last block extends to end
    if (inBlock)
    {
        blockRanges.push_back({blockStart, maxWidth - 1});
    }

    // 2D vector for each block
    vector<vector<vector<int64_t>>> blockVectors(blockRanges.size());

    // parse  each block, preserving exact positions
    for (int blockIdx = 0; blockIdx < blockRanges.size(); blockIdx++)
    {
        int startCol = blockRanges[blockIdx].first;
        int endCol = blockRanges[blockIdx].second;
        int blockWidth = endCol - startCol + 1;

        vector<vector<int64_t>> blockGrid(numRows, vector<int64_t>(blockWidth, -1));

        for (int row = 0; row < numRows; row++)
        {
            for (int col = startCol; col <= endCol; col++)
            {
                int blockCol = col - startCol;

                if (col < numberLines[row].size())
                {
                    char ch = numberLines[row][col];
                    if (isdigit(ch))
                    {
                        blockGrid[row][blockCol] = ch - '0';
                    }
                }
            }
        }
        blockVectors[blockIdx] = blockGrid;
    }

    return blockVectors;
}

int main()
{
    ifstream infile("input.txt");

    if (!infile.is_open())
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    vector<char> operation = readOperations(infile);

    cout << "Operations read from file:\n";

    // ----- Part 2 ----- //
    ifstream infile2("input.txt");

    vector<vector<vector<int64_t>>> blockVectors = parse_grid(infile2);

    // process data to obtain solution
    uint64_t result_part2 = solve_grid(blockVectors, operation);
    cout << "Total result:" << result_part2 << "\n";

    infile.close();

    return 0;
}