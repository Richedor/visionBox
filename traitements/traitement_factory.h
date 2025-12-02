#pragma once

#include "adaptateurs_qt/adaptateur_flou.h"
#include "adaptateurs_qt/adaptateur_mosaique.h"
#include "adaptateurs_qt/adaptateur_sepia.h"
#include "adaptateurs_qt/adaptateur_invertcolor.h"
#include "adaptateurs_qt/adaptateur_miroir.h"
#include "adaptateurs_qt/adaptateur_contours.h"

inline TraitementImage* creerTraitementDepuisId(const QString &id) {
    if (id == "flou_gaussien") return new AdaptateurFlou();
    if (id == "mosaique")      return new AdaptateurMosaique();
    if (id == "sepia")         return new AdaptateurSepia();
    if (id == "invertcolor")   return new AdaptateurInvertColor();
    if (id == "miroir")        return new AdaptateurMiroir();
    if (id == "contours")      return new AdaptateurContours();
    return nullptr;
}
