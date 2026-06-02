CC = gcc
CFLAGS = -I ./headers
SRC_DIR = src
TEST_DIR = test
OBJ_DIR = bin
RM = rm

# Shared sources (API, queue, main)
SHARED_SRC = $(SRC_DIR)/main.c $(SRC_DIR)/API.c $(SRC_DIR)/queue.c
SHARED_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SHARED_SRC))

# Cardinal solver
CARDINAL_SRC = $(SRC_DIR)/maze_cardinal.c $(SRC_DIR)/mouse_cardinal.c $(SRC_DIR)/utils_cardinal.c
CARDINAL_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(CARDINAL_SRC))

# Diagonal solver
DIAGONAL_SRC = $(SRC_DIR)/maze_diagonal.c $(SRC_DIR)/mouse_diagonal.c $(SRC_DIR)/utils_diagonal.c
DIAGONAL_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(DIAGONAL_SRC))

# Test sources
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ = $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.o,$(TEST_SRC))

.PHONY: all cardinal diagonal test clean run_cardinal run_diagonal

all: cardinal diagonal

cardinal: $(OBJ_DIR)/cardinal

diagonal: $(OBJ_DIR)/diagonal

$(OBJ_DIR)/cardinal: $(SHARED_OBJ) $(CARDINAL_OBJ) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/diagonal: $(SHARED_OBJ) $(DIAGONAL_OBJ) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $^ -o $@

test: $(OBJ_DIR)/tester

$(OBJ_DIR)/tester: $(DIAGONAL_OBJ) $(OBJ_DIR)/API.o $(OBJ_DIR)/queue.o $(TEST_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) -f ./$(OBJ_DIR)/*

run_cardinal:
	./$(OBJ_DIR)/cardinal

run_diagonal:
	./$(OBJ_DIR)/diagonal
