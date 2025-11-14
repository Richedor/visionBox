#include "webcamdialog.h"
#include <QMessageBox>

WebcamDialog::WebcamDialog(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle("Webcam");
    this->resize(640, 480);

    // widget d'affichage vidéo
    m_view = new QVideoWidget(this);

    // bouton capturer
    m_btnCapture = new QPushButton("Capturer", this);

    // layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_view, 1);
    layout->addWidget(m_btnCapture, 0);
    setLayout(layout);

    // caméra par défaut
    m_camera = new QCamera(this);
    m_captureSession.setCamera(m_camera);
    m_captureSession.setVideoOutput(m_view);

    // module de capture
    m_imageCapture = new QImageCapture(this);
    m_captureSession.setImageCapture(m_imageCapture);

    // connect signals
    connect(m_btnCapture, &QPushButton::clicked,
            this, &WebcamDialog::onCaptureClicked);

    connect(m_imageCapture, &QImageCapture::imageCaptured,
            this, &WebcamDialog::onImageCaptured);

    // démarrer la webcam
    m_camera->start();
}

WebcamDialog::~WebcamDialog()
{
    if (m_camera)
        m_camera->stop();
}

void WebcamDialog::onCaptureClicked()
{
    if (m_imageCapture && m_imageCapture->isReadyForCapture()) {
        m_imageCapture->capture();
    }
}

void WebcamDialog::onImageCaptured(int, const QImage &img)
{
    emit imageCaptured(img);  // envoyer au MainWindow
    this->accept();           // fermer la fenêtre
}
