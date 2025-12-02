#include "adaptateur_contours.h"
#include "../core/contours.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <opencv2/opencv.hpp>

// ------ conversions QImage <-> cv::Mat ------

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

// ---------------- AdaptateurContours ----------------

AdaptateurContours::AdaptateurContours(QObject *parent)
    : TraitementImage(parent)
{
    m_parametresCourants["seuilBas"] = 100;
    m_parametresCourants["seuilHaut"] = 200;
}

QString AdaptateurContours::nom() const
{
    return QStringLiteral("Contours (Canny)");
}

int AdaptateurContours::extraireSeuilBas(const QVariantMap &p) const
{
    return p.value("seuilBas", m_parametresCourants["seuilBas"]).toInt();
}

int AdaptateurContours::extraireSeuilHaut(const QVariantMap &p) const
{
    return p.value("seuilHaut", m_parametresCourants["seuilHaut"]).toInt();
}

QImage AdaptateurContours::appliquer(const QImage &entree,
                                     const QVariantMap &parametres)
{
    return appliquerContoursQt(
        entree,
        extraireSeuilBas(parametres),
        extraireSeuilHaut(parametres));
}

QImage AdaptateurContours::genererApercu(const QImage &entree,
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

    return appliquerContoursQt(
        base,
        extraireSeuilBas(parametres),
        extraireSeuilHaut(parametres));
}

QImage AdaptateurContours::appliquerContoursQt(
    const QImage &entree,
    int seuilBas, int seuilHaut) const
{
    if (entree.isNull())
        return entree;

    ParametresContours p;
    p.seuilBas = seuilBas;
    p.seuilHaut = seuilHaut;

    cv::Mat mat = qImageToMat(entree);
    cv::Mat out = appliquerContoursCore(mat, p);

    return matToQImage(out);
}

QWidget* AdaptateurContours::creerEditeurParametres(QWidget *parent)
{
    QWidget *conteneur = new QWidget(parent);
    auto *layout = new QVBoxLayout(conteneur);
    layout->setContentsMargins(16, 16, 16, 16);

    QLabel *titre = new QLabel(tr("Paramètres Contours (Canny)"), conteneur);
    layout->addWidget(titre);

    // ----- seuil bas -----
    QLabel *lblBas = new QLabel(tr("Seuil bas"), conteneur);
    layout->addWidget(lblBas);

    auto *ligneBas = new QHBoxLayout();
    QLabel *lblVB = new QLabel(QString::number(extraireSeuilBas({})), conteneur);
    QSlider *sldBas = new QSlider(Qt::Horizontal, conteneur);
    sldBas->setRange(0, 255);
    sldBas->setValue(extraireSeuilBas({}));

    ligneBas->addWidget(lblVB);
    ligneBas->addWidget(sldBas);
    layout->addLayout(ligneBas);

    QObject::connect(sldBas, &QSlider::valueChanged,
                     conteneur, [this, lblVB](int v){
                         m_parametresCourants["seuilBas"] = v;
                         lblVB->setText(QString::number(v));
                         emit parametresModifies();
                     });

    // ----- seuil haut -----
    QLabel *lblHaut = new QLabel(tr("Seuil haut"), conteneur);
    layout->addWidget(lblHaut);

    auto *ligneHaut = new QHBoxLayout();
    QLabel *lblVH = new QLabel(QString::number(extraireSeuilHaut({})), conteneur);
    QSlider *sldHaut = new QSlider(Qt::Horizontal, conteneur);
    sldHaut->setRange(0, 255);
    sldHaut->setValue(extraireSeuilHaut({}));

    ligneHaut->addWidget(lblVH);
    ligneHaut->addWidget(sldHaut);
    layout->addLayout(ligneHaut);

    QObject::connect(sldHaut, &QSlider::valueChanged,
                     conteneur, [this, lblVH](int v){
                         m_parametresCourants["seuilHaut"] = v;
                         lblVH->setText(QString::number(v));
                         emit parametresModifies();
                     });

    layout->addStretch();
    return conteneur;
}

QString AdaptateurContours::idTraitement() const
{
    return QStringLiteral("contours");
}

QVariantMap AdaptateurContours::lireParametresCourants() const
{
    return m_parametresCourants;
}

void AdaptateurContours::appliquerParametresDepuis(const QVariantMap &p)
{
    m_parametresCourants["seuilBas"] = extraireSeuilBas(p);
    m_parametresCourants["seuilHaut"] = extraireSeuilHaut(p);
}
