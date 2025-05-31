#include "Motion.h"
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
		: wxFrame(nullptr, wxID_ANY, "Motion", wxDefaultPosition, wxSize(WIDTH, HEIGHT))
	{
		m_originalImage.LoadFile("image.png", wxBITMAP_TYPE_PNG);
		m_bitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(m_originalImage));
		m_powerSlider = new wxSlider(this, wxID_ANY, 0, 0, 70, wxDefaultPosition, wxSize(200, 20), wxSL_HORIZONTAL | wxSL_LABELS);
		m_directionSlider = new wxSlider(this, wxID_ANY, 0, 0, 70, wxDefaultPosition, wxSize(200, 20), wxSL_HORIZONTAL | wxSL_LABELS);

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->AddStretchSpacer();
		sizer->Add(m_bitmap, 0, wxALIGN_CENTER);
		sizer->Add(m_powerSlider, 0, wxALIGN_CENTER | wxTOP, 10);
		sizer->Add(m_directionSlider, 0, wxALIGN_CENTER | wxTOP, 10);
		sizer->AddStretchSpacer();

		this->SetSizer(sizer);

		m_powerSlider->Bind(wxEVT_SLIDER, &MyFrame::OnSliderChange, this);
		m_directionSlider->Bind(wxEVT_SLIDER, &MyFrame::OnSliderChange, this);
		m_blur.SetImage(m_originalImage);
	}

private:
	void OnSliderChange(wxCommandEvent& event)
	{
		const int power = m_powerSlider->GetValue();

		wxImage image;
		if (power == 0)
		{
			image = m_originalImage;
		}
		else
		{
			const double direction = m_directionSlider->GetValue();
			const auto angle = std::lerp(0.0, 2 * M_PI, direction / m_directionSlider->GetMax());
			const auto x = std::cos(angle);
			const auto y = std::sin(angle);

			m_blur.SetStrength(power);
			m_blur.SetDirection(x, y);
			image = m_blur.Blur();
		}

		m_bitmap->SetBitmap(wxBitmap(image));
		Refresh();
	}

private:
	wxImage m_originalImage;
	wxStaticBitmap* m_bitmap;
	wxSlider* m_powerSlider;
	wxSlider* m_directionSlider;
	MotionBlur m_blur;
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