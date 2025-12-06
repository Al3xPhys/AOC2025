#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// Extract operations from the operator line
vector<char> readOperations(const vector<string>& lines) {
    for (const string& line : lines) {
        // Check if this line has operators
        for (char ch : line) {
            if (ch != ' ' && !isdigit(ch)) {
                // Found operator line - extract all non-space chars
                vector<char> ops;
                for (char c : line) {
                    if (c != ' ') ops.push_back(c);
                }
                return ops;
            }
        }
    }
    return {};
}

// Filter to get only number lines (no operators)
vector<string> getNumberLines(const vector<string>& lines) {
    vector<string> result;
    for (const string& line : lines) {
        if (line.empty()) continue;
        
        bool isNumberLine = true;
        for (char ch : line) {
            if (ch != ' ' && !isdigit(ch)) {
                isNumberLine = false;
                break;
            }
        }
        if (isNumberLine) result.push_back(line);
    }
    return result;
}

// Find block column ranges by detecting vertical whitespace
vector<pair<int, int>> findBlockRanges(const vector<string>& numberLines) {
    if (numberLines.empty()) return {};
    
    size_t maxWidth = 0;
    for (const string& line : numberLines) {
        maxWidth = max(maxWidth, line.size());
    }
    
    vector<pair<int, int>> ranges;
    bool inBlock = false;
    int start = 0;
    
    for (int col = 0; col < maxWidth; col++) {
        bool hasContent = false;
        for (const string& line : numberLines) {
            if (col < line.size() && line[col] != ' ') {
                hasContent = true;
                break;
            }
        }
        
        if (hasContent && !inBlock) {
            start = col;
            inBlock = true;
        } else if (!hasContent && inBlock) {
            ranges.push_back({start, col - 1});
            inBlock = false;
        }
    }
    
    if (inBlock) ranges.push_back({start, maxWidth - 1});
    return ranges;
}

// Parse grid into blocks, storing digits with exact positions
vector<vector<vector<int64_t>>> parse_grid(const vector<string>& lines) {
    vector<string> numberLines = getNumberLines(lines);
    if (numberLines.empty()) return {};
    
    vector<pair<int, int>> blockRanges = findBlockRanges(numberLines);
    vector<vector<vector<int64_t>>> blocks(blockRanges.size());
    
    for (size_t blockIdx = 0; blockIdx < blockRanges.size(); blockIdx++) {
        int startCol = blockRanges[blockIdx].first;
        int endCol = blockRanges[blockIdx].second;
        int width = endCol - startCol + 1;
        
        vector<vector<int64_t>> grid(numberLines.size(), vector<int64_t>(width, -1));
        
        for (size_t row = 0; row < numberLines.size(); row++) {
            for (int col = startCol; col <= endCol && col < numberLines[row].size(); col++) {
                char ch = numberLines[row][col];
                if (isdigit(ch)) {
                    grid[row][col - startCol] = ch - '0';
                }
            }
        }
        blocks[blockIdx] = grid;
    }
    
    return blocks;
}

// Solve: form numbers from each column and apply operations
uint64_t solve_grid(const vector<vector<vector<int64_t>>>& blocks, const vector<char>& ops) {
    uint64_t total = 0;
    
    for (size_t blockIdx = 0; blockIdx < blocks.size(); blockIdx++) {
        uint64_t blockResult = 0;
        const auto& block = blocks[blockIdx];
        char op = ops[blockIdx];
        
        // Process each column in the block
        for (size_t col = 0; col < block[0].size(); col++) {
            // Form number from column (top to bottom, skip -1)
            int64_t num = 0;
            for (size_t row = 0; row < block.size(); row++) {
                int64_t val = block[row][col];
                if (val != -1) {
                    num = num * 10 + val;
                }
            }
            
            // Apply operation
            if (num != 0) {
                if (op == '+') {
                    blockResult += num;
                } else if (op == '*') {
                    blockResult = (blockResult == 0) ? num : blockResult * num;
                }
            }
        }
        total += blockResult;
    }
    
    return total;
}

int main() {
    ifstream infile("input_al.txt");
    if (!infile.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }
    
    // Read all lines
    vector<string> lines;
    string line;
    while (getline(infile, line)) {
        lines.push_back(line);
    }
    infile.close();
    
    // Parse and solve
    vector<char> operations = readOperations(lines);
    vector<vector<vector<int64_t>>> blocks = parse_grid(lines);
    uint64_t result = solve_grid(blocks, operations);
    
    cout << "Total result: " << result << "\n";
    
    return 0;
}