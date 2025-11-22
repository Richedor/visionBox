#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "clickablelabel.h"
#include "webcamdialog.h"

#include <QFileDialog>
#include <QPixmap>
#include <QMessageBox>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QStringList>
#include <QSpacerItem>

#include <QResizeEvent>

#include <QScrollArea>

#include <opencv2/opencv.hpp>

#include "traitements/adaptateurs_qt/traitement_image.h"
#include "traitements/adaptateurs_qt/adaptateur_flou.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>
#include <QDateTime>

#include "traitements/traitement_factory.h"


// plus tard :
// #include "traitements/adaptateurs_qt/adaptateur_mosaique.h"
// #include "traitements/adaptateurs_qt/adaptateur_contours.h"


using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_typeAcquisition.clear();
    m_cheminSource.clear();


    initialiserPageAffichage();
    appliquerStyleVisionBox();

    // S'assurer qu'on démarre sur la page de sélection
    ui->stackedWidget->setCurrentWidget(ui->page_1_Select);

    // Connexion clic "Image"
    connect(ui->labelImage, &ClickableLabel::clicked,
            this, &MainWindow::onImageClicked);

    // Connexion clic "Webcam"
    connect(ui->labelWebcam, &ClickableLabel::clicked,
            this, &MainWindow::onWebcamClicked);
}


MainWindow::~MainWindow()
{
    delete ui;
}

// -----------------  CLIC SUR "IMAGE"  -----------------

void MainWindow::onImageClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Choisir une image"),
        QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp)")
        );

    if (fileName.isEmpty())
        return; // utilisateur a annulé

    // 1) Charger l'image avec OpenCV (en BGR)
    m_imageCourante = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);

    if (m_imageCourante.empty()) {
        QMessageBox::warning(this, tr("Erreur"),
                             tr("Impossible de charger l'image avec OpenCV."));
        return;
    }

    // 2) Convertir en QImage pour l'affichage
    QImage qimg = matToQImage(m_imageCourante);
    if (qimg.isNull()) {
        QMessageBox::warning(this, tr("Erreur"),
                             tr("Conversion OpenCV -> QImage échouée."));
        return;
    }

    //  Infos acquisition pour le log
    m_typeAcquisition = "image_fichier";
    m_cheminSource    = fileName;
    mettreAJourLogsTexte();


    // On mémorise l'image originale comme source du pipeline
    m_imageSource = qimg;

    // Passage à la page d'affichage
    ui->stackedWidget->setCurrentWidget(ui->page_2_Display);

    QPixmap pix = QPixmap::fromImage(qimg);
    afficherImageDansPreview(pix);
}



// -----------------  CLIC SUR "WEBCAM"  -----------------

void MainWindow::onWebcamClicked()
{
    WebcamDialog dlg(this);

    // quand une image est capturée, on appelle onWebcamImageReady
    connect(&dlg, &WebcamDialog::imageCaptured,
            this, &MainWindow::onWebcamImageReady);

    dlg.exec();  // fenêtre modale : on attend qu'elle se ferme
}

// -----------------  IMAGE REÇUE DE LA WEBCAM  -----------------

void MainWindow::onWebcamImageReady(const QImage &img)
{
    if (img.isNull()) {
        QMessageBox::warning(this, tr("Erreur"),
                             tr("Image webcam invalide."));
        return;
    }

    // 1) Stocker en Mat pour les traitements OpenCV
    m_imageCourante = qImageToMat(img);
    if (m_imageCourante.empty()) {
        QMessageBox::warning(this, tr("Erreur"),
                             tr("Conversion QImage -> OpenCV échouée."));
        return;
    }

    //  On mémorise l'image originale comme source du pipeline
    m_imageSource = img;

    // Infos acquisition pour le log
    m_typeAcquisition = "webcam";
    m_cheminSource.clear();
    mettreAJourLogsTexte();


    // 2) Affichage dans la page d'aperçu
    ui->stackedWidget->setCurrentWidget(ui->page_2_Display);

    QPixmap pix = QPixmap::fromImage(img);
    afficherImageDansPreview(pix);
}



// -----------------  CONVERSIONS OpenCV <-> Qt  -----------------

QImage MainWindow::matToQImage(const cv::Mat &mat)
{
    if (mat.empty())
        return QImage();

    switch (mat.type()) {
    case CV_8UC3: {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data,
                      rgb.cols,
                      rgb.rows,
                      rgb.step,
                      QImage::Format_RGB888).copy();
    }
    case CV_8UC4: {
        cv::Mat rgba;
        cv::cvtColor(mat, rgba, cv::COLOR_BGRA2RGBA);
        return QImage(rgba.data,
                      rgba.cols,
                      rgba.rows,
                      rgba.step,
                      QImage::Format_RGBA8888).copy();
    }
    case CV_8UC1: {
        return QImage(mat.data,
                      mat.cols,
                      mat.rows,
                      mat.step,
                      QImage::Format_Grayscale8).copy();
    }
    default:
        // type non géré
        return QImage();
    }
}

cv::Mat MainWindow::qImageToMat(const QImage &image)
{
    // On force en RGBA 8 bits
    QImage img = image.convertToFormat(QImage::Format_RGBA8888);

    cv::Mat mat(img.height(),
                img.width(),
                CV_8UC4,
                const_cast<uchar*>(img.bits()),
                img.bytesPerLine());

    cv::Mat matBGR;
    cv::cvtColor(mat, matBGR, cv::COLOR_RGBA2BGR);

    return matBGR.clone(); // clone => buffer indépendant
}




void MainWindow::initialiserPageAffichage()
{
    QWidget *page = ui->page_2_Display;

    // --- Nettoyage des anciens widgets de page_2_Display ---
    // On garde uniquement labelPreview, tout le reste on le supprime
    const auto enfants = page->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget *w : enfants) {
        if (w == ui->labelPreview)
            continue;               // on garde labelPreview pour l'aperçu
        w->deleteLater();           // supprime l’ancienne grosse frame, etc.
    }


    // Supprimer tout ancien layout sur la page 2
    if (page->layout()) {
        delete page->layout();
    }

    // ===== Layout principal horizontal (3 colonnes) =====
    auto *dispositionPrincipale = new QHBoxLayout(page);
    dispositionPrincipale->setContentsMargins(20, 20, 20, 20);
    dispositionPrincipale->setSpacing(20);

    // =========================
    // 1) PANNEAU GAUCHE
    // =========================
    QFrame *panneauGauche = new QFrame(page);
    panneauGauche->setObjectName("panneauGauche");
    auto *dispositionPanneauGauche = new QVBoxLayout(panneauGauche);
    dispositionPanneauGauche->setSpacing(10);

    QLabel *labelTitreTraitements = new QLabel("Mes traitements", panneauGauche);
    labelTitreTraitements->setObjectName("labelTitreTraitements");
    labelTitreTraitements->setAlignment(Qt::AlignHCenter);
    dispositionPanneauGauche->addWidget(labelTitreTraitements);

    QStringList traitements = {
        "Mosaïque",
        "Flou",
        "Sépia",
        "Détection de visage",
        "Gris",
        "Contours"
    };

    for (const QString &nom : traitements) {
        QPushButton *bouton = new QPushButton(nom, panneauGauche);
        bouton->setProperty("role", "traitement");
        bouton->setCursor(Qt::PointingHandCursor);
        bouton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        bouton->setFixedHeight(40);
        dispositionPanneauGauche->addWidget(bouton);

        // Quand on clique sur un traitement, on l’ajoute dans la barre en bas
        connect(bouton, &QPushButton::clicked, this, [this, nom]() {
            ajouterTraitementDansBarre(nom);
        });
    }


    dispositionPanneauGauche->addStretch();

    // on limite un peu la largeur de la colonne gauche
    panneauGauche->setMinimumWidth(230);
    panneauGauche->setMaximumWidth(280);

    dispositionPrincipale->addWidget(panneauGauche);


    // 2) PANNEAU CENTRAL  (doit être le plus grand)
    // =========================
    QFrame *panneauCentre = new QFrame(page);
    panneauCentre->setObjectName("panneauCentre");
    auto *dispositionPanneauCentre = new QVBoxLayout(panneauCentre);
    dispositionPanneauCentre->setSpacing(12);

    // Zone graph / cartes (grand rectangle au milieu)
    QFrame *zoneGraphe = new QFrame(panneauCentre);
    zoneGraphe->setObjectName("zoneGraphe");
    auto *layoutGraphe = new QVBoxLayout(zoneGraphe);
    layoutGraphe->setContentsMargins(0, 0, 0, 0);
    layoutGraphe->setSpacing(0);

    // --- ScrollArea pour les boîtes de traitements ---
    QScrollArea *scrollFlux = new QScrollArea(zoneGraphe);
    scrollFlux->setObjectName("scrollFluxTraitements");
    scrollFlux->setFrameShape(QFrame::NoFrame);
    // on laisse le widget décider de sa largeur -> permet le scroll horizontal
    scrollFlux->setWidgetResizable(false);
    scrollFlux->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollFlux->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Le vrai panneau qui contient les boîtes
    m_panneauFlux = new PanneauFluxTraitements();
    scrollFlux->setWidget(m_panneauFlux);

    // On met la scrollArea dans la zone graphe
    layoutGraphe->addWidget(scrollFlux);

    // La zone graphe prend la plus grande partie du panneau central
    dispositionPanneauCentre->addWidget(zoneGraphe, 4);




    // Label "Barre de traitement"
    QLabel *labelBarre = new QLabel("Barre de traitement", panneauCentre);
    labelBarre->setObjectName("labelBarreTraitement");
    labelBarre->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dispositionPanneauCentre->addWidget(labelBarre);

    // Barre de traitement
    // Barre de traitement avec scroll horizontal
    QScrollArea *scrollBarre = new QScrollArea(panneauCentre);
    scrollBarre->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollBarre->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollBarre->setWidgetResizable(true);
    scrollBarre->setFrameShape(QFrame::NoFrame);

    // Conteneur réel de la barre (stylé avec "barreTraitement")
    QFrame *barreTraitement = new QFrame;
    barreTraitement->setObjectName("barreTraitement");
    auto *layoutBarre = new QHBoxLayout(barreTraitement);
    layoutBarre->setContentsMargins(20, 8, 20, 8);
    layoutBarre->setSpacing(6);
    layoutBarre->addStretch();

    scrollBarre->setWidget(barreTraitement);
    dispositionPanneauCentre->addWidget(scrollBarre, 1);

    m_layoutBarreTraitement = layoutBarre;   // <--- on garde le pointeur



    dispositionPrincipale->addWidget(panneauCentre);


    // =========================
    // 3) PANNEAU DROITE
    // =========================
    QFrame *panneauDroite = new QFrame(page);
    panneauDroite->setObjectName("panneauDroite");
    auto *dispositionPanneauDroite = new QVBoxLayout(panneauDroite);
    dispositionPanneauDroite->setSpacing(8);

    QLabel *labelLogs = new QLabel("Logs", panneauDroite);
    labelLogs->setObjectName("labelLogs");
    dispositionPanneauDroite->addWidget(labelLogs);

    QTextEdit *zoneLogs = new QTextEdit(panneauDroite);
    zoneLogs->setObjectName("zoneLogs");
    zoneLogs->setReadOnly(true);

    //  couleur du texte + fond
    zoneLogs->setStyleSheet(
        "QTextEdit#zoneLogs {"
        "  background-color: #FFF7F9;"   // ton fond rose pâle actuel
        "  color: #5B2A0C;"              // marron (même ton que les boutons/menu)"
        "  border: none;"
        "}"
        );


    zoneLogs->setReadOnly(true);
    dispositionPanneauDroite->addWidget(zoneLogs);

    // ... après avoir ajouté le labelPreview (apercu)
    QLabel *apercu = ui->labelPreview;
    apercu->setParent(panneauDroite);
    apercu->setMinimumHeight(140);
    apercu->setMaximumHeight(260);
    apercu->setAlignment(Qt::AlignCenter);
    apercu->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dispositionPanneauDroite->addWidget(apercu);

    // Bouton pour exporter la session (image + log)
    QPushButton *boutonExporterSession =
        new QPushButton(tr("Exporter la session (image + log)"), panneauDroite);
    boutonExporterSession->setObjectName("boutonExporterSession");
    // même style que les autres boutons → on ne touche PAS au style ici
    boutonExporterSession->setCursor(Qt::PointingHandCursor);
    boutonExporterSession->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dispositionPanneauDroite->addWidget(boutonExporterSession);

    // Bouton existant : exporter uniquement l'image finale
    QPushButton *boutonExporter =
        new QPushButton(tr("Exporter l'image finale"), panneauDroite);
    boutonExporter->setObjectName("boutonExporter");
    boutonExporter->setCursor(Qt::PointingHandCursor);
    boutonExporter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dispositionPanneauDroite->addWidget(boutonExporter);




    // On limite aussi la largeur de la colonne droite pour laisser respirer le centre
    panneauDroite->setMinimumWidth(260);
    panneauDroite->setMaximumWidth(420);

    dispositionPrincipale->addWidget(panneauDroite);


    // ===== Répartition des LARGEURS des 3 colonnes =====
    // largeurs relatives : gauche 2, centre 4, droite 3
    dispositionPrincipale->setStretch(0, 2);
    dispositionPrincipale->setStretch(1, 4);
    dispositionPrincipale->setStretch(2, 3);

    // ===== Répartition VERTICALE du panneau droit =====
    // labelLogs (0) / zoneLogs (2) / preview (3) / bouton (0)
    dispositionPanneauDroite->setStretch(0, 0);
    dispositionPanneauDroite->setStretch(1, 2);
    dispositionPanneauDroite->setStretch(2, 3);
    dispositionPanneauDroite->setStretch(3, 0);

    if (auto *btnImg = panneauDroite->findChild<QPushButton*>("boutonExporter")) {
        connect(btnImg, &QPushButton::clicked,
                this, &MainWindow::exporterImageFinale);
    }

    if (auto *btnSession = panneauDroite->findChild<QPushButton*>("boutonExporterSession")) {
        connect(btnSession, &QPushButton::clicked,
                this, &MainWindow::exporterSessionComplete);
    }

    QPushButton *btnImporter = new QPushButton(tr("Importer une session"), panneauDroite);
    btnImporter->setObjectName("boutonImporterSession");
    btnImporter->setCursor(Qt::PointingHandCursor);
    btnImporter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dispositionPanneauDroite->addWidget(btnImporter);

    connect(btnImporter, &QPushButton::clicked, this, [this] {
        QString fichier = QFileDialog::getOpenFileName(
            this,
            tr("Ouvrir un log de session"),
            QString(),
            tr("Fichiers JSON (*.json)")
            );
        if (!fichier.isEmpty()) {
            this->importerSessionDepuisJSON(fichier);
        }
    });


}


void MainWindow::afficherImageDansPreview(const QPixmap &pix)
{
    if (pix.isNull() || !ui->labelPreview)
        return;

    m_lastPreview = pix;   // <--- on garde une copie

    // Taille intérieure du label
    const QSize cible = ui->labelPreview->contentsRect().size();
    if (cible.width() <= 0 || cible.height() <= 0) {
        ui->labelPreview->setPixmap(pix);
        ui->labelPreview->setScaledContents(false);
        ui->labelPreview->setAlignment(Qt::AlignCenter);
        return;
    }

    // On remplit toute la zone → crop s’il faut
    QPixmap cropped = pix.scaled(
        cible,
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
        );

    ui->labelPreview->setScaledContents(false);
    ui->labelPreview->setAlignment(Qt::AlignCenter);
    ui->labelPreview->setPixmap(cropped);
}



void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // Si on est sur la page 2 et qu'on a déjà une image, on la recalcule
    if (ui->stackedWidget->currentWidget() == ui->page_2_Display &&
        !m_lastPreview.isNull()) {
        afficherImageDansPreview(m_lastPreview);
    }
}



void MainWindow::ajouterTraitementDansBarre(const QString &nom)
{
    if (!m_layoutBarreTraitement)
        return;

    // --- Conteneur horizontal (pill + croix) ---
    QWidget *wrapper = new QWidget(ui->page_2_Display);
    wrapper->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *layoutWrapper = new QHBoxLayout(wrapper);
    layoutWrapper->setContentsMargins(0, 0, 0, 0);
    layoutWrapper->setSpacing(4);

    // --- Bouton "pill" avec le nom du traitement ---
    QPushButton *chip = new QPushButton(nom, wrapper);
    chip->setProperty("role", "chipTraitement");
    chip->setCursor(Qt::PointingHandCursor);
    chip->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    chip->setMinimumHeight(26);

    // --- Bouton croix pour supprimer ---
    QPushButton *btnClose = new QPushButton(wrapper);
    btnClose->setProperty("role", "chipClose");
    btnClose->setCursor(Qt::PointingHandCursor);
    btnClose->setFixedSize(18, 18);
    btnClose->setIcon(QIcon(":/images/croix.png"));
    btnClose->setIconSize(QSize(10, 10));

    layoutWrapper->addWidget(chip);
    layoutWrapper->addWidget(btnClose);

    // --- Insère avant le stretch final ---
    int indexInsert = m_layoutBarreTraitement->count();
    if (indexInsert > 0)
        indexInsert -= 1;

    m_layoutBarreTraitement->insertWidget(indexInsert, wrapper);

    // ----------------------------
    // AJOUT PROPRE : mise à jour du modèle interne
    // ----------------------------
    EntreeBarreTraitement entree;
    entree.nom = nom;
    entree.widgetBarre = wrapper;

    // On crée une nouvelle instance de traitement pour CETTE entrée
    TraitementImage *traitement = nullptr;

    if (nom == "Flou") {
        traitement = new AdaptateurFlou(this);
    }
    // plus tard :
    // else if (nom == "Mosaïque") {
    //     traitement = new AdaptateurMosaique(this);
    // } else if (nom == "Contours") {
    //     traitement = new AdaptateurContours(this);
    // }

    entree.traitement = traitement;
    m_listeBarreTraitements.append(entree);

    // Si ce traitement a des paramètres qui changent,
    // on recalcule automatiquement l'image finale
    if (traitement) {
        connect(traitement, &TraitementImage::parametresModifies,
                this, &MainWindow::mettreAJourImageApresPipeline);
    }

    // ----------------------------
    // Suppression propre
    // ----------------------------
    connect(btnClose, &QPushButton::clicked, this, [this, wrapper]() {

        // Retirer du modèle interne
        for (int i = 0; i < m_listeBarreTraitements.size(); ++i) {
            if (m_listeBarreTraitements[i].widgetBarre == wrapper) {
                m_listeBarreTraitements.removeAt(i);
                break;
            }
        }

        // Retirer du layout
        if (m_layoutBarreTraitement)
            m_layoutBarreTraitement->removeWidget(wrapper);

        wrapper->deleteLater();

        // Mettre à jour les boîtes
        mettreAJourFluxDepuisBarre();
    });

    // ----------------------------
    // Mise à jour du panneau de boites
    // ----------------------------
    mettreAJourFluxDepuisBarre();
}



void MainWindow::mettreAJourFluxDepuisBarre()
{
    if (!m_panneauFlux)
        return;

    QList<TraitementImage*> listeTraitements;

    for (const EntreeBarreTraitement &entree : std::as_const(m_listeBarreTraitements)) {
        if (entree.traitement) {
            listeTraitements << entree.traitement;
        }
    }

    // Image source pour les aperçus
    QImage imageSource = m_imageSource;
    if (imageSource.isNull() && !m_lastPreview.isNull()) {
        imageSource = m_lastPreview.toImage();
    }

    m_panneauFlux->definirOrdreTraitements(listeTraitements, imageSource);
    mettreAJourImageApresPipeline();
    mettreAJourLogsTexte();
}



QImage MainWindow::executerPipeline(const QImage &imageSource)
{
    if (imageSource.isNull())
        return QImage();

    QImage image = imageSource;

    // On applique chaque traitement dans l'ordre de la barre
    for (const EntreeBarreTraitement &entree : std::as_const(m_listeBarreTraitements)) {

        TraitementImage *t = entree.traitement;  // 🔹 instance propre à CETTE entrée

        if (!t)
            continue;

        // On enchaîne l'image au travers des traitements
        image = t->appliquer(image, QVariantMap());
    }

    return image;
}






void MainWindow::mettreAJourImageApresPipeline()
{
    // On doit avoir une vraie image source (originale)
    if (m_imageSource.isNull()) {
        return;
    }

    // Image d'entrée = image originale (sans traitements)
    QImage source = m_imageSource;

    // On lance le pipeline en chaîne
    QImage resultat = executerPipeline(source);

    if (!resultat.isNull()) {
        afficherImageDansPreview(QPixmap::fromImage(resultat));
    } else {
        // Si le pipeline ne renvoie rien (aucun traitement actif),
        // on réaffiche l'image originale
        afficherImageDansPreview(QPixmap::fromImage(m_imageSource));
    }
}


void MainWindow::mettreAJourLogsTexte()
{
    QTextEdit *zoneLogs = findChild<QTextEdit*>("zoneLogs");
    if (!zoneLogs)
        return;

    QStringList lignes;

    // Infos acquisition
    lignes << "Acquisition :";
    if (m_typeAcquisition.isEmpty()) {
        lignes << "  (aucune image chargée)";
    } else {
        lignes << "  Type   : " + m_typeAcquisition;
        if (!m_cheminSource.isEmpty())
            lignes << "  Source : " + m_cheminSource;
    }

    lignes << "";
    lignes << "Pipeline de traitements :";

    if (m_listeBarreTraitements.isEmpty()) {
        lignes << "  (aucun traitement)";
    } else {
        int index = 1;
        for (const EntreeBarreTraitement &entree : std::as_const(m_listeBarreTraitements)) {
            lignes << QString("  %1. %2").arg(index++).arg(entree.nom);
        }
    }

    zoneLogs->setPlainText(lignes.join("\n"));
}


void MainWindow::exporterImageFinale()
{
    if (m_lastPreview.isNull()) {
        QMessageBox::warning(this, tr("Export image"),
                             tr("Aucune image finale à exporter."));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Exporter l'image finale"),
        "resultat.png",
        tr("Images (*.png *.jpg *.jpeg *.bmp)")
        );

    if (fileName.isEmpty())
        return;

    QImage img = m_lastPreview.toImage();
    if (!img.save(fileName)) {
        QMessageBox::warning(this, tr("Export image"),
                             tr("Impossible d'enregistrer l'image."));
    }
}


void MainWindow::exporterSessionComplete()
{
    if (m_lastPreview.isNull() && m_imageSource.isNull()) {
        QMessageBox::warning(this, tr("Export session"),
                             tr("Aucune image à exporter."));
        return;
    }

    QString baseDir = QFileDialog::getExistingDirectory(
        this,
        tr("Choisir un dossier pour l'export")
        );

    if (baseDir.isEmpty())
        return;

    QDateTime now = QDateTime::currentDateTime();
    QString folderName = now.toString("yyyyMMdd_HHmmss");
    QDir dir(baseDir);
    if (!dir.mkdir(folderName)) {
        QMessageBox::warning(this, tr("Export session"),
                             tr("Impossible de créer le dossier d'export."));
        return;
    }
    dir.cd(folderName);

    // 1) Sauver l'image finale
    QImage imgFinale = m_lastPreview.isNull()
                           ? m_imageSource
                           : m_lastPreview.toImage();

    QString imageFilePath = dir.filePath("resultat.png");
    imgFinale.save(imageFilePath);

    // 2) Construire le JSON minimal avec acquisition + pipeline
    QJsonObject root;
    root["version"] = "1.0";
    root["date"] = now.toString(Qt::ISODate);

    QJsonObject acq;
    acq["type"] = m_typeAcquisition;
    acq["source"] = m_cheminSource;
    root["acquisition"] = acq;

    QJsonObject res;
    res["image_finale"] = imageFilePath;
    root["resultat"] = res;

    QJsonArray pipelineArray;
    for (const EntreeBarreTraitement &entree : std::as_const(m_listeBarreTraitements)) {
        TraitementImage *t = entree.traitement;
        if (!t)
            continue;

        QJsonObject tjson;
        tjson["id"]  = t->idTraitement();  // ex: "flou_gaussien"
        tjson["nom"] = entree.nom;         // ex: "Flou"

        QVariantMap params = t->lireParametresCourants();
        QJsonObject paramsJson;
        for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
            paramsJson[it.key()] = QJsonValue::fromVariant(it.value());
        }
        tjson["params"] = paramsJson;

        pipelineArray.append(tjson);
    }
    root["pipeline"] = pipelineArray;

    QJsonDocument doc(root);
    QString jsonPath = dir.filePath("log.json");
    QFile f(jsonPath);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(doc.toJson(QJsonDocument::Indented));
        f.close();
    }

    QMessageBox::information(
        this,
        tr("Export session"),
        tr("Session exportée dans :\n%1").arg(dir.absolutePath())
        );
}


void MainWindow::importerSessionDepuisJSON(const QString &fichier)
{
    QFile f(fichier);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Import session"),
                             tr("Impossible d'ouvrir le fichier : %1").arg(fichier));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    QJsonObject root = doc.object();

    // On lit le pipeline
    QJsonArray pipelineArray = root["pipeline"].toArray();

    // 1) Vider la barre de traitement actuelle (UI + modèle)
    for (const EntreeBarreTraitement &entree : std::as_const(m_listeBarreTraitements)) {
        if (entree.widgetBarre)
            entree.widgetBarre->deleteLater();
    }
    m_listeBarreTraitements.clear();
    m_ordreTraitements.clear();

    // 2) Reconstruire la barre et appliquer les paramètres
    for (const QJsonValue &v : pipelineArray) {
        QJsonObject tjson = v.toObject();
        QString nom = tjson["nom"].toString();  // ex: "Flou"

        // on ajoute une entrée comme si l'utilisateur avait cliqué sur l'icône
        ajouterTraitementDansBarre(nom);

        if (m_listeBarreTraitements.isEmpty())
            continue;

        // on récupère le dernier traitement ajouté
        EntreeBarreTraitement &entree = m_listeBarreTraitements.last();
        TraitementImage *t = entree.traitement;
        if (!t)
            continue;

        // on applique les paramètres stockés dans le JSON
        QVariantMap params;
        QJsonObject paramsJson = tjson["params"].toObject();
        for (auto it = paramsJson.begin(); it != paramsJson.end(); ++it) {
            params[it.key()] = it.value().toVariant();
        }

        t->appliquerParametresDepuis(params);
    }

    // 3) On synchronise tout et on recalcule l'image
    mettreAJourFluxDepuisBarre();
    mettreAJourLogsTexte();
}


void MainWindow::appliquerStyleVisionBox()
{
    if (!ui->page_2_Display)
        return;

    ui->page_2_Display->setStyleSheet(R"(

        /************** PALETTE **************/
        /* Principal : #88421D (brun foncé)
           Accent :    #F9C863 (jaune doux)
           Fond clair :#FFF6D8
           Fond panel :#FFFCF5
        ****************************************/

        /* ====== FOND PAGE ====== */
        QWidget#page_2_Display {
            background-color: #FFF6D8;
        }

        /* ====== PANNEAUX ====== */
        QFrame#panneauGauche,
        QFrame#panneauCentre,
        QFrame#panneauDroite {
            background-color: #FFFCF5;
            border: 1px solid #88421D;
            border-radius: 10px;     /* AVANT = 20–22, maintenant = 10 */
            padding: 10px;
        }

        /* ====== ZONE GRAPHE ====== */
        QFrame#zoneGraphe {
            background-color: #FFF6D8;
            border-radius: 8px;      /* AVANT 16 */
            border: 1px solid #F9C863;
        }

        /* ====== BARRE DE TRAITEMENT ====== */
        QFrame#barreTraitement {
            background-color: #FFF6D8;
            border-radius: 8px;
            border: 1px solid #F9C863;
        }


        /* ====== TITRES ====== */
        QLabel#labelTitreTraitements,
        QLabel#labelLogs,
        QLabel#labelBarreTraitement {
            color: #88421D;
            font-size: 14px;
            font-weight: 600;
            font-family: "Segoe UI", "Arial", sans-serif;
        }

        QLabel#labelTitreTraitements {
            qproperty-alignment: AlignHCenter;
            margin-bottom: 6px;
        }

        /* ====== PILLS TRAITEMENTS ====== */
        QPushButton[role="traitement"] {
            background-color: #88421D;
            color: #FFFFFF;
            border-radius: 12px;      /* AVANT 18 → réduit */
            padding: 4px 8px;
            margin: 4px 0;
            font-size: 13px;
            font-weight: 500;
            border: none;
            text-align: left;
        }

        QPushButton[role="traitement"]:hover {
            background-color: #A85B2E;
        }

        QPushButton[role="traitement"]:pressed {
            background-color: #5F2D13;
        }

        /* ====== ZONE DE LOGS ====== */
        QTextEdit#zoneLogs {
            background-color: #FFFCF5;
            border-radius: 8px;       /* AVANT 12 */
            border: 1px solid #F9C863;
            font-size: 11px;
            font-family: Consolas, "Courier New", monospace;
        }

        /* ====== APERCU IMAGE ====== */
        QLabel#labelPreview {
            background-color: #FFFCF5;
            border-radius: 8px;
            border: 1px solid #F9C863;
            padding: 0px;   /* petite marge entre image et bord */
        }


        /* ====== BOUTONS D'EXPORT / IMPORT ====== */
        QPushButton#boutonExporter,
        QPushButton#boutonExporterSession,
        QPushButton#boutonImporterSession {
            background-color: #88421D;
            color: white;
            border-radius: 12px;
            padding: 8px 12px;
            border: none;
            font-size: 13px;
            font-weight: 600;
        }

        QPushButton#boutonExporter:hover,
        QPushButton#boutonExporterSession:hover,
        QPushButton#boutonImporterSession:hover {
            background-color: #A85B2E;
        }

        QPushButton#boutonExporter:pressed,
        QPushButton#boutonExporterSession:pressed,
        QPushButton#boutonImporterSession:pressed {
            background-color: #5F2D13;
        }



        /* ====== CHIPS DANS LA BARRE DE TRAITEMENT ====== */
        QPushButton[role="chipTraitement"] {
            background-color: #FFFCF5;
            color: #88421D;
            border-radius: 12px;
            border: 1px solid #88421D;
            padding: 4px 12px;
            margin-right: 4px;
            font-size: 12px;
            font-weight: 500;
        }

        QPushButton[role="chipTraitement"]:hover {
            background-color: #F9C863;
        }

        /* Petit bouton croix rond */
        QPushButton[role="chipClose"] {
            background-color: #88421D;
            border: none;
            border-radius: 9px;
            padding: 0;
        }

        QPushButton[role="chipClose"]:hover {
            background-color: #A85B2E;
        }

        QPushButton[role="chipClose"]:pressed {
            background-color: #5F2D13;
        }





        /* ====== SCROLLBAR — BARRE DE TRAITEMENT ====== */

        /* ScrollArea transparente */
        QWidget#page_2_Display QScrollArea {
            background: transparent;
            border: none;
        }

        /* Barre horizontale (container général) */
        QWidget#page_2_Display QScrollBar:horizontal {
            background: transparent;
            height: 17px;                       /* ← Épaisseur */
            margin: 6px 24px 6px 24px;          /* espace autour */
        }

        /* Track (zone de fond derrière le handle) */
        QWidget#page_2_Display QScrollBar::add-page:horizontal,
        QWidget#page_2_Display QScrollBar::sub-page:horizontal {
            background: #F9C863;
            border-radius: 7px;
        }

        /* Handle */
        QWidget#page_2_Display QScrollBar::handle:horizontal {
            background: #88421D;
            border-radius: 7px;
            min-width: 40px;
        }

        QWidget#page_2_Display QScrollBar::handle:horizontal:hover {
            background: #A85B2E; /* hover */
        }

        /* ===== Boutons Début / Fin ===== */

        /* Bouton gauche */
        QWidget#page_2_Display QScrollBar::sub-line:horizontal {
            background: #88421D;
            width: 24px;
            border-top-left-radius: 7px;
            border-bottom-left-radius: 7px;
            subcontrol-origin: margin;
            subcontrol-position: left;
        }

        /* Bouton droit */
        QWidget#page_2_Display QScrollBar::add-line:horizontal {
            background: #88421D;
            width: 24px;
            border-top-right-radius: 7px;
            border-bottom-right-radius: 7px;
            subcontrol-origin: margin;
            subcontrol-position: right;
        }

        /* Icône flèche gauche (Qt native) */
        QWidget#page_2_Display QScrollBar::sub-line:horizontal:enabled {
            image: url(qrc:/qt-project.org/styles/commonstyle/images/leftarrow.png);
            /* icône embarquée par Qt */
        }

        /* Icône flèche droite (Qt native) */
        QWidget#page_2_Display QScrollBar::add-line:horizontal:enabled {
            image: url(qrc:/qt-project.org/styles/commonstyle/images/rightarrow.png);
        }

        /* Désactive la zone clic vide des boutons */
        QWidget#page_2_Display QScrollBar::sub-line:horizontal:disabled,
        QWidget#page_2_Display QScrollBar::add-line:horizontal:disabled {
            background: #C9A67A;
            image: none;
        }

        /* Ça évite l'affichage d’une ligne grise */
        QWidget#page_2_Display QScrollBar::up-arrow:horizontal,
        QWidget#page_2_Display QScrollBar::down-arrow:horizontal {
            background: transparent;
        }



        /* Fenêtre de paramètres des traitements */
        QDialog {
            background-color: #ffeef4;
        }

        /* Labels de la fenêtre de paramètres (flou, etc.) */
        QDialog QLabel[role="titreParametresTraitement"],
        QDialog QLabel[role="nomParametreTraitement"],
        QDialog QLabel[role="valeurParametreTraitement"] {
            color: #333333;
        }



    )");
}

