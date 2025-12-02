#include "panneau_flux_traitements.h"
#include "boite_traitement.h"
#include "traitements/adaptateurs_qt/traitement_image.h"
#include "mainwindow.h"

#include <QPainter>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QVariantMap>
#include <QLineF>
#include <QPointF>
#include <QPolygonF>
#include <QTimer>                  // pour le QTimer utilisé dans la fenêtre de paramètres
#include <QPushButton>             // pour le bouton Pause/Play

#include <cmath>
#include <opencv2/opencv.hpp>      // pour cv::VideoCapture, cv::Mat, etc.

#include <QStyle>                  // pour SP_MediaPlay / SP_MediaPause




// Helper local pour convertir un cv::Mat en QImage (copie de MainWindow::matToQImage)
static QImage matToQImage(const cv::Mat &mat)
{
    if (mat.empty())
        return QImage();

    switch (mat.type()) {
    case CV_8UC3: {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data,
                      rgb.cols,
                      rgb.rows,
                      rgb.step,
                      QImage::Format_RGB888).copy();
    }
    case CV_8UC4: {
        cv::Mat rgba;
        cv::cvtColor(mat, rgba, cv::COLOR_BGRA2RGBA);
        return QImage(rgba.data,
                      rgba.cols,
                      rgba.rows,
                      rgba.step,
                      QImage::Format_RGBA8888).copy();
    }
    case CV_8UC1: {
        return QImage(mat.data,
                      mat.cols,
                      mat.rows,
                      mat.step,
                      QImage::Format_Grayscale8).copy();
    }
    default:
        return QImage();
    }
}


PanneauFluxTraitements::PanneauFluxTraitements(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(320);
    setStyleSheet("background-color:#FFF6D8;");
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

    // Label d'aperçu
    QLabel *etiquetteApercu = new QLabel(&fenetre);
    etiquetteApercu->setScaledContents(true);
    etiquetteApercu->setFixedSize(300, 200);
    etiquetteApercu->setStyleSheet("background-color: black;");

    // Bouton Pause/Play superposé sur l'aperçu
    QPushButton *btnPlayPause = new QPushButton(etiquetteApercu);
    btnPlayPause->setCheckable(true);
    btnPlayPause->setChecked(false); // false = lecture
    btnPlayPause->setFlat(true);
    btnPlayPause->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(0, 0, 0, 80);"
        "  border: none;"
        "  border-radius: 20px;"
        "  min-width: 40px;"
        "  min-height: 40px;"
        "  font-size: 20px;"
        "  color: white;"
        "}"
        "QPushButton:checked {"
        "  background-color: rgba(0, 0, 0, 120);"
        "}"
        );

    btnPlayPause->setIcon(
        fenetre.style()->standardIcon(QStyle::SP_MediaPause)
        );
    btnPlayPause->setIconSize(QSize(24, 24));

    // position approximative au centre du label
    int cx = etiquetteApercu->width() / 2 - 20;
    int cy = etiquetteApercu->height() / 2 - 20;
    if (cx < 0) cx = 0;
    if (cy < 0) cy = 0;
    btnPlayPause->move(cx, cy);
    btnPlayPause->raise();

    // --- On regarde si la source actuelle est une vidéo ---
    MainWindow *mw = qobject_cast<MainWindow*>(this->window());
    QString typeAcq   = mw ? mw->typeAcquisition() : QString();
    QString cheminSrc = mw ? mw->cheminSource()    : QString();

    bool sourceVideo = (typeAcq == "video" && !cheminSrc.isEmpty());

    // Objets pour la lecture vidéo (valables pendant toute la durée de fenetre.exec())
    cv::VideoCapture cap;
    QTimer timer(&fenetre);
    bool paused = false;

    if (sourceVideo) {
        // On essaie d'ouvrir la vidéo
        if (!cap.open(cheminSrc.toStdString())) {
            // Si échec, on retombe sur le comportement image fixe
            sourceVideo = false;
        } else {
            double fps = cap.get(cv::CAP_PROP_FPS);
            int intervalMs = 33;
            if (fps > 1.0)
                intervalMs = static_cast<int>(1000.0 / fps);

            // Lancement de la lecture (une seule fois, pas de boucle automatique)
            timer.start(intervalMs);

            QObject::connect(&timer, &QTimer::timeout,
                             &fenetre,
                             [t, etiquetteApercu, boite, &cap, &timer, &paused]()
                             {
                                 if (paused)
                                     return;

                                 cv::Mat frame;
                                 if (!cap.read(frame) || frame.empty()) {
                                     // Fin de la vidéo -> on s'arrête, pas de boucle
                                     timer.stop();
                                     paused = true;
                                     return;
                                 }

                                 QImage frameImg = matToQImage(frame);
                                 if (frameImg.isNull())
                                     return;

                                 // On applique UNIQUEMENT CE traitement sur la frame
                                 QImage traite = t->genererApercu(frameImg, QVariantMap());

                                 etiquetteApercu->setPixmap(QPixmap::fromImage(traite));
                                 boite->definirImageApercu(traite);
                             });

            // Gestion du bouton Pause/Play
            QObject::connect(btnPlayPause, &QPushButton::clicked,
                             &fenetre,
                             [&paused, btnPlayPause, &timer, &cap, cheminSrc]()
                             {
                                 paused = btnPlayPause->isChecked();

                                 if (paused) {
                                     // Icône "Play"
                                     btnPlayPause->setIcon(
                                         btnPlayPause->style()->standardIcon(QStyle::SP_MediaPlay)
                                         );
                                 } else {
                                     // Icône "Pause"
                                     btnPlayPause->setIcon(
                                         btnPlayPause->style()->standardIcon(QStyle::SP_MediaPause)
                                         );

                                     // Si la vidéo est arrivée à la fin (timer arrêté), on peut relancer depuis le début
                                     if (!timer.isActive() && !cheminSrc.isEmpty()) {
                                         cap.release();
                                         if (cap.open(cheminSrc.toStdString())) {
                                             double fps = cap.get(cv::CAP_PROP_FPS);
                                             int intervalMs = 33;
                                             if (fps > 1.0)
                                                 intervalMs = static_cast<int>(1000.0 / fps);
                                             timer.start(intervalMs);
                                         }
                                     }
                                 }
                             });
        }
    }

    // Si ce n'est PAS une vidéo ou si l'ouverture a échoué -> comportement image fixe
    if (!sourceVideo) {
        btnPlayPause->hide();

        if (!m_imageSource.isNull()) {
            QImage ap = t->genererApercu(m_imageSource, QVariantMap());
            etiquetteApercu->setPixmap(QPixmap::fromImage(ap));
        }

        // On garde ton ancien mécanisme "paramètres → prévisualisation"
        QObject::connect(t, &TraitementImage::parametresModifies,
                         &fenetre,
                         [this, t, etiquetteApercu, boite]()
                         {
                             if (m_imageSource.isNull())
                                 return;


                             QImage nouveauApercu = t->genererApercu(m_imageSource, QVariantMap());
                             etiquetteApercu->setPixmap(QPixmap::fromImage(nouveauApercu));
                             boite->definirImageApercu(nouveauApercu);
                         });
    }

    QWidget *editeur = t->creerEditeurParametres(&fenetre);

    disposition->addWidget(etiquetteApercu);
    disposition->addWidget(editeur);

    fenetre.resize(360, 400);
    fenetre.exec();
}
