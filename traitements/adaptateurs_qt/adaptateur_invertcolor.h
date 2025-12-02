#pragma once

#include "traitement_image.h"

    class AdaptateurInvertColor : public TraitementImage
{
    Q_OBJECT
public:
    explicit AdaptateurInvertColor(QObject *parent = nullptr);

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
    QImage appliquerInvertColorQt(const QImage &entree) const;
};


/* À coller dans traitement_factory.h :

#include "adaptateurs_qt/adaptateur_invertcolor.h"

if (id == "invertcolor")
    return new AdaptateurInvertColor();

*/
