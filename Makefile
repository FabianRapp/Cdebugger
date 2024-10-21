CC := cc

INCLUDES := -I./includes
CFLAGS := -g $(INCLUDES)

OBJS_DIR := ./objs/

SRCS := srcs/debugger/debugger.c
OBJS := $(SRCS:srcs/%.c=$(OBJS_DIR)%.o)
NAME := debugger

SRCS_PROGRAMM := srcs/dummy/dummy.c
OBJS_PROGRAMM := $(SRCS_PROGRAMM:srcs/%.c=$(OBJS_DIR)%.o)
NAME_PROGRAMM := dummy

.PHONY: all debugger dummy clean fclean re

all: debugger dummy

debugger: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

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
