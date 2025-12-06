#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// Extract operations from the operator line
vector<char> readOperations(const vector<string> &lines)
{
    for (const string &line : lines)
    {
        // Check if this line has operators
        for (char ch : line)
        {
            if (ch != ' ' && (ch < '0' || ch > '9'))
            {
                // Found operator line - extract all non-space chars
                vector<char> ops;
                ops.reserve(20); // Typical AOC size
                for (char c : line)
                {
                    if (c != ' ')
                        ops.push_back(c);
                }
                return ops;
            }
        }
    }
    return {};
}

// Parse grid and operations in a single pass
struct ParsedData
{
    vector<vector<vector<int8_t>>> blocks;
    vector<char> operations;
};

ParsedData parseInput(const vector<string> &lines)
{
    ParsedData data;
    vector<string> numberLines;
    numberLines.reserve(lines.size());

    // Single pass: separate number lines and find operations
    for (const string &line : lines)
    {
        if (line.empty())
            continue;

        bool hasOperator = false;
        for (char ch : line)
        {
            if (ch != ' ' && (ch < '0' || ch > '9'))
            {
                hasOperator = true;
                // Extract operations
                data.operations.reserve(20);
                for (char c : line)
                {
                    if (c != ' ')
                        data.operations.push_back(c);
                }
                break;
            }
        }
        if (!hasOperator)
            numberLines.push_back(line);
    }

    if (numberLines.empty())
        return data;

    // Find max width
    size_t maxWidth = 0;
    for (const string &line : numberLines)
    {
        if (line.size() > maxWidth)
            maxWidth = line.size();
    }

    // Find block ranges
    vector<pair<int, int>> ranges;
    ranges.reserve(10);
    bool inBlock = false;
    size_t start = 0;

    
    for (size_t col = 0; col < maxWidth; col++)
    {
        bool hasContent = false;
        for (const string &line : numberLines)
        {
            if (col < line.size() && line[col] != ' ')
            {
                hasContent = true;
                break;
            }
        }

        if (hasContent)
        {
            if (!inBlock)
            {
                start = col;
                inBlock = true;
            }
        }
        else if (inBlock)
        {
            ranges.push_back({start, col - 1});
            inBlock = false;
        }
    }
    if (inBlock)
        ranges.push_back({start, maxWidth - 1});

    // Build blocks with int8_t (saves memory, improves cache)
    data.blocks.resize(ranges.size());
    for (size_t blockIdx = 0; blockIdx < ranges.size(); blockIdx++)
    {
        int startCol = ranges[blockIdx].first;
        int endCol = ranges[blockIdx].second;
        int width = endCol - startCol + 1;

        vector<vector<int8_t>> grid(numberLines.size(), vector<int8_t>(width, -1));

        for (size_t row = 0; row < numberLines.size(); row++)
        {
            const string &line = numberLines[row];
            for (size_t col = startCol; col <= endCol && col < line.size(); col++)
            {
                char ch = line[col];
                if (ch >= '0' && ch <= '9')
                {
                    grid[row][col - startCol] = ch - '0';
                }
            }
        }
        data.blocks[blockIdx] = move(grid);
    }

    return data;
}

// Solve: form numbers from each column and apply operations
uint64_t solve(const vector<vector<vector<int8_t>>> &blocks, const vector<char> &ops)
{
    uint64_t total = 0;

    for (size_t blockIdx = 0; blockIdx < blocks.size(); blockIdx++)
    {
        const auto &block = blocks[blockIdx];
        const char op = ops[blockIdx];
        uint64_t blockResult = 0;

        // Process each column in the block
        for (size_t col = 0; col < block[0].size(); col++)
        {
            // Form number from column (top to bottom, skip -1)
            uint64_t num = 0;
            for (size_t row = 0; row < block.size(); row++)
            {
                int8_t val = block[row][col];
                if (val != -1)
                {
                    num = num * 10 + val;
                }
            }

            // Apply operation
            if (num)
            {
                if (op == '+')
                {
                    blockResult += num;
                }
                else
                { // op == '*'
                    blockResult = blockResult ? blockResult * num : num;
                }
            }
        }
        total += blockResult;
    }

    return total;
}

int main()
{
    ifstream infile("input.txt");
    if (!infile.is_open())
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    // Read all lines
    vector<string> lines;
    lines.reserve(10); // Typical AOC size
    string line;
    while (getline(infile, line))
    {
        lines.push_back(move(line));
    }
    infile.close();

    // Parse and solve
    ParsedData data = parseInput(lines);
    uint64_t result = solve(data.blocks, data.operations);

    cout << "Total result: " << result << "\n";

    return 0;
}