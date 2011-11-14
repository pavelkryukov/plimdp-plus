CXX:=g++
CXXFLAGS:= -Wall

ifeq ($(DEBUG), 1)
  ifeq ($(PROFILE), 1)
    CXXFLAGS:= $(CXXFLAGS) -pg
    LDFLAGS:= -pg
  endif
    CXXFLAGS:= $(CXXFLAGS) -O0 -g -c
else
    CXXFLAGS:= $(CXXFLAGS) -O3 -c
endif

ifeq ($(TRACE), 1)
    CXXFLAGS:= $(CXXFLAGS) -DENABLE_TRACE=1
endif

SRC_DIR:=source
BIN_DIR:=bin
OBJ_DIR:=obj

OUTPUT := $(BIN_DIR)/PlimDP

CPP_FILES := \
	$(SRC_DIR)/memory.cpp \
	$(SRC_DIR)/bus.cpp \
	$(SRC_DIR)/core.cpp \
	$(SRC_DIR)/coredump.cpp \
	$(SRC_DIR)/register.cpp	\
	$(SRC_DIR)/isa.cpp	\
	$(SRC_DIR)/main.cpp
    
ifeq ($(DISASM), 1)
    CXXFLAGS:= $(CXXFLAGS) -DENABLE_DISASM=1
    CPP_FILES:= $(CPP_FILES) $(SRC_DIR)/disassembler.cpp
endif
	
OBJS_FILES:=${CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o} 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

plimdp: $(OBJS_FILES)
	$(CXX) $(LDFLAGS) $(OBJS_FILES) -o $(OUTPUT)

legacy: $(SRC_DIR)/legacy/PlimDP.c
	gcc -Wall -O3 $< -o $(BIN_DIR)/$@ 

all: build_dirs plimdp

build_dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
    
clean:
	rm -rf $(OBJ_DIR)  
	rm -rf $(BIN_DIR)

.PHONY: clean
