include ../Makefile_public

#CPPFLAGS= -ggdb -Wall $(INC)

TARGET	= $(DEBUG_TARGET)

DEBUG_TARGET = $(LIB_DIR)/libcommon.a

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJ_DIR)/,$(subst .cpp,.o,$(SRC)))

INC = -I$(LIGHTFRAME_INCLUDE_DIR) -I$(MYSQL_INCLUDE_DIR)

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $?

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS) $(TARGET)
