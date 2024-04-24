/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

/*
 * Stuff to base the solution on:
 * https://cses.fi/problemset/task/1192
 * https://cses.fi/problemset/task/1193
 * https://cses.fi/problemset/task/1194
 * https://github.com/mrsac7/CSES-Solutions/blob/master/src/1194%20-%20Monsters.cpp
 */

/*
 * Critical notes in here:
 * 0. Always put important notes or progress point to fix with TODO
 * 1. Should never use [] to access the graph, as it will create a new node if it does not exist
 * 2. Should always use .at() to access the graph, as it will throw an exception if the node does not exist
 * 3. Should always use .count() to check if a node exists in the graph
 */

#include <iostream>

#include <vector>
#include <stack>
#include <algorithm>
#include <unordered_map>

using namespace std;

// Labyrinth node structure
struct Node {
    // Variables
    // ================================================================================================
    vector<int> neighbors; // Neighbors of the node

    /* Types of nodes
     * Type 0: Path
     * Type 1: Manhole
     * Type 2: Door
     * Type 3: Exit
     */
    int type;

    // Constructors
    // ================================================================================================
    Node() : type(0) {} // Default constructor
    explicit Node(int type) : type(type) {}
};

// Labyrinth structure
struct Labyrinth {
    // Variables
    // ================================================================================================
    // Input variables
    int numRows, numCols; // Grid dimensions
    vector<string> grid{}; // Grid representation

    int numCovers{}; // Number of manhole covers

    // Data variables
    int doorNode{}, exitNode{}; // Door and exit nodes
    vector<int> manholeNodes{}; // Manhole nodes
    vector<pair<int, int>> bridges; // Bridges in the labyrinth

    unordered_map<int, Node> graph; // Graph representation

    // Constructors
    // ================================================================================================
    Labyrinth(int numRows, int numCols) : numRows(numRows), numCols(numCols), grid(numRows) {} // Default Constructor

    // Functions
    // ================================================================================================
    // Check if a cell is valid
    static bool isValid(int row, int col, int numRows, int numCols, const string& rowString) {
        return row >= 0 && row < numRows && col >= 0 && col < numCols && rowString[col] != '#';
    }

    // Convert row and column to an id
    static int toId(int row, int col, int numCols) {
        return row * numCols + col;
    }

    // Build the graph
    void buildGraph() {
        vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        // Store everything first into a grid so we can check for valid adjacent cells
        for (int row = 0; row < numRows; ++row)
            cin >> grid[row];

        // Check all cells and build the graph
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                char cell = grid[row][col];
                if (cell == '#') continue; // Skip walls

                int cellId = toId(row, col, numCols);
                int type = 0;
                if (cell == 'M') {
                    type = 1;
                    manholeNodes.emplace_back(cellId);
                } else if (cell == 'D') {
                    type = 2;
                    doorNode = cellId;
                } else if (cell == 'E') {
                    type = 3;
                    exitNode = cellId;
                }
                Node node(type);

                // Check adjacent cells
                for (auto& dir : directions) {
                    int newRow = row + dir.first;
                    int newCol = col + dir.second;
                    if (isValid(newRow, newCol, numRows, numCols, grid[newRow])) {
                        int neighborId = toId(newRow, newCol, numCols);
                        node.neighbors.push_back(neighborId);
                    }
                }

                graph[cellId] = node;
            }
        }
    }

    // Depth-first search to support bridge finding
    void dfs(int node, int& id, vector<int>& ids, vector<int>& low, vector<int>& parent) {
        if (graph.count(node) == 0) return;

        ids[node] = low[node] = id++; // Set the id and low of the node

        // Check all neighbors
        for (int neighbor : graph.at(node).neighbors) {
            if (ids[neighbor] == -1) {
                parent[neighbor] = node;
                dfs(neighbor, id, ids, low, parent);
                low[node] = min(low[node], low[neighbor]);
                if (low[neighbor] > ids[node]) bridges.emplace_back(node, neighbor);
            } else if (neighbor != parent[node]) low[node] = min(low[node], ids[neighbor]);
        }
    }

    // Find bridges in the labyrinth
    void findBridges() {
        // Initialize variables
        vector<int> ids(numRows * numCols, -1);
        vector<int> low(numRows * numCols, -1);
        vector<int> parent(numRows * numCols, -1);
        int id = 0;

        // Check all nodes
        for (auto& entry : graph) {
            if (ids[entry.first] == -1) {
                dfs(entry.first, id, ids, low, parent);
            }
        }
    }

    // Find how many manholes in the labyrinth are reachable from a given node
    int countManholes(int node, vector<bool>& visited) {
        if (graph.count(node) == 0 || visited[node]) return 0;

        visited[node] = true;
        int count = (graph[node].type == 1) ? 1 : 0;

        for (int neighbor : graph[node].neighbors)
            count += countManholes(neighbor, visited);

        return count;
    }

    // Find the a suitable bridge to remove
    pair<int, int> removableBridge() {
        pair<int, int> bestBridge = {-1, -1};
        int maxReachable = INT_MAX; // Used for solution 2

        for (auto& bridge : bridges) {
            // Temporarily remove the bridge
            graph[bridge.first].neighbors.erase(
                    remove(
                            graph[bridge.first].neighbors.begin(),
                            graph[bridge.first].neighbors.end(),
                            bridge.second),
                    graph[bridge.first].neighbors.end());
            graph[bridge.second].neighbors.erase(
                    remove(
                            graph[bridge.second].neighbors.begin(),
                            graph[bridge.second].neighbors.end(),
                            bridge.first),
                    graph[bridge.second].neighbors.end());

            // Count reachable manholes from the door
            vector<bool> visited(numRows * numCols, false);
            int reachable = countManholes(doorNode, visited);

            // If it doesn't reach the exit or has enough manhole covers
            if (!visited[exitNode] || reachable > numCovers) {
                // Restore the bridge
                graph[bridge.first].neighbors.push_back(bridge.second);
                graph[bridge.second].neighbors.push_back(bridge.first);
                continue;
            }

            // Possible solutions
            // 1. Best bridge is the first one found
            /*
            if (reachable <= numCovers) {
                bestBridge = bridge;
                break;
            }
            */

            // 2. Best bridge to remove is the one that leads to the least number of reachable manholes
             if (reachable < maxReachable) {
                bestBridge = bridge;
                maxReachable = reachable;
                cout << "New suitable removable bridge between nodes " << bridge.first << " and " << bridge.second << endl;
            }

            // Restore the bridge
            graph[bridge.first].neighbors.push_back(bridge.second);
            graph[bridge.second].neighbors.push_back(bridge.first);
        }

        return bestBridge;
    }

    // Support functions
    // ================================================================================================
    // Print the grid
    void printGrid() {
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                int cellId = toId(row, col, numCols);
                char cellChar = (graph.count(cellId)) ?
                                (graph[cellId].type == 1 ?
                                'M' : (graph[cellId].type == 2 ?
                                'D' : (graph[cellId].type == 3 ?
                                'E' : '.'))) : '#';
                cout << cellChar << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    // Print the graph
    void printGraph() const {
        for (auto& entry : graph) {
            cout << "Node " << entry.first << " of type " << entry.second.type << " connects to: ";
            for (int neighbor : entry.second.neighbors) {
                cout << neighbor << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    // Print the bridges
    void printBridges() {
        for (auto& bridge : bridges)
            cout << "Bridge found between nodes " << bridge.first << " and " << bridge.second << endl;
        cout << endl;
    }
};

int main() {
    // Makes I/O faster
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read the number of tests
    int nr_tests;
    cin >> nr_tests;

    for (int t = 0; t < nr_tests; t++) {
        // Read the dimensions of the labyrinth
        int rows, columns;
        cin >> rows >> columns;
        Labyrinth lab(rows, columns);

        // Build the graph based on input and dimensions
        lab.buildGraph();

        // Store number of manhole covers
        cin >> lab.numCovers;

        // Find bridges
        lab.findBridges();

        // Debug
        lab.printGrid();
        lab.printGraph();
        lab.printBridges();

        // Find the best bridge to remove
        pair<int, int> bestBridge = lab.removableBridge();
        cout << "Best bridge to remove is between nodes " << bestBridge.first << " and " << bestBridge.second << endl;
    }

    return 0;
}