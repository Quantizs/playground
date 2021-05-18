#include "opencv2/opencv.hpp"
#include <iostream>
#include <memory>

bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

int main(int argc, char** argv)
{
	try
	{
		if (argc != 4)
		{
			throw std::exception("Usage:\n .exe [resolution (integer assumed)] [input file (video path or webcam id assumed)] [output file]");
			//example: 60 "C:\Benchmarking\Test\input.mp4" "C:\Benchmarking\Test\output.mp4"
		}

		std::string resolutionParam = argv[1];
		std::string videoInputParam = argv[2];
		std::string videoOutputParam = argv[3];

		if (!is_number(resolutionParam))
		{
			throw std::exception("Resolution must be an integer!");
		}

		int resolution = std::stoi(resolutionParam, nullptr);

		std::unique_ptr<cv::VideoCapture> cap;
		if (is_number(videoInputParam))//Camera id
		{
			int cameraId = std::stoi(videoInputParam, nullptr);
			cap = std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture(cameraId));
		}
		else//Video path
		{
			cap = std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture(videoInputParam));
		}

		if (!cap->isOpened())
		{
			throw std::exception("Unable to connect to the video source!");
		}

		cv::VideoWriter outputVideo;

		int imageWidth = static_cast<int>(cap->get(cv::CAP_PROP_FRAME_WIDTH));
		int imageHeight = static_cast<int>(cap->get(cv::CAP_PROP_FRAME_HEIGHT));
		int videoFps = cap->get(cv::CAP_PROP_FPS);

		cv::Size S(imageWidth, imageHeight);
		//int ex = static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
		outputVideo.open(videoOutputParam, cv::VideoWriter::fourcc('P', 'I', 'M', '1') /*ex*/, videoFps, S, true);

		cv::Mat displayImage;

		int step = imageHeight / resolution;
		cv::Rect imageRect(0, 0, imageWidth, imageHeight);

		std::vector<cv::Mat> container;
		while (true)
		{
			cv::Mat frame;
			*cap >> frame;
			if (frame.empty())
			{
				std::cout << "Camera frame empty" << std::endl;
				break;
			}

			container.push_back(frame);
			if (container.size() > resolution)
			{
				displayImage = frame.clone();
				container.erase(container.begin());
				cv::Rect cropRect(0, 0, imageWidth, step);
				for (int i = 0; i < container.size(); i++)
				{
					cropRect = cropRect & imageRect;
					container[i](cropRect).copyTo(displayImage(cropRect));
					cropRect.y += step;
				}
			}
			else
			{
				displayImage = frame;
			}

			outputVideo << displayImage;
			cv::imshow("Rolling Shutter Effect", displayImage);
			cv::waitKey(30);
			//TODO: Remove
			if (cv::waitKey(10) == 27) break; // stop capturing by pressing ESC 
		}
	}
	catch (std::exception& e)
	{
		std::cout << "\nException thrown!" << std::endl;
		std::cout << e.what() << std::endl;
	}

	system("pause");
	return 0;
}