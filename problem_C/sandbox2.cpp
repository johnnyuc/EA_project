/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <iostream> // cin, cout
#include <chrono> // high_resolution_clock

#include <stack> // stack
#include <vector> // vector
#include <algorithm> // reverse
#include <unordered_map> // unordered_map
#include <unordered_set> // unordered_set

using namespace std;

// Profiler class to measure the time taken by each function [DEBUG]
class Profiler {
public:
    void start(const string& functionName) {
        startTimes[functionName] = chrono::high_resolution_clock::now();
    }

    void stop(const string& functionName) {
        auto duration = chrono::duration_cast<chrono::nanoseconds>(
                chrono::high_resolution_clock::now() - startTimes[functionName]
        ).count();
        accumulatedTimes[functionName] += duration;
    }

    void printSummary() const {
        cout << "Accumulated execution times:" << endl;
        for (const auto& pair : accumulatedTimes) {
            double seconds = static_cast<double>(pair.second) * 1e-9; // Convert nanoseconds to seconds
            cout << " - [" << pair.first << "]: " << fixed << seconds << " s" << endl;
        }
    }

private:
    unordered_map<string, chrono::high_resolution_clock::time_point> startTimes;
    unordered_map<string, long long> accumulatedTimes;
};

// Profiler object [DEBUG]
Profiler profiler;

// Maze node structure
struct Node {
    unordered_set<int> neighbors; // Neighbors of the node
    int type; // 0: Path, 1: Manhole, 2: Door, 3: Exit

    Node() : type(0) {} // Default constructor
    explicit Node(int type) : type(type) {}
};

// Maze structure
struct Maze {
    int numRows, numCols; // Grid dimensions
    vector<string> grid{}; // Grid representation
    int numCovers{}; // Number of manhole covers

    // Data variables
    int doorNode{}, exitNode{}; // Door and exit nodes positions
    vector<int> manholeNodes{}; // Total manhole nodes found

    vector<pair<int, int>> bridges; // Bridges in the maze
    pair<int, int> floodgate{}; // Floodgate nodes position
    vector<int> coveredManholes{}; // Manholes to be covered after finding floodGate position
    vector<pair<int, int>> path; // Path from the door to the exit

    unordered_map<int, Node> graph; // Graph representation

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
        profiler.start(__func__);
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
        profiler.stop(__func__);
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

    // Find bridges in the maze
    void findBridges() {
        profiler.start(__func__);

        // Initialize variables
        vector<int> ids(numRows * numCols, -1);
        vector<int> low(numRows * numCols, -1);
        vector<int> parent(numRows * numCols, -1);
        int id = 0;

        // Check all nodes
        for (auto &entry: graph)
            if (ids[entry.first] == -1)
                dfs(entry.first, id, ids, low, parent);

        profiler.stop(__func__);
    }

    // Find a suitable bridge to place a flood gate and update the graph
    pair<int, int> findFloodgate() {
        profiler.start(__func__);

        pair<int, int> bestBridge = {0, 0};

        for (auto& bridge : bridges) {
            // Temporarily remove the bridge
            graph[bridge.first].neighbors.erase(bridge.second);
            graph[bridge.second].neighbors.erase(bridge.first);

            // Count reachable manholes from the door
            vector<bool> visited(numRows * numCols, false);
            stack<int> s;
            s.push(doorNode);
            visited[doorNode] = true;
            vector<int> reachable;

            // DFS process
            while (!s.empty()) {
                int node = s.top();
                s.pop();

                // If the node is a manhole, add it to the reachable vector
                if (graph[node].type == 1) reachable.push_back(node);

                // Visit all neighbors
                for (int neighbor : graph[node].neighbors)
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        s.push(neighbor);
                    }
            }

            if (!visited[exitNode] || reachable.size() > numCovers) {
                // Restore the bridge
                graph[bridge.first].neighbors.insert(bridge.second);
                graph[bridge.second].neighbors.insert(bridge.first);
                continue;
            }

            bestBridge = bridge;
            coveredManholes.insert(coveredManholes.end(), reachable.begin(), reachable.end());
            break;
        }

        profiler.stop(__func__);
        return bestBridge;
    }

    // Find a path from the door to the exit, using DFS
    void findPath() {
        profiler.start(__func__);

        // Find the best bridge and remove it
        floodgate = findFloodgate();

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

        profiler.stop(__func__);
    }

    // [DEBUG FUNCTIONS]
    // Print the solution
    void solution() {
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
        //cout << endl;
    }

    // [DEBUG]
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
            pair<int, int> coordinates = toRowCol(entry.first, numCols);
            cout << "Node (" << coordinates.first << ", " << coordinates.second << ") of type " << entry.second.type << " connects to: ";
            for (int neighbor : entry.second.neighbors) {
                pair<int, int> neighborCoordinates = toRowCol(neighbor, numCols);
                cout << "(" << neighborCoordinates.first << ", " << neighborCoordinates.second << ") ";
            }
            cout << endl;
        }
        cout << endl;
    }

    // Print the bridges
    void printBridges() {
        if (bridges.empty()) {
            cout << "No bridges found in the maze" << endl << endl;
            return;
        }
        for (auto& bridge : bridges) {
            pair<int, int> from = toRowCol(bridge.first, numCols);
            pair<int, int> to = toRowCol(bridge.second, numCols);
            cout << "Bridge found between nodes (" << from.first << ", " << from.second << ") and (" << to.first << ", " << to.second << ")" << endl;
        }
        cout << endl;
    }

    // Count the number of reachable manholes after crossing each bridge
    /*void countReachableManholes() {
        profiler.start(__func__);

        for (auto& bridge : bridges) {
            int bridgeNode1 = bridge.first;
            int bridgeNode2 = bridge.second;

            // Perform DFS from each side of the bridge
            for (int startNode : {bridgeNode1, bridgeNode2}) {
                vector<bool> visited(numRows * numCols, false);
                stack<int> s;
                s.push(startNode);
                visited[startNode] = true;
                int reachableCount = 0;

                // DFS process
                while (!s.empty()) {
                    int node = s.top();
                    s.pop();

                    // If the node is a manhole, increment the reachable count
                    if (graph[node].type == 1) reachableCount++;

                    // Visit all neighbors except the other end of the bridge
                    for (int neighbor : graph[node].neighbors) {
                        if (neighbor != bridgeNode1 && neighbor != bridgeNode2 && !visited[neighbor]) {
                            visited[neighbor] = true;
                            s.push(neighbor);
                        }
                    }
                }

                // Print the number of reachable manholes from this side of the bridge
                pair<int, int> from = toRowCol(startNode, numCols);
                pair<int, int> to = toRowCol(startNode == bridgeNode1 ? bridgeNode2 : bridgeNode1, numCols);
                cout << "Number of reachable manholes from (" << from.first << ", " << from.second
                     << ") to (" << to.first << ", " << to.second << "): " << reachableCount << endl;
            }
        }

        profiler.stop(__func__);
    }*/

    // Find paths from the door to all manholes and the exit using DFS
    void findAllPaths() {
        profiler.start(__func__);

        vector<vector<int>> paths(numRows * numCols); // Store paths to each node
        vector<bool> visited(numRows * numCols, false);
        stack<int> s;

        // Start from the door
        visited[doorNode] = true;
        s.push(doorNode);

        // DFS process
        while (!s.empty()) {
            int node = s.top();
            s.pop();

            // Visit all neighbors
            for (int neighbor : graph[node].neighbors) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    paths[neighbor] = paths[node]; // Copy the path from the parent node
                    paths[neighbor].push_back(node); // Add the current node to the path
                    s.push(neighbor);
                }
            }
        }

        // Print paths for each manhole and the exit
        for (int node = 0; node < numRows * numCols; ++node) {
            if (paths[node].empty()) continue; // Skip unreachable nodes

            // Check if the node is a manhole or the exit
            if (graph[node].type == 1 || node == exitNode) {
                pair<int, int> coordinates = toRowCol(node, numCols);
                cout << "Path to node (" << coordinates.first << ", " << coordinates.second << "): ";
                for (int pathNode : paths[node]) {
                    pair<int, int> pathCoordinates = toRowCol(pathNode, numCols);
                    cout << "(" << pathCoordinates.first << ", " << pathCoordinates.second << ") ";
                }
                pair<int, int> nodeCoordinates = toRowCol(node, numCols);
                cout << "(" << nodeCoordinates.first << ", " << nodeCoordinates.second << ")" << endl;
            }
        }
        cout << endl;

        profiler.stop(__func__);
    }


};

int main(int argc, char* argv[]) {
    // Check if the debug flag is set [DEBUG]
    bool flagDebug = std::find(argv, argv + argc, std::string("-d")) != argv + argc;

    // Makes I/O faster
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read the number of tests
    int nr_tests;
    cin >> nr_tests;

    for (int t = 0; t < nr_tests; t++) {
        if (flagDebug) cout << "Test " << t + 1 << " ===============================" << endl;
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

        // Solution
        lab.makeGraph();
        lab.findBridges();
        //lab.countReachableManholes();
        lab.findPath();

        // Print accumulated times and data [DEBUG]
        if (flagDebug) {
            // Print door and exit nodes
            cout << endl;
            pair<int, int> door = Maze::toRowCol(lab.doorNode, numCols);
            pair<int, int> exit = Maze::toRowCol(lab.exitNode, numCols);
            cout << "Door node: (" << door.first << ", " << door.second << ")" << endl;
            cout << "Exit node: (" << exit.first << ", " << exit.second << ")" << endl;
            cout << endl;

            lab.printGrid();
            lab.printGraph();
            lab.printBridges();
            lab.findAllPaths();
            profiler.printSummary();
            cout << endl;
        }

        // Print solution
        lab.solution();
    }

    return 0;
}