CC := c++

INCLUDES := -I./includes -I./capstone/include/

CFLAGS := -Wall -Wextra -O0 -g $(INCLUDES) -lreadline

LIBS := -L./capstone/lib -lcapstone

OBJS_DIR := ./objs/

SRCS := srcs/debug/debugger.cpp \
		srcs/debug/op_len.cpp \
		srcs/debug/forking.cpp \
		srcs/debug/breakpoint_handler.cpp \
		srcs/debug/get_regs.cpp \
		srcs/debug/debug_debugger.cpp \
		srcs/debug/classes/Breakpoint.cpp \



OBJS := $(SRCS:srcs/%.c=$(OBJS_DIR)%.o)
NAME := debugger

SRCS_PROGRAMM := srcs/programm/dummy.c
OBJS_PROGRAMM := $(SRCS_PROGRAMM:srcs/%.c=$(OBJS_DIR)%.o)
NAME_PROGRAMM := dummy

.PHONY: all clean fclean re

all: debugger dummy

debugger: $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(LIBS) $(CFLAGS)

dummy: $(OBJS_PROGRAMM)
	$(CC) $(CFLAGS) $(OBJS_PROGRAMM) -o $(NAME_PROGRAMM)

$(OBJS_DIR)%.o: srcs/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OBJS_PROGRAMM)

fclean:
	rm -rf $(OBJS_DIR)
	rm -f $(NAME) $(NAME_PROGRAMM)

re: fclean all
