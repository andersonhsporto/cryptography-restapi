NAME		= crypto-api

RM			= rm -rf
CXX			= gcc
CXXFLAGS	= -Wall -Wextra -Werror -g
INC_DIR		= ./include
SRC_DIR		= ./src
OBJ_DIR		= ./build
LDFLAGS		= -lsqlite3 -lssl -lcrypto

SRCS		= $(addprefix $(SRC_DIR)/, \
				api.c database.c json.c server.c crypto.c \
			)

OBJS		= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS		= $(OBJS:.o=.d)

INC_DIRS	= $(shell find $(INC_DIR) -type d)
INC_FLAGS	= $(addprefix -I,$(INC_DIRS))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
