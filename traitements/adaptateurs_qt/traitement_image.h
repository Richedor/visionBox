#pragma once

#include <QObject>
#include <QImage>
#include <QVariantMap>
#include <QWidget>

class TraitementImage : public QObject
{
    Q_OBJECT
public:
    explicit TraitementImage(QObject *parent = nullptr) : QObject(parent) {}

    virtual QString nom() const = 0;
    virtual QImage appliquer(const QImage &entree,
                             const QVariantMap &parametres) = 0;
    virtual QImage genererApercu(const QImage &entree,
                                 const QVariantMap &parametres) = 0;
    virtual QWidget* creerEditeurParametres(QWidget *parent = nullptr) = 0;

    // AJOUT POUR LOG / IMPORT-EXPORT
    virtual QString idTraitement() const = 0;                 // ex: "flou_gaussien"
    virtual QVariantMap lireParametresCourants() const = 0;   // ex: { "rayonPixels": 17 }
    virtual void appliquerParametresDepuis(const QVariantMap &p) = 0;

signals:
    void parametresModifies();
};
