#pragma once

#include "traitement_image.h"

class AdaptateurSepia : public TraitementImage
{
    Q_OBJECT
public:
    explicit AdaptateurSepia(QObject *parent = nullptr);

    // Nom affiché dans l'interface
    QString nom() const override;

    // Application du traitement
    QImage appliquer(const QImage &entree,
                     const QVariantMap &parametres) override;

    // Aperçu (ici : même chose que appliquer)
    QImage genererApercu(const QImage &entree,
                         const QVariantMap &parametres) override;

    // Panneau Qt des paramètres
    QWidget* creerEditeurParametres(QWidget *parent = nullptr) override;

    // Log / import-export
    QString idTraitement() const override;
    QVariantMap lireParametresCourants() const override;
    void appliquerParametresDepuis(const QVariantMap &p) override;

private:
    int m_intensiteMin = 0;
    int m_intensiteMax = 100;
    QVariantMap m_parametresCourants; // stocke "intensite"

    int extraireIntensite(const QVariantMap &parametres) const;
    QImage appliquerSepiaQt(const QImage &entree, int intensite) const;
};


/* À coller dans traitement_factory.h :

#include "adaptateurs_qt/adaptateur_sepia.h"

if (id == "sepia")
    return new AdaptateurSepia();

*/
