#ifndef PUZZLE_UTILS_H
#define PUZZLE_UTILS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

extern std::vector<cv::Scalar> colors;

void printMatType(const cv::Mat& mat);
void plotEdges(const std::vector<std::vector<cv::Point>> &plotData, std::string name, cv::Size srcSize);


#endif // PUZZLE_UTILS_H
