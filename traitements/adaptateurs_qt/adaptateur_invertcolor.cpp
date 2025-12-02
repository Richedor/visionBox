#include "adaptateur_invertcolor.h"
#include "../core/invertcolor.h"

#include <QVBoxLayout>
#include <QLabel>
#include <opencv2/opencv.hpp>

// ---- Conversion QImage <-> cv::Mat ----

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

// ---- AdaptateurInvertColor ----

AdaptateurInvertColor::AdaptateurInvertColor(QObject *parent)
    : TraitementImage(parent)
{
    m_parametresCourants["dummy"] = true; // rien à régler, mais VisionBox exige une map
}

QString AdaptateurInvertColor::nom() const
{
    return QStringLiteral("Inversion des couleurs");
}

QImage AdaptateurInvertColor::appliquer(const QImage &entree,
                                        const QVariantMap &parametres)
{
    Q_UNUSED(parametres);
    return appliquerInvertColorQt(entree);
}

QImage AdaptateurInvertColor::genererApercu(const QImage &entree,
                                            const QVariantMap &parametres)
{
    Q_UNUSED(parametres);
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

    return appliquerInvertColorQt(base);
}

QImage AdaptateurInvertColor::appliquerInvertColorQt(const QImage &entree) const
{
    if (entree.isNull())
        return entree;

    ParametresInvertColor p;

    cv::Mat mat = qImageToMat(entree);
    cv::Mat out = appliquerInvertColorCore(mat, p);

    return matToQImage(out);
}

QWidget* AdaptateurInvertColor::creerEditeurParametres(QWidget *parent)
{
    QWidget *conteneur = new QWidget(parent);
    auto *layout = new QVBoxLayout(conteneur);

    QLabel *titre = new QLabel(tr("Inversion des couleurs\n(Aucun paramètre)"), conteneur);
    layout->addWidget(titre);

    layout->addStretch();
    return conteneur;
}

// ----- Import / Export -----

QString AdaptateurInvertColor::idTraitement() const
{
    return QStringLiteral("invertcolor");
}

QVariantMap AdaptateurInvertColor::lireParametresCourants() const
{
    return m_parametresCourants;
}

void AdaptateurInvertColor::appliquerParametresDepuis(const QVariantMap &p)
{
    Q_UNUSED(p);
    // Aucun paramètre à appliquer
}
