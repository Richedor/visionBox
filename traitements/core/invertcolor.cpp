/*
    FICHIER : invertcolor.cpp
    UTILITE :
    - Implémente l'inversion des couleurs (négatif)
*/

#include "invertcolor.h"

cv::Mat appliquerInvertColorCore(
    const cv::Mat &entree,
    const ParametresInvertColor &params)
{
    if (entree.empty())
        return entree;

    cv::Mat sortie = entree.clone();
    cv::bitwise_not(sortie, sortie);

    return sortie;
}
