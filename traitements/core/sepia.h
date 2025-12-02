/*
    FICHIER : sepia.h
    UTILITE :
    - Déclare la structure ParametresSepia
    - Déclare la fonction appliquerSepiaCore
*/

#pragma once
#include <opencv2/opencv.hpp>

// Paramètres du traitement Sépia
struct ParametresSepia
{
    int intensite = 70; // 0 = pas d'effet, 100 = sépia très fort
};

// Coeur du traitement Sépia (indépendant de Qt)
cv::Mat appliquerSepiaCore(
    const cv::Mat &entree,
    const ParametresSepia &params);
