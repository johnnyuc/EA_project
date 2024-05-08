/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <chrono>
#include <iostream> // cin, cout
#include <stack> // stack
#include <vector> // vector
#include <algorithm> // reverse, min
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

    void result() const {
        cout << "Accumulated execution times:" << endl;
        for (const auto& pair : accumulatedTimes) {
            double seconds = static_cast<double>(pair.second) * 1e-9; // Convert nanoseconds to seconds
            cout << " - [" << pair.first << "]: " << fixed << seconds << " s" << endl;
        }
        cout << endl;
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
    pair<int, int> floodgate; // Floodgate position

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

    // Iterative depth-first search
    void traversal() {
        profiler.start(__func__);
        bool found = false;

        // DFS vectors
        vector<int> low(numRows * numCols, -1);
        vector<int> disc(numRows * numCols, -1);
        vector<int> parent(numRows * numCols, -1);
        vector<bool> visited(numRows * numCols, false);

        vector<int> cm(numRows * numCols, 0);
        vector<bool> lte(numRows * numCols, false);

        // Traversal variables
        static int time = 0; // Order of discovery
        visited[doorNode] = true; // Set to visited
        disc[doorNode] = low[doorNode] = ++time; // Set discovery and low time

        // Stack
        stack<pair<int, int>> stack; // Stack to store nodes and their state
        stack.emplace(doorNode, -1); // Push the start node with -1 as initial parent

        // Traverse the graph
        while (!stack.empty()) {
            int node = stack.top().first; // Get the current node
            int& state = stack.top().second; // State to manage the current neighbor index
            state++; // Move to the next neighbor

            // Initialize the processing of a node [first time arriving at the node]
            if (state == 0) {
                disc[node] = low[node] = ++time;
                visited[node] = true;

                // Check if the node is the exit node and build the path
                if (node == exitNode) {
                    lte[node] = true;
                    while (node != doorNode) {
                        path.push_back(node);
                        node = parent[node];
                    }
                    path.push_back(doorNode);
                    reverse(path.begin(), path.end());
                }

                // Check if the node is a manhole and increment the connectedManholes count
                if (graph[node].type == 1) cm[node]++;
            }

            // Process all neighbors
            auto it = graph[node].neighbors.begin();
            advance(it, state); // Move iterator to the current neighbor index

            if (state < graph[node].neighbors.size()) {
                int neighbor = *it;
                if (!visited[neighbor]) {
                    parent[neighbor] = node;
                    stack.emplace(neighbor, -1);
                } else if (neighbor != parent[node])
                    low[node] = min(low[node], disc[neighbor]);
            } else {
                // After processing all neighbors
                if (node != doorNode) {
                    low[parent[node]] = min(low[parent[node]], low[node]);
                    if (cm[node] > 0) cm[parent[node]] += cm[node]; // Update cm count
                    if (low[node] > disc[parent[node]] && !found) {
                        if (numCovers >= manholeNodes.size()-cm[node] && !lte[node]) {
                            found = true; // First suitable bridge
                            floodgate = make_pair(parent[node], node);
                            // Remove connected manholes from manholeNodes
                            for (int manholeNode : manholeNodes)
                                if (disc[manholeNode] > disc[parent[node]])
                                    manholeNodes.erase(remove(manholeNodes.begin(), manholeNodes.end(), manholeNode), manholeNodes.end());
                        }
                    }
                    lte[parent[node]] = lte[parent[node]] || lte[node];
                }
                stack.pop(); // Pop the node after processing
            }
        }
        profiler.stop(__func__);
    }

    // Function to print the output
    void output() {
        profiler.start(__func__);
        // Print the floodgate bridge coordinates
        profiler.start("floodgatePR");
        pair<int, int> floodgateFrom = toRowCol(floodgate.first, numCols);
        pair<int, int> floodgateTo = toRowCol(floodgate.second, numCols);
        cout << floodgateFrom.first << " " << floodgateFrom.second << " " << floodgateTo.first << " " << floodgateTo.second << endl;
        profiler.stop("floodgatePR");

        profiler.start("manholePR");
        cout << manholeNodes.size() << endl;
        for (int manholeNode : manholeNodes) {
            pair<int, int> manhole = toRowCol(manholeNode, numCols);
            cout << manhole.first << " " << manhole.second << endl;
        }
        profiler.stop("manholePR");

        // Print the path reversed
        profiler.start("pathPR");
        cout << path.size() << endl;
        for (int node : path) {
            profiler.start("convRC");
            pair<int, int> cell = toRowCol(node, numCols);
            profiler.stop("convRC");
            profiler.start("cout");
            cout << cell.first << " " << cell.second << endl;
            profiler.stop("cout");
        }
        profiler.stop("pathPR");
        profiler.stop(__func__);
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
        // Chrono in nano seconds
        auto start = chrono::high_resolution_clock::now();

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
        lab.traversal(); // Find bridges

        // Print the output
        lab.output(); // Print the output
    }

    // Print accumulated
    profiler.result();

    return 0;
}