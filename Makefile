#
# Weather APP
#
CC      = gcc
CFLAGS  = -W -Wall -g
CFLAGS  += -D__LIB_WEATHER_APP__
CFLAGS  += -D__LIB_WEATHER_DEBUG__

#
# include <time.h>
#
# warning: implicit declaration of function ‘strptime’; did you mean ‘strftime’? [-Wimplicit-function-declaration]
#
CFLAGS  += -D__USE_XOPEN -D_GNU_SOURCE

INCLUDE = -I/usr/local/include

# apt install ibcurl4-openssl-dev libcjson-dev
LDFLAGS = -L/usr/local/lib -lpthread -lcurl -lm -lcjson
#
# 기본적으로 Makefile은 indentation가 TAB 4로 설정되어있음.
# Indentation이 space인 경우 아래 내용이 활성화 되어야 함.
.RECIPEPREFIX +=

# 폴더이름으로 실행파일 생성
TARGET  := $(notdir $(shell pwd))

# 정의되어진 이름으로 실행파일 생성
# TARGET := test

SRC_DIRS = .
# SRCS     = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
SRCS     = $(shell find . -name "*.c")
OBJS     = $(SRCS:.c=.o)

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -f $(OBJS)
	rm -f $(TARGET)