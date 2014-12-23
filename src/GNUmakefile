# ==================
#   Configuration
# ==================
INCLUDE	:= -I./include
CFLAGS 	:= -g -ansi -pedantic -Wall -Werror -Wextra -fPIC $(INCLUDE)
LDLIBS  := -llua -lpanel -lncurses

LIB_NAME  := termview
OBJS      := lua_termview.o view.o

LIB_DYNAMIC := lib$(LIB_NAME).so
LIB_STATIC  := lib$(LIB_NAME).a

# ==================
#   Auto-dependency
# ==================
srcs := $(wildcard *.c)
CFLAGS += -MMD
-include $(srcs:.c=.d)

# ==================
#   Recipes
# ==================
$(LIB_DYNAMIC): LDFLAGS += -shared -install_name @rpath/$@
$(LIB_DYNAMIC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(LIB_STATIC): $(LIB_STATIC)($(OBJS))

# ==================
#   Phony targets
# ==================
.PHONY: all debug clean test
.PHONY: static dynamic

all: static dynamic
static:  $(LIB_STATIC)
dynamic: $(LIB_DYNAMIC)

clean:
	$(RM) $(LIB_DYNAMIC) $(LIB_STATIC) $(OBJS)

test:
	$(MAKE) -C tests/ $@
