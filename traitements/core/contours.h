/*
    FICHIER : contours.h
    UTILITE :
    - Déclare la structure ParametresContours
    - Déclare la fonction appliquerContoursCore
*/

#pragma once
#include <opencv2/opencv.hpp>

struct ParametresContours
{
    int seuilBas = 100;
    int seuilHaut = 200;
};

cv::Mat appliquerContoursCore(
    const cv::Mat &entree,
    const ParametresContours &params);
