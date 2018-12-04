// Load this file into cling, like so:
//
// > cling --std=c++14 -Lstdc++
// ****************** CLING ******************
// * Type C++ code and press enter to run it *
// *             Type .q to exit             *
// *******************************************
// [cling]$ .L init.cpp          <=== Type this to load this file

// these lines are for MacOS
// you might have to adjust them for linux
#pragma cling add_include_path("opencv-install/include")
#pragma cling add_library_path("opencv-install/lib")
#pragma cling load("libopencv_core.dylib")
#pragma cling load("libopencv_calib3d.dylib")
#pragma cling load("libopencv_highgui.dylib")
#pragma cling load("libopencv_imgcodecs.dylib")
#pragma cling load("libopencv_imgproc.dylib")

#pragma cling add_include_path("fplus_include/")

#include <string>
#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fplus/fplus.hpp>
