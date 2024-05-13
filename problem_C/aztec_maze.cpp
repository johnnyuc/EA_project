/*
 * Author: Johnny Fernandes
 * Std. No: 2021190668
 * Author: Miguel Leopoldo
 * Std. No: 2021225940
 * Algorithmic Strategies 2023/24
 */

#include <iostream> // cin, cout
#include <stack> // stack
#include <vector> // vector
#include <algorithm> // reverse, min
#include <unordered_map> // unordered_map
#include <unordered_set> // unordered_set
#include <queue> // queue

using namespace std;

// Maze node structure
struct Node {
    vector<int> neighbors; // Neighbors of the node
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
    unordered_set<int> manholeNodes{}; // Total manhole nodes found

    // Processed data
    unordered_map<int, Node> graph; // Graph representation

    // Output data
    vector<int> path;
    unordered_set<int> cPath; // Path from door to exit
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
                    case 'M': type = 1; manholeNodes.insert(cellId); break;
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
                        node.neighbors.push_back(neighborId);
                    }
                }

                graph[cellId] = node;
            }
        }
    }

    // BFS to find shortest path
    void bfs() {
        // Queue
        queue<int> queue;
        queue.push(doorNode);

        // Visited vector
        vector<bool> visited(numRows * numCols, false);
        visited[doorNode] = true;

        // Parent vector
        vector<int> parent(numRows * numCols, -1);

        // Traverse the graph
        while (!queue.empty()) {
            int node = queue.front();
            queue.pop();

            // Process all neighbors
            for (int neighbor : graph[node].neighbors) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    parent[neighbor] = node;
                    queue.push(neighbor);
                }
            }
        }

        // Build the path
        int node = exitNode;
        while (node != doorNode) {
            cPath.insert(node);
            path.push_back(node);
            node = parent[node];
        }
        cPath.insert(doorNode);
        path.push_back(doorNode);

        // Reverse the path
        reverse(path.begin(), path.end());
    }

    // Iterative depth-first search
    void traversal() {
        bool found = false;

        // DFS vectors
        vector<int> low(numRows * numCols, -1);
        vector<int> disc(numRows * numCols, -1);
        vector<int> parent(numRows * numCols, -1);
        vector<bool> visited(numRows * numCols, false);

        // Best bridge
        vector<int> bridges; // Bridges
        unordered_map<int, vector<int>> cm; // Connected manholes

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

                // Check if the node is a manhole and increment the connectedManholes count
                if (graph[node].type == 1) cm[node].push_back(node);
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
                    // Update the parent connected manholes
                    cm[parent[node]].insert(cm[parent[node]].end(), cm[node].begin(), cm[node].end());
                    // Check if the node is a bridge
                    if (low[node] > disc[parent[node]]) {
                        if (numCovers >= manholeNodes.size()-cm[node].size()) {
                            if (cPath.count(node) == 0) {
                                found = true; // First suitable bridge
                                floodgate = make_pair(parent[node], node);
                                for (int n : cm[node]) manholeNodes.erase(n);
                            }
                        }
                    }
                }
                stack.pop(); // Pop the node after processing
            }
            if (found) break;
        }
    }

    // Function to print the output
    void output() {
        // Print the floodgate bridge coordinates
        pair<int, int> floodgateFrom = toRowCol(floodgate.first, numCols);
        pair<int, int> floodgateTo = toRowCol(floodgate.second, numCols);
        cout << floodgateFrom.first << " " << floodgateFrom.second << " " << floodgateTo.first << " " << floodgateTo.second << endl;

        cout << manholeNodes.size() << endl;
        for (int manholeNode : manholeNodes) {
            pair<int, int> manhole = toRowCol(manholeNode, numCols);
            cout << manhole.first << " " << manhole.second << endl;
        }

        // Print the path
        cout << path.size() << endl;
        for (int node : path) {
            pair<int, int> cell = toRowCol(node, numCols);
            cout << cell.first << " " << cell.second << endl;
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
        lab.bfs(); // Find the shortest path
        lab.traversal(); // Find a suitable bridge

        // Print the output
        lab.output(); // Print the output
    }

    return 0;
}