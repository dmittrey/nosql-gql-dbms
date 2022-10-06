EXEC 	:= app

SRC_DIR := src
BLD_DIR := build
INC_DIR	:= include
DOC_DIR := doc

SRC  	:= $(wildcard $(SRC_DIR)/*.c)
OBJS	:= $(patsubst $(SRC_DIR)/%.c,$(BLD_DIR)/%.o, $(SRC))

.PHONY: dirs build clean

all: build
	
dirs:
	@mkdir -p $(BLD_DIR)
	@mkdir -p $(SRC_DIR)
	@mkdir -p $(INC_DIR)
	@mkdir -p $(DOC_DIR)

build: $(OBJS) | dirs
	gcc $^ -o ${BLD_DIR}/${EXEC}.out

$(BLD_DIR)/%.o: $(SRC_DIR)/%.c 
	gcc -I $(INC_DIR) -c $< -o $@

clean:
	rm -rf $(BLD_DIR) 
	rm -rf *.out