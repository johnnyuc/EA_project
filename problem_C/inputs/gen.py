import random
import sys

def printMaze(maze):
    for row in maze:
        print(''.join(row))

def surroundingCells(maze, rand_wall):
    s_cells = 0
    if maze[rand_wall[0]-1][rand_wall[1]] == '.':
        s_cells += 1
    if maze[rand_wall[0]+1][rand_wall[1]] == '.':
        s_cells += 1
    if maze[rand_wall[0]][rand_wall[1]-1] == '.':
        s_cells +=1
    if maze[rand_wall[0]][rand_wall[1]+1] == '.':
        s_cells += 1

    return s_cells

def generateMaze(height, width):
    wall = '#'
    cell = '.'
    unvisited = 'u'

    maze = []

    # Denote all cells as unvisited
    for i in range(height):
        line = []
        for j in range(width):
            line.append(unvisited)
        maze.append(line)

    # Randomize starting point and set it a cell
    starting_height = random.randint(1, height-2)
    starting_width = random.randint(1, width-2)

    # Mark it as cell and add surrounding walls to the list
    maze[starting_height][starting_width] = cell
    walls = []
    walls.append([starting_height - 1, starting_width])
    walls.append([starting_height, starting_width - 1])
    walls.append([starting_height, starting_width + 1])
    walls.append([starting_height + 1, starting_width])

    # Denote walls in maze
    maze[starting_height-1][starting_width] = wall
    maze[starting_height][starting_width - 1] = wall
    maze[starting_height][starting_width + 1] = wall
    maze[starting_height + 1][starting_width] = wall

    while walls:
        # Pick a random wall
        rand_wall = walls[random.randint(0, len(walls)-1)]

        # Check if it is a left wall
        if rand_wall[1] != 0:
            if maze[rand_wall[0]][rand_wall[1]-1] == 'u' and maze[rand_wall[0]][rand_wall[1]+1] == '.':
                s_cells = surroundingCells(maze, rand_wall)

                if s_cells < 2:
                    maze[rand_wall[0]][rand_wall[1]] = '.'

                    # Mark the new walls
                    if rand_wall[0] != 0:
                        if maze[rand_wall[0]-1][rand_wall[1]] != '.':
                            maze[rand_wall[0]-1][rand_wall[1]] = wall
                        if [rand_wall[0]-1, rand_wall[1]] not in walls:
                            walls.append([rand_wall[0]-1, rand_wall[1]])

                    if rand_wall[0] != height-1:
                        if maze[rand_wall[0]+1][rand_wall[1]] != '.':
                            maze[rand_wall[0]+1][rand_wall[1]] = wall
                        if [rand_wall[0]+1, rand_wall[1]] not in walls:
                            walls.append([rand_wall[0]+1, rand_wall[1]])

                    if rand_wall[1] != 0:    
                        if maze[rand_wall[0]][rand_wall[1]-1] != '.':
                            maze[rand_wall[0]][rand_wall[1]-1] = wall
                        if [rand_wall[0], rand_wall[1]-1] not in walls:
                            walls.append([rand_wall[0], rand_wall[1]-1])
                
                walls.remove(rand_wall)

                continue

        # Check if it is an upper wall
        if rand_wall[0] != 0:
            if maze[rand_wall[0]-1][rand_wall[1]] == 'u' and maze[rand_wall[0]+1][rand_wall[1]] == '.':

                s_cells = surroundingCells(maze, rand_wall)
                if s_cells < 2:
                    maze[rand_wall[0]][rand_wall[1]] = '.'

                    if rand_wall[0] != 0:
                        if maze[rand_wall[0]-1][rand_wall[1]] != '.':
                            maze[rand_wall[0]-1][rand_wall[1]] = wall
                        if [rand_wall[0]-1, rand_wall[1]] not in walls:
                            walls.append([rand_wall[0]-1, rand_wall[1]])

                    if rand_wall[1] != 0:
                        if maze[rand_wall[0]][rand_wall[1]-1] != '.':
                            maze[rand_wall[0]][rand_wall[1]-1] = wall
                        if [rand_wall[0], rand_wall[1]-1] not in walls:
                            walls.append([rand_wall[0], rand_wall[1]-1])

                    if rand_wall[1] != width-1:
                        if maze[rand_wall[0]][rand_wall[1]+1] != '.':
                            maze[rand_wall[0]][rand_wall[1]+1] = wall
                        if [rand_wall[0], rand_wall[1]+1] not in walls:
                            walls.append([rand_wall[0], rand_wall[1]+1])

                walls.remove(rand_wall)

                continue

        # Check the bottom wall
        if rand_wall[0] != height-1:
            if maze[rand_wall[0]+1][rand_wall[1]] == 'u' and maze[rand_wall[0]-1][rand_wall[1]] == '.':

                s_cells = surroundingCells(maze, rand_wall)
                if s_cells < 2:
                    maze[rand_wall[0]][rand_wall[1]] = '.'

                    if rand_wall[0] != height-1:
                        if maze[rand_wall[0]+1][rand_wall[1]] != '.':
                            maze[rand_wall[0]+1][rand_wall[1]] = wall
                        if [rand_wall[0]+1, rand_wall[1]] not in walls:
                            walls.append([rand_wall[0]+1, rand_wall[1]])
                    if rand_wall[1] != 0:
                        if maze[rand_wall[0]][rand_wall[1]-1] != '.':
                            maze[rand_wall[0]][rand_wall[1]-1] = wall
                        if [rand_wall[0], rand_wall[1]-1] not in walls:
                            walls.append([rand_wall[0], rand_wall[1]-1])
                    if rand_wall[1] != width-1:
                        if maze[rand_wall[0]][rand_wall[1]+1] != '.':
                            maze[rand_wall[0]][rand_wall[1]+1] = wall
                        if [rand_wall[0], rand_wall[1]+1] not in walls:
                            walls.append([rand_wall[0], rand_wall[1]+1])

                walls.remove(rand_wall)

                continue

        # Check the right wall
        if rand_wall[1] != width-1:
            if maze[rand_wall[0]][rand_wall[1]+1] == 'u' and maze[rand_wall[0]][rand_wall[1]-1] == '.':

                s_cells = surroundingCells(maze, rand_wall)
                if s_cells < 2:
                    maze[rand_wall[0]][rand_wall[1]] = '.'

                    if rand_wall[1] != width-1:
                        if maze[rand_wall[0]][rand_wall[1]+1] != '.':
                            maze[rand_wall[0]][rand_wall[1]+1] = wall
                        if [rand_wall[0], rand_wall[1]+1] not in walls:
                            walls.append([rand_wall[0], rand_wall[1]+1])
                    if rand_wall[0] != height-1:
                        if maze[rand_wall[0]+1][rand_wall[1]] != '.':
                            maze[rand_wall[0]+1][rand_wall[1]] = wall
                        if [rand_wall[0]+1, rand_wall[1]] not in walls:
                            walls.append([rand_wall[0]+1, rand_wall[1]])
                    if rand_wall[0] != 0:    
                        if maze[rand_wall[0]-1][rand_wall[1]] != '.':
                            maze[rand_wall[0]-1][rand_wall[1]] = wall
                        if [rand_wall[0]-1, rand_wall[1]] not in walls:
                            walls.append([rand_wall[0]-1, rand_wall[1]])

                walls.remove(rand_wall)

                continue

        walls.remove(rand_wall)
        

    # Mark the remaining unvisited cells as walls
    for i in range(height):
        for j in range(width):
            if maze[i][j] == 'u':
                maze[i][j] = wall

    # Set entrance (D) and exit (E)
    entrance_found = False
    exit_found = False
    for i in range(width):
        if not entrance_found and maze[1][i] == '.':
            maze[0][i] = 'E'
            entrance_found = True
        if not exit_found and maze[height-2][i] == '.':
            maze[height-1][i] = 'D'
            exit_found = True
        if entrance_found and exit_found:
            break

    # Place random 'M' values
    num_m = random.randint(1, height * width // 10)
    m_placed = 0

    while m_placed < num_m:
        row = random.randint(0, height - 1)
        col = random.randint(0, width - 1)
        if maze[row][col] == '.':
            maze[row][col] = 'M'
            m_placed += 1

    return maze, num_m

## Main code
if len(sys.argv) != 6:
    print("Usage: python gen3.py <num_cases> <min_height> <max_height> <min_width> <max_width>")
    sys.exit(1)

try:
    num_cases = int(sys.argv[1])
    min_height = int(sys.argv[2])
    max_height = int(sys.argv[3])
    min_width = int(sys.argv[4])
    max_width = int(sys.argv[5])
except ValueError:
    print("Invalid arguments. Please provide integers for num_cases, min_height, max_height, min_width, and max_width.")
    sys.exit(1)

print(num_cases)

for _ in range(num_cases):
    height = random.randint(min_height, max_height)
    width = random.randint(min_width, max_width)
    maze, num_m = generateMaze(height, width)
    print(f"{height} {width}")
    printMaze(maze)
    print(int(num_m * 0.9))
