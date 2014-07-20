CC = g++
CCFLAGS = -g -Wno-deprecated -Wall
RM = /bin/rm


# OpenCV internal headers -- from build dir
OPENCV_INC_DIR_CX__ = -I/common/opencv-1.0.0/cxcore/src

# OpenCV paths
OPENCV_INC_DIR = -I/usr/intel/include/opencv $(OPENCV_INC_DIR_CX__)
OPENCV_LIB_DIR= -L/usr/intel/lib
OPENCV_LIB = $(OPENCV_LIB_DIR) -lcv -lhighgui -lcxcore -lcvaux


# YConfigFile
YC_INC_DIR = -I/home/indika/programming/YConfigFile
YC_LIB_DIR= -L/home/indika/programming/YConfigFile
YC_LIB = $(YC_LIB_DIR) -lYConfigFile

INCLUDEDIR = $(OPENCV_INC_DIR)  $(YC_INC_DIR)
LDFLAGS =  $(OPENCV_LIB) $(YC_LIB)
OUTDIR = .


SRCS = $(wildcard *.C)
OBJS = $(patsubst %.C,%.o,$(SRCS))
PROGS = $(OUTDIR)/test $(OUTDIR)/frec



all: $(PROGS)

-include .depend

.PHONY:depend
depend: $(SRCS)
	$(CC) -MM $(CCFLAGS) $(INCLUDEDIR) $^ > .depend

%.o: %.C
	$(CC) $(CCFLAGS) $(INCLUDEDIR) -o $@ -c $< 

$(OUTDIR)/frec: Loader.o 		\
		Utils.o  		\
		Logger.o 		\
		Face.o			\
		FaceDetector.o		\
		EigenMethod.o		\
		PreProcessorEx.o		\
		Kernel.o		\
		main.o			\
		App.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

$(OUTDIR)/test:	test.o\
		Logger.o\
		Utils.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY:clean
clean:
	-$(RM) $(OBJS) $(PROGS)
