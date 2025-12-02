/*
    FICHIER : mosaique.h
    UTILITE :
    - Déclare la structure ParametresMosaique
    - Déclare la fonction appliquerMosaiqueCore
*/

#pragma once
#include <opencv2/opencv.hpp>

// Structure des paramètres du traitement mosaïque.
// Ajoute un paramètre simple : tailleBloc.
struct ParametresMosaique
{
    int tailleBloc = 10;   // taille de la mosaïque (>=1)
};

// Déclaration de la fonction OpenCV (stateless).
cv::Mat appliquerMosaiqueCore(
    const cv::Mat &entree,
    const ParametresMosaique &params);
