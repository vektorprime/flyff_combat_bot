// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


#include "ocr_matching.h"


std::string match_flyff_text_ocr(cv::Mat oldim1, const int threshold_value)
{
	const double X_RATIO = 0.25;
	const double Y_RATIO = 0.7;
	const double X_RATIO_ROI = 0.5;
	const double Y_RATIO_ROI = 0.05;
	const int thresh_max_val = 255;
	const int thresh_type = 1;
	cv::Mat roimat{};

	const int rx = static_cast<int>(oldim1.cols * X_RATIO);
	const int ry = static_cast<int>(oldim1.rows * Y_RATIO);
	int nx = static_cast<int>(oldim1.cols * X_RATIO_ROI);
	int ny = static_cast<int>(oldim1.rows * Y_RATIO_ROI);

	//catch any errors
	if (rx + nx > oldim1.cols)
	{
		nx = oldim1.cols - rx;
	}
	if (ry + ny > oldim1.rows)
	{
		ny = oldim1.rows - ry;
	}
	//create ROI
	cv::Rect roi(rx, ry, nx, ny);

	//get ROI of image
	roimat = oldim1(roi);

	//threshold
	threshold(roimat, roimat, threshold_value, thresh_max_val, thresh_type);

	//run the ocr
	auto api = std::make_unique<tesseract::TessBaseAPI>();
	api->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
	api->SetPageSegMode(tesseract::PSM_AUTO);
	//api->SetDebugVariable("segment_debug", "false");
	api->SetImage(roimat.data, roimat.cols, roimat.rows, 1, roimat.step);
	std::string outText = std::string(api->GetUTF8Text());
	api->End();
	return outText;
}