#include <iostream>
#include <vector>

struct Labyrinth {
    int rows, columns;
    std::vector<std::vector<int>> adjacencyList;

    Labyrinth(int rows, int columns) : rows(rows), columns(columns), adjacencyList(rows * columns) {}

    void read() {
        static const std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                char c;
                std::cin >> c;
                if (c != '#') {
                    int index = i * columns + j;
                    for (const auto& dir : directions) {
                        int ni = i + dir.first;
                        int nj = j + dir.second;
                        if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
                            adjacencyList[index].push_back(ni * columns + nj);
                        }
                    }
                }
            }
        }
    }

    void print() const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                int index = i * columns + j;
                std::cout << (adjacencyList[index].empty() ? "0 " : "1 ");
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // DFS
    void dfs(int node, std::vector<bool>& visited) {
        visited[node] = true;
        for (int neighbor : adjacencyList[node]) {
            if (!visited[neighbor]) {
                dfs(neighbor, visited);
            }
        }
    }

    // Count connected components
    int countConnectedComponents() {
        std::vector<bool> visited(rows * columns, false);
        int count = 0;
        for (int i = 0; i < rows * columns; i++) {
            if (!visited[i]) {
                dfs(i, visited);
                count++;
            }
        }
        return count;
    }

};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int nr_tests;
    std::cin >> nr_tests;

    for (int t = 0; t < nr_tests; t++) {
        int rows, columns;
        std::cin >> rows >> columns;
        Labyrinth lab(rows, columns);
        lab.read();
        lab.print();
        std::cout << lab.countConnectedComponents() << std::endl;
    }

    return 0;
}
