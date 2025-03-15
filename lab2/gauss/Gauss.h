#pragma once
#include <string>
#include <wx/wx.h>

struct Args
{
	std::string inputFileName;
	std::string outputFileName;
	int radius;
	int threadsNum;
};

void GaussBlur(Args const& args);
wxImage BlurParallel(wxImage const& img, int radius, int threadsNum);