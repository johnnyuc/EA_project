#include <iostream>
#include <vector>
#include <unordered_map>

// WORKING GRAPH MAKER

using namespace std;

bool isValid(int row, int col, int numRows, int numCols, const vector<string>& grid) {
    return row >= 0 && row < numRows && col >= 0 && col < numCols && grid[row][col] != '#';
}

int toId(int row, int col, int numCols) {
    return row * numCols + col;
}

void buildGraph(const vector<string>& grid, unordered_map<int, vector<int>>& graph) {
    int numRows = (int) grid.size();
    int numCols = (int) grid[0].size();

    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            if (grid[row][col] == '#') continue;

            int cellId = toId(row, col, numCols);
            vector<int> neighbors;

            for (auto& dir : directions) {
                int newRow = row + dir.first;
                int newCol = col + dir.second;

                if (isValid(newRow, newCol, numRows, numCols, grid)) {
                    int neighborId = toId(newRow, newCol, numCols);
                    neighbors.push_back(neighborId);
                }
            }

            graph[cellId] = neighbors;
        }
    }
}

int main() {
    vector<string> grid = {
            "###E###",
            "#M#.#M#",
            "#...#.#",
            "#.#...#",
            "#...#M#",
            "##D####"
    };

    unordered_map<int, vector<int>> graph;
    buildGraph(grid, graph);

    for (auto& entry : graph) {
        cout << "Node " << entry.first << " connects to: ";
        for (int neighbor : entry.second) {
            cout << neighbor << " ";
        }
        cout << endl;
    }

    return 0;
}
