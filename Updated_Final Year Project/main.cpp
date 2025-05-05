#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <iostream>

using namespace cv;

int main()
{
    std::string image_path = "C:/Users/gooyk/Desktop/160494.jpeg";
    std::cout << "Attempting to load image from: " << image_path << std::endl;

    Mat img = imread(image_path, IMREAD_COLOR);

    if (img.empty())
    {
        std::cerr << "Could not read the image: " << image_path << std::endl;
        std::cerr << "Possible reasons: incorrect path, file does not exist, or OpenCV is not configured correctly." << std::endl;
        return 1;
    }

    std::cout << "Image loaded successfully!" << std::endl;

    imshow("Display window", img);
    int k = waitKey(0); // Wait for a keystroke in the window

    if (k == 's')
    {
        imwrite("starry_night.png", img);
        std::cout << "Image saved as starry_night.png" << std::endl;
    }

    return 0;
}