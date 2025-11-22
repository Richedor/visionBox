#pragma once

#include <opencv2/opencv.hpp>

// Paramètres du flou (sans Qt)
struct ParametresFlou
{
    int rayonPixels = 5;
};

// Coeur du traitement flou (indépendant de Qt)
cv::Mat appliquerFlouCore(const cv::Mat &entree, const ParametresFlou &params);
