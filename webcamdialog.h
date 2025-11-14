#pragma once

#include <QDialog>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QImageCapture>
#include <QPushButton>
#include <QVBoxLayout>

class WebcamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WebcamDialog(QWidget *parent = nullptr);
    ~WebcamDialog();

signals:
    void imageCaptured(const QImage &image);  // envoyé au MainWindow

private slots:
    void onCaptureClicked();
    void onImageCaptured(int id, const QImage &img);

private:
    QCamera *m_camera = nullptr;
    QMediaCaptureSession m_captureSession;
    QVideoWidget *m_view = nullptr;
    QImageCapture *m_imageCapture = nullptr;
    QPushButton *m_btnCapture = nullptr;
};
