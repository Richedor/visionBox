/*
    FICHIER : sepia.cpp
    UTILITE :
    - Implémente la fonction OpenCV du traitement Sépia.
*/

#include "sepia.h"
#include <algorithm>

cv::Mat appliquerSepiaCore(
    const cv::Mat &entree,
    const ParametresSepia &params)
{
    if (entree.empty())
        return entree;

    cv::Mat sortie = entree.clone();

    int intensite = std::clamp(params.intensite, 0, 100);
    if (intensite <= 0)
        return sortie;

    double t = intensite / 100.0; // 0.0 -> 1.0

    for (int y = 0; y < sortie.rows; ++y) {
        for (int x = 0; x < sortie.cols; ++x) {
            cv::Vec3b &pix = sortie.at<cv::Vec3b>(y, x);

            double b = pix[0];
            double g = pix[1];
            double r = pix[2];

            double tr = 0.393*r + 0.769*g + 0.189*b;
            double tg = 0.349*r + 0.686*g + 0.168*b;
            double tb = 0.272*r + 0.534*g + 0.131*b;

            tr = std::min(255.0, tr);
            tg = std::min(255.0, tg);
            tb = std::min(255.0, tb);

            double newR = (1.0 - t) * r + t * tr;
            double newG = (1.0 - t) * g + t * tg;
            double newB = (1.0 - t) * b + t * tb;

            pix[2] = static_cast<uchar>(std::min(255.0, std::max(0.0, newR)));
            pix[1] = static_cast<uchar>(std::min(255.0, std::max(0.0, newG)));
            pix[0] = static_cast<uchar>(std::min(255.0, std::max(0.0, newB)));
        }
    }

    return sortie;
}
