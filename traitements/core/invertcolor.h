/*
    FICHIER : invertcolor.h
    UTILITE :
    - Déclare la structure ParametresInvertColor
    - Déclare la fonction appliquerInvertColorCore
*/

#pragma once
#include <opencv2/opencv.hpp>

// Pas de paramètres pour ce traitement.
struct ParametresInvertColor
{
    bool dummy = false; // nécessaire pour VisionBox, même si non utilisé
};

cv::Mat appliquerInvertColorCore(
    const cv::Mat &entree,
    const ParametresInvertColor &params);
