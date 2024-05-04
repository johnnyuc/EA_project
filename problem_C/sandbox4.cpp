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
#include <queue>

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
    vector<int> dist;
    vector<vector<int>> paths;
    pair<int, int> floodgate{}; // Floodgate nodes position
    vector<int> coveredManholes{}; // Manholes to be covered after finding floodGate position


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
    void dfs(int startNode, vector<int>& ids, vector<int>& low, vector<int>& parent) {
        stack<pair<int, int>> s; // Stack to store nodes and their states
        s.emplace(startNode, 0); // Push the start node with state 0
        int id = 0;

        while (!s.empty()) {
            int node = s.top().first;
            int state = s.top().second;
            s.pop();

            if (state == 0) {
                // First visit to the node
                ids[node] = low[node] = id++;
                s.emplace(node, 1); // Push the node again with state 1

                // Iterate over the neighbors
                for (int neighbor : graph.at(node).neighbors) {
                    if (ids[neighbor] == -1) {
                        parent[neighbor] = node;
                        s.emplace(neighbor, 0); // Push the neighbor with state 0
                    }
                }
            } else {
                // Second visit to the node (after exploring all neighbors)
                for (int neighbor : graph.at(node).neighbors) {
                    if (neighbor != parent[node]) {
                        low[node] = min(low[node], low[neighbor]);
                        if (low[neighbor] > ids[node]) {
                            bridges.emplace_back(node, neighbor);
                        }
                    }
                }
            }
        }
    }

    void bfs() {
        dist.assign(numRows * numCols, -1);
        paths.assign(numRows * numCols, vector<int>());
        queue<int> q;
        q.push(doorNode);
        dist[doorNode] = 0;
        paths[doorNode].push_back(doorNode);

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            for (int neighbor : graph[node].neighbors) {
                if (dist[neighbor] == -1) {
                    dist[neighbor] = dist[node] + 1;
                    paths[neighbor] = paths[node];
                    paths[neighbor].push_back(neighbor);
                    q.push(neighbor);
                }
            }
        }
    }

    // Find bridges in the maze
    void solver() {
        profiler.start(__func__);

        vector<int> ids(numRows * numCols, -1);
        vector<int> low(numRows * numCols, -1);
        vector<int> parent(numRows * numCols, -1);

        for (auto &entry: graph)
            if (ids[entry.first] == -1)
                dfs(entry.first, ids, low, parent);

        bfs();

        for (auto& bridge : bridges) {
            if (dist[bridge.second] < dist[bridge.first]) {
                swap(bridge.first, bridge.second);
            }
        }

        int maxManholes = -1;
        pair<int, int> bestBridge;
        unordered_set<int> exitPath(paths[exitNode].begin(), paths[exitNode].end());

        for (auto& bridge : bridges) {
            if (exitPath.count(bridge.first) || exitPath.count(bridge.second)) {
                continue;
            }

            int bridgeNode1 = bridge.first;
            int bridgeNode2 = bridge.second;

            vector<bool> visited(numRows * numCols, false);
            stack<int> s;
            s.push(bridgeNode2);
            visited[bridgeNode2] = true;
            int reachableCount = 0;

            while (!s.empty()) {
                int node = s.top();
                s.pop();

                if (graph[node].type == 1) {
                    reachableCount++;
                    if (reachableCount == manholeNodes.size()) break;
                }

                for (int neighbor : graph[node].neighbors) {
                    if (neighbor != bridgeNode1 && !visited[neighbor]) {
                        visited[neighbor] = true;
                        s.push(neighbor);
                    }
                }
            }

            if (reachableCount > maxManholes) {
                maxManholes = reachableCount;
                bestBridge = bridge;
            }
        }

        floodgate = bestBridge;

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
        cout << paths[exitNode].size() << endl;
        for (auto& node : paths[exitNode]) {
            pair<int, int> coordinates = Maze::toRowCol(node, numCols);
            cout << coordinates.first << " " << coordinates.second << endl;
        }
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
        lab.solver();

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
            profiler.printSummary();
            cout << endl;
        }

        // Print solution
        lab.solution();
    }

    return 0;
}