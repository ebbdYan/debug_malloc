# md5dir 

#CROSS = mips-linux-uclibc-gnu-

CC = $(CROSS)gcc
CPP = $(CROSS)g++

TARGET = a.out

SRC_DIRS = .

CFLAGS = -Wall

LFLAGS = 

%/.:
	mkdir -p $*

SRCS_C = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
SRCS_CPP = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

OBJS_C = $(patsubst %.c,%.o, $(SRCS_C))
OBJS_CPP = $(patsubst %.cpp,%.o, $(SRCS_CPP))

.PHONY: release clean

release: $(TARGET)
clean:
	rm *.o *.txt $(TARGET) -fv

$(TARGET):$(OBJS_C) $(OBJS_CPP)
	$(CPP) -g -o $@ $(LFLAGS) $^

%.o:%.c
	$(CC) -c $^ -o $@ $(CFLAGS)

%.o:%.cpp
	$(CPP) -c $^ -o $@ $(CFLAGS)

