CC = cc
FLAGS = -Wall -Wextra -Werror -Iinclude

H = include/
HEADER = $(H)client.h

SRC_F = src/

OBJ_F = obj/

SRC = main.c

OBJ = $(SRC:.c=.o)

OBJ_P = $(addprefix $(OBJ_F), $(OBJ))

NAME = client

all : $(NAME)

$(NAME): $(OBJ_P) $(HEADER)
	$(CC) $(FLAGS) $(OBJ_P) -o $(NAME)
	@echo "✅ successfully created 🏗️ 🛠️!"

$(OBJ_F)%.o:$(SRC_F)%.c | $(OBJ_F)
	$(CC) $(FLAGS) -c $< -o $@
	@echo "successfully $@ created ✅"

$(OBJ_F): 
	@mkdir -p $(OBJ_F)

clean :
	rm -f $(OBJ_P)

fclean : clean
	rm -f $(NAME)

re : fclean $(NAME)

.PHONY: all clean fclean re