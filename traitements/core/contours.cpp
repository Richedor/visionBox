/*
    FICHIER : contours.cpp
    UTILITE :
    - Implémente la détection de contours (Canny)
*/

#include "contours.h"

cv::Mat appliquerContoursCore(
    const cv::Mat &entree,
    const ParametresContours &params)
{
    if (entree.empty())
        return entree;

    cv::Mat gray;
    cv::cvtColor(entree, gray, cv::COLOR_BGR2GRAY);

    int seuil1 = std::max(0, params.seuilBas);
    int seuil2 = std::max(seuil1 + 1, params.seuilHaut);

    cv::Mat edges;
    cv::Canny(gray, edges, seuil1, seuil2);

    cv::Mat bgr;
    cv::cvtColor(edges, bgr, cv::COLOR_GRAY2BGR);

    return bgr;
}
