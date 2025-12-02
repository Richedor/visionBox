#include "adaptateur_miroir.h"
#include "../core/miroir.h"

#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <opencv2/opencv.hpp>

// ------- Conversion QImage <-> cv::Mat -------

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

// -------- AdaptateurMiroir --------

AdaptateurMiroir::AdaptateurMiroir(QObject *parent)
    : TraitementImage(parent)
{
    m_parametresCourants["horizontal"] = true;
}

QString AdaptateurMiroir::nom() const
{
    return QStringLiteral("Miroir");
}

bool AdaptateurMiroir::extraireFlip(const QVariantMap &parametres) const
{
    return parametres.value("horizontal",
                            m_parametresCourants.value("horizontal", true)).toBool();
}

QImage AdaptateurMiroir::appliquer(const QImage &entree,
                                   const QVariantMap &parametres)
{
    bool horizontal = extraireFlip(parametres);
    return appliquerMiroirQt(entree, horizontal);
}

QImage AdaptateurMiroir::genererApercu(const QImage &entree,
                                       const QVariantMap &parametres)
{
    if (entree.isNull())
        return QImage();

    bool horizontal = extraireFlip(parametres);
    return appliquerMiroirQt(entree, horizontal);
}

QImage AdaptateurMiroir::appliquerMiroirQt(const QImage &entree,
                                           bool horizontal) const
{
    if (entree.isNull())
        return entree;

    ParametresMiroir p;
    p.horizontal = horizontal;

    cv::Mat mat = qImageToMat(entree);
    cv::Mat out = appliquerMiroirCore(mat, p);

    return matToQImage(out);
}

QWidget* AdaptateurMiroir::creerEditeurParametres(QWidget *parent)
{
    QWidget *conteneur = new QWidget(parent);
    auto *layout = new QVBoxLayout(conteneur);
    layout->setContentsMargins(16, 16, 16, 16);

    QLabel *titre = new QLabel(tr("Paramètres miroir"), conteneur);
    layout->addWidget(titre);

    QCheckBox *chkH = new QCheckBox(tr("Miroir horizontal"), conteneur);
    chkH->setChecked(m_parametresCourants["horizontal"].toBool());
    layout->addWidget(chkH);

    QObject::connect(chkH, &QCheckBox::stateChanged,
                     conteneur, [this](int v){
                         m_parametresCourants["horizontal"] = (v == Qt::Checked);
                         emit parametresModifies();
                     });

    layout->addStretch();
    return conteneur;
}

// -------- Import / Export --------

QString AdaptateurMiroir::idTraitement() const
{
    return QStringLiteral("miroir");
}

QVariantMap AdaptateurMiroir::lireParametresCourants() const
{
    return m_parametresCourants;
}

void AdaptateurMiroir::appliquerParametresDepuis(const QVariantMap &p)
{
    bool h = extraireFlip(p);
    m_parametresCourants["horizontal"] = h;
}
