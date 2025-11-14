#ifndef BOITE_TRAITEMENT_H
#define BOITE_TRAITEMENT_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QImage>

class QMouseEvent;
class QPaintEvent;

class TraitementImage; // déclaration avancée

class BoiteTraitement : public QWidget
{
    Q_OBJECT
public:
    explicit BoiteTraitement(TraitementImage *traitement,
                             QWidget *parent = nullptr);

    void definirImageApercu(const QImage &image);

    TraitementImage* traitement() const { return m_traitement; }

signals:
    void parametresDemandes(BoiteTraitement *boite);

protected:
    void mousePressEvent(QMouseEvent *evenement) override;
    void mouseMoveEvent(QMouseEvent *evenement) override;
    void mouseReleaseEvent(QMouseEvent *evenement) override;
    void paintEvent(QPaintEvent *evenement) override;

private slots:
    void surBoutonParametresClique();

private:
    TraitementImage *m_traitement;

    QLabel *m_etiquetteTitre;
    QLabel *m_etiquetteApercu;
    QPushButton *m_boutonParametres;

    bool m_enDeplacement = false;
    QPoint m_positionSourisLocal;
};

#endif // BOITE_TRAITEMENT_H
