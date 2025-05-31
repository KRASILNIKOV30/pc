#include "Gauss.h"
#include <memory>
#include <thread>
#include <vector>
#include <wx/slider.h>
#include <wx/wx.h>

constexpr int WIDTH = 1200;
constexpr int HEIGHT = 800;

class MyFrame final : public wxFrame
{
public:
	MyFrame()
		: wxFrame(nullptr, wxID_ANY, "Gauss", wxDefaultPosition, wxSize(WIDTH, HEIGHT))
	{
		m_originalImage.LoadFile("image.png", wxBITMAP_TYPE_PNG);
		m_bitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(m_originalImage));
		m_slider = new wxSlider(this, wxID_ANY, 0, 0, 70, wxDefaultPosition, wxSize(200, 20), wxSL_HORIZONTAL | wxSL_LABELS);

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->AddStretchSpacer();
		sizer->Add(m_bitmap, 0, wxALIGN_CENTER);
		sizer->Add(m_slider, 0, wxALIGN_CENTER | wxTOP, 10);
		sizer->AddStretchSpacer();

		this->SetSizer(sizer);

		m_slider->Bind(wxEVT_SLIDER, &MyFrame::OnSliderChange, this);
		m_blur.SetImage(m_originalImage);
	}

private:
	void OnSliderChange(wxCommandEvent& event)
	{
		const int radius = m_slider->GetValue();

		wxImage image;
		if (radius == 0)
		{
			image = m_originalImage;
		}
		else
		{
			m_blur.SetRadius(radius);
			image = m_blur.Blur();
		}

		m_bitmap->SetBitmap(wxBitmap(image));
		Refresh();
	}

private:
	wxImage m_originalImage;
	wxStaticBitmap* m_bitmap;
	wxSlider* m_slider;
	GaussBlur m_blur;
};

class MyApp final : public wxApp
{
public:
	bool OnInit() override
	{
		wxImage::AddHandler(new wxPNGHandler());
		m_frame = new MyFrame();
		m_frame->Show(true);
		return true;
	}

private:
	MyFrame* m_frame = nullptr;
};

wxIMPLEMENT_APP(MyApp);