#include "adaptateur_sepia.h"
#include "../core/sepia.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <opencv2/opencv.hpp>

// ------------------ Conversion QImage <-> Mat ------------------

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

    cv::Mat matRGBA;
    cv::cvtColor(mat, matRGBA, cv::COLOR_BGR2RGBA);

    QImage img(matRGBA.data, matRGBA.cols, matRGBA.rows,
               matRGBA.step, QImage::Format_RGBA8888);
    return img.copy();
}

// ------------------ AdaptateurSepia ------------------

AdaptateurSepia::AdaptateurSepia(QObject *parent)
    : TraitementImage(parent)
{
    m_parametresCourants["intensite"] = 70;
}

QString AdaptateurSepia::nom() const
{
    return tr("Sépia");
}

int AdaptateurSepia::extraireIntensite(const QVariantMap &parametres) const
{
    int valeur = parametres.value("intensite",
                                  m_parametresCourants.value("intensite", 70)).toInt();
    if (valeur < m_intensiteMin) valeur = m_intensiteMin;
    if (valeur > m_intensiteMax) valeur = m_intensiteMax;
    return valeur;
}

QImage AdaptateurSepia::appliquerSepiaQt(const QImage &entree,
                                         int intensite) const
{
    cv::Mat matEntree = qImageToMat(entree);

    ParametresSepia params;
    params.intensite = intensite;

    cv::Mat matSortie = appliquerSepiaCore(matEntree, params);

    return matToQImage(matSortie);
}

QImage AdaptateurSepia::appliquer(const QImage &entree,
                                  const QVariantMap &parametres)
{
    int intensite = extraireIntensite(parametres);
    return appliquerSepiaQt(entree, intensite);
}

QImage AdaptateurSepia::genererApercu(const QImage &entree,
                                      const QVariantMap &parametres)
{
    // Ici : même rendu que le traitement complet
    return appliquer(entree, parametres);
}

QWidget* AdaptateurSepia::creerEditeurParametres(QWidget *parent)
{
    QWidget *conteneur = new QWidget(parent);
    auto *layout = new QVBoxLayout(conteneur);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(10);

    QLabel *titre = new QLabel(tr("Paramètres du sépia"), conteneur);
    layout->addWidget(titre);

    QLabel *labelNom = new QLabel(tr("Intensité (0 à 100)"), conteneur);
    layout->addWidget(labelNom);

    titre->setStyleSheet("color: #222222;");
    labelNom->setStyleSheet("color: #222222;");

    auto *ligne = new QHBoxLayout();
    ligne->setSpacing(8);

    int valeurInitiale = extraireIntensite(QVariantMap());
    QLabel *labelValeur = new QLabel(QString::number(valeurInitiale), conteneur);
    labelValeur->setMinimumWidth(40);
    labelValeur->setStyleSheet("color: #222222;");

    QSlider *slider = new QSlider(Qt::Horizontal, conteneur);
    slider->setMinimum(m_intensiteMin);
    slider->setMaximum(m_intensiteMax);
    slider->setValue(valeurInitiale);

    ligne->addWidget(labelValeur);
    ligne->addWidget(slider, 1);

    layout->addLayout(ligne);
    layout->addStretch();

    QObject::connect(slider, &QSlider::valueChanged,
                     conteneur, [this, labelValeur](int v) {
                         m_parametresCourants["intensite"] = v;
                         labelValeur->setText(QString::number(v));
                         emit parametresModifies();
                     });

    return conteneur;
}

// -------- Log / import-export --------

QString AdaptateurSepia::idTraitement() const
{
    return QStringLiteral("sepia");
}

QVariantMap AdaptateurSepia::lireParametresCourants() const
{
    return m_parametresCourants;
}

void AdaptateurSepia::appliquerParametresDepuis(const QVariantMap &p)
{
    int val = extraireIntensite(p);
    m_parametresCourants["intensite"] = val;
}
