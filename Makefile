# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2018/04/21 15:56:23 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	TabGraph
SRC			=	./src/Manager.cpp \
				./src/Node.cpp \
				./src/test.cpp

OBJ			=	$(SRC:.c=.o)
HYPER_OBJ	=	final.o
CC			=	g++

INCLUDE_REP	=	./include

LIBDIR		=	./libs/

LIBFILES	=	

INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
CFLAGS		=	-Ofast -std=c++11 -Wall -Wextra -Werror $(INCLUDE)

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)

ifeq ($(OS), Windows_NT)
OK_STRING	=	[OK]
LIBS		=	$(addprefix -L , $(LIBDIR)) -lmingw32 -lsfml-graphics -lsfml-window -lsfml-system -lopengl32
else ifeq ($(shell uname -s), Darwin)
LIBS		=	$(addprefix -L , $(LIBDIR)) -lsfml-graphics -lsfml-window -lsfml-system -framework OpenGL
else
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -lSDL2main -lSDL2 -lGL -lm -lft
endif

$(NAME): $(LIBFILES) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $(NAME)

hyper: $(LIBFILES) $(HYPER_OBJ)
	$(CC) $(CFLAGS) $(HYPER_OBJ) $(LIBS) -o $(addprefix Hyper, $(NAME))

%.o: %.c
	@echo -n Compiling $@...
	@($(CC) $(CFLAGS) -o $@ -c $<)
	@echo "$(OK_STRING)"

%.o: %.hyper
	@echo -n Compiling $@...
	@($(CC) -x c $(CFLAGS) -o $@ -c $<)
	@echo "$(OK_STRING)"

.INTERMEDIATE: final.hyper
final.hyper: $(SRC)
	@echo -n Generating $@...
	@(cat $^ > final.hyper)
	@echo "$(OK_STRING)"

./libs/ezmem/libezmem.a :
	$(MAKE) -C ./libs/ezmem/

./libs/vml/libvml.a :
	$(MAKE) -C ./libs/vml/

./libs/libft/libft.a :
	$(MAKE) -C ./libs/libft/

pull:
	git pull
	git submodule update --init --recursive
	git submodule foreach git reset --hard origin/42
	git submodule foreach git pull origin 42

clean:
	rm -rf $(OBJ) $(HYPER_OBJ)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) clean && ) true

fclean:
	rm -rf $(OBJ) $(HYPER_OBJ) $(NAME)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) fclean && ) true

re: fclean $(NAME)

.PHONY: all clean fclean re