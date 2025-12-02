#pragma once

#include "traitement_image.h"

class AdaptateurMosaique : public TraitementImage
{
    Q_OBJECT
public:
    explicit AdaptateurMosaique(QObject *parent = nullptr);

    QString nom() const override;
    QImage appliquer(const QImage &entree,
                     const QVariantMap &parametres) override;
    QImage genererApercu(const QImage &entree,
                         const QVariantMap &parametres) override;
    QWidget* creerEditeurParametres(QWidget *parent = nullptr) override;

    // Import / Export
    QString idTraitement() const override;
    QVariantMap lireParametresCourants() const override;
    void appliquerParametresDepuis(const QVariantMap &p) override;

private:
    int m_blockMin = 1;
    int m_blockMax = 50;
    QVariantMap m_parametresCourants;

    int extraireTailleBloc(const QVariantMap &parametres) const;
    QImage appliquerMosaiqueQt(const QImage &entree, int block) const;
};


/* À coller dans traitement_factory.h :

#include "adaptateurs_qt/adaptateur_mosaique.h"

if (id == "mosaique")
    return new AdaptateurMosaique();

*/
