# 8-Neighbor Diagonal Floodfill Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Enable the micromouse to move diagonally through the maze using 45-degree turns and half-steps, producing faster paths by cutting through staircases.

**Architecture:** Expand floodfill to 8 neighbors (NE/SE/SW/NW added). A diagonal move from cell (x,y) is valid only if both adjacent cardinal walls are open (e.g., NE requires no NORTH wall AND no EAST wall). The mouse discovers walls proactively each step (front/left/right) while facing cardinal directions, then the floodfill suggests diagonals once wall data confirms they're safe. Diagonal physical movement uses two `moveForwardHalf` calls.

**Tech Stack:** C, mms simulator API (stdin/stdout protocol)

---

## Key Design Decisions

1. **Diagonal cost = 1** (same as cardinal). The speed benefit comes from fewer turns, not shorter distance. Keeps `unsigned char` distances simple.
2. **Wall discovery stays cardinal.** The mouse checks front/left/right walls each step while facing a cardinal direction. Diagonals are only attempted when maze data confirms both adjacent walls are open.
3. **Diagonal validity:** NE from (x,y) requires `!IsThereAWall(x,y,NORTH) && !IsThereAWall(x,y,EAST)`. The adjacent cell must also be in-bounds.
4. **Turn logic:** `LEFT45`/`RIGHT45` are 1-step rotations in the 8-heading enum. `LEFT`/`RIGHT` remain 2-step (90-degree) rotations.
5. **Position tracking:** Diagonal headings update both x and y simultaneously. Movement uses `API_moveForwardHalf()` x2 instead of `API_moveForward()`.

## Coordinate & heading reference

```
Maze grid:  x = row (0=top, 15=bottom), y = column (0=left, 15=right)
Mouse starts at (15, 0) facing NORTH

Heading enum (already exists):
  NORTH=0, NORTHEAST=1, EAST=2, SOUTHEAST=3,
  SOUTH=4, SOUTHWEST=5, WEST=6, NORTHWEST=7

Direction deltas:
  NORTH:     dx=-1, dy= 0
  NORTHEAST: dx=-1, dy=+1
  EAST:      dx= 0, dy=+1
  SOUTHEAST: dx=+1, dy=+1
  SOUTH:     dx=+1, dy= 0
  SOUTHWEST: dx=+1, dy=-1
  WEST:      dx= 0, dy=-1
  NORTHWEST: dx=-1, dy=-1

Wall bits (already exists):
  NORTH=bit3(0x08), EAST=bit2(0x04), SOUTH=bit1(0x02), WEST=bit0(0x01)
```

## File Map

| File | Change | Responsibility |
|------|--------|----------------|
| `headers/API.h` | Modify | Add `API_moveForwardHalf` declaration |
| `src/API.c` | Modify | Add `API_moveForwardHalf` implementation |
| `headers/maze.h` | Modify | Add `CanMoveDiagonally` declaration |
| `src/maze.c` | Modify | 8-neighbor floodfill, diagonal traversal checks, expanded `GetNextMove` |
| `src/mouse.c` | Modify | Diagonal movement, 45-degree turns, proactive wall sensing |
| `src/utils.c` | Modify | Diagonal heading abbreviations and strings |
| `test/tests.c` | Modify | Tests for diagonal wall checks and floodfill |

---

### Task 1: Add `API_moveForwardHalf` wrapper

**Files:**
- Modify: `headers/API.h:10`
- Modify: `src/API.c:54-56`

- [ ] **Step 1: Add declaration to API.h**

In `headers/API.h`, add after the `API_moveForward` declaration:

```c
int API_moveForwardHalf();  // Returns 0 if crash, else returns 1
```

- [ ] **Step 2: Add implementation to API.c**

In `src/API.c`, add after the `API_moveForward` function:

```c
int API_moveForwardHalf() {
    return getAck("moveForwardHalf");
}
```

- [ ] **Step 3: Verify it compiles**

Run: `make rebuild`
Expected: compiles with no errors

- [ ] **Step 4: Commit**

```bash
git add headers/API.h src/API.c
git commit -m "feat: add API_moveForwardHalf wrapper for diagonal movement"
```

---

### Task 2: Add diagonal traversal check to maze

**Files:**
- Modify: `headers/maze.h`
- Modify: `src/maze.c`
- Modify: `test/tests.c`

- [ ] **Step 1: Write the failing test**

Add to `test/tests.c`:

```c
void testCanMoveDiagonally()
{
    struct Maze * maze = CreateMaze(16);
    maze->SetUpMaze(maze);

    // Interior cell (5,5) - no internal walls set, diagonals should be open
    assert(CanMoveDiagonally(maze, 5, 5, NORTHEAST) == 1);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHEAST) == 1);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHWEST) == 1);
    assert(CanMoveDiagonally(maze, 5, 5, NORTHWEST) == 1);

    // Set NORTH wall at (5,5) - blocks NE and NW
    maze->SetWall(maze, 5, 5, NORTH);
    assert(CanMoveDiagonally(maze, 5, 5, NORTHEAST) == 0);
    assert(CanMoveDiagonally(maze, 5, 5, NORTHWEST) == 0);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHEAST) == 1);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHWEST) == 1);

    // Set EAST wall at (5,5) - now NE blocked by both, SE also blocked
    maze->SetWall(maze, 5, 5, EAST);
    assert(CanMoveDiagonally(maze, 5, 5, NORTHEAST) == 0);
    assert(CanMoveDiagonally(maze, 5, 5, SOUTHEAST) == 0);

    // Corner cells - out of bounds diagonals
    assert(CanMoveDiagonally(maze, 0, 0, NORTHWEST) == 0);
    assert(CanMoveDiagonally(maze, 0, 0, NORTHEAST) == 0);
    assert(CanMoveDiagonally(maze, 15, 15, SOUTHEAST) == 0);

    // Cardinal headings should return 0 (not diagonal)
    assert(CanMoveDiagonally(maze, 5, 5, NORTH) == 0);
    assert(CanMoveDiagonally(maze, 5, 5, EAST) == 0);

    FreeMaze(maze);
}
```

Call it from `main()` in tests.c:

```c
int main()
{
    testMazeWall();
    testCanMoveDiagonally();
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `make test && make run_tests`
Expected: FAIL - `CanMoveDiagonally` undefined

- [ ] **Step 3: Add declaration to maze.h**

In `headers/maze.h`, add after the `IsThereAWall` declaration:

```c
unsigned char CanMoveDiagonally(struct Maze* maze, int x, int y, Heading heading);
```

- [ ] **Step 4: Implement CanMoveDiagonally in maze.c**

Add to `src/maze.c` after the `IsThereAWall` function:

```c
unsigned char CanMoveDiagonally(struct Maze* maze, int x, int y, Heading heading)
{
    int dx = 0, dy = 0;
    Heading wall1, wall2;

    switch (heading)
    {
        case NORTHEAST:
            dx = -1; dy = 1;
            wall1 = NORTH; wall2 = EAST;
            break;
        case SOUTHEAST:
            dx = 1; dy = 1;
            wall1 = SOUTH; wall2 = EAST;
            break;
        case SOUTHWEST:
            dx = 1; dy = -1;
            wall1 = SOUTH; wall2 = WEST;
            break;
        case NORTHWEST:
            dx = -1; dy = -1;
            wall1 = NORTH; wall2 = WEST;
            break;
        default:
            return 0;
    }

    int newX = x + dx;
    int newY = y + dy;
    if (newX < 0 || newX >= maze->mazeDimension || newY < 0 || newY >= maze->mazeDimension)
    {
        return 0;
    }

    if (maze->IsThereAWall(maze, x, y, wall1) || maze->IsThereAWall(maze, x, y, wall2))
    {
        return 0;
    }

    return 1;
}
```

- [ ] **Step 5: Run tests to verify they pass**

Run: `make test && make run_tests`
Expected: PASS (note: tests that call `SetUpMaze` will trigger API calls which need stdin - may need to pipe empty input or skip in unit test context. If tests hang, pipe `/dev/null`: `echo "" | make run_tests` or mock API calls.)

- [ ] **Step 6: Commit**

```bash
git add headers/maze.h src/maze.c test/tests.c
git commit -m "feat: add CanMoveDiagonally check for 8-neighbor traversal"
```

---

### Task 3: Expand floodfill to 8 neighbors

**Files:**
- Modify: `src/maze.c:79-160` (`SetUpInitialDistances`)
- Modify: `src/maze.c:324-397` (`UpdateMaze`)

- [ ] **Step 1: Define direction delta arrays**

Add at the top of `src/maze.c`, after the includes:

```c
static const int DX[8] = {-1, -1,  0,  1,  1,  1,  0, -1};
static const int DY[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
```

These map `Heading` enum values (NORTH=0 through NORTHWEST=7) to coordinate deltas.

- [ ] **Step 2: Add helper to check if a neighbor is reachable**

Add to `src/maze.c` after `CanMoveDiagonally`:

```c
static unsigned char CanMoveInDirection(struct Maze* maze, int x, int y, Heading heading)
{
    int newX = x + DX[heading];
    int newY = y + DY[heading];

    if (newX < 0 || newX >= maze->mazeDimension || newY < 0 || newY >= maze->mazeDimension)
    {
        return 0;
    }

    switch (heading)
    {
        case NORTH:
        case EAST:
        case SOUTH:
        case WEST:
            return maze->IsThereAWall(maze, x, y, heading) == 0;
        default:
            return CanMoveDiagonally(maze, x, y, heading);
    }
}
```

- [ ] **Step 3: Rewrite SetUpInitialDistances with 8 neighbors**

Replace the entire `SetUpInitialDistances` function in `src/maze.c`:

```c
void SetUpInitialDistances(struct Maze * maze)
{
    struct Queue* q = QueueInit(255);
    int midPoint = maze->mazeDimension/2;

    q->QueueEnqueue(q, GetLocationFromCoordinates(midPoint, midPoint));
    q->QueueEnqueue(q, GetLocationFromCoordinates(midPoint, midPoint-1));
    q->QueueEnqueue(q, GetLocationFromCoordinates(midPoint-1, midPoint));
    q->QueueEnqueue(q, GetLocationFromCoordinates(midPoint-1, midPoint-1));

    while (q->QueueIsEmpty(q) == 0)
    {
        struct Location * loc = q->QueueDequeue(q);
        unsigned char newDist = maze->maze[loc->x][loc->y] + 1;

        for (int h = 0; h < NUM_HEADINGS; ++h)
        {
            if (CanMoveInDirection(maze, loc->x, loc->y, (Heading)h))
            {
                int nx = loc->x + DX[h];
                int ny = loc->y + DY[h];
                if (maze->maze[nx][ny] == 255)
                {
                    q->QueueEnqueue(q, GetLocationFromCoordinates(nx, ny));
                }
                if (maze->maze[nx][ny] > newDist)
                {
                    maze->SetCellDistance(maze, nx, ny, newDist);
                }
            }
        }

        free(loc);
    }
    free(q);
}
```

- [ ] **Step 4: Rewrite UpdateMaze with 8 neighbors**

Replace the entire `UpdateMaze` function in `src/maze.c`:

```c
void UpdateMaze(struct Maze * maze, int x, int y)
{
    struct Queue * q = QueueInit(255);
    q->QueueEnqueue(q, GetLocationFromCoordinates(x, y));

    while(q->QueueIsEmpty(q) == 0)
    {
        struct Location * loc = q->QueueDequeue(q);

        struct Location * accessibleNeighbors[8];
        int neighborIndex = 0;

        for (int h = 0; h < NUM_HEADINGS; ++h)
        {
            if (CanMoveInDirection(maze, loc->x, loc->y, (Heading)h))
            {
                int nx = loc->x + DX[h];
                int ny = loc->y + DY[h];
                accessibleNeighbors[neighborIndex++] = GetLocationFromCoordinates(nx, ny);
            }
        }

        if (neighborIndex > 0)
        {
            unsigned char min = 255;
            for(int i = 0; i < neighborIndex; ++i)
            {
                if (maze->maze[accessibleNeighbors[i]->x][accessibleNeighbors[i]->y] <= min)
                {
                    min = maze->maze[accessibleNeighbors[i]->x][accessibleNeighbors[i]->y];
                }
            }

            if (maze->maze[loc->x][loc->y] <= min)
            {
                maze->SetCellDistance(maze, loc->x, loc->y, min+1);
                for(int i = 0; i < neighborIndex; ++i)
                {
                    q->QueueEnqueue(q, accessibleNeighbors[i]);
                }
            }
            else
            {
                for(int i = 0; i < neighborIndex; ++i)
                {
                    free(accessibleNeighbors[i]);
                }
            }
        }
        free(loc);
    }
    FreeQueue(q);
}
```

- [ ] **Step 5: Verify it compiles**

Run: `make rebuild`
Expected: compiles with no errors

- [ ] **Step 6: Commit**

```bash
git add src/maze.c
git commit -m "feat: expand floodfill to 8 neighbors for diagonal pathfinding"
```

---

### Task 4: Expand GetNextMove for 8 directions

**Files:**
- Modify: `src/maze.c:254-321` (`GetNextMove`)

- [ ] **Step 1: Rewrite GetNextMove with 8-neighbor search**

Replace the entire `GetNextMove` function in `src/maze.c`:

```c
Action GetNextMove(struct Maze* maze, int x, int y, Heading heading)
{
    if (maze->maze[x][y] == 0)
    {
        return IDLE;
    }

    Heading newHeading = heading;
    int dist = 255;

    for (int h = 0; h < NUM_HEADINGS; ++h)
    {
        if (CanMoveInDirection(maze, x, y, (Heading)h))
        {
            int nx = x + DX[h];
            int ny = y + DY[h];
            if (maze->maze[nx][ny] < dist)
            {
                newHeading = (Heading)h;
                dist = maze->maze[nx][ny];
            }
        }
    }

    if (heading == newHeading)
    {
        return FORWARD;
    }

    char rightTurns = (newHeading - heading + NUM_HEADINGS) % NUM_HEADINGS;
    char leftTurns = (heading - newHeading + NUM_HEADINGS) % NUM_HEADINGS;

    if (rightTurns == 1) return RIGHT45;
    if (leftTurns == 1) return LEFT45;
    return leftTurns <= rightTurns ? LEFT : RIGHT;
}
```

- [ ] **Step 2: Verify it compiles**

Run: `make rebuild`
Expected: compiles with no errors

- [ ] **Step 3: Commit**

```bash
git add src/maze.c
git commit -m "feat: GetNextMove considers 8 directions and returns 45-degree turn actions"
```

---

### Task 5: Update mouse movement for diagonals and 45-degree turns

**Files:**
- Modify: `src/mouse.c`

- [ ] **Step 1: Update MoveForward for diagonal headings**

Replace the `MoveForward` function in `src/mouse.c`:

```c
void MoveForward(struct Mouse * mouse)
{
    switch (mouse->heading)
    {
        case NORTH:
            API_moveForward();
            mouse->location.x += -1;
            break;
        case EAST:
            API_moveForward();
            mouse->location.y += 1;
            break;
        case SOUTH:
            API_moveForward();
            mouse->location.x += 1;
            break;
        case WEST:
            API_moveForward();
            mouse->location.y += -1;
            break;
        case NORTHEAST:
            API_moveForwardHalf();
            API_moveForwardHalf();
            mouse->location.x += -1;
            mouse->location.y += 1;
            break;
        case SOUTHEAST:
            API_moveForwardHalf();
            API_moveForwardHalf();
            mouse->location.x += 1;
            mouse->location.y += 1;
            break;
        case SOUTHWEST:
            API_moveForwardHalf();
            API_moveForwardHalf();
            mouse->location.x += 1;
            mouse->location.y += -1;
            break;
        case NORTHWEST:
            API_moveForwardHalf();
            API_moveForwardHalf();
            mouse->location.x += -1;
            mouse->location.y += -1;
            break;
    }
}
```

- [ ] **Step 2: Update TakeAction for LEFT45, RIGHT45, and diagonal FORWARD**

Replace the `TakeAction` function in `src/mouse.c`:

```c
void TakeAction(struct Mouse * mouse, Action action)
{
    switch (action)
    {
        case FORWARD:
        {
            Heading h = mouse->heading;
            if (h == NORTHEAST || h == SOUTHEAST || h == SOUTHWEST || h == NORTHWEST)
            {
                if (CanMoveDiagonally(mouse->maze, mouse->location.x, mouse->location.y, h))
                {
                    mouse->MoveForward(mouse);
                }
                else
                {
                    mouse->maze->UpdateMaze(mouse->maze, mouse->location.x, mouse->location.y);
                }
            }
            else
            {
                if (mouse->CanMoveForward(mouse) == 0)
                {
                    mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, mouse->heading);
                    mouse->maze->UpdateMaze(mouse->maze, mouse->location.x, mouse->location.y);
                }
                else
                {
                    mouse->MoveForward(mouse);
                }
            }
            break;
        }
        case LEFT:
            mouse->TurnLeft(mouse);
            break;
        case RIGHT:
            mouse->TurnRight(mouse);
            break;
        case LEFT45:
            mouse->TurnLeft45(mouse);
            break;
        case RIGHT45:
            mouse->TurnRight45(mouse);
            break;
        default:
            break;
    }
}
```

- [ ] **Step 3: Add TurnLeft45 and TurnRight45 functions**

Add to `src/mouse.c` after `TurnRight`:

```c
void TurnLeft45(struct Mouse * mouse)
{
    API_turnLeft45();
    mouse->heading = ComputeModulo((int)mouse->heading - 1, NUM_HEADINGS);
}

void TurnRight45(struct Mouse * mouse)
{
    API_turnRight45();
    mouse->heading = ComputeModulo((int)mouse->heading + 1, NUM_HEADINGS);
}
```

- [ ] **Step 4: Add TurnLeft45/TurnRight45 declarations and function pointers**

In `headers/mouse.h`, add declarations:

```c
void TurnLeft45(struct Mouse * mouse);
void TurnRight45(struct Mouse * mouse);
```

Add function pointers to the `Mouse` struct in `headers/mouse.h`:

```c
void(*TurnLeft45)(struct Mouse * mouse);
void(*TurnRight45)(struct Mouse * mouse);
```

In `CreateMouse` in `src/mouse.c`, add the assignments:

```c
mouse->TurnLeft45 = &TurnLeft45;
mouse->TurnRight45 = &TurnRight45;
```

- [ ] **Step 5: Verify it compiles**

Run: `make rebuild`
Expected: compiles with no errors

- [ ] **Step 6: Commit**

```bash
git add headers/mouse.h src/mouse.c
git commit -m "feat: add diagonal movement and 45-degree turns to mouse"
```

---

### Task 6: Add proactive wall sensing to main loop

**Files:**
- Modify: `src/main.c`
- Modify: `src/mouse.c`

The mouse currently only discovers walls by bumping forward. Adding left/right wall checks each step gives the floodfill better data, which is critical for diagonal decisions.

- [ ] **Step 1: Add SenseWalls function to mouse**

Add to `src/mouse.c`:

```c
void SenseWalls(struct Mouse * mouse)
{
    Heading h = mouse->heading;
    if (h == NORTH || h == EAST || h == SOUTH || h == WEST)
    {
        mouse->CheckWallLeft(mouse);
        mouse->CheckWallRight(mouse);
        if (API_wallFront())
        {
            mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, h);
        }
    }
}
```

Add declaration to `headers/mouse.h`:

```c
void SenseWalls(struct Mouse * mouse);
```

Add function pointer to `Mouse` struct and assignment in `CreateMouse`:

```c
// In struct Mouse:
void (*SenseWalls)(struct Mouse * mouse);

// In CreateMouse:
mouse->SenseWalls = &SenseWalls;
```

- [ ] **Step 2: Fix CheckWallLeft/CheckWallRight for heading arithmetic**

The current code uses `mouse->heading - 2` and `mouse->heading + 2` which can underflow/overflow. Replace both functions in `src/mouse.c`:

```c
void CheckWallLeft(struct Mouse * mouse)
{
    if (API_wallLeft())
    {
        Heading wallDir = ComputeModulo((int)mouse->heading - 2, NUM_HEADINGS);
        if (wallDir == NORTH || wallDir == EAST || wallDir == SOUTH || wallDir == WEST)
        {
            mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, wallDir);
        }
    }
}

void CheckWallRight(struct Mouse * mouse)
{
    if (API_wallRight())
    {
        Heading wallDir = ComputeModulo((int)mouse->heading + 2, NUM_HEADINGS);
        if (wallDir == NORTH || wallDir == EAST || wallDir == SOUTH || wallDir == WEST)
        {
            mouse->maze->SetWall(mouse->maze, mouse->location.x, mouse->location.y, wallDir);
        }
    }
}
```

- [ ] **Step 3: Update main loop to sense walls each iteration**

Replace the main loop in `src/main.c`:

```c
int main(int argc, char* argv[]) {
    struct Mouse* mouse = CreateMouse(16);
    mouse->SetUpMouse(mouse);

    debug_log("Running...");
    Action nextMove = IDLE;
    do
    {
        mouse->DebugMouseState(mouse);
        mouse->SenseWalls(mouse);
        nextMove = mouse->GetNextAction(mouse);
        mouse->TakeAction(mouse, nextMove);
    } while (nextMove != IDLE);

    FreeMouse(mouse);
}
```

- [ ] **Step 4: Verify it compiles**

Run: `make rebuild`
Expected: compiles with no errors

- [ ] **Step 5: Commit**

```bash
git add headers/mouse.h src/mouse.c src/main.c
git commit -m "feat: add proactive wall sensing for safer diagonal navigation"
```

---

### Task 7: Update utility functions for diagonal headings

**Files:**
- Modify: `src/utils.c`

- [ ] **Step 1: Update GetHeadingAbbreviation**

Replace the function in `src/utils.c`:

```c
char GetHeadingAbbreviation(Heading heading)
{
    switch (heading)
    {
        case NORTH:     return 'n';
        case EAST:      return 'e';
        case SOUTH:     return 's';
        case WEST:      return 'w';
        default:        return '?';
    }
}
```

No change needed - diagonal headings don't have single-char wall abbreviations (walls are cardinal only). Keep returning `'?'` for diagonals.

- [ ] **Step 2: Update GetHeadingStr**

Replace the function in `src/utils.c`:

```c
char* GetHeadingStr(Heading heading)
{
    switch (heading)
    {
        case NORTH:     return "NORTH";
        case NORTHEAST: return "NORTHEAST";
        case EAST:      return "EAST";
        case SOUTHEAST: return "SOUTHEAST";
        case SOUTH:     return "SOUTH";
        case SOUTHWEST: return "SOUTHWEST";
        case WEST:      return "WEST";
        case NORTHWEST: return "NORTHWEST";
        default:        return "?";
    }
}
```

- [ ] **Step 3: Verify it compiles**

Run: `make rebuild`
Expected: compiles with no errors

- [ ] **Step 4: Commit**

```bash
git add src/utils.c
git commit -m "feat: add diagonal heading strings for debug output"
```

---

### Task 8: Integration test in simulator

**Files:** None (manual testing)

- [ ] **Step 1: Build the project**

Run: `make rebuild`
Expected: compiles with no errors or warnings

- [ ] **Step 2: Run in mms simulator**

Launch the mms simulator, configure it to use `./bin/micromouse` as the mouse algorithm. Run and observe:
- Mouse should explore cardinally at first (discovering walls)
- As walls are mapped, mouse should start taking diagonal shortcuts
- Distance values displayed in cells should update correctly
- Mouse should reach the center goal cells (7,7)/(7,8)/(8,7)/(8,8)

- [ ] **Step 3: Verify diagonal behavior**

Watch for:
- 45-degree turns followed by half-step diagonal movement
- No crashes (mouse never attempts diagonal through a wall)
- Distance values remain consistent after wall discoveries

- [ ] **Step 4: Final commit if any fixes needed**

```bash
git add -A
git commit -m "fix: integration fixes for diagonal floodfill"
```
