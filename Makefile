NVCCFLAGS = -lineinfo
CFLAGS = -Wall -Wfatal-errors
INCLUDES = -I/usr/local/include/opencv4 -I/usr/include/eigen3
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_imgcodecs
LIBDIRS = -L/usr/local/lib 

edit:
	# g++ -o test_mouse test_mouse.cpp $(INCLUDES) $(LIBDIRS) $(LIBS) -std=c++11
	# g++ -o test_mouse_auto test_mouse_auto.cpp $(INCLUDES) $(LIBDIRS) $(LIBS) -std=c++11
	# g++ -o test_mouse_obbr test_mouse_obbr.cpp $(INCLUDES) $(LIBDIRS) $(LIBS) -std=c++11
	g++ -o test_mouse_obbr_auto test_mouse_obbr_auto.cpp $(INCLUDES) $(LIBDIRS) $(LIBS) -std=c++11

clean:
	rm test_mouse
	rm test_mouse_auto
	rm test_mouse_obbr
	rm test_mouse_obbr_auto
