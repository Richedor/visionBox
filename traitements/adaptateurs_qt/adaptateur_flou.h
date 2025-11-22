#pragma once

#include "traitement_image.h"

class AdaptateurFlou : public TraitementImage
{
    Q_OBJECT
public:
    explicit AdaptateurFlou(QObject *parent = nullptr);

    QString nom() const override;
    QImage appliquer(const QImage &entree,
                     const QVariantMap &parametres) override;
    QImage genererApercu(const QImage &entree,
                         const QVariantMap &parametres) override;
    QWidget* creerEditeurParametres(QWidget *parent = nullptr) override;

    //  pour le log / import-export
    QString idTraitement() const override;
    QVariantMap lireParametresCourants() const override;
    void appliquerParametresDepuis(const QVariantMap &p) override;

private:
    int m_rayonMin = 0;
    int m_rayonMax = 25;
    QVariantMap m_parametresCourants;   // stocke "rayonPixels"

    int extraireRayon(const QVariantMap &parametres) const;
    QImage appliquerFlouQt(const QImage &entree, int rayon) const;
};
