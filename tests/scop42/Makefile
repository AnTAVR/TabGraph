APP_NAME			=	Scop.exe
APP_SRC				=	src/main.cpp			\
						src/callbacks.cpp
APP_INCLUDE_PATH	=	include
APP_RES_FILES		=	$(shell find ./res -type f)

APP_PATH			=	$(shell pwd)/

export

all:
	cd ../.. && $(MAKE) application

debug: APP_NAME = ScopD.exe
debug: APP_CXXFLAGS = -g
debug:
	cd ../.. && $(MAKE) application DEBUG=1