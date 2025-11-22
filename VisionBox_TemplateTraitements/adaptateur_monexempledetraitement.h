#pragma once

#include "traitement_image.h"
#include "../core/monexempledetraitement.h"

class AdaptateurMonExempleDeTraitement : public TraitementImage
{
    Q_OBJECT
public:
    explicit AdaptateurMonExempleDeTraitement(QObject *parent = nullptr);

    QString nom() const override;
    QString idTraitement() const override;

    QImage appliquer(const QImage &entree,
                     const QVariantMap &parametres) override;

    QImage genererApercu(const QImage &entree,
                         const QVariantMap &parametres) override;

    QWidget* creerEditeurParametres(QWidget *parent = nullptr) override;

    QVariantMap lireParametresCourants() const override;
    void appliquerParametresDepuis(const QVariantMap &p) override;

private:
    QVariantMap m_parametresCourants;

    int m_forceMin = 0;
    int m_forceMax = 100;

    int m_seuilMin = 0;
    int m_seuilMax = 255;
};

/*

Code a integrer par Miguel dans traitement_factory.h

#include "adaptateurs_qt/adaptateur_monexempledetraitement.h"

if (id == "monexempledetraitement")
    return new AdaptateurMonExempleDeTraitement();

*/
