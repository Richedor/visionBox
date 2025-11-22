#include "panneau_flux_traitements.h"
#include "boite_traitement.h"
#include "traitements/adaptateurs_qt/traitement_image.h"
#include <QPainter>
#include <QDialog>
#include <QVBoxLayout>

#include <QLabel>
#include <QPixmap>
#include <QVariantMap>
#include <QLineF>
#include <QPointF>
#include <QPolygonF>
#include <cmath>


PanneauFluxTraitements::PanneauFluxTraitements(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(320);
    setStyleSheet("background-color:#ffeef4;");
}

QSize PanneauFluxTraitements::sizeHint() const
{
    // Taille par défaut proposée au layout / QScrollArea
    int w = minimumWidth();
    int h = minimumHeight();

    if (w <= 0) w = 400;   // largeur par défaut si rien de spécial
    if (h <= 0) h = 320;   // hauteur par défaut si rien de spécial

    return QSize(w, h);
}


void PanneauFluxTraitements::definirOrdreTraitements(
    const QList<TraitementImage*> &liste,
    const QImage &imageSource)
{
    // on supprime les anciennes boites
    for (BoiteTraitement *boite : m_boites) {
        boite->deleteLater();
    }
    m_boites.clear();

    m_traitements = liste;
    m_imageSource = imageSource;

    int x = 40;
    int y = 40;

    // imageCourante va servir à faire un pipeline pour les aperçus
    QImage imageCourante = m_imageSource;

    for (TraitementImage *t : m_traitements) {
        auto *boite = new BoiteTraitement(t, this);
        boite->move(x, y);
        boite->show();

        // --- Aperçu en pipeline ---
        if (!imageCourante.isNull()) {
            // On génère un aperçu à partir de l'image courante
            QImage apercu = t->genererApercu(imageCourante, QVariantMap());
            boite->definirImageApercu(apercu);

            // Et pour le traitement suivant, on utilise cet aperçu comme nouvelle entrée
            imageCourante = apercu;
        }

        connect(boite, &BoiteTraitement::parametresDemandes,
                this, &PanneauFluxTraitements::ouvrirFenetreParametres);

        m_boites << boite;

        x += boite->width() + 80; // espacement horizontal
    }

    // Ajuster la largeur minimale du panneau en fonction du nombre de boîtes
    int largeurTotale = x + 40;          // marge de droite
    if (largeurTotale < 400)
        largeurTotale = 400;            // largeur minimale par défaut

    setMinimumWidth(largeurTotale);
    // garder une hauteur confortable
    if (minimumHeight() < 320)
        setMinimumHeight(320);
    update(); // redessiner flèches
}


void PanneauFluxTraitements::paintEvent(QPaintEvent *evenement)
{
    QWidget::paintEvent(evenement);

    if (m_boites.size() < 2)
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPen stylo(Qt::black, 2);
    p.setPen(stylo);

    // On définit PI correctement, compatible MSVC
    constexpr double PI = 3.14159265358979323846;

    for (int i = 0; i < m_boites.size() - 1; ++i) {
        BoiteTraitement *source = m_boites[i];
        BoiteTraitement *cible  = m_boites[i+1];

        QPoint debut(source->geometry().right(),
                     source->geometry().center().y());
        QPoint fin(cible->geometry().left(),
                   cible->geometry().center().y());

        // ----------- Ligne principale ----------- //
        p.drawLine(debut, fin);

        // ----------- Petite flèche côté cible ----------- //
        const double tailleFleche = 8.0;
        QLineF ligne(debut, fin);

        // angle de la ligne
        double angle = std::atan2(-ligne.dy(), ligne.dx());

        // points des côtés de la flèche
        QPointF p1 = fin + QPointF(
                         -tailleFleche * std::cos(angle + PI / 6),
                         tailleFleche * std::sin(angle + PI / 6)
                         );

        QPointF p2 = fin + QPointF(
                         -tailleFleche * std::cos(angle - PI / 6),
                         tailleFleche * std::sin(angle - PI / 6)
                         );

        // flèche en forme de triangle
        QPolygonF tete;
        tete << fin << p1 << p2;

        p.setBrush(Qt::black);
        p.drawPolygon(tete);
    }
}

// Ouvre une fenêtre simple avec l’éditeur de paramètres fourni par le traitement
void PanneauFluxTraitements::ouvrirFenetreParametres(BoiteTraitement *boite)
{
    TraitementImage *t = boite->traitement();
    if (!t) return;

    QDialog fenetre(this);
    fenetre.setWindowTitle("Paramètres - " + t->nom());
    fenetre.setModal(true);

    QVBoxLayout *disposition = new QVBoxLayout(&fenetre);

    // aperçu actuel
    QLabel *etiquetteApercu = new QLabel(&fenetre);
    etiquetteApercu->setScaledContents(true);
    etiquetteApercu->setFixedSize(300, 200);

    if (!m_imageSource.isNull()) {
        QImage ap = t->genererApercu(m_imageSource, QVariantMap());
        etiquetteApercu->setPixmap(QPixmap::fromImage(ap));
    }

    QWidget *editeur = t->creerEditeurParametres(&fenetre);

    // 🔹 NOUVEAU : actualiser l'aperçu local et la boîte quand les paramètres changent
    QObject::connect(t, &TraitementImage::parametresModifies,
                     &fenetre,
                     [this, t, etiquetteApercu, boite]()
                     {
                         if (m_imageSource.isNull())
                             return;

                         // On régénère l'aperçu avec les paramètres courants internes du traitement
                         QImage nouveauApercu = t->genererApercu(m_imageSource, QVariantMap());

                         // Aperçu dans la fenêtre de paramètres
                         etiquetteApercu->setPixmap(QPixmap::fromImage(nouveauApercu));

                         // Aperçu sur la boîte dans le panneau
                         boite->definirImageApercu(nouveauApercu);
                     });

    disposition->addWidget(etiquetteApercu);
    disposition->addWidget(editeur);

    fenetre.resize(360, 400);
    fenetre.exec();
}
