IDIR = ./include
ODIR = ./obj
SDIR = ./src

PCHSRC = $(IDIR)/pch.h
PCHOUT = $(IDIR)/pch.h.gch
HFILES = $(IDIR)/*.h
OFILES = $(ODIR)/main.o $(ODIR)/MotionDectAlgo.o $(ODIR)/ObjectTracking.o

CC = g++
CFLAGS = -I $(IDIR)
LIBS = -lm -lpthread -lrt

#opencv.pc to packconfg path
PKG_CONFIG_PATH=$(PKG_CONFIG_PATH):/usr/local/lib/pkgconfig/

#linking object files
Object_Tracking.out: $(PCHOUT) $(OFILES)
	#
	#=================== SRC Compilation SUCCESS ================
	#
	$(CC) -o $@ $(OFILES) $(LIBS) `pkg-config --cflags --libs opencv`
	#	
	#-------------------- Compilation ----------------- : SUCCESS
	#

#building precompiled headers
$(PCHOUT): $(PCHSRC)
	$(CC) -c -o $@ $(PCHSRC) $(CFLAGS)

#building individual object  
$(ODIR)/main.o: $(SDIR)/main.cpp $(HFILES)
	$(CC) -include $(PCHSRC) -c -o $@ $< $(CFLAGS)

$(ODIR)/MotionDectAlgo.o: $(SDIR)/MotionDectAlgo.cpp $(HFILES)
	$(CC) -include $(PCHSRC) -c -o $@ $< $(CFLAGS)

$(ODIR)/ObjectTracking.o: $(SDIR)/ObjectTracking.cpp $(HFILES)
	$(CC) -include $(PCHSRC) -c -o $@ $< $(CFLAGS)

clean:
	-rm -f $(ODIR)/*.o
	-rm -f $(ODIR)/*~
	-rm -f $(IDIR)/pch.h.gch
	-rm -f Object_Tracking.out

	#	
	#------------- Object_Tracking Clean ----------------- : SUCCESS
	#

rebuild: clean Object_Tracking.out
	#	
	#----------------- Object_Tracking Rebuild Completed ----------------- : SUCCESS
	#