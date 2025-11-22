#include "webcamdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QPixmap>

WebcamDialog::WebcamDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle("Webcam");
    this->resize(640, 480);

    // Zone d'affichage vidéo (remplace QVideoWidget)
    m_view = new QLabel(this);
    m_view->setAlignment(Qt::AlignCenter);
    m_view->setMinimumSize(320, 240);
    m_view->setStyleSheet("background-color: black;");

    // Bouton capturer
    m_btnCapture = new QPushButton("Capturer", this);

    // Layout
    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_view, 1);
    m_layout->addWidget(m_btnCapture, 0);
    setLayout(m_layout);

    // Ouvrir la webcam par défaut (index 0)
    if (!m_cap.open(0)) {
        QMessageBox::warning(this,
                             "Erreur webcam",
                             "Impossible d'ouvrir la webcam (index 0).");
        m_btnCapture->setEnabled(false);
        return;
    }

    // Timer pour rafraîchir les images ~30 FPS
    connect(&m_timer, &QTimer::timeout,
            this, &WebcamDialog::updateFrame);
    m_timer.start(30);

    // Clic sur "Capturer"
    connect(m_btnCapture, &QPushButton::clicked,
            this, &WebcamDialog::onCaptureClicked);
}

WebcamDialog::~WebcamDialog()
{
    m_timer.stop();

    if (m_cap.isOpened()) {
        m_cap.release();
    }
}

void WebcamDialog::updateFrame()
{
    cv::Mat frame;
    m_cap >> frame;  // lire une nouvelle frame

    if (frame.empty()) {
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
        QMessageBox::information(this,
                                 "Capture",
                                 "Aucune image disponible à capturer.");
        return;
    }

    QImage img = matToQImage(m_lastFrame);
    if (img.isNull()) {
        QMessageBox::warning(this,
                             "Capture",
                             "Échec de la conversion de l'image.");
        return;
    }

    // On envoie l'image au MainWindow (comme avant)
    emit imageCaptured(img);

    // On ferme la fenêtre après capture
    this->accept();
}

QImage WebcamDialog::matToQImage(const cv::Mat &mat)
{
    if (mat.empty())
        return QImage();

    QImage image;

    switch (mat.type()) {
    case CV_8UC3: {
        // BGR -> RGB
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
        // BGRA -> RGBA
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
        // Niveaux de gris
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
