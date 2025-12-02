#include "webcamdialog.h"

#include <QMessageBox>
#include <QDebug>
#include <QPixmap>

WebcamDialog::WebcamDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle("Webcam");
    this->resize(640, 480);

    // ---------- UI ----------
    m_view = new QLabel(this);
    m_view->setAlignment(Qt::AlignCenter);
    m_view->setMinimumSize(320, 240);
    m_view->setStyleSheet("background-color: black;");

    m_btnCapture = new QPushButton(tr("Capturer"), this);
    connect(m_btnCapture, &QPushButton::clicked,
            this, &WebcamDialog::onCaptureClicked);

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_view, 1);
    m_layout->addWidget(m_btnCapture, 0, Qt::AlignCenter);
    setLayout(m_layout);

    // ---------- Ouverture de la webcam ----------
    // 0 = première webcam détectée
    if (!m_cap.open(0)) {
        QMessageBox::critical(this, tr("Erreur webcam"),
                              tr("Impossible d'ouvrir la webcam (index 0)."));
        return;
    }

    // Optionnel : réduire un peu la taille pour ne pas surcharger
    m_cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    // ---------- Timer pour lire les frames en continu ----------
    connect(&m_timer, &QTimer::timeout,
            this, &WebcamDialog::onFrameTimeout);

    // ~30 fps => 33 ms ; tu peux passer à 50–100 ms si c’est trop lourd
    m_timer.start(33);
}

WebcamDialog::~WebcamDialog()
{
    m_timer.stop();
    if (m_cap.isOpened()) {
        m_cap.release();
    }
}

void WebcamDialog::onFrameTimeout()
{
    if (!m_cap.isOpened())
        return;

    cv::Mat frame;
    m_cap >> frame;    // lit une nouvelle frame

    if (frame.empty()) {
        qWarning() << "WebcamDialog: frame vide";
        return;
    }

    // On garde la dernière frame pour la capture
    m_lastFrame = frame.clone();

    // Conversion en QImage et affichage
    QImage img = matToQImage(m_lastFrame);
    if (!img.isNull()) {
        m_view->setPixmap(QPixmap::fromImage(img).scaled(
            m_view->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));
    }
}

void WebcamDialog::onCaptureClicked()
{
    if (m_lastFrame.empty()) {
        QMessageBox::warning(this, tr("Capture"),
                             tr("Aucune image disponible à capturer."));
        return;
    }

    QImage img = matToQImage(m_lastFrame);
    if (img.isNull()) {
        QMessageBox::warning(this, tr("Capture"),
                             tr("Conversion de l'image capturée impossible."));
        return;
    }

    // On envoie l'image au MainWindow
    emit imageCaptured(img);

    // On ferme la boîte de dialogue
    accept();   // ou close();
}

// ---------- Conversion Mat -> QImage ----------
QImage WebcamDialog::matToQImage(const cv::Mat &mat)
{
    if (mat.empty())
        return QImage();

    QImage image;

    switch (mat.type()) {
    case CV_8UC3: {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        image = QImage(rgb.data,
                       rgb.cols,
                       rgb.rows,
                       rgb.step,
                       QImage::Format_RGB888).copy();
        break;
    }
    case CV_8UC4: {
        cv::Mat rgba;
        cv::cvtColor(mat, rgba, cv::COLOR_BGRA2RGBA);
        image = QImage(rgba.data,
                       rgba.cols,
                       rgba.rows,
                       rgba.step,
                       QImage::Format_RGBA8888).copy();
        break;
    }
    case CV_8UC1: {
        image = QImage(mat.data,
                       mat.cols,
                       mat.rows,
                       mat.step,
                       QImage::Format_Grayscale8).copy();
        break;
    }
    default:
        qWarning() << "matToQImage: type non supporté:" << mat.type();
        break;
    }

    return image;
}
