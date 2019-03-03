# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2019/03/03 19:42:53 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			=	libTabGraph.a

#   Paths Declaration   #
OBJ_PATH		=	obj/
SRC_PATH		=	src/
BUILD_PATH		=	build/
HEADERS_PATH	=	include/
REL_PATH		=	release/
DBG_PATH		=	debug/
TEST_PATH		=	$(BUILD_PATH)tests/
SHADERS_PATH	=	$(SRC_PATH)shaders/
RELOBJ_PATH		=	$(OBJ_PATH)$(REL_PATH)
DBGOBJ_PATH		=	$(OBJ_PATH)$(DBG_PATH)
RELBUILD_PATH	=	$(BUILD_PATH)$(REL_PATH)
DBGBUILD_PATH	=	$(BUILD_PATH)$(DBG_PATH)
# Paths Declaration End #

# Files Declaration #
SHADERS_FILES	=	blur.frag				\
					deferred.frag			\
					deferred.vert			\
					depth.frag				\
					depth.vert				\
					empty.glsl				\
					forward.frag			\
					forward.vert			\
					forward_default.frag	\
					lighting.frag			\
					lighting_env.frag		\
					passthrough.frag		\
					passthrough.vert		\
					present.frag			\
					refraction.frag			\
					ssao.frag
HEADERS_FILES	=	AABB.hpp					\
					BoundingElement.hpp			\
					Camera.hpp					\
					ComputeObject.hpp			\
					Config.hpp					\
					Cubemap.hpp					\
					CubeMesh.hpp				\
					DLLExport.hpp				\
					Engine.hpp					\
					Environment.hpp				\
					Debug.hpp					\
					Events.hpp					\
					Framebuffer.hpp				\
					GameController.hpp			\
					GLIncludes.hpp				\
					InputDevice.hpp				\
					Keyboard.hpp				\
					Light.hpp					\
					Material.hpp				\
					Mesh.hpp					\
					Mouse.hpp					\
					Node.hpp					\
					Object.hpp					\
					parser						\
					Render.hpp					\
					Renderable.hpp				\
					scop.hpp					\
					Shader.hpp					\
					Texture.hpp					\
					TextureArray.hpp			\
					VertexArray.hpp				\
					VertexBuffer.hpp			\
					Vgroup.hpp					\
					Window.hpp					\
					parser/BMP.hpp				\
					parser/FBX.hpp				\
					parser/GLSL.hpp				\
					parser/HDR.hpp				\
					parser/InternalTools.hpp	\
					parser/MTLLIB.hpp			\
					parser/OBJ.hpp				\
					parser/TABSCENE.hpp
SRC_FILES		=	Camera.cpp			\
					ComputeObject.cpp	\
					Config.cpp			\
					Cubemap.cpp			\
					CubeMesh.cpp		\
					Engine.cpp			\
					Environment.cpp		\
					Events.cpp			\
					Framebuffer.cpp		\
					GameController.cpp	\
					Keyboard.cpp		\
					Light.cpp			\
					Material.cpp		\
					Mesh.cpp			\
					Mouse.cpp			\
					Node.cpp			\
					Object.cpp			\
					Render.cpp			\
					Renderable.cpp		\
					Shader.cpp			\
					Texture.cpp			\
					TextureArray.cpp	\
					VertexArray.cpp		\
					VertexBuffer.cpp	\
					Vgroup.cpp			\
					Window.cpp			\
					parser/BMP.cpp		\
					parser/FBX.cpp		\
					parser/GLSL.cpp		\
					parser/HDR.cpp		\
					parser/MTLLIB.cpp	\
					parser/OBJ.cpp		\
					parser/tools.cpp	\
					render/shadow.cpp
SRC_FILES_TEST	=	scop42/main.cpp			\
					scop42/callbacks.cpp
RES_FILES		=	$(shell find ./res -type f)
BUILD_RES_FILES	=	$(addprefix $(TEST_PATH), $(RES_FILES))
# Files Declaration End #

#   Files Generation   #
SRC				=	$(addprefix $(SRC_PATH), $(SRC_FILES))
TEST_SRC		=	$(addprefix $(SRC_PATH), $(SRC_FILES_TEST))
SHADERS			=	$(addprefix $(SHADERS_PATH), $(SHADERS_FILES))
HEADERS			=	$(addprefix $(HEADERS_PATH), $(HEADERS_FILES))
OBJ				=	$(SRC_FILES:.cpp=.o)
OBJ_TEST		=	$(SRC_FILES_TEST:.cpp=.o)
RELOBJ			=	$(addprefix $(RELOBJ_PATH), $(OBJ))
DBGOBJ			=	$(addprefix $(DBGOBJ_PATH), $(OBJ))
RELOBJ_TEST		=	$(addprefix $(RELOBJ_PATH), $(OBJ_TEST))
DBGOBJ_TEST		=	$(addprefix $(DBGOBJ_PATH), $(OBJ_TEST))
# Files Generation End #

INCLUDE_PATH	=	./include				\
					./libs/vml/include

LIBDIR		=	./libs/vml/
LDFLAGS		+=	$(addprefix -L , $(LIBDIR))

LIBFILES	=	./libs/vml/libvml.a

CPPFLAGS	+=	$(addprefix -I, $(INCLUDE_PATH))
CPPFLAGS	+=	$(addprefix -I, $(SHADERS_PATH))
CXXFLAGS	+=	-std=c++17 -Wall -Wextra -Werror $(CPPFLAGS)
DBGFLAGS	=	-DDEBUG_MOD -g
RELFLAGS	=	-Ofast

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)

ifeq ($(OS), Windows_NT)
OK_STRING	=	[OK]
LDLIBS		+=	-static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -lSDL2_image -limagehlp -ljpeg -lpng -ltiff -lwebp -lz -llzma -lmingw32 $(LDFLAGS) -lvml -Wl,-Bdynamic -lSDL2main -lSDL2 -lglew32 -lopengl32
else ifeq ($(shell uname -s), Darwin)
LDLIBS		+=	$(LDFLAGS) -lvml -lm -lGLEW -framework OpenGL -framework SDL2
else
LDLIBS		+=	$(LDFLAGS) -lvml -lstdc++ -lpthread -lz -lm -lSDL2main -lSDL2 -lGLEW -lGL 
endif

all: tests

release: CXXFLAGS += $(RELFLAGS)
release: $(RELBUILD_PATH)$(NAME)

$(RELBUILD_PATH)$(NAME) : $(LIBFILES) $(RELOBJ)
	@(mkdir -p $(@D))
	ar -rc $(RELBUILD_PATH)$(NAME) $(RELOBJ)
	ranlib $(RELBUILD_PATH)$(NAME)

$(RELOBJ_PATH)%.o: $(SRC_PATH)%.cpp $(HEADERS) $(SHADERS)
	@(mkdir -p $(@D))
	@echo Compiling $@...
	@($(CXX) $(CXXFLAGS) -o $@ -c $<)
	@echo $@ compilation "$(OK_STRING)"

debug: CXXFLAGS += $(DBGFLAGS)
debug: $(DBGBUILD_PATH)$(NAME)

$(DBGBUILD_PATH)$(NAME) : $(LIBFILES) $(DBGOBJ)
	@(mkdir -p $(@D))
	ar -rc $(DBGBUILD_PATH)$(NAME) $(DBGOBJ)
	ranlib $(DBGBUILD_PATH)$(NAME)

$(DBGOBJ_PATH)%.o: $(SRC_PATH)%.cpp $(HEADERS) $(SHADERS)
	@(mkdir -p $(@D))
	@echo Compiling $@...
	@($(CXX) $(CXXFLAGS) -o $@ -c $<)
	@echo $@ compilation "$(OK_STRING)"

$(BUILD_RES_FILES): %: $(RES_FILES)
	@(mkdir -p $(@D))
	@(cp $(patsubst $(TEST_PATH)%,%,$@) $@)
	@echo Copied $(patsubst $(TEST_PATH)%,%,$@) to $@

info:
	@echo $(SHADERS)

depend:
	makedepend -- $(CXXFLAGS) -- $(SRC)

tests: release debug $(RELOBJ_TEST) $(DBGOBJ_TEST) $(BUILD_RES_FILES)
	$(CXX) $(CXXFLAGS) $(RELFLAGS) $(RELOBJ_TEST) -L $(RELBUILD_PATH) -lTabGraph $(LDLIBS) -o $(TEST_PATH)Scop
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(DBGOBJ_TEST) -L $(DBGBUILD_PATH) -lTabGraph $(LDLIBS) -o $(TEST_PATH)ScopD
	./scripts/copyDlls.sh $(TEST_PATH)Scop.exe
	./scripts/copyDlls.sh $(TEST_PATH)ScopD.exe

./libs/vml/libvml.a :
	$(MAKE) -C ./libs/vml/

tidy:
	clang-tidy $(SRC) -checks=* -- $(CXXFLAGS) $(INCLUDE_PATH)

format:
	clang-format -i -style=WebKit $(SRC) $(HEADERS)

pull:
	git pull
	git submodule update --init --recursive
	git submodule foreach git reset --hard origin/vml++
	git submodule foreach git pull origin vml++

clean:
	rm -rf $(OBJ_PATH)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) clean && ) true

fclean:
	rm -rf $(TEST_PATH) $(BUILD_PATH) $(OBJ_PATH)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) fclean && ) true

re: fclean all

.PHONY: all clean fclean re
