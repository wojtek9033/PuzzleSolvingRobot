#include "puzzle_utils.h"

std::vector<cv::Scalar> colors = {
    cv::Scalar(4, 255, 207),   // Yellow
    cv::Scalar(0, 255, 0),   // Green
    cv::Scalar(0, 0, 255),   // Red
    cv::Scalar(238, 129, 48)  // Blue
};

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


void plotEdges(const std::vector<std::vector<cv::Point>> &plotData, std::string name, cv::Size srcSize){

    cv::Mat graph = cv::Mat::zeros(srcSize, CV_8UC3);
    cv::Point graphCentre(graph.rows/2, graph.cols/2);
    cv::line(graph,cv::Point(0,graphCentre.x),cv::Point(graph.cols,graphCentre.x),cv::Scalar(255,255,255)); //x-axis
    cv::line(graph,cv::Point(graphCentre.y, 0),cv::Point(graphCentre.y,graph.rows),cv::Scalar(255,255,255)); //y-axis

    for (size_t i = 0; i < plotData.size(); i++) {
        const auto& d = plotData[i];
        for (size_t j = 0; j < d.size() - 1; j++) {
            cv::line(graph, d[j]+ cv::Point(graphCentre.y,graphCentre.x), d[j + 1]+ cv::Point(graphCentre.y,graphCentre.x), colors.at(i), 1);
        }
    }
    imshow(name, graph);

}
