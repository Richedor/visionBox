#pragma once

#include "traitement_image.h"

class AdaptateurMiroir : public TraitementImage
{
    Q_OBJECT
public:
    explicit AdaptateurMiroir(QObject *parent = nullptr);

    QString nom() const override;
    QImage appliquer(const QImage &entree,
                     const QVariantMap &parametres) override;
    QImage genererApercu(const QImage &entree,
                         const QVariantMap &parametres) override;
    QWidget* creerEditeurParametres(QWidget *parent = nullptr) override;

    QString idTraitement() const override;
    QVariantMap lireParametresCourants() const override;
    void appliquerParametresDepuis(const QVariantMap &p) override;

private:
    QVariantMap m_parametresCourants;

    bool extraireFlip(const QVariantMap &parametres) const;
    QImage appliquerMiroirQt(const QImage &entree, bool horizontal) const;
};


/* À coller dans traitement_factory.h :

#include "adaptateurs_qt/adaptateur_miroir.h"

if (id == "miroir")
    return new AdaptateurMiroir();

*/
