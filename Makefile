BASEPATH = .
SRC_DIR = $(BASEPATH)/src
INC_DIR = $(BASEPATH)/include
OBJ_DIR = $(BASEPATH)/obj
APPNAME = $(BASEPATH)/delaunay

VPATH = $(SRC_DIR)

INC_DIRS = \
	$(INC_DIR)                  \

LIB_DIRS = \
	/usr/local/lib	

LIBS = \

SOURCES = \
	main.cpp     \
	predicates.cpp \
	delaunay.cpp \
	vertexreader.cpp \
	quadedge.cpp \
	edge.cpp \
	vertex.cpp \

OBJECTS = $(SOURCES:.cpp=.o)

CC = g++
#CPPFLAGS = -g -Wall
CPPFLAGS = -O
CPPFLAGS += $(addprefix -I, $(INC_DIRS))
LDFLAGS = $(addprefix -L, $(LIB_DIRS)) $(addprefix -l, $(LIBS))

all : $(APPNAME)

$(APPNAME) : $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	$(CC) $(CPPFLAGS) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o : %.cpp
	$(CC) $(CPPFLAGS) -MD -c $< -o $@
	@cp $(OBJ_DIR)/$*.d $(OBJ_DIR)/$*.P; \
            sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
                -e '/^$$/ d' -e 's/$$/ :/' < $(OBJ_DIR)/$*.d >> $(OBJ_DIR)/$*.P; \
            rm -f $(OBJ_DIR)/$*.d

-include $(OBJECTS:%.o=$(OBJ_DIR)/%.P)

clean : 
	rm  $(OBJ_DIR)/* $(APPNAME)
