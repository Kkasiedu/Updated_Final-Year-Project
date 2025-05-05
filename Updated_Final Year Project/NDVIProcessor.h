#ifndef NDVIPROCESSOR_H
#define NDVIPROCESSOR_H

#include <opencv4/opencv2/opencv.hpp>

class NDVIProcessor {
public:
    cv::Mat Process(const cv::Mat& image);
};

#endif // NDVIPROCESSOR_H
