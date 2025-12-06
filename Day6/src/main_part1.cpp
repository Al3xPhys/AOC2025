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

vector<vector<uint64_t>> readGrid(ifstream &infile, int num_rows)
{
    string line;

    // read first line to determine number of columns
    getline(infile, line);
    stringstream ss(line);
    int val;
    int num_cols = 0;
    vector<int> first_row;
    while (ss >> val)
    {
        first_row.push_back(val);
        num_cols++;
    }

    // initialize homework grid, preallocate space to avoid resizing, SPEEEEEEED!
    vector<vector<uint64_t>> homework_grid(num_rows, vector<uint64_t>(num_cols));

    // store first row
    for (int c = 0; c < num_cols; c++)
    {
        homework_grid[0][c] = first_row[c];
    }

    // read remaining lines from file
    int row = 1;
    while (row < num_rows && getline(infile, line))
    {
        stringstream ss(line);
        int value;
        int col = 0;

        while (col < num_cols && ss >> value)
        {
            homework_grid[row][col] = value;
            col++;
        }
        row++;
    }

    return homework_grid;
}

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

void printGrid(const vector<vector<uint64_t>> &grid)
{
    for (int r = 0; r < grid.size(); r++)
    {
        cout << "Row " << r << ": ";
        for (uint64_t v : grid[r])
        {
            cout << v << " ";
        }
        cout << "\n";
    }
}

void printOperations(const vector<char> &operations)
{
    cout << "Operations: ";
    for (char op : operations)
    {
        cout << op << " ";
    }
    cout << "\n";
}

// ----- HELPER FUNCTIONS ----- //

vector<int64_t> getColumn(const vector<vector<int64_t>> &grid, int colIdx)
{
    vector<int64_t> column;
    for (const auto &row : grid)
    {
        if (colIdx < row.size())
        {
            column.push_back(row[colIdx]);
        }
    }
    return column;
}

// FUNCTION TO PROCESS DATA AND OBTAIN SOLUTION //

uint64_t solve_grid(vector<vector<uint64_t>> &grid, const vector<char> &operations)
{
    /* Read numbers in row by row for each column, perform operation on numbers for corresponding
        column in "operations". Return total sum of all results */

    uint64_t total_result = 0;
    // iterate through each element of operation
    for (int i = 0; i < operations.size(); i++)
    {
        char op = operations[i];

        if (op == '+')
        {
            // perform addition on column i
            uint64_t sum = 0;
            for (int r = 0; r < grid.size(); r++)
            {
                sum += grid[r][i];
            }
            // cout << "Sum of column " << i << ": " << sum << "\n";
            total_result += sum;
        }
        else if (op == '*')
        {
            // perform multiplication on column i
            uint64_t product = 1;
            for (int r = 0; r < grid.size(); r++)
            {
                product *= grid[r][i];
            }
            // cout << "Product of column " << i << ": " << product << "\n";
            total_result += product;
        }
    }
    // placeholder return value
    return total_result;
}

int main()
{
    ifstream infile("input.txt");

    if (!infile.is_open())
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    // ----- Part 1 ----- //

    // read data from file
    vector<vector<uint64_t>> homework_grid = readGrid(infile, 4);
    vector<char> operation = readOperations(infile);

    // process data to obtain solution
    uint64_t result = solve_grid(homework_grid, operation);

    cout << "Total result: " << result << "\n";

    infile.close();

    return 0;
}