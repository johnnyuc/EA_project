/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <iostream>

#include <stack>
#include <vector>
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
    int numCovers{}; // Number of manhole covers
    vector<string> grid{}; // Grid representation

    // Data variables
    int doorNode{}, exitNode{}; // Door and exit nodes positions
    pair<int, int> floodgate{}; // Floodgate nodes position
    vector<int> manholeNodes{}; // Total manhole nodes found
    vector<int> coveredManholes{}; // Manholes to be covered after finding floodGate position

    // Output variables
    vector<pair<int, int>> bridges; // Bridges in the labyrinth
    vector<pair<int, int>> path; // Path from the door to the exit

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

    // Convert an id to row and column
    static pair<int, int> toRowCol(int id, int numCols) {
        return make_pair(id / numCols, id % numCols);
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

                // Get id
                int cellId = toId(row, col, numCols);

                // Get type
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
        for (auto &entry: graph)
            if (ids[entry.first] == -1)
                dfs(entry.first, id, ids, low, parent);
    }

    // Find how many manholes in the labyrinth are reachable from a given node
    vector<int> reachableManholes(int node, vector<bool>& visited) {
        if (graph.count(node) == 0 || visited[node]) return {};

        visited[node] = true;
        vector<int> reachable;

        // If the node is a manhole, add it to the reachable vector
        if (graph[node].type == 1) reachable.push_back(node);

        // Check all neighbors and add their reachable manholes to the reachable vector
        for (int neighbor : graph[node].neighbors) {
            vector<int> neighborReachable = reachableManholes(neighbor, visited);
            reachable.insert(reachable.end(), neighborReachable.begin(), neighborReachable.end());
        }

        return reachable;
    }

    // Update the graph by removing or restoring a bridge [flag: del]
    void updateGraph(pair<int, int> bridge, bool del) {
        if (del) {
            // Remove the bridge
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
        } else {
            // Restore the bridge
            graph[bridge.first].neighbors.push_back(bridge.second);
            graph[bridge.second].neighbors.push_back(bridge.first);
        }
    }

    // Find a suitable bridge to place a flood gate
    pair<int, int> findFloodgate(bool flagBest) {
        pair<int, int> bestBridge = {-1, -1};
        size_t maxReachable = INT_MAX; // Used for solution 2

        for (auto& bridge : bridges) {
            // Temporarily remove the bridge
            updateGraph(bridge, true);

            // Count reachable manholes from the door
            vector<bool> visited(numRows * numCols, false);
            vector<int>reachable = reachableManholes(doorNode, visited);

            // If it doesn't reach the exit or has enough manhole covers
            if (!visited[exitNode] || reachable.size() > numCovers) {
                updateGraph(bridge, false); // Restore the bridge
                continue;
            }

            // Possible solutions
            // 1. Best bridge is the first one found
            if (flagBest) {
                bestBridge = bridge;
                coveredManholes.insert(coveredManholes.end(), reachable.begin(), reachable.end());
                break;
            }
                // 2. Best bridge is the one that leads to the least number of reachable manholes
            else {
                if (reachable.size() < maxReachable) {
                    bestBridge = bridge;
                    maxReachable = reachable.size();
                    coveredManholes.clear();
                    coveredManholes.insert(coveredManholes.end(), reachable.begin(), reachable.end());
                }
            }

            updateGraph(bridge, false); // Restore the bridge
        }

        return bestBridge;
    }

    // Find a path from the door to the exit, using DFS
    void findPath(bool flagBest) {
        // Find the best bridge and remove it
        floodgate = findFloodgate(flagBest);
        updateGraph(floodgate, true);

        // Depth-first search to find a path from the door to the exit
        vector<int> parent(numRows * numCols, -1);
        vector<bool> visited(numRows * numCols, false);
        stack<int> s;

        // Start from the door
        visited[doorNode] = true;
        s.push(doorNode);

        // DFS process
        while (!s.empty()) {
            int node = s.top();
            s.pop();

            // If this is the exit, we have found a path
            if (node == exitNode) break;

            // Visit all neighbors
            for (int neighbor : graph[node].neighbors)
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    parent[neighbor] = node; // Store the parent node
                    s.push(neighbor);
                }
        }

        // If we didn't find a path, clear the path vector
        if (!visited[exitNode]) {
            path.clear();
            return;
        }

        // Build the path from the exit to the door
        for (int node = exitNode; node != -1; node = parent[node])
            path.push_back(toRowCol(node, numCols));
        reverse(path.begin(), path.end());
    }

    // Print the solution
    void printSol(bool flagBest) {
        // Find solutions
        findBridges();
        findPath(flagBest);

        // Print solutions
        pair<int, int> from = toRowCol(floodgate.first, numCols);
        pair<int, int> to = toRowCol(floodgate.second, numCols);
        cout << from.first << " " << from.second << " " << to.first << " " << to.second << endl;
        cout << coveredManholes.size() << endl;
        for (int manhole : coveredManholes) {
            pair<int, int> rowCol = toRowCol(manhole, numCols);
            cout << rowCol.first << " " << rowCol.second << endl;
        }
        cout << path.size() << endl;
        for (auto& cell : path)
            cout << cell.first << " " << cell.second << endl;
        cout << endl;
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

int main(int argc, char* argv[]) {
    bool flagDebug = false;
    bool flagBest = false;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            string arg = argv[i];
            if (arg == "-d") flagDebug = true;
            else if (arg == "-b") flagBest = true;
        }
    }

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

        // Debug
        if (flagDebug) {
            lab.printGrid();
            lab.printGraph();
            lab.printBridges();
        }

        // Solution
        lab.printSol(flagBest);
    }

    return 0;
}