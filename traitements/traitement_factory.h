#pragma once

#include "adaptateurs_qt/adaptateur_flou.h"
// ajouter ici les futurs traitements
// #include "adaptateurs_qt/adaptateur_mosaique.h"
// #include "adaptateurs_qt/adaptateur_contours.h"

inline TraitementImage* creerTraitementDepuisId(const QString &id) {
    if (id == "flou_gaussien") return new AdaptateurFlou();
    // if (id == "mosaique") return new AdaptateurMosaique();
    // if (id == "contours") return new AdaptateurContours();
    return nullptr;
}
