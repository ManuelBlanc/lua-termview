# ==================
#   Configuration
# ==================

INCLUDE	:= -I../include
CFLAGS 	:= -g -ansi -pedantic -Wall -Werror -Wextra $(INCLUDE)
LDFLAGS := -Wl,-rpath,../
LDLIBS	:= -L../ -llua -ltermview -lpanel -lncurses

EXE := tester

# ==================
#   Recipes
# ==================
# -implicit

# ==================
#   Phony targets
# ==================
.PHONY: all clean

all: $(EXE)

clean:
	$(RM) $(EXE) $(EXE).o

test: $(EXE)
	@echo "[test] Loading the library from C:"
	./$(EXE) main.lua
	@echo "[test] Loading the library from Lua:"
	lua main.lua
