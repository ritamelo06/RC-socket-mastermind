CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

PLAYER_DIR = Player
GAMESERVER_DIR = GameServer

PLAYER_SRC := $(shell find $(PLAYER_DIR) -name '*.cpp')
GAMESERVER_SRC := $(shell find $(GAMESERVER_DIR) -name '*.cpp')

PLAYER_OBJ = $(PLAYER_SRC:.cpp=.o)
GAMESERVER_OBJ = $(GAMESERVER_SRC:.cpp=.o)

PLAYER_EXEC = player
GAMESERVER_EXEC = GS

all: $(PLAYER_EXEC) $(GAMESERVER_EXEC)

$(PLAYER_EXEC): $(PLAYER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(GAMESERVER_EXEC): $(GAMESERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(PLAYER_OBJ) $(GAMESERVER_OBJ) $(PLAYER_EXEC) $(GAMESERVER_EXEC)
