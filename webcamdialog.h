#pragma once

#include <QDialog>
#include <QTimer>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <opencv2/opencv.hpp>

class WebcamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WebcamDialog(QWidget *parent = nullptr);
    ~WebcamDialog();

signals:
    // Image capturée et envoyée au MainWindow (inchangé)
    void imageCaptured(const QImage &image);

private slots:
    void onCaptureClicked();   // clic sur le bouton "Capturer"
    void updateFrame();        // appelé périodiquement pour rafraîchir la webcam

private:
    // OpenCV
    cv::VideoCapture m_cap;    // flux vidéo (webcam)
    cv::Mat m_lastFrame;       // dernière frame brute OpenCV

    // Timer pour acquisition périodique
    QTimer m_timer;

    // Interface
    QLabel *m_view = nullptr;        // zone d'affichage de l'image
    QPushButton *m_btnCapture = nullptr;
    QVBoxLayout *m_layout = nullptr;

    // Utilitaire de conversion Mat -> QImage
    QImage matToQImage(const cv::Mat &mat);
};
