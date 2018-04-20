# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2018/04/20 15:14:27 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	Scop
SRC			=	

OBJ			=	$(SRC:.c=.o)
HYPER_OBJ	=	final.o
CC			=	g++

INCLUDE_REP	=	./include				\
				./libs/glm/

LIBDIR		=	./libs/ezmem/	\
				./libs/vml/		\
				./libs/libft/

LIBFILES	=	./libs/ezmem/libezmem.a	\
				./libs/vml/libvml.a		\
				./libs/libft/libft.a

INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
CFLAGS		=	-Ofast -fno-ipa-cp-clone -Wall -Wextra -Werror $(INCLUDE)

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)

ifeq ($(OS), Windows_NT)
OK_STRING	=	[OK]
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -lft -lmingw32 -lSDL2main -lSDL2 -lm -lglew32 -lopengl32
else ifeq ($(shell uname -s), Darwin)
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -lm -lGLEW -lft -framework OpenGL -framework SDL2
INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
CFLAGS		=	-Ofast -arch x86_64 -Wall -Wextra -Werror $(INCLUDE)
else
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -lSDL2main -lSDL2 -lGL -lm -lGLEW -lft
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