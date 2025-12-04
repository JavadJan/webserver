#CC = c++
#FLAGS = -Wall -Wextra -Werror -Iinclude -std=c++98



#NAME = server

#H = include/
#HEADER = $(H)server.hpp\
#		$(H)ParsedDate.hpp



#OBJ_F = obj/

##src folder
#SRC_F = src/

##src files
#SRC = ParsedDate.cpp main.cpp 

#OBJ = $(SRC:.cpp=.o)
##OBJ = $(SRC:src/%.cpp=%.o)


#OBJ_P = $(addprefix $(OBJ_F), $(OBJ))

#all: $(NAME)

#$(NAME) : $(OBJ_P) 
#	$(CC) $(FLAGS) $(OBJ_P) -o $(NAME)
#	@echo "✅ successfully created 🏗️ 🛠️!"

#$(OBJ_F)%.o:$(SRC_F)%.cpp $(HEADER) | $(OBJ_F) #how make objects 
#	$(CC) $(FLAGS) -c $< -o $@
#	@echo "successfully $@ created ✅"

#$(OBJ_F):
#	@mkdir -p $(OBJ_F)

#clean :
#	rm -f $(OBJ_P)

#fclean : clean
#	rm -f $(NAME)

#re : fclean $(NAME)

#.PHONY: all clean fclean re

CC = c++
FLAGS = -Wall -Wextra -Werror -Iinclude -std=c++98

NAME = server

H = include/
HEADER = $(H)server.hpp \
         $(H)ParsedData.hpp

OBJ_F = obj/
SRC_F = src/

SRC = ParsedData.cpp main.cpp
OBJ = $(SRC:.cpp=.o)
OBJ_P = $(addprefix $(OBJ_F), $(OBJ))

all: $(NAME)

$(NAME): $(OBJ_P)
	$(CC) $(FLAGS) $(OBJ_P) -o $(NAME)
	@echo "✅ successfully created 🏗️ 🛠️!"

$(OBJ_F)%.o: $(SRC_F)%.cpp $(HEADER) | $(OBJ_F)
	$(CC) $(FLAGS) -c $< -o $@
	@echo "successfully $@ created ✅"

$(OBJ_F):
	@mkdir -p $(OBJ_F)

clean:
	rm -f $(OBJ_P)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
