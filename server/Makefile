
CC = c++
FLAGS = -Wall -Wextra -Werror -Iinclude -std=c++98


H = include/
HEADER = $(H)Server.hpp\
         $(H)Request.hpp\
         $(H)Config.hpp\
         $(H)HttpRequest.hpp\
         $(H)Response.hpp\
         $(H)ResponseHandler.hpp\

OBJ_F = obj/
SRC_F = src/

SRC = main.cpp Config.cpp HttpRequest.cpp Server.cpp FSM.cpp ResponseHandler.cpp

NAME = server

OBJ = $(SRC:.cpp=.o)

OBJ_P = $(addprefix $(OBJ_F), $(OBJ))

all: $(NAME)

$(NAME) : $(OBJ_P) $(HEADER)
	$(CC) $(FLAGS) $(OBJ_P) -o $(NAME)
	@echo "\033[1;32m✅ successfully created : $(NAME) 🏗️ 🛠️! \033[0m"

$(OBJ_F)%.o:$(SRC_F)%.cpp | $(OBJ_F) #how make objects 
	$(CC) $(FLAGS) -c $< -o $@
	@echo "successfully $@ created ✅"

$(OBJ_F):
	@mkdir -p $(OBJ_F)


clean:
	rm -f $(OBJ_P)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re
