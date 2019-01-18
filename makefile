TARGET          = Galaxeirb_cu

CC 				= g++
NVCC            = nvcc
LINKER          = g++ -o

CUDA_INSTALL_PATH ?= /usr/local/cuda

INCD            = -I$(CUDA_INSTALL_PATH)/include
LIBS            = -L$(CUDA_INSTALL_PATH)/lib -lcudart

DEFINES         = -D CUDA_ENABLE
CFLAGS          = -O3 -Wall $(DEFINES)
NFLAGS          = -O3 -use_fast_math -arch=compute_32 $(DEFINES)
LFLAGS          = -Wall -lm -lGLEW -lSDL2 -lGL -lGLU
#LFLAGS         = -Wall -lm -lGLEW -lSDL2 -framework OpenGL -framework GLUT

SRCDIR          = src
OBJDIR          = obj
BINDIR          = bin

INCLUDES        = . cuda display thread tools universe
INC_PARAMS      = $(foreach i, $(INCLUDES),-I./$(SRCDIR)/$i)

SOURCES         := $(shell find $(SRCDIR) -name '*.cpp')
SOURCES_CU      := $(shell find $(SRCDIR) -name '*.cu')
HEADERS         := $(shell find $(SRCDIR) -name '*.hpp')
INCLUDES_CU 	:= $(shell find $(SRCDIR) -name '*.cuh')
OBJECTS         := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
OBJECTS_CU      := $(SOURCES_CU:$(SRCDIR)/%.cu=$(OBJDIR)/%.cuo)
rm 				= rm -rf

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS_CU) $(OBJECTS) 
	$(LINKER) $@ $(OBJECTS_CU) $(OBJECTS) $(LFLAGS) $(LIBS) $(INC_PARAMS) 

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp | createObjDir
	$(CC) $(CFLAGS) $(INCD) $(INC_PARAMS) -c $< -o $@

$(OBJECTS_CU): $(OBJDIR)/%.cuo : $(SRCDIR)/%.cu | createObjDir
	$(NVCC) $(NFLAGS) $(INCD) $(INC_PARAMS) -c $< -o $@

# Take the list of objects and create the needed 
# directories by removing filenames from paths
.PHONY: createObjDir
createObjDir:
	@mkdir -p $(dir $(OBJECTS) $(OBJECTS_CU))

.PHONY: clean
clean:
	$(rm) $(OBJDIR)

.PHONY: remove
remove: clean
	$(rm) $(BINDIR)/$(TARGET)