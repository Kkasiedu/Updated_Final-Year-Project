#include "NDVIProcessor.h"

cv::Mat NDVIProcessor::Process(const cv::Mat& image) {
    // Implement NDVI processing logic here
    cv::Mat ndviImage;
    // Example NDVI processing logic
    // Convert the image to float
    cv::Mat floatImage;
    image.convertTo(floatImage, CV_32F);

    // Split the image into its channels
    std::vector<cv::Mat> channels;
    cv::split(floatImage, channels);

    // Calculate NDVI
    cv::Mat ndvi = (channels[2] - channels[0]) / (channels[2] + channels[0]);

    // Normalize the NDVI image to 0-255
    cv::normalize(ndvi, ndviImage, 0, 255, cv::NORM_MINMAX, CV_8U);

    return ndviImage;
}
