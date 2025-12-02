/*
    FICHIER : mosaique.cpp
    UTILITE :
    - Implémente la fonction OpenCV du traitement.
*/

#include "mosaique.h"
#include <algorithm>

cv::Mat appliquerMosaiqueCore(
    const cv::Mat &entree,
    const ParametresMosaique &params)
{
    if (entree.empty())
        return entree;

    int block = std::max(1, params.tailleBloc);

    cv::Mat sortie = entree.clone();

    int smallW = std::max(1, entree.cols / block);
    int smallH = std::max(1, entree.rows / block);

    cv::Mat small;
    cv::resize(entree, small, cv::Size(smallW, smallH));
    cv::resize(small, sortie, entree.size(), 0, 0, cv::INTER_NEAREST);

    return sortie;
}
