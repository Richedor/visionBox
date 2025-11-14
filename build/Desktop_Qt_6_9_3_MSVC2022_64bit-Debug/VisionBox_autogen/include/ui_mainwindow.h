/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "clickablelabel.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_2;
    QStackedWidget *stackedWidget;
    QWidget *page_1_Select;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *layoutMain;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    ClickableLabel *labelVideo;
    ClickableLabel *labelImage;
    ClickableLabel *labelWebcam;
    QWidget *page_2_Display;
    QLabel *labelPreview;
    QFrame *panneauGauche;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *dispositionPanneauGauche;
    QWidget *zoneEtiquettes;
    QLabel *labelTitreTraitements;
    QFrame *panneauCentre;
    QWidget *verticalLayoutWidget_4;
    QVBoxLayout *dispositionPanneauCentre;
    QFrame *panneauDroite;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *dispositionPanneauDroite;
    QMenuBar *menubar;
    QMenu *menuVisionBox;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1102, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setStyleSheet(QString::fromUtf8("QWidget {\n"
"    background-color: #FFF0F3; /* fond rose clair */\n"
"}\n"
"\n"
"QLabel#labelWebcam,\n"
"QLabel#labelImage,\n"
"QLabel#labelVideo {\n"
"    border: 2px solid #A92B48;\n"
"    border-radius: 12px;\n"
"    padding: 8px;\n"
"}\n"
"\n"
"QLabel#labelWebcam:hover,\n"
"QLabel#labelImage:hover,\n"
"QLabel#labelVideo:hover {\n"
"    background-color: #FFDCE2;\n"
"    cursor: pointer;\n"
"}\n"
""));
        horizontalLayout_2 = new QHBoxLayout(centralwidget);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        page_1_Select = new QWidget();
        page_1_Select->setObjectName("page_1_Select");
        verticalLayoutWidget = new QWidget(page_1_Select);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(70, 70, 965, 391));
        layoutMain = new QVBoxLayout(verticalLayoutWidget);
        layoutMain->setObjectName("layoutMain");
        layoutMain->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName("label");
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        QBrush brush1(QColor(255, 240, 243, 255));
        brush1.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush1);
        QBrush brush2(QColor(0, 0, 0, 128));
        brush2.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush2);
#endif
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush1);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush2);
#endif
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush1);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush2);
#endif
        label->setPalette(palette);
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        font.setPointSize(14);
        font.setBold(true);
        font.setItalic(false);
        label->setFont(font);
        label->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    color: black;\n"
"    font: bold 14pt \"Segoe UI\";\n"
"}\n"
"\n"
""));
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutMain->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(30);
        horizontalLayout->setObjectName("horizontalLayout");
        labelVideo = new ClickableLabel(verticalLayoutWidget);
        labelVideo->setObjectName("labelVideo");
        labelVideo->setPixmap(QPixmap(QString::fromUtf8(":/images/images/video.png")));
        labelVideo->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout->addWidget(labelVideo);

        labelImage = new ClickableLabel(verticalLayoutWidget);
        labelImage->setObjectName("labelImage");
        labelImage->setPixmap(QPixmap(QString::fromUtf8(":/images/images/image.png")));
        labelImage->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout->addWidget(labelImage);

        labelWebcam = new ClickableLabel(verticalLayoutWidget);
        labelWebcam->setObjectName("labelWebcam");
        labelWebcam->setPixmap(QPixmap(QString::fromUtf8(":/images/images/webcam.png")));
        labelWebcam->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout->addWidget(labelWebcam);


        layoutMain->addLayout(horizontalLayout);

        stackedWidget->addWidget(page_1_Select);
        page_2_Display = new QWidget();
        page_2_Display->setObjectName("page_2_Display");
        page_2_Display->setStyleSheet(QString::fromUtf8("/* ======== PANNEAU GAUCHE ======== */\n"
"QFrame#panneauGauche {\n"
"    background-color: #FFEFF4;\n"
"    border: 2px solid #B30033;\n"
"    border-radius: 22px; /* angles bien arrondis */\n"
"    padding: 15px;\n"
"}\n"
"\n"
"/* Titre \"Mes traitements\" */\n"
"QLabel#labelTitreTraitements {\n"
"    color: #B30033;\n"
"    font-size: 20px;\n"
"    font-weight: 600;\n"
"    qproperty-alignment: AlignHCenter;\n"
"    font-family: \"Segoe UI\", \"Arial\", sans-serif;\n"
"}\n"
"\n"
"/* ===== LISTE ===== */\n"
"QListWidget#listeTraitements {\n"
"    background-color: white;\n"
"    border: 2px solid #B30033;\n"
"    border-radius: 18px;   /* angles courb\303\251s */\n"
"    padding: 8px;\n"
"    font-size: 14px;\n"
"    font-family: \"Segoe UI\", \"Arial\", sans-serif;\n"
"    color: #333;\n"
"}\n"
"\n"
"QListWidget::item {\n"
"    padding: 10px;\n"
"    margin: 6px 4px;\n"
"    border-radius: 12px;  /* arrondis pour les items */\n"
"}\n"
"\n"
"QListWidget::item:hover {\n"
"    background-color: #FFE1EA;\n"
"  "
                        "  border-radius: 12px;\n"
"}\n"
"\n"
"QListWidget::item:selected {\n"
"    background-color: #F7B6C4;\n"
"    border-radius: 12px;\n"
"    color: #000;\n"
"}\n"
"\n"
"/* ======== PANNEAU CENTRE ======== */\n"
"QFrame#panneauCentre {\n"
"    background-color: #FFF7FB;\n"
"    border: 2px solid #B30033;\n"
"    border-radius: 22px;\n"
"    padding: 15px;\n"
"}\n"
"\n"
"/* ======== PANNEAU DROITE ======== */\n"
"QFrame#panneauDroite {\n"
"    background-color: #FFF7FB;\n"
"    border: 2px solid #B30033;\n"
"    border-radius: 22px;\n"
"    padding: 15px;\n"
"}\n"
"\n"
"/* ======== BOUTONS ======== */\n"
"QPushButton {\n"
"    background-color: #B30033;\n"
"    color: white;\n"
"    border-radius: 16px;  /* arrondi */\n"
"    padding: 10px 18px;\n"
"    border: 2px solid #8A0024;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    font-family: \"Segoe UI\", \"Arial\", sans-serif;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #D31545;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-col"
                        "or: #8A0024;\n"
"}\n"
""));
        labelPreview = new QLabel(page_2_Display);
        labelPreview->setObjectName("labelPreview");
        labelPreview->setGeometry(QRect(860, 450, 121, 101));
        labelPreview->setScaledContents(true);
        panneauGauche = new QFrame(page_2_Display);
        panneauGauche->setObjectName("panneauGauche");
        panneauGauche->setGeometry(QRect(0, 20, 191, 511));
        panneauGauche->setFrameShape(QFrame::Shape::StyledPanel);
        panneauGauche->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayoutWidget_2 = new QWidget(panneauGauche);
        verticalLayoutWidget_2->setObjectName("verticalLayoutWidget_2");
        verticalLayoutWidget_2->setGeometry(QRect(-1, -1, 191, 521));
        dispositionPanneauGauche = new QVBoxLayout(verticalLayoutWidget_2);
        dispositionPanneauGauche->setObjectName("dispositionPanneauGauche");
        dispositionPanneauGauche->setContentsMargins(0, 0, 0, 0);
        zoneEtiquettes = new QWidget(verticalLayoutWidget_2);
        zoneEtiquettes->setObjectName("zoneEtiquettes");

        dispositionPanneauGauche->addWidget(zoneEtiquettes);

        labelTitreTraitements = new QLabel(verticalLayoutWidget_2);
        labelTitreTraitements->setObjectName("labelTitreTraitements");
        labelTitreTraitements->setAlignment(Qt::AlignmentFlag::AlignHCenter);

        dispositionPanneauGauche->addWidget(labelTitreTraitements);

        panneauCentre = new QFrame(page_2_Display);
        panneauCentre->setObjectName("panneauCentre");
        panneauCentre->setGeometry(QRect(210, 20, 621, 511));
        panneauCentre->setFrameShape(QFrame::Shape::StyledPanel);
        panneauCentre->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayoutWidget_4 = new QWidget(panneauCentre);
        verticalLayoutWidget_4->setObjectName("verticalLayoutWidget_4");
        verticalLayoutWidget_4->setGeometry(QRect(0, 0, 621, 511));
        dispositionPanneauCentre = new QVBoxLayout(verticalLayoutWidget_4);
        dispositionPanneauCentre->setObjectName("dispositionPanneauCentre");
        dispositionPanneauCentre->setContentsMargins(0, 0, 0, 0);
        panneauDroite = new QFrame(page_2_Display);
        panneauDroite->setObjectName("panneauDroite");
        panneauDroite->setGeometry(QRect(880, 10, 221, 511));
        panneauDroite->setFrameShape(QFrame::Shape::StyledPanel);
        panneauDroite->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayoutWidget_3 = new QWidget(panneauDroite);
        verticalLayoutWidget_3->setObjectName("verticalLayoutWidget_3");
        verticalLayoutWidget_3->setGeometry(QRect(-10, 110, 221, 511));
        dispositionPanneauDroite = new QVBoxLayout(verticalLayoutWidget_3);
        dispositionPanneauDroite->setObjectName("dispositionPanneauDroite");
        dispositionPanneauDroite->setContentsMargins(0, 0, 0, 0);
        stackedWidget->addWidget(page_2_Display);

        horizontalLayout_2->addWidget(stackedWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1102, 21));
        menuVisionBox = new QMenu(menubar);
        menuVisionBox->setObjectName("menuVisionBox");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuVisionBox->menuAction());
        menuVisionBox->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Veuillez choisir un mode d\342\200\231acquisition :", nullptr));
        labelPreview->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        labelTitreTraitements->setText(QCoreApplication::translate("MainWindow", "Mes traitements", nullptr));
        menuVisionBox->setTitle(QCoreApplication::translate("MainWindow", "VisionBox", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
