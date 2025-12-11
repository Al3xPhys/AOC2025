#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <bitset>
#include <cstdint>
#include <bit>

using namespace std;

/*
                 ;i.
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
        :YYYYYY$$$$$$$$$$$$$$$$$$YYYYYYYiiiiYYYYYYi' cmang
*/

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
  uint64_t patternMask;
  vector<uint64_t> buttonMasks;
  size_t bitWidth;
};

vector<PuzzleData> parseDataLines(const vector<string> &dataLines)
{
  /* Parse lines from dataLines. Lines will be format "[....] (1,2,3) (1,2,3,4) etc.
    Extract the first block [....] and validate character (.) and (#) to 0 and 1s. Store in vector<uint64_T>
    Scan line and for each set of paraenthesis, split by commas, parse the integers, validate them 0 <= idx < n, toggle bit "idx" in the button mask. Collect buttons in a vector<uint64_t>"*/
  vector<PuzzleData> parsedData; // Puzzle data with width tracking
  // Iterate over each line
  for (const auto &line : dataLines)
  {
    size_t pos = 0;
    // Find the first block [...]
    size_t startBracket = line.find('[');
    size_t endBracket = line.find(']');
    if (startBracket == string::npos || endBracket == string::npos || endBracket <= startBracket)
    {
      throw runtime_error("Invalid line format: missing brackets");
    }
    // Parse pattern block
    string patternBlock = line.substr(startBracket + 1, endBracket - startBracket - 1);
    uint64_t patternMask = 0;
    size_t bitWidth = patternBlock.length(); // Track actual width

    // Convert pattern block to bitmask
    for (char c : patternBlock)
    {
      patternMask <<= 1;
      if (c == '#')
      {
        patternMask |= 1;
      }
      else if (c != '.')
      {
        throw runtime_error("Invalid character in pattern block");
      }
    }

    // Parse button masks
    vector<uint64_t> buttonMasks;
    pos = endBracket + 1;
    while (true)
    {
      size_t startParen = line.find('(', pos);
      size_t endParen = line.find(')', pos);
      if (startParen == string::npos || endParen == string::npos || endParen <= startParen)
      {
        break; // No more button masks
      }

      string buttonBlock = line.substr(startParen + 1, endParen - startParen - 1);
      uint64_t buttonMask = 0;
      size_t commaPos = 0;
      // Parse indices in button block
      while (true)
      {
        size_t nextComma = buttonBlock.find(',', commaPos);
        string indexStr = (nextComma == string::npos) ? buttonBlock.substr(commaPos) : buttonBlock.substr(commaPos, nextComma - commaPos);
        uint64_t index = stoull(indexStr);
        if (index >= bitWidth) // Validate against actual width
        {
          throw runtime_error("Button index out of range");
        }
        // Convert from left-to-right index to bit position (right-to-left)
        uint64_t bitPos = bitWidth - 1 - index;
        buttonMask ^= (1ULL << bitPos); // Toggle the bit at position

        if (nextComma == string::npos)
        {
          break;
        }
        commaPos = nextComma + 1;
      }
      buttonMasks.push_back(buttonMask);
      pos = endParen + 1;
    }

    parsedData.push_back({patternMask, buttonMasks, bitWidth});
  }
  return parsedData;
}

pair<uint64_t, uint64_t> brute_force_min_solution(const PuzzleData &puzzle)
{
  size_t m = puzzle.buttonMasks.size(); // Number of buttons
  uint64_t best_presses = UINT64_MAX;   // Reset best presses for each puzzle
  uint64_t best_subset = 0;             // Reset best subset for each puzzle

  // Iterate over all possible subsets of buttons
  for (uint64_t subset = 0; subset < (1ULL << m); ++subset)
  {
    uint64_t state = 0; // Current state after pressing buttons in subset

    // Apply each button in the subset
    for (size_t j = 0; j < m; ++j)
    {
      if (subset & (1ULL << j))
      {
        state ^= puzzle.buttonMasks[j]; // Apply button mask
      }
    }

    // Check if the current state matches the target pattern
    if (state == puzzle.patternMask)
    {
      uint64_t presses = popcount(subset); // Count number of buttons pressed
      if (presses < best_presses)
      {
        best_presses = presses;
        best_subset = subset;
      }
    }
  }

  if (best_presses != UINT64_MAX)
  {
    return {best_presses, best_subset};
  }
  return {0, 0}; // No solution found
}

int main()
{
  fstream file("input.txt");
  if (!file.is_open())
  {
    cerr << "Error opening file!" << endl;
    return 1;
  }

  string dogeLine;
  string dataLine;
  // Read file line by line and print out up to the "--DOGE END--" marker, then start saving to a seperate string
  for (; getline(file, dogeLine);)
  {
    if (dogeLine == "-- DOGE END --")
    {
      break;
    }
    cout << dogeLine << endl; // Print doge lines
  }
  // Now read the rest of the file, turn each line into an entry in a vector<string>
  vector<string> dataLines;
  for (; getline(file, dataLine);)
  {
    dataLines.push_back(dataLine);
  }

  // Parse data lines
  auto parsedData = parseDataLines(dataLines);

  // Process each puzzle
  int puzzleNum = 0;
  uint64_t total_sum_of_buttons = 0;
  for (const auto &puzzle : parsedData)
  {
    // cout << "\n=== Puzzle " << ++puzzleNum << " ===" << endl;
    // cout << "Pattern: " << printBitmask(puzzle.patternMask, puzzle.bitWidth) << " (decimal: " << puzzle.patternMask << ")" << endl;
    // cout << "Bit width: " << puzzle.bitWidth << endl;
    // cout << "Number of buttons: " << puzzle.buttonMasks.size() << endl;

    // Print each button's mask
    for (size_t i = 0; i < puzzle.buttonMasks.size(); ++i)
    {
      // cout << "  Button " << i << ": " << printBitmask(puzzle.buttonMasks[i], puzzle.bitWidth) << " (decimal: " << puzzle.buttonMasks[i] << ")" << endl;
    }

    auto [minButtons, numSolutions] = brute_force_min_solution(puzzle);
    // cout << "Minimum buttons to press: " << minButtons << endl;
    if (minButtons > 0)
    {
      // cout << "One optimal solution (button indices): ";
      for (size_t i = 0; i < puzzle.buttonMasks.size(); ++i)
      {
        if (numSolutions & (1ULL << i))
        {
          // cout << i << " ";       // Print button index
          total_sum_of_buttons++; // Increment total sum of buttons
        }
      }
      // cout << endl;
    }
    else
    {
      cout << "No solution found." << endl;
    }
  }
  cout << "\nTotal sum of buttons across all puzzles: " << total_sum_of_buttons << endl;
  file.close(); // Close the file
}
