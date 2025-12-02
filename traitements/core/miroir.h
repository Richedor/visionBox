/*
    FICHIER : miroir.h
    UTILITE :
    - Déclare la structure ParametresMiroir
    - Déclare la fonction appliquerMiroirCore
*/

#pragma once
#include <opencv2/opencv.hpp>

// Pas de paramètres nécessaires, mais VisionBox impose une structure.
struct ParametresMiroir
{
    bool horizontal = true;   // flip horizontal
    // (si plus tard tu veux ajouter flip vertical : flip=0)
};

// Fonction principale du traitement (stateless, ne modifie pas l’entrée)
cv::Mat appliquerMiroirCore(
    const cv::Mat &entree,
    const ParametresMiroir &params);
