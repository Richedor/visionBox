#include "adaptateur_flou.h"
#include "../core/flou.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QtMath>
#include <opencv2/opencv.hpp>

// ---- petits helpers de conversion QImage <-> cv::Mat ----

static cv::Mat qImageToMat(const QImage &image)
{
    QImage img = image.convertToFormat(QImage::Format_RGBA8888);
    cv::Mat mat(img.height(), img.width(), CV_8UC4,
                const_cast<uchar*>(img.bits()), img.bytesPerLine());
    cv::Mat matBGR;
    cv::cvtColor(mat, matBGR, cv::COLOR_RGBA2BGR);
    return matBGR.clone();
}

static QImage matToQImage(const cv::Mat &mat)
{
    if (mat.empty())
        return QImage();

    cv::Mat rgba;
    cv::cvtColor(mat, rgba, cv::COLOR_BGR2RGBA);

    QImage img(rgba.data, rgba.cols, rgba.rows, rgba.step,
               QImage::Format_RGBA8888);
    return img.copy();
}

// ---- AdaptateurFlou ----

AdaptateurFlou::AdaptateurFlou(QObject *parent)
    : TraitementImage(parent)
{
    m_parametresCourants["rayonPixels"] = 5;
}

QString AdaptateurFlou::nom() const
{
    return QStringLiteral("Flou");
}

int AdaptateurFlou::extraireRayon(const QVariantMap &parametres) const
{
    int rayon = parametres.value("rayonPixels",
                                 m_parametresCourants.value("rayonPixels", 5)).toInt();
    return qBound(m_rayonMin, rayon, m_rayonMax);
}

QImage AdaptateurFlou::appliquer(const QImage &entree,
                                 const QVariantMap &parametres)
{
    int rayon = extraireRayon(parametres);
    return appliquerFlouQt(entree, rayon);
}

QImage AdaptateurFlou::genererApercu(const QImage &entree,
                                     const QVariantMap &parametres)
{
    if (entree.isNull())
        return QImage();

    int rayon = extraireRayon(parametres);
    QImage base = entree;

    const int maxDim = 400;
    if (base.width() > maxDim || base.height() > maxDim) {
        base = base.scaled(maxDim, maxDim,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);
    }

    return appliquerFlouQt(base, rayon);
}

QImage AdaptateurFlou::appliquerFlouQt(const QImage &entree, int rayon) const
{
    if (entree.isNull())
        return entree;

    ParametresFlou p;
    p.rayonPixels = rayon;

    cv::Mat mat = qImageToMat(entree);
    cv::Mat out = appliquerFlouCore(mat, p);
    return matToQImage(out);
}

QWidget* AdaptateurFlou::creerEditeurParametres(QWidget *parent)
{
    QWidget *conteneur = new QWidget(parent);
    auto *layout = new QVBoxLayout(conteneur);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(10);

    QLabel *titre = new QLabel(tr("Paramètres du flou"), conteneur);
    layout->addWidget(titre);

    QLabel *labelNom = new QLabel(tr("Rayon du flou (en pixels)"), conteneur);
    layout->addWidget(labelNom);

    titre->setStyleSheet("color: #222222;");
    labelNom->setStyleSheet("color: #222222;");

    auto *ligne = new QHBoxLayout();
    ligne->setSpacing(8);

    int valeurInitiale = extraireRayon(QVariantMap());

    QLabel *labelValeur = new QLabel(QString::number(valeurInitiale), conteneur);
    labelValeur->setMinimumWidth(40);
    labelValeur->setStyleSheet("color: #222222;");

    QSlider *slider = new QSlider(Qt::Horizontal, conteneur);
    slider->setMinimum(m_rayonMin);
    slider->setMaximum(m_rayonMax);
    slider->setValue(valeurInitiale);

    ligne->addWidget(labelValeur);
    ligne->addWidget(slider, 1);

    layout->addLayout(ligne);
    layout->addStretch();

    QObject::connect(slider, &QSlider::valueChanged,
                     conteneur, [this, labelValeur](int v) {
                         m_parametresCourants["rayonPixels"] = v;
                         labelValeur->setText(QString::number(v));
                         emit parametresModifies();
                     });

    return conteneur;
}

// ======================
//  NOUVELLES MÉTHODES
// ======================

QString AdaptateurFlou::idTraitement() const
{
    // identifiant stable utilisé dans le JSON / import-export
    return QStringLiteral("flou_gaussien");
}

QVariantMap AdaptateurFlou::lireParametresCourants() const
{
    // on renvoie la map des paramètres actuels
    return m_parametresCourants;
}

void AdaptateurFlou::appliquerParametresDepuis(const QVariantMap &p)
{
    int rayon = extraireRayon(p);
    m_parametresCourants["rayonPixels"] = rayon;
}
