#ifndef PANNEAU_FLUX_TRAITEMENTS_H
#define PANNEAU_FLUX_TRAITEMENTS_H

#include <QWidget>
#include <QList>
#include <QImage>

class TraitementImage;
class BoiteTraitement;
class QPaintEvent;

class PanneauFluxTraitements : public QWidget
{
    Q_OBJECT
public:
    explicit PanneauFluxTraitements(QWidget *parent = nullptr);

    // Appelé par ta fenêtre principale quand la barre de traitements change
    void definirOrdreTraitements(const QList<TraitementImage*> &liste,
                                 const QImage &imageSource);

protected:
    void paintEvent(QPaintEvent *evenement) override;
    QSize sizeHint() const override;

private slots:
    void ouvrirFenetreParametres(BoiteTraitement *boite);

private:
    QList<TraitementImage*> m_traitements;
    QList<BoiteTraitement*> m_boites;
    QImage m_imageSource;
};

#endif // PANNEAU_FLUX_TRAITEMENTS_H
