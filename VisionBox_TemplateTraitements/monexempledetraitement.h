#pragma once
#include <opencv2/opencv.hpp>

struct ParametresMonExempleDeTraitement
{
    int forceTraitement = 50;
    int seuil = 128;
    bool activerOption = false;
};

cv::Mat appliquerMonExempleDeTraitementCore(
    const cv::Mat &entree,
    const ParametresMonExempleDeTraitement &params);
