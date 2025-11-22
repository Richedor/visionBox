#include "monexempledetraitement.h"

cv::Mat appliquerMonExempleDeTraitementCore(
        const cv::Mat &entree,
        const ParametresMonExempleDeTraitement &params)
{
    if (entree.empty())
        return entree;

    cv::Mat sortie = entree.clone();

    int force = std::clamp(params.forceTraitement, 0, 100);
    double sigma = (force / 100.0) * 10.0;

    if (sigma > 0)
        cv::GaussianBlur(entree, sortie, cv::Size(0, 0), sigma);

    if (params.activerOption)
        cv::threshold(sortie, sortie, params.seuil, 255, cv::THRESH_BINARY);

    return sortie;
}
