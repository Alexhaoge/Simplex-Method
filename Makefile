# OS Name (Linux or Darwin)
OSUPPER = $(shell uname -s 2>/dev/null | tr [:lower:] [:upper:])
OSLOWER = $(shell uname -s 2>/dev/null | tr [:upper:] [:lower:])

# Flags to detect 32-bit or 64-bit OS platform
OS_SIZE = $(shell uname -m | sed -e "s/i.86/32/" -e "s/x86_64/64/")
OS_ARCH = $(shell uname -m | sed -e "s/i386/i686/")

# These flags will override any settings
ifeq ($(i386),1)
	OS_SIZE = 32
	OS_ARCH = i686
endif

ifeq ($(x86_64),1)
	OS_SIZE = 64
	OS_ARCH = x86_64
endif

# Flags to detect either a Linux system (linux) or Mac OSX (darwin)
DARWIN = $(strip $(findstring DARWIN, $(OSUPPER)))

# Location of the CUDA Toolkit binaries and libraries
CUDA_PATH       ?= /usr/local/cuda-11.2
CUDA_INC_PATH   ?= $(CUDA_PATH)/include
CUDA_BIN_PATH   ?= $(CUDA_PATH)/bin
ifneq ($(DARWIN),)
  CUDA_LIB_PATH  ?= $(CUDA_PATH)/lib
else
  ifeq ($(OS_SIZE),32)
    CUDA_LIB_PATH  ?= $(CUDA_PATH)/lib
  else
    CUDA_LIB_PATH  ?= $(CUDA_PATH)/lib64
  endif
endif


# Common binaries
NVCC            ?= $(shell which nvcc > /dev/null && echo "nvcc" || echo "$(CUDA_BIN_PATH)/nvcc")
CC 				=  g++

# Extra user flags
EXTRA_NVCCFLAGS ?=
EXTRA_LDFLAGS   ?=
#CFLAGS = -O3 -Wall std=gnu99
CFLAGS=-O3 -Wall

# CUDA code generation flags
# GENCODE_SM20    := -gencode arch=compute_20,code=sm_20 -Wno-deprecated-gpu-targets
GENCODE_SM35	  := -gencode arch=compute_35,code=sm_35
GENCODE_SM50    := -gencode arch=compute_50,code=sm_50
GENCODE_SM52    := -gencode arch=compute_52,code=sm_52
GENCODE_SM62    := -gencode arch=compute_62,code=sm_62
GENCODE_SM72    := -gencode arch=compute_72,code=sm_72
GENCODE_SM75    := -gencode arch=compute_75,code=sm_75
GENCODE_FLAGS   := $(GENCODE_SM75)


# OS-specific build flags
ifneq ($(DARWIN),)
      LDFLAGS   := -Xlinker -rpath $(CUDA_LIB_PATH) -L$(CUDA_LIB_PATH) -lcudart
      CCFLAGS   := -arch $(OS_ARCH)
else
  ifeq ($(OS_SIZE),32)
      LDFLAGS   := -L$(CUDA_LIB_PATH) -lcudart
      CCFLAGS   := -m32
  else
      LDFLAGS   := -L$(CUDA_LIB_PATH) -lcudart
      CCFLAGS   := -m64
  endif
endif

# OS-architecture specific flags
ifeq ($(OS_SIZE),32)
      NVCCFLAGS := -m32
else
      NVCCFLAGS := -m64
endif

# Common includes and paths for CUDA
# INCLUDES      := -I$(CUDA_INC_PATH) -I. -I.. -I../../common/inc
INCLUDES      := -I$(CUDA_INC_PATH) -I.

#openCL libs
CL_LIBS=OpenCL

#CUDA Library
NVCC_LIB = -lcublas -lcurand
DIR_OBJS=build
SRCS=$(wildcard *.c)
CPP_SRCS=$(wildcard *.cpp)
CU_SRCS=$(wildcard *.cu)
HEADERS=$(wildcard *.h)
# CU_HEADERS=$(wildcard *.cuh)
OBJS=$(SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)  $(CU_SRCS:.cu=.o)
EXE=simplex
OBJS := $(addprefix $(DIR_OBJS)/,$(OBJS))
EXE := $(addprefix $(DIR_OBJS)/,$(EXE))
#Target Rules

$(EXE):$(OBJS)
	$(CC) $(INCLUDES) $(OBJS) $(NVCC_LIB) -L $(CUDA_LIB_PATH) -l$(CL_LIBS) $(LDFLAGS) $(EXTRA_LDFLAGS) -o $(EXE)

$(DIR_OBJS)/%.o:%.cpp %.h
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(DIR_OBJS)/%.o:%.c %.h
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

$(DIR_OBJS)/%.o:%.cu %.h
	$(NVCC) $(NVCCFLAGS) $(EXTRA_NVCCFLAGS) $(NVCC_LIB) $(GENCODE_FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJS) $(EXE)

test:
	./$(EXE)
