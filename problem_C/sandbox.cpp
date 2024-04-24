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

    // Support variables
    // ================================================================================================
    bool visited = false;
};

// Labyrinth structure
struct Labyrinth {
    // Variables
    // ================================================================================================
    // Input variables
    int numRows, numCols; // Grid dimensions
    int numCovers{}; // Number of manhole covers

    // Data variables
    int doorNode{}, exitNode{}; // Door and exit nodes
    vector<int> manholeNodes{}; // Manhole nodes

    unordered_map<int, Node> graph; // Graph representation

    // Constructors
    // ================================================================================================
    Labyrinth(int numRows, int numCols) : numRows(numRows), numCols(numCols) {} // Default Constructor

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
        vector<string> grid(numRows);

        // Store everything first into a grid so we can check for valid adjacent cells
        for (int row = 0; row < numRows; ++row)
            cin >> grid[row];

        // Check all cells and build the graph
        for (int row = 0; row < numRows; ++row) {
            for (int col = 0; col < numCols; ++col) {
                char cell = grid[row][col];
                if (cell == '#') continue;

                int cellId = toId(row, col, numCols);
                int type = (cell == 'M') ? 1 : (cell == 'D') ? 2 : (cell == 'E') ? 3 : 0;
                Node node(type);

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
        if (graph.count(node) == 0) {
            return;
        }

        ids[node] = low[node] = id++;
        for (int neighbor : graph.at(node).neighbors) {
            if (ids[neighbor] == -1) {
                parent[neighbor] = node;
                dfs(neighbor, id, ids, low, parent);
                low[node] = min(low[node], low[neighbor]);
                if (low[neighbor] > ids[node]) {
                    cout << "Bridge found between nodes " << node << " and " << neighbor << endl;
                }
            } else if (neighbor != parent[node]) {
                low[node] = min(low[node], ids[neighbor]);
            }
        }
    }

    // Find bridges in the labyrinth
    void findBridges() {
        vector<int> ids(numRows * numCols, -1);
        vector<int> low(numRows * numCols, -1);
        vector<int> parent(numRows * numCols, -1);
        int id = 0;

        for (auto& entry : graph) {
            if (ids[entry.first] == -1) {
                dfs(entry.first, id, ids, low, parent);
            }
        }
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

        // Debug
        lab.printGrid();
        lab.printGraph();

        // Find bridges
        lab.findBridges();
    }

    return 0;
}