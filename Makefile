CC = gcc
CFLAGS = -I ./headers
SRC_DIR = src
TEST_DIR = test
OBJ_DIR = bin
PROJ_NAME = micromouse
TEST_NAME = tester
RM = rm

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

TEST_SRC_FILES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.o,$(TEST_SRC_FILES))

SRC_FILES_NO_MAIN = $(filter-out $(SRC_DIR)/main.c, $(SRC_FILES))
OBJ_FILES_NO_MAIN = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES_NO_MAIN))

rebuild: clean all

all: $(OBJ_DIR)/$(PROJ_NAME)

$(OBJ_DIR)/$(PROJ_NAME): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@  

test: $(OBJ_DIR)/$(TEST_NAME)

$(OBJ_DIR)/$(TEST_NAME): $(OBJ_FILES_NO_MAIN) $(TEST_OBJ_FILES)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@  

#Compile .c files in /sc
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

#Compile .c files in /test
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR) 

.PHONY: clean
clean: 
	$(RM) -f ./$(OBJ_DIR)/*

.PHONY: run
run: 
	./$(OBJ_DIR)/$(PROJ_NAME)

.PHONY: srun
srun: 
	sudo ./$(OBJ_DIR)/$(PROJ_NAME)

.PHONY: run_test
run_test: 
	./$(OBJ_DIR)/$(TEST_NAME)
