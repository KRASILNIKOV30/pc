#pragma once
#include <opencv2/opencv.hpp>
#include <utility>

class VideoCamera
{
public:
	using FrameHandler = std::function<void(const cv::Mat&)>;

	explicit VideoCamera(FrameHandler frameHandler)
		: m_frameHandler(std::move(frameHandler))
	{
		if (!m_capture.isOpened())
		{
			throw std::runtime_error("Could not open camera");
		}
	}

	// решить вопрос с копированием

	void Run()
	{
		while (true)
		{
			m_capture.read(m_frame);

			if (m_frame.empty())
			{
				std::cout << "Empty frame" << std::endl;
				break;
			}

			m_frameHandler(m_frame);
		}
	}

	~VideoCamera()
	{
		m_capture.release();
		cv::destroyAllWindows();
	}

private:
	cv::VideoCapture m_capture{ 0 };
	cv::Mat m_frame;
	FrameHandler m_frameHandler;
};
