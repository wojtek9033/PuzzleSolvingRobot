#include "puzzle_solver/puzzle_processing.h"

double scale_down = 0.4;

// Parameters
double SCALE_DOWN{0.4};
int BLUR_KERNEL_SIZE = 1;
int BINARY_TRESHOLD_VAL = 25;
int STRUCT_ELEM_SIZE = 1;
int CORNERS_BLOCK_SIZE{7}; // corner neighbourhood
int CORNERS_K_SIZE{1}; // higher = edge is more blurry
double CORNERS_K{0.04};
int CORNERS_TRESH_VAL{190};

const int CORNERS_REINF_BOX_SIZE{10};
const unsigned int NORMALIZE_SAMPLES_VAL{300};
const double FLAT_TRESHOLD{10.0};
const int LOCAL_MAXIMA_NEIGHBORHOOD{6}; // neighborhood size of the pixel for finding local maximas

size_t PUZZLE_SIZE{9};
Size PUZZLE_IMAGES_SIZE;
vector<Mat> initialPuzzleImages;

int loadParameters(std::string configFile ) {
    std::cout << "Searching \"" + configFile + "\" for config file..."<< std::endl;
    std::ifstream file(configFile);
    if (file.is_open()) {
        std::cout << "Succesfully found solver config file."<< std::endl;
        std::string line;
        if (std::getline(file,line)){
            std::stringstream ss(line);

            char delim;
            if (!(ss >> BLUR_KERNEL_SIZE >> delim >> BINARY_TRESHOLD_VAL >> delim >> STRUCT_ELEM_SIZE
                  >> delim >> CORNERS_BLOCK_SIZE >> delim >> CORNERS_K_SIZE >> delim >> CORNERS_TRESH_VAL
                  >> delim >> SCALE_DOWN)){
                std::cerr << "@@@ Error parsing parameters!" << std::endl;
                return 1;
            }
            return 0;
        }
    } else {
        std::cerr << "@@@ Error! No config file found. Using default parameters." << std::endl;
        return 0;
    }
}

Mat readImage(std::string path){
    Mat img;
    img = imread(path, IMREAD_COLOR);
    int x_range = img.cols/5;
    int y_range = img.rows/4;
    if(img.empty()) return img;
    img  = img(Range(y_range, y_range*3), Range(x_range,x_range*4));      // cropping
    resize(img, img, Size(),SCALE_DOWN,SCALE_DOWN,INTER_LINEAR);     // scaling
    preprocImage(img);
    return img;
}

int loadImages(std::string directory){
    vector<std::string> fn;
    glob(directory, fn, false);

    PUZZLE_SIZE = fn.size();

    for(size_t i = 0; i < PUZZLE_SIZE; i++)
        initialPuzzleImages.push_back(readImage(fn[i]));

    PUZZLE_IMAGES_SIZE = initialPuzzleImages.at(0).size();
    return 0;
}

void preprocImage(Mat &img){

    cvtColor(img,img,COLOR_BGR2GRAY);
    Mat blured = Mat::zeros(img.size(), CV_8UC1);
    GaussianBlur(img,img,Size(BLUR_KERNEL_SIZE,BLUR_KERNEL_SIZE),0);     // smoothing
    threshold(img,img,BINARY_TRESHOLD_VAL,255,THRESH_BINARY_INV);

    Mat element = getStructuringElement(MORPH_RECT, Size(STRUCT_ELEM_SIZE,STRUCT_ELEM_SIZE)); // fill small gaps
    morphologyEx(img, img,MORPH_CLOSE, element);
}

bool sortPointsCounterClockwise(const Point& p1, const Point& p2, const Point2f& centroid) {
    double angle1 = std::atan2(p1.y - centroid.y, p1.x - centroid.x);
    double angle2 = std::atan2(p2.y - centroid.y, p2.x - centroid.x);
    return angle1 < angle2; // Counterclockwise order
}

// Pipeline for detecting corners and choosing the right ones
vector<Point> getCorners(Mat img, int nmsNeighbourhoodSize){

    Mat corners = Mat::zeros( img.size(), CV_32FC1 );
    Mat cornersTresholded = Mat::zeros( img.size(), CV_32FC1 );
    cornerHarris( img, corners, CORNERS_BLOCK_SIZE, CORNERS_K_SIZE, CORNERS_K);
    Mat corners_norm, corners_scaled;
    normalize( corners, corners_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( corners_norm, corners_scaled );
    // apply tresholding
    for (int i = 0; i < corners_norm.rows; i++) {
        for (int j = 0; j < corners_norm.cols; j++) {
            if (static_cast<int>(corners_norm.at<float>(i, j)) > CORNERS_TRESH_VAL) {
                cornersTresholded.at<float>(i, j) = static_cast<int>(corners_norm.at<float>(i, j));
            }
        }
    }

    Mat nmsCorners = nonMaximaSuppression(cornersTresholded, nmsNeighbourhoodSize);
    nmsCorners = localCornerAvereging(nmsCorners,8);

    Mat extractedCorners = img.clone();
    cvtColor(extractedCorners, extractedCorners, COLOR_GRAY2BGR);
    int counterCorners{0};
    vector<Point> vcorners;
    for (int i = 0; i < nmsCorners.rows; i++){
        for (int j = 0; j < nmsCorners.cols; j++){
            if (nmsCorners.at<float>(i,j) != 0){
                vcorners.push_back(Point(j,i));
                circle(extractedCorners, cv::Point(j,i), 1, cv::Scalar(0, 0, 255),2);
                counterCorners++;
            }
        }
    }
    if(counterCorners < 4) return {};

    //Sorting corners clockwise
    Point centroid = getCentroid(vcorners);
    std::sort(vcorners.begin(),vcorners.end(), [&centroid](const Point &p1, const Point &p2){
        return sortPointsCounterClockwise(p1,p2,centroid);
    });

    // extractin four corners - stage1: max area
    vector<Point> mainCorners = getLargestArea(vcorners);
    for (int i = 0; i < 4; i++){
        circle(extractedCorners, mainCorners.at(i), 15, cv::Scalar(0, 255, 0),2);
    }
    //imshow("extracted corners",extractedCorners);
    return mainCorners;
}

Mat nonMaximaSuppression(Mat img, int windowSize){

    // output Matrix
    Mat nmsCorners = Mat::zeros(img.size(), CV_32FC1);
    int halfWindow = windowSize / 2;

    //iterate through each pixel in the matrix
    for(int y = halfWindow; y < img.rows - halfWindow; y++){
        for (int x = halfWindow; x < img.cols - halfWindow; x++){
            //get the current value
            float currentValue = img.at<float>(y,x);
            if (currentValue == 0 ) continue;
            // Check if it's the local maximum in the neighborhood

            bool isMax = true;
            for (int dy = -halfWindow; dy <= halfWindow; dy++){
                for(int dx = -halfWindow; dx <= halfWindow; dx++){
                    if(img.at<float>(y + dy, x + dx) > currentValue){
                        isMax = false;
                        break;
                    }
                }
                if(!isMax) break;
            }
            // retaining the value only if its local maximum
            if(isMax){
                nmsCorners.at<float>(y,x) = currentValue;
            }
        }
    }
    return nmsCorners;
}

Mat localCornerAvereging(Mat img, int windowSize){
    int halfWindow = windowSize / 2;

    //iterate through each pixel in the matrix
    for(int y = halfWindow; y < img.rows - halfWindow; y++){
        for (int x = halfWindow; x < img.cols - halfWindow; x++){
            //get the current value
            float currentValue = img.at<float>(y,x);
            if (currentValue == 0 ) continue;
            // Check if it's the local maximum in the neighborhood
            int new_x = x;
            int new_y = y;
            int numEqualValues{1};
            for (int dy = -halfWindow; dy <= halfWindow; dy++){
                for(int dx = -halfWindow; dx <= halfWindow; dx++){
                    if(img.at<float>(y + dy, x + dx) == currentValue){
                        new_x += x+dx;
                        new_y += y+dy;
                        img.at<float>(y,x) = 0;
                        img.at<float>(y+dy,x+dx) = 0;
                        numEqualValues++;
                    }
                }
            }
            new_y = (int)(new_y/numEqualValues);
            new_x = (int)(new_x/numEqualValues);
            img.at<float>(new_y,new_x) = currentValue;
        }
    }
    return img;
}

vector<Point> getLargestArea(const vector<Point> vcorners){
    vector<Point> maxAreaCorners;
    double maxArea = 0;

    // Collect the 4-point combination
    for (int i = 0; i < vcorners.size() - 3; i++) {
        for (int j = i + 1; j < vcorners.size() - 2; j++) {
            for (int k = j + 1; k < vcorners.size() - 1; k++) {
                for (int l = k + 1; l < vcorners.size(); l++) {
                    vector<Point> quad = {vcorners[i], vcorners[j], vcorners[k], vcorners[l]};
                    double area = contourArea(quad);
                    if (area > maxArea){
                        maxArea = area;
                        maxAreaCorners = quad;
                    }
                }
            }
        }
    }

    return maxAreaCorners;
}

vector<Point> getContour(const Mat &img){

    vector<vector<Point>> imageContours;
    vector<Point> puzzleContour;
    findContours(img, imageContours, RETR_TREE, CHAIN_APPROX_NONE);
    if (imageContours.size() > 1) imageContours.erase(imageContours.begin()); //if detected, delete the contour of image itself
    puzzleContour = imageContours.at(0);

    //Mat imgPuzzleContours = Mat::zeros(img.size(),CV_8UC3);
    //drawContours(imgPuzzleContours, imageContours, 0, Scalar(128,0,128), 1, LINE_8);

    return puzzleContour;
}

vector<Point> reinforceCorners(const vector<Point> aproxCorners, const vector<Point> contour, const int boxSize){
    vector<Point> reinforcedCorners(4);
    const int numCorners{4};
    vector<vector<Point>> contourInBox(aproxCorners.size());
    //filtering out contouir points that not fall int the area of corner
    for (int i = 0; i < numCorners; i++){
        Rect box(aproxCorners.at(i).x - boxSize/2, aproxCorners.at(i).y - boxSize/2, boxSize, boxSize);
        for(const auto contourPoint : contour){
            if(box.contains(contourPoint)){
                contourInBox.at(i).push_back(contourPoint);
            }
        }
    }

    // get first and last point of the  contourInBox and find the furthest point to the line
    for (int i = 0; i < numCorners; i++){
        double A = contourInBox.at(i).back().y - contourInBox.at(i).front().y;
        double B = contourInBox.at(i).front().x - contourInBox.at(i).back().x;
        double C = (contourInBox.at(i).back().x * contourInBox.at(i).front().y) - (contourInBox.at(i).front().x * contourInBox.at(i).back().y);
        double distance, maxDistance{0};
        for (int j = 0; j < contourInBox.at(i).size(); j++)
        {
            distance = std::abs(A * contourInBox.at(i).at(j).x + B * contourInBox.at(i).at(j).y + C) / std::sqrt(A * A + B * B);
            if (distance > maxDistance){
                maxDistance = distance;
                reinforcedCorners.at(i) = contourInBox.at(i).at(j);
            }
        }
    }

    return reinforcedCorners;
}

vector<vector<Point>> getPuzzleEdges(const vector<Point> contour, const vector<Point> reinfCorners){
    vector<vector<Point>> edges(4);

    // Find indexes of each corner in the contour
    vector<int> cornerIndexes;
    for (const auto corner : reinfCorners){
        auto it = std::find(contour.begin(), contour.end(), corner);
        if (it != contour.end()){
            cornerIndexes.push_back(std::distance(contour.begin(),it));
        } else {
            std::cerr << "Error: Corner not found in contour." << std::endl;
            return edges;
        }
    }
    // Sort corners indexes to have sequential order
    std::sort(cornerIndexes.begin(), cornerIndexes.end());
    // handling special case when first corner (top most) comes before beginning of the contour
    if ( contour.at(cornerIndexes.at(0)).y > contour.at(cornerIndexes.at(3)).y){
        int temp = cornerIndexes.at(3);
        cornerIndexes.insert(cornerIndexes.begin(), temp);
        cornerIndexes.erase(cornerIndexes.end());
    }

    // Extract edges between consecutive corners
    for (size_t i = 0; i < cornerIndexes.size(); i++){
        int startIdx = cornerIndexes[i];
        int endIdx = cornerIndexes[(i+1) % cornerIndexes.size()]; // Wrap around to the first corner
        std:: vector<Point> edge;
        // Wrap-around when end idex < start index
        if (startIdx <= endIdx){
            edge.insert(edge.end(), contour.begin() + startIdx, contour.begin() + endIdx +1);
        } else {
            edge.insert(edge.end(), contour.begin() + startIdx, contour.end());
            edge.insert(edge.end(), contour.begin(), contour.begin() + endIdx + 1);
        }

        edges[i] = edge;

    }

    Mat image = Mat::zeros(initialPuzzleImages.at(0).size(),CV_8UC3);
    for (size_t i = 0; i < edges.size(); i++) {
        const auto& edge = edges[i];

        // Draw the edge by connecting points
        for (size_t j = 0; j < edge.size() - 1; j++) {
            cv::line(image, edge[j], edge[j + 1], colors.at(i), 1);
        }
    }
    imshow("Edges separated", image);

    return edges;
}

Point getCentroid (const vector<Point> points){

    if(points.empty()) {
        std::cerr<<"@@@ Error calculating centroid! Passed vector is empty!" << std::endl;
        return {};
    }

    int64_t x_centroid = 0, y_centroid = 0;
    for (const auto& p : points) {
        x_centroid += p.x;
        y_centroid += p.y;
    }

    x_centroid /= points.size();
    y_centroid /= points.size();

    return Point(static_cast<int>(x_centroid), static_cast<int>(y_centroid));
}

void normalizeEdges(Element &elem, int numSamples){

    vector<vector<Point>> edges = elem.edges;
    vector<vector<Point>> normalizedEdges[4];

    for (int i = 0; i < edges.size(); i++){
        vector<Point> edge = edges.at(i);

        // Calculate legth
        vector<double> arcLength(edge.size(), 0.0);
        for (size_t j = 1; j < edge.size(); j++){
            float dx = edge.at(j).x - edge.at(j-1).x;
            float dy = edge.at(j).y - edge.at(j-1).y;
            arcLength.at(j) = arcLength.at(j-1) + std::sqrt(dx * dx + dy * dy);
        }
        double totalLength = arcLength.back();

        // Interpolating new points to have each edge evenly sampled for calculating chamfer distance
        double stepSize = totalLength / (numSamples - 1);
        vector<Point> resampledEdge;
        resampledEdge.push_back(edge.front()); //storing first point

        double currentLength = stepSize;
        size_t j = 1;
        while(resampledEdge.size() < numSamples - 1){
            while (j < edge.size() && arcLength[j] < currentLength){
                j++;
            }
            if (j >= edge.size()) break;

            // Linear Interpolation
            double t = (currentLength - arcLength[j-1])/(arcLength[j] - arcLength[j-1]);
            int newX = static_cast<int>((1 - t) * edge[j-1].x + t * edge[j].x);
            int newY = static_cast<int>((1 - t) * edge[j-1].y + t * edge[j].y);

            resampledEdge.push_back(Point(newX,newY));
            currentLength += stepSize;
        }

        resampledEdge.push_back(edge.back());

        //"moving" edges so that their centre of mass is in (0, 0)
        // means elimination of absolute coordinates of an edge within a puzzle image.
        Point centroid = getCentroid(resampledEdge);
        for (auto &edgePoint : resampledEdge){
            edgePoint.x -= centroid.x;
            edgePoint.y -= centroid.y;
        }

        // Removing edge orientation
        Point first = resampledEdge.front();
        Point last = resampledEdge.back();
        double angle = std::atan2(last.y - first.y, last.x - first.x);
        std::transform(resampledEdge.begin(),resampledEdge.end(),resampledEdge.begin(),
                        [angle](Point p) {
                            return Point{
                                static_cast<int>(p.x * cos(angle) - p.y * sin(-angle)),
                                static_cast<int>(p.x * sin(-angle) + p.y * cos(angle))
                            };
                        });
        if (angle < 0) angle = CV_2PI + angle;
        elem.initialEdgeOrientation.push_back(angle);

        // Rotating each edge in compliance with its type In/Out
        if(elem.edgeType.at(i) == 1 || elem.edgeType.at(i) == 0)
            elem.normalizedEdges.push_back(resampledEdge);
        else if (elem.edgeType.at(i) == -1){
            elem.normalizedEdges.push_back(resampledEdge); // populate with init data
            // Reverse and negate the points of InLet's from `normalizedEdges.at(i)`
            std::transform(resampledEdge.rbegin(), resampledEdge.rend(), elem.normalizedEdges.at(i).begin(), [](Point p){ return p*(-1);});
        }
        elem.edgeCentroid[i] = getCentroid(elem.edges.at(i));
        //double edgeAngle = std::atan2(pieceData.edgeCentroid[i].y - pieceData.centroid.y,  pieceData.edgeCentroid[i].x - pieceData.centroid.x);

    }
}



std::array<int,4> determineInOut(const vector<vector<Point>> edges, const double flatTreshold){
    std::array<int,4> edgeType;
    int i = 0;
    std::cout << "[ ";
    for (const auto &edge : edges) {
        if (edge.empty()) continue;

        // Compute centroid
        Point centroid = getCentroid(edge);
        Point p1 = edge.back();
        Point p2 = edge.front();

        double a = (p2.y - p1.y) / static_cast<double>(p2.x - p1.x);
        double b = p2.y - a * p2.x;

        // Standard line equation Ax + By + C = 0
        double A = p2.y - p1.y;
        double B = p1.x - p2.x;
        double C = p2.x * p1.y - p1.x * p2.y;

        // Compute perpendicular distance from centroid to line
        double distance = std::abs(A * centroid.x + B * centroid.y + C) / std::sqrt(A * A + B * B);

        if (distance < flatTreshold) {
            edgeType.at(i) = 0; // FLAT
            std::cout << "FLAT ";
        } else {

            if( i == 1 || i == 2 ){
                if ( centroid.y > centroid.x * a + b ) {
                    edgeType.at(i) = 1; // OUTLET
                    std::cout << "OUT ";
                }
                else {
                    edgeType.at(i) = -1; //INLET
                    std::cout << "IN ";
                }

            } else {
                if ( centroid.y < centroid.x * a + b ) {
                    edgeType.at(i) = 1; // OUTLET
                    std::cout << "OUT ";
                }
                else {
                    edgeType.at(i) = -1; //INLET

                    std::cout << "IN ";
                }
            }
        }
        i++;
    }
    std::cout << "]" << std::endl;
    return edgeType;
}

Element elementPipeline(Mat puzzleImage, int id){
    Element pieceData;
    pieceData.id = id;

    // find puzzle centroid
    Mat inverted,dist;
    bitwise_not(puzzleImage,inverted);
    distanceTransform(inverted,dist, DIST_L1, DIST_MASK_PRECISE, CV_8U);
    double minVal, maxVal;
    cv::Point maxLoc;
    cv::minMaxLoc(dist, &minVal, &maxVal, nullptr, &maxLoc);
    cvtColor(dist, dist, COLOR_GRAY2BGR);
    circle(dist,maxLoc,5,cv::Scalar(255, 0, 0),2);
    pieceData.centroid = maxLoc;
    //imshow("Centroid", dist);

    vector<Point> candidateCorners = getCorners(puzzleImage,LOCAL_MAXIMA_NEIGHBORHOOD);
    if (candidateCorners.size() < 4) {
        std::cerr << "@@@ Error! Could not find at least 4 corners!" << std::endl;
        return {};
    } else if (candidateCorners.size() >= 40) {
        std::cerr << "@@@ Error! Too many potential corners! Run Tuner to adjust parameters" << std::endl;
        return {};
    }
    vector<Point> puzzleContour = getContour(puzzleImage);
    vector<Point> reinforcedCorners = reinforceCorners(candidateCorners,puzzleContour, CORNERS_REINF_BOX_SIZE);
    pieceData.edges = getPuzzleEdges(puzzleContour,reinforcedCorners);
    pieceData.edgeType = determineInOut(pieceData.edges, FLAT_TRESHOLD);

    // normalize element edges for matching algorithm and get orientation of each edge
    normalizeEdges(pieceData, NORMALIZE_SAMPLES_VAL);

    int numFlats = std::count(pieceData.edgeType.begin(), pieceData.edgeType.end(),0);
    if (numFlats > 2){
        std::cerr << "@@@ Error! Piece " << id << " was assigned more than two flat edges!" << std::endl;
        return {};
    } else if(numFlats == 1){
        pieceData.isEdgePiece = true;
    } else if(numFlats == 2){
        pieceData.isCornerPiece = true;
    }

    return pieceData;
}

