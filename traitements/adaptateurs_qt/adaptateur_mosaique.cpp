#include "adaptateur_mosaique.h"
#include "../core/mosaique.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
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

    cv::Mat rgba;
    cv::cvtColor(mat, rgba, cv::COLOR_BGR2RGBA);

    QImage img(rgba.data, rgba.cols, rgba.rows, rgba.step,
               QImage::Format_RGBA8888);
    return img.copy();
}

// ------------------ AdaptateurMosaique ------------------

AdaptateurMosaique::AdaptateurMosaique(QObject *parent)
    : TraitementImage(parent)
{
    m_parametresCourants["tailleBloc"] = 10;
}

QString AdaptateurMosaique::nom() const
{
    return QStringLiteral("Mosaïque");
}

int AdaptateurMosaique::extraireTailleBloc(const QVariantMap &parametres) const
{
    int v = parametres.value("tailleBloc",
                             m_parametresCourants.value("tailleBloc", 10)).toInt();
    return qBound(m_blockMin, v, m_blockMax);
}

QImage AdaptateurMosaique::appliquer(const QImage &entree,
                                     const QVariantMap &parametres)
{
    int block = extraireTailleBloc(parametres);
    return appliquerMosaiqueQt(entree, block);
}

QImage AdaptateurMosaique::genererApercu(const QImage &entree,
                                         const QVariantMap &parametres)
{
    if (entree.isNull())
        return QImage();

    QImage base = entree;

    const int maxDim = 400;
    if (base.width() > maxDim || base.height() > maxDim)
    {
        base = base.scaled(maxDim, maxDim,
                           Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);
    }

    int block = extraireTailleBloc(parametres);
    return appliquerMosaiqueQt(base, block);
}

QImage AdaptateurMosaique::appliquerMosaiqueQt(const QImage &entree, int block) const
{
    if (entree.isNull())
        return entree;

    ParametresMosaique p;
    p.tailleBloc = block;

    cv::Mat mat = qImageToMat(entree);
    cv::Mat out = appliquerMosaiqueCore(mat, p);

    return matToQImage(out);
}

QWidget* AdaptateurMosaique::creerEditeurParametres(QWidget *parent)
{
    QWidget *conteneur = new QWidget(parent);
    auto *layout = new QVBoxLayout(conteneur);
    layout->setContentsMargins(16, 16, 16, 16);

    QLabel *titre = new QLabel(tr("Paramètres mosaïque"), conteneur);
    layout->addWidget(titre);

    QLabel *labelNom = new QLabel(tr("Taille du bloc (pixels)"), conteneur);
    layout->addWidget(labelNom);

    int valInit = extraireTailleBloc(QVariantMap());

    auto *ligne = new QHBoxLayout();

    QLabel *labelValeur = new QLabel(QString::number(valInit), conteneur);
    labelValeur->setMinimumWidth(40);

    QSlider *slider = new QSlider(Qt::Horizontal, conteneur);
    slider->setMinimum(m_blockMin);
    slider->setMaximum(m_blockMax);
    slider->setValue(valInit);

    ligne->addWidget(labelValeur);
    ligne->addWidget(slider);
    layout->addLayout(ligne);

    QObject::connect(slider, &QSlider::valueChanged,
                     conteneur, [this, labelValeur](int v){
                         m_parametresCourants["tailleBloc"] = v;
                         labelValeur->setText(QString::number(v));
                         emit parametresModifies();
                     });

    return conteneur;
}

// ------------------ Import / Export ------------------

QString AdaptateurMosaique::idTraitement() const
{
    return QStringLiteral("mosaique");
}

QVariantMap AdaptateurMosaique::lireParametresCourants() const
{
    return m_parametresCourants;
}

void AdaptateurMosaique::appliquerParametresDepuis(const QVariantMap &p)
{
    int b = extraireTailleBloc(p);
    m_parametresCourants["tailleBloc"] = b;
}
