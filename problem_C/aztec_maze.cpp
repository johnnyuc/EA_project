/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <chrono> // high_resolution_clock
#include <iostream> // cin, cout
#include <vector> // vector
#include <algorithm> // reverse, min
#include <unordered_map> // unordered_map
#include <unordered_set> // unordered_set

using namespace std;

// Maze node structure
struct Node {
    unordered_set<int> neighbors; // Neighbors of the node
    int type; // 0: Path, 1: Manhole, 2: Door, 3: Exit

    Node() : type(0) {} // Default constructor
    explicit Node(int type) : type(type) {}
};

// Maze structure
struct Maze {
    // Input data
    int numRows, numCols, numCovers;
    vector<string> grid{};

    // Found data
    int doorNode{}, exitNode{}; // Door and exit nodes positions
    vector<int> manholeNodes{}; // Total manhole nodes found

    // Processed data
    unordered_map<int, Node> graph; // Graph representation

    // Output data
    vector<int> path; // Path from door to exit
    pair<pair<int, int>, vector<int>> floodgate; // Floodgate position
    vector<pair<pair<int, int>, vector<int>>> bridges; // Bridges and their connected manholes

    Maze (int numRows, int numCols, const vector<string>& grid, int numCovers) :
            numRows(numRows), numCols(numCols), grid(grid), numCovers(numCovers) {} // Default constructor

    // Check if a cell is valid
    static bool isValid(int row, int col, int numRows, int numCols, const string& rowString) {
        return row >= 0 && row < numRows && col >= 0 && col < numCols && rowString[col] != '#';
    }

    // Convert row and column to an id
    static int toId(int row, int col, int numCols) {
        return row * numCols + col;
    }

    // Convert an id to row and column
    static pair<int, int> toRowCol(int id, int numCols) {
        return make_pair(id / numCols, id % numCols);
    }

    // Build the graph
    void makeGraph() {
        vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        // Check all cells and build the graph
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                char cell = grid[row][col];
                if (cell == '#') continue; // Skip walls

                // Get id
                int cellId = toId(row, col, numCols);

                // Get type
                int type = 0;
                switch (cell) {
                    case 'M': type = 1; manholeNodes.emplace_back(cellId); break;
                    case 'D': type = 2; doorNode = cellId; break;
                    case 'E': type = 3; exitNode = cellId; break;
                    default: break;
                }

                Node node(type);

                // Check adjacent cells
                for (auto& dir : directions) {
                    int newRow = row + dir.first;
                    int newCol = col + dir.second;
                    if (isValid(newRow, newCol, numRows, numCols, grid[newRow])) {
                        int neighborId = toId(newRow, newCol, numCols);
                        node.neighbors.insert(neighborId);
                    }
                }

                graph[cellId] = node;
            }
        }
    }

    // Depth-first search to find bridges and it's connected manholes [also checks exit path]
    vector<int> dfs(int node, vector<int>& low, vector<int>& disc, vector<int>& parent, vector<bool>& visited) {
        static int time = 0; // Order of discovery
        visited[node] = true; // Set to visited
        disc[node] = low[node] = ++time; // Set discovery and low time
        vector<int> manholeIds; // Manhole ids connected to the node

        // Check if the node is the exit node and build the path
        if (node == exitNode) {
            while (node != doorNode) {
                path.push_back(node);
                node = parent[node];
            }
            path.push_back(doorNode);
            reverse(path.begin(), path.end());
        }

        // Check if the node is a manhole and add it to the manholeIds vector
        if (graph[node].type == 1) manholeIds.push_back(node);

        // Traverse the neighbors
        for (int neighbor : graph[node].neighbors) {
            if (!visited[neighbor]) {
                parent[neighbor] = node; // Set parent

                // Check how many manholes are connected to the neighbor
                vector<int> childManholeIds = dfs(neighbor, low, disc, parent, visited);
                manholeIds.insert(manholeIds.end(), childManholeIds.begin(), childManholeIds.end());

                // Update the low time
                low[node] = min(low[node], low[neighbor]);
                if (low[neighbor] > disc[node]) bridges.emplace_back(make_pair(node, neighbor), childManholeIds);
            } else if (neighbor != parent[node]) low[node] = min(low[node], disc[neighbor]);
        }

        return manholeIds;
    }

    // Function to traverse the graph, find bridges and which manholes they connect
    void findBridges() {
        vector<int> low(numRows * numCols, -1);
        vector<int> disc(numRows * numCols, -1);
        vector<int> parent(numRows * numCols, -1);
        vector<bool> visited(numRows * numCols, false);

        // Find bridges starting from the door node
        if (!visited[doorNode]) dfs(doorNode, low, disc, parent, visited);
    }

    // Function to find the floodgate bridge
    void findFloodgateBridge() {
        // Sort the bridgeManholes vector based on the number of connected manholes in descending order
        sort(bridges.rbegin(), bridges.rend(),
             [](const auto& a, const auto& b) {
                 return a.second.size() < b.second.size();
             });

        // Iterate through the sorted bridgeManholes vector in reverse order
        for (const auto& entry : bridges) {
            // Check if the bridge is not on the path and the number of connected manholes is less than or equal to numCovers
            if (find(path.begin(), path.end(), entry.first.first) == path.end() ||
                find(path.begin(), path.end(), entry.first.second) == path.end()) {
                floodgate = entry;
                return;
            }
        }

        // If no suitable floodgate bridge is found, return a default value
        floodgate = make_pair(make_pair(0, 0), vector<int>());
    }

    // Function to print the output
    void output() {
        // Print the floodgate bridge coordinates
        pair<int, int> floodgateFrom = toRowCol(floodgate.first.first, numCols);
        pair<int, int> floodgateTo = toRowCol(floodgate.first.second, numCols);
        cout << floodgateFrom.first << " " << floodgateFrom.second << " " << floodgateTo.first << " " << floodgateTo.second << endl;

        // Print the manhole covers
        vector<int> manholeCovers;
        for (int manholeNode : manholeNodes)
            if (find(floodgate.second.begin(), floodgate.second.end(), manholeNode) == floodgate.second.end())
                manholeCovers.push_back(manholeNode);

        cout << manholeCovers.size() << endl;
        for (int manholeNode : manholeCovers) {
            pair<int, int> manhole = toRowCol(manholeNode, numCols);
            cout << manhole.first << " " << manhole.second << endl;
        }

        // Print the path
        cout << path.size() << endl;
        for (int i : path) {
            pair<int, int> node = toRowCol(i, numCols);
            cout << node.first << " " << node.second << endl;
        }
    }

};

int main() {
    // Making I/O faster
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read the number of tests
    int nr_tests;
    cin >> nr_tests;

    for (int t = 0; t < nr_tests; t++) {
        // Read the dimensions of the maze
        int numRows, numCols;
        cin >> numRows >> numCols;

        // Read the maze
        vector<string> grid(numRows);
        for (int row = 0; row < numRows; ++row)
            cin >> grid[row];

        // Store number of manhole covers
        int numCovers;
        cin >> numCovers;

        // Create the maze with the given dimensions
        Maze lab(numRows, numCols, grid, numCovers);

        // Process the maze
        lab.makeGraph(); // Build the graph
        lab.findBridges(); // Find bridges
        lab.findFloodgateBridge(); // Find the floodgate

        // Print the output
        lab.output(); // Print the output
    }

    return 0;
}