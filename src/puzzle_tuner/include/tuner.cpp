  #include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <array>
#include <glob.h>
#include <string>
#include <sstream>

using namespace cv;
using std::vector;
using std::pair;

// Parameters
const char* preprocWindowName = "Adjusting preprocessing";
const char* cornersWindowName = "Adjusting corner detection";
const char* configFile = "config.txt";

double scale_down = 0.4;
int blurKernelSize{1};
int thresholdValue{128};
int structElementSize{1};
int cornersBlockSize{7}; // corner neighbourhood
int cornersKSize{1}; // higher = edge is more blurry
double cornersK{0.04};
int cornersTreshVal{190};

const int maxBlur = 10;
const int maxThreshold = 255;
const int maxStructElementSize = 21;
const int maxCornersBlockSize = 12;
const int maxCornersKSize = 5;
const int maxCornersTreshVal{255};

Mat src, processed;
Size puzzleImagesSize;

void preprocImage();
void adjustCorners();
Mat readImage(std::string path);
void saveParameters();
void loadParameters();
void getCorners(int, void*);
void updateImage(int, void*);
void printMatType(const cv::Mat& mat);

int main()
{
    loadParameters();
    std::string dir= "images/21.jpg";
    src = readImage(dir);
    preprocImage();

    while (true){
        char key = (char)cv::waitKey(0);
        if (key == 's') {
            saveParameters();
            break;
        }
        if (key == 27) break;
    }
    adjustCorners();
    while (true){
        char key = (char)cv::waitKey(0);
        if (key == 's') {
            saveParameters();
            break;
        }
        if (key == 27) break;
    }


    destroyAllWindows();
    return 0;
}

void updateImage(int, void*){
    int blurKernel = blurKernelSize * 2 + 1;
    int elemKernel = structElementSize * 2 + 1;
    Mat blured = Mat::zeros(src.size(), CV_8UC1);
    GaussianBlur(src, blured, Size(blurKernel, blurKernel), 0);
    threshold(blured, blured, thresholdValue, maxThreshold, THRESH_BINARY_INV);

    //Mat element = getStructuringElement(MORPH_RECT, Size(7,7)); // fill small gaps
    morphologyEx(blured, blured,MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(elemKernel,elemKernel)));
    processed = blured.clone();
    imshow(preprocWindowName, processed);
}

Mat readImage(std::string path){
    Mat img;
    img = imread(path, IMREAD_COLOR);
    int x_range = img.cols/5;
    int y_range = img.rows/4;
    if(img.empty()) return img;
    img  = img(Range(y_range, y_range*3), Range(x_range,x_range*4));      // cropping
    resize(img, img, Size(),scale_down,scale_down,INTER_LINEAR);     // scaling
    return img;
}

void getCorners(int, void*){

    int blockSize = cornersBlockSize * 2;
    int kSize = cornersKSize * 2 + 1;

    Mat corners = Mat::zeros( processed.size(), CV_32FC1 );
    Mat cornersTresholded = Mat::zeros( processed.size(), CV_32FC1 );

    cornerHarris( processed, corners, blockSize, kSize, cornersK );
    Mat corners_norm, corners_scaled;
    normalize( corners, corners_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( corners_norm, corners_scaled );

    // apply tresholding
    for (int i = 0; i < corners_norm.rows; i++) {
        for (int j = 0; j < corners_norm.cols; j++) {
            if (static_cast<int>(corners_norm.at<float>(i, j)) > cornersTreshVal) {
                cornersTresholded.at<float>(i, j) = static_cast<int>(corners_norm.at<float>(i, j));
            }
        }
    }

    Mat extractedCorners = processed.clone();
    cvtColor(extractedCorners, extractedCorners, COLOR_GRAY2BGR);

    for (int i = 0; i < cornersTresholded.rows; i++){
        for (int j = 0; j < cornersTresholded.cols; j++){
            if (cornersTresholded.at<float>(i,j) != 0){
                circle(extractedCorners, cv::Point(j,i), 1, cv::Scalar(0, 0, 255),2);
            }
        }
    }
    imshow(cornersWindowName,extractedCorners);
}

void preprocImage(){
    printMatType(src);
    cvtColor(src,src,COLOR_BGR2GRAY);
    printMatType(src);
    namedWindow(preprocWindowName, WINDOW_AUTOSIZE);
    createTrackbar("Blur", preprocWindowName, &blurKernelSize, maxBlur, updateImage);
    createTrackbar("Binary thresh.", preprocWindowName, &thresholdValue, maxThreshold, updateImage);
    createTrackbar("Struct elem.", preprocWindowName, &structElementSize, maxStructElementSize, updateImage);
    updateImage(0,0);
}

void adjustCorners(){

    namedWindow(cornersWindowName, WINDOW_AUTOSIZE);
    createTrackbar("Block Size", cornersWindowName, &cornersBlockSize, maxCornersBlockSize, getCorners);
    createTrackbar("KSize", cornersWindowName, &cornersKSize, maxCornersKSize, getCorners);
    createTrackbar("Treshold", cornersWindowName, &cornersTreshVal, maxCornersTreshVal, getCorners);
    getCorners(0,0);
}

void saveParameters(){
    std::ofstream file(configFile);
    if(file.is_open()){
        file << (blurKernelSize * 2 + 1) << "," << thresholdValue << "," << (structElementSize  * 2 + 1) << ","
             << cornersBlockSize * 2 << "," << cornersKSize * 2 +1 << "," << cornersTreshVal << ","
             << scale_down << std::endl;
        file.close();
        std::cout << "Parameters saved to " << configFile << std::endl;
    } else {
        std::cerr << "@@@ Erro saving parameters!" << std::endl;
    }
}

void loadParameters() {
    std::ifstream file(configFile);
    int tempBlurKernelSize;
    int tempThresholdValue;
    int tempStructElementSize;
    int tempcornersBlockSize;
    int tempcornersKSize;
    int tempcornersTreshVal;
    if (file.is_open()) {
        std::string line;
        if (std::getline(file, line)){
            std::stringstream ss(line);
            char delim;
            if (!(ss >> tempBlurKernelSize >> delim >> tempThresholdValue >> delim >> tempStructElementSize
                     >> delim  >> tempcornersBlockSize >> delim >>tempcornersKSize >> delim >> tempcornersTreshVal
                     >> delim >> scale_down)) {
                std::cout << "Error parsing parameters, using initial values" << std::endl;
                return;
            }
            else {
                blurKernelSize = (tempBlurKernelSize-1)/2;
                thresholdValue = tempThresholdValue;
                structElementSize = (tempStructElementSize-1)/2;
                cornersBlockSize = tempcornersBlockSize/2;
                cornersKSize = (tempcornersKSize-1)/2;
                cornersTreshVal = tempcornersTreshVal;
            }
        }
        file.close();
    } else {
        std::cerr << "No config file found. Using default parameters." << std::endl;
    }
}

void printMatType(const cv::Mat& mat){
    int type = mat.type();

    // Decoding depth
    int depth = type & CV_MAT_DEPTH_MASK;
    std::string depthStr;

    switch (depth) {
    case CV_8U:  depthStr = "CV_8U"; break;
    case CV_8S:  depthStr = "CV_8S"; break;
    case CV_16U: depthStr = "CV_16U"; break;
    case CV_16S: depthStr = "CV_16S"; break;
    case CV_32S: depthStr = "CV_32S"; break;
    case CV_32F: depthStr = "CV_32F"; break;
    case CV_64F: depthStr = "CV_64F"; break;
    default:     depthStr = "Unknown"; break;
    }

    // Get number of channels
    int channels = 1 + (type >> CV_CN_SHIFT);

    std::cout << "Type: " << depthStr << " with " << channels << " channels" << std::endl;
}
