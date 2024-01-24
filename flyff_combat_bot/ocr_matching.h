// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#pragma once

//regular includes
#include <Windows.h>
#include <iostream>

//used for image matching
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"

//used for OCR
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>


std::string match_flyff_text_ocr(cv::Mat oldim1, const int threshold_value);
