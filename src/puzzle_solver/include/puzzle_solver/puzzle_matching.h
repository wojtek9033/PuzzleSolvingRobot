#ifndef PUZZLE_MATCHING_H
#define PUZZLE_MATCHING_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <array>

#include "puzzle_processing.h"

struct MatchInfo {
    int idB;
    int edgeB;
    double similarityScore;
};

double chamferDistance(const std::vector<cv::Point> e1, const std::vector<cv::Point> e2);
std::vector<Element> matchingPipeline(std::vector<Element> &processedElements);
void printMatches(const std::map<std::pair<int,int>, std::vector<MatchInfo>> data);
std::map<std::pair<int,int>, std::vector<MatchInfo>> findMatches(std::vector<Element> &allElementsData);
std::map<std::pair<int,int>, MatchInfo> extractBestMatches(std::map<std::pair<int,int>, std::vector<MatchInfo>> allMatches);
void assignMatches(std::vector<Element> &puzzle,const std::map<std::pair<int,int>, MatchInfo> bestMatches);
void printMatches(const std::map<std::pair<int,int>, MatchInfo> matchMap);
std::vector<Element> puzzleAssembly(const std::vector<Element> &allElementsData);
void drawAssembly(const std::vector<Element> &assembly);
std::vector<std::array<double,3>> placeElementsIn2D(std::vector<Element>& assembly);

#endif // PUZZLE_MATCHING_H
