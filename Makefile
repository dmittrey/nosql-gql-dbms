EXEC 		:= app
EXEC_TEST	:= test

SRC_DIR := src
BLD_DIR := build
BIN_DIR := ${BLD_DIR}/bin
INC_DIR	:= include
DOC_DIR := doc
TEST_DIR:= tests

SRC  	:= $(wildcard $(SRC_DIR)/*.c)
OBJS	:= $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o, $(SRC))

TESTS		:= $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS	:= $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/%.o, $(TESTS))

.PHONY: dirs build clean

all: dirs build
	
dirs:
	@mkdir -p $(BLD_DIR)
	@mkdir -p ${BIN_DIR}
	@mkdir -p $(SRC_DIR)
	@mkdir -p $(INC_DIR)
	@mkdir -p $(DOC_DIR)

build: $(OBJS) | dirs
	gcc -g $^ -o ${BLD_DIR}/${EXEC}.out

test: $(OBJS) $(TEST_OBJS)
	gcc -g $^ -o ${BLD_DIR}/${EXEC_TEST}.out

$(BIN_DIR)/%.o: $(TEST_DIR)/%.c 
	gcc -g -I $(INC_DIR) -c $< -o $@
	
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c 
	gcc -g -I $(INC_DIR) -c $< -o $@

clean:
	rm -rf $(BLD_DIR) 
	rm -rf *.out