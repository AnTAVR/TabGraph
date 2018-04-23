# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abartz <abartz@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2018/04/23 18:56:35 by abartz           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	libTabGraph.a

SOURCES		=	Manager.cpp Node.cpp

OBJS		=	$(SOURCES:.cpp=.o)
OBJS		:=	$(addprefix objs/, $(OBJS))

INCLUDES	=	-I ./include \
				-I ~/.brew/Cellar/sfml/2.4.2_1/include \
				-I ~/.brew/Cellar/glm/0.9.8.5/include

FLAGS		=	-std=c++1z -Wall -Wextra
CC			=	clang++ $(FLAGS)

all : $(NAME)

$(NAME): $(OBJS)
	@ar -rc $@ $(OBJS)
	@ranlib $@

objs/%.o: sources/%.cpp
	@mkdir -p objs
	$(CC) $(INCLUDES) -o $@ -c $<

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean $(NAME)

.PHONY: all $(NAME) clean fclean re
