/*
    FICHIER : miroir.cpp
    UTILITE :
    - Implémente le traitement miroir
*/

#include "miroir.h"

cv::Mat appliquerMiroirCore(
    const cv::Mat &entree,
    const ParametresMiroir &params)
{
    if (entree.empty())
        return entree;

    cv::Mat sortie = entree.clone();
    int flipCode = params.horizontal ? 1 : 0;

    cv::Mat out;
    cv::flip(sortie, out, flipCode);

    return out;
}
