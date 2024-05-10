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

    // DFS
    vector<int> low, disc, parent, cm;
    vector<bool> visited, lte;
    bool found;

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

    // Depth-first search to find bridges and it's connected manholes [also checks exit path]
    void dfs(int node) {
        static int time = 0; // Order of discovery
        visited[node] = true; // Set to visited
        disc[node] = low[node] = ++time; // Set discovery and low time

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

        // Check if the node is a manhole and add it to the connectedManholes vector
        if (graph[node].type == 1) cm[node]++;

        // Traverse the neighbors
        for (int neighbor : graph[node].neighbors) {
            if (!visited[neighbor]) {
                parent[neighbor] = node; // Set parent

                // Check how many manholes are connected to the neighbor
                dfs(neighbor);

                cm[node] += cm[neighbor];

                // Update the low time
                low[node] = min(low[node], low[neighbor]);
                if (low[neighbor] > disc[node] && !found) {
                    if (numCovers >= manholeNodes.size()-cm[node] && !lte[neighbor]) {
                        found = true;
                        floodgate = make_pair(parent[node], node);
                        cout << "Floodgate: " << parent[node] << " " << node << "\n";
                        // Remove connected manholes from manholeNodes
                        for (int manholeNode : manholeNodes)
                            if (disc[manholeNode] > disc[parent[node]]) {
                                manholeNodes.erase(remove(manholeNodes.begin(), manholeNodes.end(), manholeNode), manholeNodes.end());
                                cout << "Erasing manhole: " << manholeNode << "\n";
                            }
                    }
                }
                lte[node] = lte[node] || lte[neighbor];
            } else if (neighbor != parent[node]) low[node] = min(low[node], disc[neighbor]);
        }
    }

    // Function to traverse the graph, find bridges and which manholes they connect
    void findBridges() {
        profiler.start(__func__);

        // Initialize the vectors
        low.assign(numRows * numCols, -1);
        disc.assign(numRows * numCols, -1);
        parent.assign(numRows * numCols, -1);
        visited.assign(numRows * numCols, false);

        cm.assign(numRows * numCols, 0);
        lte.assign(numRows * numCols, false);

        // Find bridges starting from the door node
        dfs(doorNode);

        profiler.stop(__func__);
    }

    // Function to print the output
    void output() {
        profiler.start(__func__);

        // Print the floodgate bridge coordinates
        pair<int, int> first = toRowCol(floodgate.first, numCols);
        pair<int, int> second = toRowCol(floodgate.second, numCols);
        cout << first.first << " " << first.second << " " << second.first << " " << second.second << endl;

        cout << manholeNodes.size() << endl;
        for (int manholeNode : manholeNodes) {
            pair<int, int> manhole = toRowCol(manholeNode, numCols);
            cout << manhole.first << " " << manhole.second << endl;
        }

        // Print the path
        cout << path.size() << endl;
        for (int i : path) {
            pair<int, int> node = toRowCol(i, numCols);
            cout << node.first << " " << node.second << endl;
        }

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

        // Print the output
        lab.output(); // Print the output

        // Print profiler times
        profiler.result();
    }

    return 0;
}