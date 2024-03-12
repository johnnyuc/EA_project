#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <climits>

const int MAX_R = 5;
const int MAX_C = 5;
int R, C, M;
std::vector<int> grid;
int target[MAX_R][MAX_C];
std::unordered_map<int, int> stateMoves;
int globalMinMoves = INT_MAX;

bool rotate(std::vector<int> &mat, int cols, int r, int c, bool clockwise) {
    int idx = r * cols + c;
    int idxRight = idx + 1;
    int idxBelow = idx + cols;
    int idxDiag = idxBelow + 1;

    int origTopLeft = mat[idx];
    int origTopRight = mat[idxRight];
    int origBottomLeft = mat[idxBelow];
    int origBottomRight = mat[idxDiag];

    if (clockwise) {

        mat[idx] = origBottomLeft;
        mat[idxRight] = origTopLeft;
        mat[idxBelow] = origBottomRight;
        mat[idxDiag] = origTopRight;
    } else {

        mat[idx] = origTopRight;
        mat[idxRight] = origBottomRight;
        mat[idxBelow] = origTopLeft;
        mat[idxDiag] = origBottomLeft;
    }


    return !(origTopLeft == mat[idx] && origTopRight == mat[idxRight] &&
             origBottomLeft == mat[idxBelow] && origBottomRight == mat[idxDiag]);
}



bool isExcludedMatrix(const std::vector<int> &grid, int rows, int cols)
{
    if (rows != 2 || cols != 2)
    {
        return false;
    }
    return grid[0] == 1 && grid[1] == 2 &&
           grid[2] == 2 && grid[3] == 1;
}





bool isSolved() {
    int row = 0, col = 0;
    for (int i = 0; i < R * C; ++i) {
        if (grid[i] != target[row][col])
            return false;


        col++;
        if (col == C) {
            col = 0;
            row++;
        }
    }
    return true;
}

int getStateHash(const std::vector<int> &grid)
{
    int hash = 0;
    for (int val : grid)
    {
        hash = hash * 6 + val;
    }
    return hash;
}

void blockRows(std::vector<bool>& blockedRows) {
    std::vector<bool> visited(R * C, false);

    for (int i = 0; i < R; i++) {
        bool isRowSorted = true;
        for (int j = 0; j < C; j++) {
            if (grid[i * C + j] != target[i][j]) {
                isRowSorted = false;
                break;
            }
        }
        if (isRowSorted) {
            for (int j = 0; j < C; j++) {
                visited[i * C + j] = true;
            }
        } else {
            break;
        }
    }

    for (int i = R - 1; i >= 0; i--) {
        if (visited[i * C]) continue;

        bool isRowSorted = true;
        for (int j = 0; j < C; j++) {
            if (grid[i * C + j] != target[i][j]) {
                isRowSorted = false;
                break;
            }
        }
        if (isRowSorted) {
            for (int j = 0; j < C; j++) {
                visited[i * C + j] = true;
            }
        } else {
            break;
        }
    }

    blockedRows = std::move(visited);
}

int dfs(int moves) {
    std::vector<bool> blockedRows(R * C, false);
    blockRows(blockedRows);

    if (moves > M || moves >= globalMinMoves)
        return -1;

    if (isSolved()) {
        globalMinMoves = moves;
        return moves;
    }

    int hash = getStateHash(grid);
    if (stateMoves.find(hash) != stateMoves.end() && stateMoves[hash] <= moves) {
        return -1;
    }
    stateMoves[hash] = moves;

    int minMoves = INT_MAX;
    int submatrixCount = (R - 1) * (C - 1);

    for (int i = 0; i < submatrixCount; ++i) {
        int r = i / (C - 1);
        int c = i % (C - 1);

        if (blockedRows[r * C + c] || blockedRows[(r + 1) * C + c] ||
            blockedRows[r * C + c + 1] || blockedRows[(r + 1) * C + c + 1]) {
            continue;
        }

        if (rotate(grid, C, r, c, true)) {
            int result = dfs(moves + 1);
            if (result != -1) {
                minMoves = std::min(minMoves, result);
            }
            rotate(grid, C, r, c, false);
        }

        if (rotate(grid, C, r, c, false)) {
            int result = dfs(moves + 1);
            if (result != -1) {
                minMoves = std::min(minMoves, result);
            }
            rotate(grid, C, r, c, true);
        }
    }
    return minMoves == INT_MAX ? -1 : minMoves;
}


bool isDiagonalEqual(std::vector<int> &mat, int cols, int r, int c)
{
    return mat[r * cols + c] == mat[(r + 1) * cols + c + 1] &&
           mat[r * cols + c + 1] == mat[(r + 1) * cols + c] &&
           mat[r * cols + c] != mat[r * cols + c + 1];
}



int estimateMinimumMoves(const std::vector<int> &mat, int rows, int cols) {
    int totalMoves = 0;
    for (int i = 0; i < mat.size(); ++i) {
        int currentValue = mat[i];
        int currentRow = i / cols ;
        int diff = currentValue - currentRow;

        if (diff > 0) {

            totalMoves += (diff - 1);
        } else if (diff < 0) {

            totalMoves += (-diff + 1);
        }

    }

    return totalMoves;
}




bool isPreprocessingPossible(std::vector<int> &mat, int rows, int cols)
{
    std::vector<int> count(rows + 1, 0);

    if (rows == 2 && cols == 2 && isDiagonalEqual(mat, cols, 0, 0))
    {
        return false;
    }


    for (int val : mat)
    {
        if (val < 1 || val > rows)
            return false;
        count[val]++;
    }

    for (int i = 1; i <= rows; ++i)
    {
        if (count[i] != cols)
        {
            return false;
        }
    }

    int estimatedMoves = estimateMinimumMoves(mat, rows, cols);
    if (estimatedMoves > M)
    {
        return false;
    }

    return true;
}

int main()
{
    int T;
    std::cin >> T;

    while (T--)
    {
        std::cin >> R >> C >> M;
        grid.resize(R * C);

        for (int r = 0; r < R; ++r)
        {
            for (int c = 0; c < C; ++c)
            {
                int num;
                std::cin >> num;
                grid[r * C + c] = num;
                target[r][c] = r + 1;
            }
        }

        if (isSolved())
        {
            std::cout << "0\n";
            continue;
        }

        if (!isPreprocessingPossible(grid, R, C))
        {
            std::cout << "the treasure is lost!\n";
            continue;
        }



        stateMoves.clear();
        globalMinMoves = INT_MAX;
        int result = dfs(0);
        if (result == -1)
        {
            std::cout << "the treasure is lost!\n";
        }
        else
        {
            std::cout << result << "\n";
        }
    }
    return 0;
}