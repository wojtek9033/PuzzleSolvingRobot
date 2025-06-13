#ifndef PUZZLE_PROCESSING_H
#define PUZZLE_PROCESSING_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <array>
#include <glob.h>
#include "puzzle_utils.h"

using namespace cv;
using std::vector;
using std::pair;

extern size_t PUZZLE_SIZE;
extern Size PUZZLE_IMAGES_SIZE;
extern vector<Mat> initialPuzzleImages;

struct Element {
    // in this struct, id is a unique id of puzzle element. Each edge has its id as an index.
    // Index of an edge corresponds to array edgeType and most outer vector of matches.
    int id;
    cv::Point centroid;
    std::vector<std::vector<cv::Point>> edges;
    std::vector<std::vector<cv::Point>> normalizedEdges;
    std::array<int,4> edgeType;
    std::array<int,4> matchingElements = {-1, -1, -1, -1};
    std::array<int,4> matchingEdges = {-1, -1, -1, -1};
    std::array<cv::Point,4> edgeCentroid;
    std::vector<double> initialEdgeOrientation;
    std::vector<double> finalEdgeOrientation;
    bool isCornerPiece{false};
    bool isEdgePiece{false};
    double rotationAngle{0};
    std::pair<int, int> pairedEdges;
};

void preprocImage(Mat &img);
Mat readImage(std::string path);
vector<Point> getCorners(Mat img, int nmsNeighbourhoodSize);
Mat nonMaximaSuppression(Mat img, int windowSize);
Mat localCornerAvereging(Mat img, int windowSize);
vector<Point> reinforceCorners(const vector<Point> aproxCorners, const vector<Point> contour, const int boxSize = 20);
vector<Point> getContour(const Mat &img);
vector<Point> getLargestArea(const vector<Point>);
vector<vector<Point>> getPuzzleEdges(const vector<Point> contour, const vector<Point> reinf_corners);
Point getCentroid (const vector<Point> edges);
std::array<int,4> determineInOut(const vector<vector<Point>> edges, const double FLAT_TRESHOLD);
//vector<vector<Point>> normalizeEdges(const vector<vector<Point>> edges, int numSamples);
void normalizeEdges(Element &elem, int numSamples);
int loadImages(std::string directory);
int loadParameters(std::string configFile );
Element elementPipeline(Mat puzzleImage, int id);
double getOrientation(cv::Point a, cv::Point b);

#endif // PUZZLE_PROCESSING_H
