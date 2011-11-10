$(CXX):=g++

ifeq ($(DEBUG), yes)
    CXXFLAGS:= -Wall -O0 -g -c
else
    CXXFLAGS:= -Wall -O3 -c
endif

SRC_DIR:=source
BIN_DIR:=bin
OBJ_DIR:=obj
EXT_DIR:=external

OUTPUT := $(BIN_DIR)/PlimDP

CPP_FILES := \
	$(SRC_DIR)/memory.cpp \
	$(SRC_DIR)/core.cpp \
	$(SRC_DIR)/coredump.cpp \
	$(SRC_DIR)/main.cpp
	
OBJS_FILES:=${CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o} 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

asm: $(EXT_DIR)/as11
#$(CC) $< $(CFLAGS) -o $(BIN_DIR)/$@
	cp $< $(BIN_DIR)/$@

pdp11: $(EXT_DIR)/pdp11
#$(CC) $< $(CFLAGS) -o $(BIN_DIR)/$@
	cp $< $(BIN_DIR)/$@
    
plimdp: $(OBJS_FILES)
	$(CXX) $(OBJS_FILES) -o $(OUTPUT)

legacy: $(SRC_DIR)/legacy/PlimDP.c
	gcc -Wall -O3 $< -o $(BIN_DIR)/$@ 

all: build_dirs plimdp asm pdp11

build_dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
    
clean:
	rm -rf $(OBJ_DIR)  
	rm -rf $(BIN_DIR)

.PHONY: clean
