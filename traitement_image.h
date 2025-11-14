#ifndef TRAITEMENT_IMAGE_H
#define TRAITEMENT_IMAGE_H

#include <QObject>
#include <QImage>
#include <QVariantMap>
#include <QWidget>

class TraitementImage : public QObject
{
    Q_OBJECT
public:
    explicit TraitementImage(QObject *parent = nullptr) : QObject(parent) {}

    // Nom affiché dans l’UI (Flou, Mosaïque, Contours…)
    virtual QString nom() const = 0;

    // Applique le traitement complet (appelé par ton pipeline)
    virtual QImage appliquer(const QImage &entree,
                             const QVariantMap &parametres) = 0;

    // Retourne un aperçu rapide pour la boite
    virtual QImage genererApercu(const QImage &entree,
                                 const QVariantMap &parametres) = 0;

    // Crée un petit widget avec les contrôles de paramètres (slider, spinbox…)
    // L’autre équipe fournit l’implémentation.
    virtual QWidget* creerEditeurParametres(QWidget *parent = nullptr) = 0;
};

#endif // TRAITEMENT_IMAGE_H
