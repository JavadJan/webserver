#CC = c++
#FLAGS = -Wall -Wextra -Werror -Iinclude -std=c++98



#NAME = server

#H = include/
#HEADER = $(H)config/serverConfig.hpp\
		$(H)config/LocationConfig.hpp\
		$(H)config/ConfigParser.hpp\
		$(H)config/Token.hpp




#OBJ_F = obj/

##src folder
#SRC_F = src/

##src files
#SRC =main_ConfigParserTest.cpp \
	config/ConfigParser.cpp \
	config/ServerConfig.cpp \
	config/LocationConfig.cpp\
	

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

NAME := server
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98

# Adjust include dir as needed: include (local) or ../include (repo root)
INCLUDES := -Iinclude

# Auto-find sources under src
SRCS := $(shell find src -name '*.cpp')
OBJS := $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
    $(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) -o $@

src/%.o: src/%.cpp
    @mkdir -p $(dir $@)
    $(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
    rm -f $(OBJS)

fclean: clean
    rm -f $(NAME)

re: fclean all
