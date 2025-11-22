#include "adaptateur_monexempledetraitement.h"
#include <QSlider>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>

static cv::Mat qImageToMat(const QImage &image)
{
    QImage img = image.convertToFormat(QImage::Format_RGBA8888);
    return cv::Mat(img.height(), img.width(), CV_8UC4,
                   (void*)img.bits(), img.bytesPerLine()).clone();
}

static QImage matToQImage(const cv::Mat &mat)
{
    if (mat.empty())
        return QImage();

    cv::Mat matRGBA;
    if (mat.channels() == 1)
        cv::cvtColor(mat, matRGBA, cv::COLOR_GRAY2RGBA);
    else if (mat.channels() == 3)
        cv::cvtColor(mat, matRGBA, cv::COLOR_BGR2RGBA);
    else if (mat.channels() == 4)
        matRGBA = mat.clone();
    else
        return QImage();

    return QImage((uchar*)matRGBA.data, matRGBA.cols, matRGBA.rows,
                  matRGBA.step, QImage::Format_RGBA8888).copy();
}

AdaptateurMonExempleDeTraitement::AdaptateurMonExempleDeTraitement(QObject *parent)
    : TraitementImage(parent)
{
    m_parametresCourants["forceTraitement"] = 50;
    m_parametresCourants["seuil"] = 128;
    m_parametresCourants["activerOption"] = false;
}

QString AdaptateurMonExempleDeTraitement::nom() const
{
    return "Mon exemple de traitement";
}

QString AdaptateurMonExempleDeTraitement::idTraitement() const
{
    return "monexempledetraitement";
}

QImage AdaptateurMonExempleDeTraitement::appliquer(
        const QImage &entree,
        const QVariantMap &parametres)
{
    ParametresMonExempleDeTraitement p;
    p.forceTraitement = parametres.value("forceTraitement").toInt();
    p.seuil = parametres.value("seuil").toInt();
    p.activerOption = parametres.value("activerOption").toBool();

    cv::Mat in = qImageToMat(entree);
    cv::Mat out = appliquerMonExempleDeTraitementCore(in, p);

    return matToQImage(out);
}

QImage AdaptateurMonExempleDeTraitement::genererApercu(
        const QImage &entree,
        const QVariantMap &parametres)
{
    return appliquer(entree, parametres);
}

QVariantMap AdaptateurMonExempleDeTraitement::lireParametresCourants() const
{
    return m_parametresCourants;
}

void AdaptateurMonExempleDeTraitement::appliquerParametresDepuis(const QVariantMap &p)
{
    m_parametresCourants = p;
    emit parametresModifies();
}

QWidget* AdaptateurMonExempleDeTraitement::creerEditeurParametres(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(w);

    QLabel *lbl1 = new QLabel("Force du traitement");
    QSlider *sld1 = new QSlider(Qt::Horizontal);
    sld1->setRange(m_forceMin, m_forceMax);
    sld1->setValue(m_parametresCourants["forceTraitement"].toInt());
    connect(sld1, &QSlider::valueChanged, this, [&](int v){
        m_parametresCourants["forceTraitement"] = v;
        emit parametresModifies();
    });
    layout->addWidget(lbl1);
    layout->addWidget(sld1);

    QLabel *lbl2 = new QLabel("Seuil");
    QSlider *sld2 = new QSlider(Qt::Horizontal);
    sld2->setRange(m_seuilMin, m_seuilMax);
    sld2->setValue(m_parametresCourants["seuil"].toInt());
    connect(sld2, &QSlider::valueChanged, this, [&](int v){
        m_parametresCourants["seuil"] = v;
        emit parametresModifies();
    });
    layout->addWidget(lbl2);
    layout->addWidget(sld2);

    QCheckBox *chk = new QCheckBox("Activer l'option");
    chk->setChecked(m_parametresCourants["activerOption"].toBool());
    connect(chk, &QCheckBox::toggled, this, [&](bool b){
        m_parametresCourants["activerOption"] = b;
        emit parametresModifies();
    });
    layout->addWidget(chk);

    return w;
}
