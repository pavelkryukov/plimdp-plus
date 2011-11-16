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

#bin directory
BIN_DIR:=bin

#sources directory
SRC_DIR:=source
SRC_PLIMDP_DIR:=$(SRC_DIR)/PlimDP
SRC_PLIMDP_CPU_DIR:=$(SRC_PLIMDP_DIR)/cpu
SRC_PLIMDP_CPU_TRACING_DIR:=$(SRC_PLIMDP_DIR)/cpu/tracing
SRC_PLIMDP_DEVICES_DIR:=$(SRC_PLIMDP_DIR)/devices
SRC_PLIMDP_COMPUTER_DIR:=$(SRC_PLIMDP_DIR)/computer

#all sources directories
SRC_DIRS := \
    $(SRC_DIR) \
    $(SRC_PLIMDP_DIR) \
    $(SRC_PLIMDP_CPU_DIR) \
    $(SRC_PLIMDP_CPU_TRACING_DIR) \
    $(SRC_PLIMDP_DEVICES_DIR) \
    $(SRC_PLIMDP_COMPUTER_DIR)

#obj directory
OBJ_DIR:=obj

#all obj directories
OBJ_DIRS:=${SRC_DIRS:$(SRC_DIR)/%=$(OBJ_DIR)/%} 

#include of source dirs
CXXFLAGS:= $(CXXFLAGS) -I $(SRC_DIR)

#plimdp cpp file to archive in $(LIBPLIMDP)
PLIMDP_DEVICES_CPP_FILES := $(SRC_PLIMDP_DEVICES_DIR)/loader.cpp

PLIMDP_CPU_CPP_FILES := \
	$(SRC_PLIMDP_CPU_DIR)/bus.cpp \
	$(SRC_PLIMDP_CPU_DIR)/core.cpp \
	$(SRC_PLIMDP_CPU_DIR)/executor.cpp \
	$(SRC_PLIMDP_CPU_DIR)/decoder.cpp \
	$(SRC_PLIMDP_CPU_DIR)/register.cpp	\
	$(SRC_PLIMDP_CPU_DIR)/isa.cpp

PLIMDP_CPP_FILES := $(PLIMDP_CPU_CPP_FILES) $(PLIMDP_DEVICES_CPP_FILES) $(SRC_PLIMDP_COMPUTER_DIR)/simple.cpp

ifeq ($(DISASM), 1)
    CXXFLAGS:= $(CXXFLAGS) -D ENABLE_DISASM=1
    PLIMDP_CPP_FILES:= $(PLIMDP_CPP_FILES) $(SRC_PLIMDP_CPU_TRACING_DIR)/disassembler.cpp
endif

ifeq ($(TRACE), 1)
    CXXFLAGS:= $(CXXFLAGS) -D ENABLE_TRACE=1
    PLIMDP_CPP_FILES:= $(PLIMDP_CPP_FILES) $(SRC_PLIMDP_CPU_TRACING_DIR)/tracer.cpp
endif
	
#plimdp objs file to archive in $(LIBPLIMDP)
PLIMDP_OBJS_FILES:=${PLIMDP_CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o}

#common rule for build
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

#building binaries
$(BIN_DIR)/libplimdp.a: $(PLIMDP_OBJS_FILES) $(BIN_DIR)
	ar rcs $@ $^

$(BIN_DIR)/simplepdp: $(OBJ_DIR)/main.o $(BIN_DIR)/libplimdp.a
	g++ -L $(BIN_DIR) $< -l plimdp -o $@
    
$(BIN_DIR)/legacy: legacy/PlimDP.c $(BIN_DIR)
	gcc -Wall -O3 $< -o $@ 2> /dev/null

all: $(BIN_DIR)/simplepdp $(BIN_DIR)/legacy

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIRS)

clean:
	rm -rf $(OBJ_DIR)  
	rm -rf $(BIN_DIR)

.PHONY: clean
