IDIR = ./include
ODIR = ./obj
SDIR = ./src

PCHSRC = $(IDIR)/pch.h
PCHOUT = $(IDIR)/pch.h.gch
HFILES = $(IDIR)/*.h
OFILES = $(ODIR)/main.o $(ODIR)/MotionDectAlgo.o $(ODIR)/ObjectTracking.o $(ODIR)/Mutex.o $(ODIR)/GPIOConst.o\
			$(ODIR)/GPIOManager.o
CVIDIR = /opt/opencv-4.0.0/include/
CC = g++
CFLAGS = -I $(IDIR) -I $(CVIDIR) -rdynamic

OPENCVLIBS = \
 -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_calib3d -lopencv_ccalib -lopencv_core -lopencv_datasets\
 -lopencv_dpm -lopencv_face -lopencv_features2d -lopencv_flann -lopencv_freetype\
 -lopencv_fuzzy -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc\
 -lopencv_line_descriptor -lopencv_ml -lopencv_objdetect -lopencv_optflow -lopencv_phase_unwrapping -lopencv_photo\
 -lopencv_plot -lopencv_reg -lopencv_rgbd -lopencv_saliency -lopencv_shape -lopencv_stereo -lopencv_stitching\
 -lopencv_structured_light -lopencv_superres -lopencv_surface_matching -lopencv_text  -lopencv_videoio\
 -lopencv_video -lopencv_videostab -lopencv_ximgproc -lopencv_xobjdetect -lopencv_xphoto

LIBS = -lm -lpthread -lrt $(OPENCVLIBS)

#opencv.pc to packconfg path
#PKG_CONFIG_PATH=$(PKG_CONFIG_PATH):/usr/local/lib/pkgconfig/

#linking object files
Object_Tracking.out: $(PCHOUT) $(OFILES)
	#
	#=================== SRC Compilation SUCCESS ================
	#
	#$(CC) -o $@ $(OFILES) $(LIBS) `pkg-config --cflags --libs opencv`
	$(CC) -o $@ $(OFILES) $(LIBS) $(CFLAGS)
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

$(ODIR)/Mutex.o: $(SDIR)/Mutex.cpp $(HFILES)
	$(CC) -include $(PCHSRC) -c -o $@ $< $(CFLAGS)

$(ODIR)/GPIOConst.o: $(SDIR)/GPIOConst.cpp $(HFILES)
	$(CC) -include $(PCHSRC) -c -o $@ $< $(CFLAGS)

$(ODIR)/GPIOManager.o: $(SDIR)/GPIOManager.cpp $(HFILES)
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
