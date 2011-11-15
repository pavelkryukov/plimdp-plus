CXX:=g++
CXXFLAGS:= -Wall
LDFLAGS := -Wall

ifeq ($(DEBUG), 1)
  ifeq ($(PROFILE), 1)
    CXXFLAGS:= $(CXXFLAGS) -pg
    LDFLAGS:= -pg
  endif
    CXXFLAGS:= $(CXXFLAGS) -O0 -g -c
else
    CXXFLAGS:= $(CXXFLAGS) -O3 -c
endif

SRC_DIR:=source
BIN_DIR:=bin
OBJ_DIR:=obj

CXXFLAGS:= $(CXXFLAGS) -I $(SRC_DIR)

OUTPUT := $(BIN_DIR)/PlimDP

CPP_FILES := \
	$(SRC_DIR)/PlimDP/devices/memory.cpp \
	$(SRC_DIR)/PlimDP/devices/loader.cpp \
	$(SRC_DIR)/PlimDP/cpu/bus.cpp \
	$(SRC_DIR)/PlimDP/cpu/core.cpp \
	$(SRC_DIR)/PlimDP/cpu/executor.cpp \
	$(SRC_DIR)/PlimDP/cpu/decoder.cpp \
	$(SRC_DIR)/PlimDP/cpu/register.cpp	\
	$(SRC_DIR)/PlimDP/cpu/isa.cpp	\
	$(SRC_DIR)/main.cpp

ifeq ($(DISASM), 1)
    CXXFLAGS:= $(CXXFLAGS) -DENABLE_DISASM=1
    CPP_FILES:= $(CPP_FILES) $(SRC_DIR)/PlimDP/cpu/tracing/disassembler.cpp
endif

ifeq ($(TRACE), 1)
    CXXFLAGS:= $(CXXFLAGS) -DENABLE_TRACE=1
    CPP_FILES:= $(CPP_FILES) $(SRC_DIR)/PlimDP/cpu/tracing/tracer.cpp
endif
	
OBJS_FILES:=${CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o} 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

plimdp: $(OBJS_FILES)
	$(CXX) $(LDFLAGS) $(OBJS_FILES) -o $(OUTPUT)

legacy: legacy/PlimDP.c
	gcc -Wall -O3 $< -o $(BIN_DIR)/$@ 

all: build_dirs plimdp

build_dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/PlimDP
	mkdir -p $(OBJ_DIR)/PlimDP/cpu
	mkdir -p $(OBJ_DIR)/PlimDP/cpu/tracing
	mkdir -p $(OBJ_DIR)/PlimDP/devices
	mkdir -p $(BIN_DIR)
    
clean:
	rm -rf $(OBJ_DIR)  
	rm -rf $(BIN_DIR)

.PHONY: clean
