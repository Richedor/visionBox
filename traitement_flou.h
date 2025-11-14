#pragma once

#include "traitement_image.h"
#include <QVariantMap>
#include <QImage>

/**
 * @brief TraitementFlou
 * Implémentation de référence d'un traitement d'image.
 *
 * - Utilise un paramètre "rayonPixels" (int) dans un QVariantMap.
 * - Peut servir de base à toute la team pour créer d'autres traitements.
 */
class TraitementFlou : public TraitementImage
{
    Q_OBJECT

public:
    explicit TraitementFlou(QObject *parent = nullptr);

    // Nom affiché dans l'UI ("Flou")
    QString nom() const override;

    // Applique le flou sur l'image d'entrée (utilisé dans le pipeline complet)
    QImage appliquer(const QImage &entree,
                     const QVariantMap &parametres) override;

    // Génère un aperçu (peut être plus léger que appliquer)
    QImage genererApercu(const QImage &entree,
                         const QVariantMap &parametres) override;

    // Crée le widget de réglage des paramètres (slider pour le rayon)
    QWidget* creerEditeurParametres(QWidget *parent = nullptr) override;

private:
    // Paramètres courants internes (utilisés si parametres est vide)
    QVariantMap m_parametresCourants;

    // Bornes pour le rayon
    int m_rayonMinimum;
    int m_rayonMaximum;

    // Récupère le rayon à partir du QVariantMap (ou des valeurs courantes)
    int extraireRayon(const QVariantMap &parametres) const;

    // Applique réellement le flou (algorithme basique, à optimiser si besoin)
    QImage appliquerFlou(const QImage &entree, int rayon) const;
};
