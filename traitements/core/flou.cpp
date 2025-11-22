#include "flou.h"

cv::Mat appliquerFlouCore(const cv::Mat &entree, const ParametresFlou &params)
{
    if (entree.empty())
        return entree;

    int rayon = std::max(0, params.rayonPixels);
    if (rayon <= 0)
        return entree.clone();

    int r = std::min(rayon, 25);
    double sigma = static_cast<double>(r);

    cv::Mat flou;
    // ksize = (0,0) -> OpenCV déduit la taille à partir de sigma
    cv::GaussianBlur(entree, flou, cv::Size(0, 0), sigma, sigma);
    return flou;
}
