#include "puzzle_matching.h"

using std::vector;
using cv::Point;

const double MATCH_SCORE_TRESH{5.0};
const double BEST_MATCH_TRESH{3.0};

// iterate through edges of both elems and find chamfer distance only for InLet&Outlet or Outlet&Inlet pairs.
// InLet pieces must be multiplied by (-1,-1) and must be read in reverse.
//std::transform(e2.edges.at(j).rbegin(), e2.edges.at(j).rend(), inletReversed.begin(), [](Point p){ return p*(-1);});
double chamferDistance(const vector<Point> e1, const vector<Point> e2){

    // Helper function to calculate the average minimum distance from one set to the other
    auto averageMinimumDistance = [](const vector<Point>& from, const vector<Point>& to) {
        double totalDistance = 0.0;

        for (const auto& p1 : from) {
            double minDistance = std::numeric_limits<double>::max();

            for (const auto& p2 : to) {
                double dx = p1.x - p2.x;
                double dy = p1.y - p2.y;
                double distance = std::sqrt(dx * dx + dy * dy);
                minDistance = std::min(minDistance, distance);
            }

            totalDistance += minDistance;
        }

        return totalDistance / from.size(); // Average distance
    };

    // Chamfer Distance = average of min distances in both directions
    double forwardDistance = averageMinimumDistance(e1, e2); // From e1 to e2
    double backwardDistance = averageMinimumDistance(e2, e1); // From e2 to e1

    return (forwardDistance + backwardDistance)/2;
}

std::map<std::pair<int,int>, std::vector<MatchInfo>> findMatches(std::vector<Element> &allElementsData){

    MatchInfo Match;
    std::map<std::pair<int,int>, std::vector<MatchInfo>>  allMatches;
    Element elemA, elemB;
    double similarityScore;

    for (int i = 0; i < allElementsData.size(); i++){
        elemA = allElementsData.at(i);

        for (int j = 0; j < allElementsData.size(); j++) {
            elemB = allElementsData.at(j);
            if (elemA.id == elemB.id) continue; // avoid self-self matching

            for (int ii = 0; ii < elemA.normalizedEdges.size(); ii++){
                if (elemA.edgeType.at(ii) == 0) continue; // skip flats

                for (int jj = 0; jj < elemB.normalizedEdges.size(); jj++){
                    if((elemA.edgeType.at(ii) == 1 && elemB.edgeType.at(jj) == -1) || (elemA.edgeType.at(ii) == -1 && elemB.edgeType.at(jj) == 1)){
                        std::pair<int, int> keyA = {elemA.id, ii};
                        similarityScore = chamferDistance(elemA.normalizedEdges.at(ii),elemB.normalizedEdges.at(jj));
                        Match = {elemB.id, jj, similarityScore};
                        allMatches[keyA].push_back(Match);

                    } else continue;
                }
            }
        }
    }
    return allMatches;
}

std::map<std::pair<int,int>, MatchInfo> extractBestMatches(std::map<std::pair<int,int>, std::vector<MatchInfo>> allMatches){
    std::map<std::pair<int,int>, MatchInfo> bestMatches;
    MatchInfo bestMatch;

    for (auto &entry : allMatches){
        std::pair<int,int> key = entry.first;
        std::vector<MatchInfo> &matches = entry.second;
        if (matches.empty()) continue;
        double bestMatchScore = std::numeric_limits<double>::infinity();
        for (auto &match : matches){
            if(match.similarityScore < bestMatchScore){
                bestMatchScore = match.similarityScore;
                bestMatch = match;
            }
        }
        bestMatches[key] = bestMatch;
    }
    return bestMatches;
}

void printMatches(const std::map<std::pair<int,int>, std::vector<MatchInfo>> matchMap){

    std::cout << "All matches: \n";
    for (const auto & [key, matches] : matchMap){
        std::cout <<"Element " << key.first + 1  << " edge " << key.second << ", matches:\n";
        for (const auto & match : matches){
            std::cout << "Element " << match.idB + 1 << " edge " << match.edgeB << ", with score: " << match.similarityScore << std::endl;
        }
    }
}

void assignMatches(std::vector<Element> &puzzle,const std::map<std::pair<int,int>, MatchInfo> bestMatchesMap){
    for (const auto & [key, match] : bestMatchesMap) {
        int searchId = key.first;
        int numEdge = key.second;

        for(auto &elem : puzzle){
            //if(elem.id == searchId && match.similarityScore <= BEST_MATCH_TRESH){
            if(elem.id == searchId){
                elem.matchingEdges[numEdge] = match.edgeB;
                elem.matchingElements[numEdge] = match.idB;
                break;
            }
        }
    }
}

void printMatches(const std::map<std::pair<int,int>, MatchInfo> matchMap){

    std::cout << "Best matches:\n";
    for (const auto& [key, match] : matchMap) {
        std::cout << "Element " << key.first + 1 << " (Edge " << key.second << ") → "
                  << "Element " << match.idB + 1<< " (Edge " << match.edgeB << ") "
                  << "Score: " << match.similarityScore << "\n";
    }
}

std::vector<Element> puzzleAssembly(const std::vector<Element> &allElementsData){

    // menaging the very first element in puzzle assembly
    Element firstElement;
    std::vector<Element> assembly;

    for (int i = 0; i < allElementsData.size(); i++){
        if(allElementsData.at(i).isCornerPiece == true){
            firstElement = allElementsData.at(i);
            break;
        }
    }

    // store rotation angle of the last flat edge in the piece
    double lastFlatAngle;
    int lastFlatIdx{0};
    for (int i = 0; i < 4; i ++){
        if (firstElement.edgeType.at(i) == 0){
            lastFlatAngle = firstElement.initialEdgeOrientation.at(i);
            lastFlatIdx = i;
        }
    }

    // rotate the first element so that its flats are rotated 360 and 270
    // clockwise means negative angles
    double rotationAngle = firstElement.rotationAngle = lastFlatAngle - CV_2PI;
    for (int i = 0; i < 4; i ++){
        if (firstElement.initialEdgeOrientation.at(i) - rotationAngle > CV_2PI )
            firstElement.finalEdgeOrientation.push_back(firstElement.initialEdgeOrientation.at(i) - rotationAngle - CV_2PI);
        else firstElement.finalEdgeOrientation.push_back(firstElement.initialEdgeOrientation.at(i) - rotationAngle);
    }

    // transform each edge centroid
    for (int i = 0; i < 4; i++){
        int x_c = firstElement.centroid.x;
        int y_c = firstElement.centroid.y;
        int x = firstElement.edgeCentroid.at(i).x;
        int y = firstElement.edgeCentroid.at(i).y;
        double new_x = x_c + (x - x_c)*cos(rotationAngle) - (y - y_c)*sin(rotationAngle);
        double new_y = y_c + (x - x_c)*sin(rotationAngle) + (y - y_c)*cos(rotationAngle);

        firstElement.edgeCentroid.at(i).x = new_x;
        firstElement.edgeCentroid.at(i).y = new_y;
    }
    assembly.push_back(firstElement);


    // ASSUMING PUZZLES LIKE 4 x 4, 6 x 6 ETC
    int rows = static_cast<int>(sqrt(PUZZLE_SIZE));
    int cols = rows;

    // Create a vactor with snake-patter numbers to track placing puzzles
    int num{0};
    vector<int> placedIdxMap(rows*cols);
    for (int i = 0; i < rows; i ++){
        if (i % 2 == 0){
            for (int j = 0; j < cols; j++){
                placedIdxMap[i*cols+j] = num++;
            }
        } else {
            for (int j = cols - 1; j >= 0; j--){
                placedIdxMap[i*cols+j] = num++;
            }
        }
    }

    int lastMatchedEdgeIdx{-1};
    for (int i = 0; i < PUZZLE_SIZE -1; i++){
        Element first = assembly.at(i);

        int firstFreeEdgeIdx;
        if (i == 0) { // Matching to first element
            firstFreeEdgeIdx = (lastFlatIdx + 1) % 4;
        } else if (placedIdxMap.at(i) % cols == cols - 1) { // RIGHT PUZZLE END
            firstFreeEdgeIdx = (lastMatchedEdgeIdx - 1) % 4;
            if (firstFreeEdgeIdx < 0) firstFreeEdgeIdx = 4 + firstFreeEdgeIdx;
        } else if (placedIdxMap.at(i) % cols == 0) { // LEFT PUZZLE END
            firstFreeEdgeIdx = (lastMatchedEdgeIdx + 1) % 4;
        } else {
            firstFreeEdgeIdx = (lastMatchedEdgeIdx + 2) % 4;
        }
        int pairID = assembly.at(i).matchingElements.at(firstFreeEdgeIdx);

        if (pairID == -1) {
            std::cerr << "@@@ Error! Element " << i << " in assembly[] does not have any pairs specified!" << std::endl;
            return {};
        }

        // get index of pair edge that matches first
        Element pair = allElementsData.at(pairID);
        auto matchFirstIt = std::find(pair.matchingElements.begin(), pair.matchingElements.end(), first.id);
        if (matchFirstIt == pair.matchingElements.end() || *matchFirstIt == -1){
            std::cerr<< "@@@ Error! Matching algorithm failed when finding pair edge for element " << first.id << std::endl;
            return {};
        }
        int matchEdgeIdx = std::distance(pair.matchingElements.begin(), matchFirstIt);
        lastMatchedEdgeIdx = matchEdgeIdx;
        // acquiring rotation angle of the pair puzzle
        double alfa = first.finalEdgeOrientation.at(firstFreeEdgeIdx);
        double beta{0.0};
        beta = alfa + CV_PI;
        if (beta > CV_2PI) beta -= CV_2PI;

        pair.rotationAngle = pair.initialEdgeOrientation.at(matchEdgeIdx) - beta;

        std::cout << "Rotation angle is is: " << pair.rotationAngle * (180/CV_PI) << std::endl;
        // getting final orientation of each edge
        for (int j = 0; j < 4; j++){
            double angle = pair.initialEdgeOrientation.at(j) - pair.rotationAngle;
            if (angle > CV_2PI) angle -= CV_2PI;
            else if (angle < 0) angle += CV_2PI;
            pair.finalEdgeOrientation.push_back(angle);
        }

        // transform each edge centroid
        int x_c = pair.centroid.x;
        int y_c = pair.centroid.y;
        for (int j = 0; j < 4; j++){
            int x = pair.edgeCentroid.at(j).x;
            int y = pair.edgeCentroid.at(j).y;
            double new_x = x_c + (x - x_c)*cos(pair.rotationAngle) - (y - y_c)*sin(pair.rotationAngle);
            double new_y = y_c + (x - x_c)*sin(pair.rotationAngle) + (y - y_c)*cos(pair.rotationAngle);

            pair.edgeCentroid.at(j).x = new_x;
            pair.edgeCentroid.at(j).y = new_y;
        }
        assembly.push_back(pair);
    }


    return assembly;
}

void drawAssembly(vector<Element> assembly){
    for (int i = 0; i < assembly.size(); i++){
        Mat MM = getRotationMatrix2D(assembly.at(i).centroid, assembly.at(i).rotationAngle * (180/CV_PI), 1);
        Mat puzzlePiece;
        int id = assembly.at(i).id;
        warpAffine(initialPuzzleImages.at(id), puzzlePiece, MM, initialPuzzleImages.at(id).size());
        std::cout << "Element " << i << ": " << std::endl;
        std::cout << "Rotated by: " << assembly.at(i).rotationAngle * (180/CV_PI) << std::endl;
        for (int j = 0; j < 4; j++){
            std::cout << "  Edge " << j << " initial rotation is " << assembly.at(i).initialEdgeOrientation.at(j) * 180/CV_PI << std::endl;
            std::cout << "  Edge " << j << " final rotation is " << assembly.at(i).finalEdgeOrientation.at(j) * 180/CV_PI << std::endl;
        }
        std::string windowName = "Element " + std::to_string(i + 1);
        imshow(windowName, puzzlePiece);
    }
    waitKey();
}


std::vector<Element> matchingPipeline(std::vector<Element> &processedElements){
    std::map<std::pair<int,int>, std::vector<MatchInfo>>  puzzleMatchInfo;
    std::map<std::pair<int,int>, MatchInfo> puzzleBestMatches;

    puzzleMatchInfo = findMatches(processedElements);
    printMatches(puzzleMatchInfo);
    std::cout<<std::endl;
    puzzleBestMatches = extractBestMatches(puzzleMatchInfo);
    printMatches(puzzleBestMatches);
    assignMatches(processedElements,puzzleBestMatches);
    std::vector<Element> assembly = puzzleAssembly(processedElements);
    // drawAssembly(assembly);
    return assembly;
}

