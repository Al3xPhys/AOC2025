#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <cstdint>
#include <bit>
#include <deque>

using namespace std;

// Helper function to print a bitmask with only the necessary bits
string printBitmask(uint64_t mask, size_t width)
{
    string result;
    for (size_t i = width; i-- > 0;)
    {
        result += (mask & (1ULL << i)) ? '1' : '0';
    }
    return result.empty() ? "0" : result;
}

struct PuzzleData
{
    vector<uint64_t> target;           // Target pattern as a vector of uint64_t
    vector<vector<uint64_t>> buttons;  // Each button stores as a vector of indices it incremenets
    size_t numDigits;                  // Number of digits in the pattern
    vector<uint64_t> buttonMasks;      // Each button as a bitmask
    vector<uint64_t> radixMultipliers; // Radix multipliers for each digit2
};

vector<PuzzleData> parseDataLines(const vector<string> &dataLines)
{
    vector<PuzzleData> parseData; // Vector to hold parsed puzzle data
    for (const auto &line : dataLines)
    {
        // cout << "Processing line: " << line << endl;

        // Find and skip the [] block at the front
        size_t bracketEnd = line.find(']');
        if (bracketEnd == string::npos)
        {
            throw runtime_error("Invalid line format: missing ] bracket");
        }

        // Parse all () blocks into buttons
        vector<vector<uint64_t>> buttons;
        size_t pos = bracketEnd + 1;

        while (true)
        {
            size_t parenStart = line.find('(', pos);
            size_t parenEnd = line.find(')', pos);

            if (parenStart == string::npos || parenEnd == string::npos || parenEnd <= parenStart)
            {
                break; // No more () blocks
            }

            // Parse the numbers in this () block
            string buttonBlock = line.substr(parenStart + 1, parenEnd - parenStart - 1);
            vector<uint64_t> buttonIndices;

            size_t commaPos = 0;
            while (true)
            {
                size_t nextComma = buttonBlock.find(',', commaPos);
                string numberStr = (nextComma == string::npos) ? buttonBlock.substr(commaPos) : buttonBlock.substr(commaPos, nextComma - commaPos);

                if (!numberStr.empty())
                {
                    buttonIndices.push_back(stoull(numberStr));
                }

                if (nextComma == string::npos)
                {
                    break;
                }
                commaPos = nextComma + 1;
            }

            buttons.push_back(buttonIndices);
            pos = parenEnd + 1;
        }

        // Parse the {} block for target pattern
        size_t patternStart = line.find('{');
        size_t patternEnd = line.find('}');

        if (patternStart == string::npos || patternEnd == string::npos || patternEnd <= patternStart)
        {
            throw runtime_error("Invalid pattern block in line: " + line);
        }

        string patternBlock = line.substr(patternStart + 1, patternEnd - patternStart - 1);
        vector<uint64_t> targetPattern;

        // Parse comma-separated numbers
        size_t commaPos = 0;
        while (true)
        {
            size_t nextComma = patternBlock.find(',', commaPos);
            string numberStr = (nextComma == string::npos) ? patternBlock.substr(commaPos) : patternBlock.substr(commaPos, nextComma - commaPos);

            if (!numberStr.empty())
            {
                targetPattern.push_back(stoull(numberStr));
            }

            if (nextComma == string::npos)
            {
                break;
            }
            commaPos = nextComma + 1;
        }

        parseData.push_back({targetPattern, buttons, targetPattern.size()});
    }

    // debug print parsed data
    // for (size_t puzzleIdx = 0; puzzleIdx < parseData.size(); ++puzzleIdx)
    // {
    //     const auto &puzzle = parseData[puzzleIdx];
    //     cout << "Puzzle " << puzzleIdx << ":\n";
    //     cout << "  Target Pattern: ";
    //     for (const auto &val : puzzle.target)
    //     {
    //         cout << val << " ";
    //     }
    //     cout << "\n  Buttons:\n";
    //     for (size_t btnIdx = 0; btnIdx < puzzle.buttons.size(); ++btnIdx)
    //     {
    //         cout << "    Button " << btnIdx << ": ";
    //         for (const auto &idx : puzzle.buttons[btnIdx])
    //         {
    //             cout << idx << " ";
    //         }
    //         cout << "\n";
    //     }
    // }

    return parseData;
}

void fill_bitmasks(PuzzleData &puzzle)
{
    /* convert each button from a list of indicies into a 0/1 vector of length puzzle.numDigits */
    vector<uint64_t> buttonMasks;
    for (auto &buttonIndices : puzzle.buttons)
    {
        uint64_t buttonMask = 0;
        for (const auto &idx : buttonIndices)
        {
            if (idx >= puzzle.numDigits)
            {
                throw runtime_error("Button index out of range");
            }
            buttonMask |= (1ULL << (puzzle.numDigits - 1 - idx)); // Set the bit at the correct position
        }
        buttonMasks.push_back(buttonMask);
    }
    puzzle.buttonMasks = buttonMasks;

    // Debug print button masks
    // for (size_t i = 0; i < puzzle.buttonMasks.size(); ++i)
    // {
    //     cout << "  Button " << i << " Mask: " << printBitmask(puzzle.buttonMasks[i], puzzle.numDigits) << " (decimal: " << puzzle.buttonMasks[i] << ")\n";
    // }
}

void set_radix_multipliers(PuzzleData &puzzle)
{
    /* given a target = [t0, t1, ..., tn-1], radixMultipliers[0] = 1, radixMultipliers[i] = radixMultipliers[i-1] * (target[i-1] + 1)
        eg if target = {3,5,4,7} then radixMultipliers[1] = 1 * (3 + 1) = 1 * 4 = 4, etc*/
    vector<uint64_t> radixMultipliers;
    radixMultipliers.resize(puzzle.numDigits);
    radixMultipliers[0] = 1;
    for (size_t i = 1; i < puzzle.numDigits; ++i)
    {
        radixMultipliers[i] = radixMultipliers[i - 1] * (puzzle.target[i - 1] + 1);
    }

    // Store in puzzle struct
    puzzle.radixMultipliers = radixMultipliers;

    // Debug print radix multipliers
    // cout << "Radix Multipliers: ";
    // for (const auto &mult : radixMultipliers)
    // {
    //     cout << mult << " ";
    // }
}

uint64_t encode_pattern(const PuzzleData &puzzle, vector<uint64_t> &decodedPattern)
{
    /* encode the target pattern into a single integer using the radix multipliers */
    uint64_t encodedValue = 0;
    for (size_t i = 0; i < puzzle.numDigits; ++i)
    {
        encodedValue += puzzle.target[i] * puzzle.radixMultipliers[i];
    }

    // Debug print encoded value
    // cout << "Encoded Pattern Value: " << encodedValue << endl;

    return encodedValue;
}

vector<uint64_t> decode_pattern(const PuzzleData &puzzle, uint64_t encodedValue)
{
    /* decode the single integer back into the target pattern using the radix multipliers */
    vector<uint64_t> decodedPattern(puzzle.numDigits);
    for (size_t i = puzzle.numDigits; i-- > 0;)
    {
        decodedPattern[i] = encodedValue / puzzle.radixMultipliers[i];
        encodedValue %= puzzle.radixMultipliers[i];
    }

    // // Debug print decoded pattern
    // cout << "Decoded Pattern: ";
    // for (const auto &val : decodedPattern)
    // {
    //     cout << val << " ";
    // }
    // cout << endl;

    return decodedPattern;
}

uint64_t BFS_solve_puzzle(const PuzzleData &puzzle)
{
    // copy of puzzle data
    PuzzleData puzzleCopy = puzzle;
    int n = puzzleCopy.numDigits; // number of digits
    int total_states = 1;
    // Calculate total number of states
    for (int i = 0; i < n; ++i)
    {
        total_states *= (puzzleCopy.target[i] + 1);
    }

    vector<uint64_t> dist(total_states, -1);         // Distance array initialized to -1 (unvisited)
    vector<uint64_t> parent(total_states, -1);       // Parent array for path reconstruction
    vector<uint64_t> parentButton(total_states, -1); // Button used to reach this state

    // encode starting state (all zeros, {0,0,0,...,0}) - index 0
    uint64_t startIdx = 0;
    dist[startIdx] = 0;

    deque<uint64_t> q;
    q.push_back(startIdx); // Enqueue starting state

    // precompute the button effect on each state
    vector<vector<uint64_t>> buttonEffects(puzzleCopy.buttonMasks.size(), vector<uint64_t>(n, 0)); // buttonEffects[buttonIdx][digitIdx]
    // loop over each button, fill in which digits it affects
    for (size_t btnIdx = 0; btnIdx < puzzleCopy.buttonMasks.size(); ++btnIdx)
    {
        for (int idx : puzzleCopy.buttons[btnIdx])
        {
            buttonEffects[btnIdx][idx] = 1;
        }
    }

    // Encode target state
    uint64_t targetIdx = encode_pattern(puzzleCopy, puzzleCopy.target);

    // BFS loop
    while (!q.empty())
    {
        int currentIdx = q.front();
        q.pop_front();
        // Check if we reached the target
        if (currentIdx == targetIdx)
        {
            // cout << "Reached target state!" << endl;
            return dist[currentIdx]; // return number of steps
        }

        vector<uint64_t> digits = decode_pattern(puzzleCopy, currentIdx);

        // Try all buttons
        for (size_t btnIdx = 0; btnIdx < puzzleCopy.buttonMasks.size(); ++btnIdx)
        {
            // Apply button effect
            vector<uint64_t> newDigits = digits;
            bool overshoot = false;
            for (int digitIdx = 0; digitIdx < n; ++digitIdx)
            {
                newDigits[digitIdx] += buttonEffects[btnIdx][digitIdx]; // increment digit if button affects it
                if (newDigits[digitIdx] > puzzleCopy.target[digitIdx])
                {
                    overshoot = true; // this button press would overshoot the target
                    break;
                }
            }
            if (overshoot)
            {
                continue; // skip this button press
            }

            // Encode newDigits to newIdx
            uint64_t newIdx = 0;
            for (int i = 0; i < n; ++i)
            {
                newIdx += newDigits[i] * puzzleCopy.radixMultipliers[i];
            }

            // If this state hasn't been visited
            if (dist[newIdx] == -1)
            {
                dist[newIdx] = dist[currentIdx] + 1;
                parent[newIdx] = currentIdx;
                parentButton[newIdx] = btnIdx;
                q.push_back(newIdx); // Enqueue new state
            }
        }
    }

    // If we exhaust the queue without finding the target
    cout << "No solution found!" << endl;
    return -1;
}

void display_doge()
{
    string dogeArt = R"(                 ;i.
                  M$L                    .;i.
                  M$Y;                .;iii;;.
                 ;$YY$i._           .iiii;;;;;
                .iiiYYYYYYiiiii;;;;i;iii;; ;;;
              .;iYYYYYYiiiiiiYYYiiiiiii;;  ;;;
           .YYYY$$$$YYYYYYYYYYYYYYYYiii;; ;;;;
         .YYY$$$$$$YYYYYY$$$$iiiY$$$$$$$ii;;;;
        :YYYF`,  TYYYYY$$$$$YYYYYYYi$$$$$iiiii;
        Y$MM: \  :YYYY$$P"````"T$YYMMMMMMMMiiYY.
     `.;$$M$$b.,dYY$$Yi; .(     .YYMMM$$$MMMMYY
   .._$MMMMM$!YYYYYYYYYi;.`"  .;iiMMM$MMMMMMMYY
    ._$MMMP` ```""4$$$$$iiiiiiii$MMMMMMMMMMMMMY;
     MMMM$:       :$$$$$$$MMMMMMMMMMM$$MMMMMMMYYL
    :MMMM$$.    .;PPb$$$$MMMMMMMMMM$$$$MMMMMMiYYU:
     iMM$$;;: ;;;;i$$$$$$$MMMMM$$$$MMMMMMMMMMYYYYY
     `$$$$i .. ``:iiii!*"``.$$$$$$$$$MMMMMMM$YiYYY
      :Y$$iii;;;.. ` ..;;i$$$$$$$$$MMMMMM$$YYYYiYY:
       :$$$$$iiiiiii$$$$$$$$$$$MMMMMMMMMMYYYYiiYYYY.
        `$$$$$$$$$$$$$$$$$$$$MMMMMMMM$YYYYYiiiYYYYYY
         YY$$$$$$$$$$$$$$$$MMMMMMM$$YYYiiiiiiYYYYYYY
        :YYYYYY$$$$$$$$$$$$$$$$$$YYYYYYYiiiiYYYYYYi')";
    cout << dogeArt << endl;
}

int main()
{
    fstream file("input.txt");
    if (!file.is_open())
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }
    display_doge();

    string dataLine;

    // Now read the rest of the file, turn each line into an entry in a vector<string>
    vector<string> dataLines;
    for (; getline(file, dataLine);)
    {
        dataLines.push_back(dataLine);
    }

    // Parse data lines
    auto parsedData = parseDataLines(dataLines);

    // convert buttons to bitmasks
    vector<vector<uint64_t>> allButtonMasks; // Store all puzzles' button masks
    for (auto &puzzle : parsedData)
    {
        // cout << "\n=== Puzzle ===" << endl;
        // cout << "Pattern: ";
        // for (const auto &val : puzzle.target)
        // {
        //     cout << val << " ";
        // }
        // cout << "(numDigits: " << puzzle.numDigits << ")\n";
        fill_bitmasks(puzzle); // Convert buttons to bitmasks and fill puzzle.buttonMasks
    }

    // set radix multipliers
    for (auto &puzzle : parsedData)
    {
        // cout << "\n=== Puzzle ===" << endl;
        // cout << "Pattern: ";
        // for (const auto &val : puzzle.target)
        // {
        //     cout << val << " ";
        // }
        // cout << "(numDigits: " << puzzle.numDigits << ")\n";
        set_radix_multipliers(puzzle);
    }

    // // encode and decode patterns
    // for (auto &puzzle : parsedData)
    // {
    //     cout << "\n=== Puzzle ===" << endl;
    //     cout << "Pattern: ";
    //     for (const auto &val : puzzle.target)
    //     {
    //         cout << val << " ";
    //     }
    //     cout << "(numDigits: " << puzzle.numDigits << ")\n";
    //     uint64_t encodedValue = encode_pattern(puzzle, puzzle.target);
    //     vector<uint64_t> decodedPattern = decode_pattern(puzzle, encodedValue);
    // }

    // solve each puzzle
    uint64_t totalSolved = 0;
    for (auto &puzzle : parsedData)
    {
        // cout << "\n=== Solving Puzzle ===" << endl;
        // cout << "Pattern: ";
        // for (const auto &val : puzzle.target)
        // {
        //     cout << val << " ";
        // }
        // cout << "(numDigits: " << puzzle.numDigits << ")\n";
        uint64_t steps = BFS_solve_puzzle(puzzle);
        if (steps != uint64_t(-1))
        {
            cout << "Solved in " << steps << " steps!" << endl;
        }
        else
        {
            cout << "No solution found." << endl;
        }
        totalSolved += steps;
    }

    cout << "\nTotal steps for all puzzles: " << totalSolved << endl;

    file.close(); // Close the file
}
