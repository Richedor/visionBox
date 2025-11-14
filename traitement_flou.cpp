#include "traitement_flou.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QLabel>
#include <QtMath> // pour qBound éventuellement

TraitementFlou::TraitementFlou(QObject *parent)
    : TraitementImage(parent),
    m_rayonMinimum(0),
    m_rayonMaximum(25)
{
    // Valeurs par défaut des paramètres
    m_parametresCourants["rayonPixels"] = 5; // flou léger par défaut
}

QString TraitementFlou::nom() const
{
    return QStringLiteral("Flou");
}

int TraitementFlou::extraireRayon(const QVariantMap &parametres) const
{
    // On prend d'abord les parametres passés à la fonction
    if (parametres.contains("rayonPixels")) {
        int rayon = parametres.value("rayonPixels").toInt();
        return qBound(m_rayonMinimum, rayon, m_rayonMaximum);
    }

    // Sinon on utilise les paramètres internes
    int rayon = m_parametresCourants.value("rayonPixels", 5).toInt();
    return qBound(m_rayonMinimum, rayon, m_rayonMaximum);
}

QImage TraitementFlou::appliquer(const QImage &entree,
                                 const QVariantMap &parametres)
{
    int rayon = extraireRayon(parametres);
    return appliquerFlou(entree, rayon);
}

QImage TraitementFlou::genererApercu(const QImage &entree,
                                     const QVariantMap &parametres)
{
    if (entree.isNull())
        return QImage();

    int rayon = extraireRayon(parametres);

    // Pour l'aperçu, on peut réduire l'image pour aller plus vite
    QImage base = entree;
    const int maxDimension = 400;
    if (base.width() > maxDimension || base.height() > maxDimension) {
        base = base.scaled(maxDimension, maxDimension,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);
    }

    return appliquerFlou(base, rayon);
}

QWidget* TraitementFlou::creerEditeurParametres(QWidget *parent)
{
    QWidget *conteneur = new QWidget(parent);
    auto *layout = new QVBoxLayout(conteneur);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(10);

    // --- Titre global de la fenêtre ---
    QLabel *etiquetteTitre = new QLabel(tr("Paramètres du flou"), conteneur);
    etiquetteTitre->setProperty("role", "titreParametresTraitement");
    layout->addWidget(etiquetteTitre);

    // ============================
    // PARAMÈTRE 1 : RAYON (SLIDER)
    // ============================

    // 1) Nom du paramètre en texte simple
    QLabel *etiquetteNomParam =
        new QLabel(tr("Rayon du flou (en pixels)"), conteneur);
    etiquetteNomParam->setProperty("role", "nomParametreTraitement");
    layout->addWidget(etiquetteNomParam);

    // 2) Ligne avec : [Valeur actuelle] [SLIDER]
    auto *ligneSlider = new QHBoxLayout();
    ligneSlider->setContentsMargins(0, 0, 0, 0);
    ligneSlider->setSpacing(8);

    // Valeur initiale (issue des paramètres internes)
    int valeurInitiale =
        m_parametresCourants.value("rayonPixels", 5).toInt();
    valeurInitiale = qBound(m_rayonMinimum, valeurInitiale, m_rayonMaximum);

    // Label qui affiche la valeur actuelle
    QLabel *etiquetteValeur = new QLabel(conteneur);
    etiquetteValeur->setMinimumWidth(50);
    etiquetteValeur->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    etiquetteValeur->setText(QString::number(valeurInitiale));
    etiquetteValeur->setProperty("role", "valeurParametreTraitement");

    // Slider horizontal
    QSlider *curseurRayon = new QSlider(Qt::Horizontal, conteneur);
    curseurRayon->setMinimum(m_rayonMinimum);
    curseurRayon->setMaximum(m_rayonMaximum);
    curseurRayon->setValue(valeurInitiale);

    // Quand le slider bouge : mise à jour texte + paramètres internes
    QObject::connect(curseurRayon, &QSlider::valueChanged,
                     conteneur, [this, etiquetteValeur](int valeur) {
                         m_parametresCourants["rayonPixels"] = valeur;
                         etiquetteValeur->setText(QString::number(valeur));
                     });

    // On place [valeur] puis [slider] sur la même ligne
    ligneSlider->addWidget(etiquetteValeur);
    ligneSlider->addWidget(curseurRayon, 1);

    layout->addLayout(ligneSlider);
    layout->addStretch();

    return conteneur;
}


// ------------------------------------------------------------------
// Implémentation basique d'un flou en moyennant un voisinage carré.
// Ce n'est pas optimisé, mais lisible pour la team.
// ------------------------------------------------------------------
QImage TraitementFlou::appliquerFlou(const QImage &entree, int rayon) const
{
    if (entree.isNull() || rayon <= 0)
        return entree;

    QImage source = entree.convertToFormat(QImage::Format_ARGB32);
    QImage resultat(source.size(), source.format());

    int largeur = source.width();
    int hauteur = source.height();

    // Pour éviter de recalculer la même chose, on limite le rayon
    int r = qBound(1, rayon, 25);
    int diametre = 2 * r + 1;
    int nbPixelsVoisinage = diametre * diametre;

    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            int sommeR = 0;
            int sommeG = 0;
            int sommeB = 0;
            int sommeA = 0;

            // Parcours du voisinage (carré (2r+1)x(2r+1))
            for (int dy = -r; dy <= r; ++dy) {
                int yy = y + dy;
                if (yy < 0) yy = 0;
                if (yy >= hauteur) yy = hauteur - 1;

                for (int dx = -r; dx <= r; ++dx) {
                    int xx = x + dx;
                    if (xx < 0) xx = 0;
                    if (xx >= largeur) xx = largeur - 1;

                    QRgb pixel = source.pixel(xx, yy);
                    sommeR += qRed(pixel);
                    sommeG += qGreen(pixel);
                    sommeB += qBlue(pixel);
                    sommeA += qAlpha(pixel);
                }
            }

            int R = sommeR / nbPixelsVoisinage;
            int G = sommeG / nbPixelsVoisinage;
            int B = sommeB / nbPixelsVoisinage;
            int A = sommeA / nbPixelsVoisinage;

            resultat.setPixel(x, y, qRgba(R, G, B, A));
        }
    }

    return resultat;
}
