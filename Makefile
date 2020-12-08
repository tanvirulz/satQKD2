# Sorry for the super messy version of makefile (  that works for now. )
# Definitely have to learn how to write a smarter one.

CC=g++
IFLAGS=-I /usr/include/timetagger
LFLAGS=-l TimeTagger 


ODIR=./obj


# nominal compile
#default: mkobjdir nondeb mem reg  proc $(SDIR)/main.c 
#	gcc -o run $(ODIR)/memory.o $(ODIR)/registers.o  $(ODIR)/debug.o $(ODIR)/processor.o $(SDIR)/main.c $(IFLAGS) 
default: readRawSwabian.cpp correctTimestamp.cpp coinmatch.cpp sifting.cpp
	$(CC) -o rrswabian readRawSwabian.cpp $(IFLAGS) $(LFLAGS)
	$(CC) -o srswabian stitchRawSwabian.cpp $(IFLAGS) $(LFLAGS)
	$(CC) -o crt correctTimestamp.cpp 
	$(CC) -o cm coinmatch.cpp 
	$(CC) -o grb genRawBasis.cpp
	$(CC) -o sift sifting.cpp
	$(CC) -o cascade bcascadeDoubling.c mtwister.c

srswabian:
	$(CC) -o srswabian stitchRawSwabian.cpp $(IFLAGS) $(LFLAGS)
rrswabian: readRawSwabian.cpp
	$(CC) -o rrswabian readRawSwabian.cpp $(IFLAGS) $(LFLAGS)

crt: correctTimestamp.cpp
	$(CC) -o crt correctTimestamp.cpp 

cm: coinmatch.cpp
	$(CC) -o cm coinmatch.cpp 

grb: genRawBasis.cpp
	$(CC) -o grb genRawBasis.cpp

sift: sifting.cpp
	$(CC) -o sift sifting.cpp

cascade: bcascadeDoubling.c mtwister.c mtwister.h
	$(CC) -o cascade bcascadeDoubling.c mtwister.c

run: default run.sh
	./run.sh /media/tanvir/SpooqyLab/Table_Top_Demo/20200824_swab_fine_longer 8mW00dB.ttbin 24Aug_usb result00db_usb.csv 1500

runbatch: default runbatch.py run.sh
	python3 runbatch.py
clean:
	rm -f rrswabian
	rm -f crt 
	rm -f cm 
	rm -f sift 
	rm -f cascade
	rm -f srswabian


