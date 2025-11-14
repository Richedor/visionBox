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

#include "traitement_flou.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Création du traitement flou réel
    m_traitementFlou = new TraitementFlou(this);


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

    // (Tu pourras plus tard connecter labelVideo ici si besoin)
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

    QPixmap pix(fileName);
    if (pix.isNull()) {
        QMessageBox::warning(this, tr("Erreur"),
                             tr("Impossible de charger l'image."));
        return;
    }

    // Passage à la page d'affichage
    ui->stackedWidget->setCurrentWidget(ui->page_2_Display);

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
    QPixmap pix = QPixmap::fromImage(img);

    if (pix.isNull()) {
        QMessageBox::warning(this, tr("Erreur"),
                             tr("Image webcam invalide."));
        return;
    }

    ui->stackedWidget->setCurrentWidget(ui->page_2_Display);

    afficherImageDansPreview(pix);

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


    // =========================
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

    m_panneauFlux = new PanneauFluxTraitements(zoneGraphe);
    layoutGraphe->addWidget(m_panneauFlux);   // <--- les boites s’afficheront ici

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
    dispositionPanneauDroite->addWidget(zoneLogs);

    // Aperçu : on réutilise ton labelPreview mais on EMPÊCHE qu’il élargisse toute la colonne
    QLabel *apercu = ui->labelPreview;
    apercu->setParent(panneauDroite);
    apercu->setMinimumHeight(140);
    apercu->setMaximumHeight(260);                 // <-- pour éviter la grosse saucisse verticale
    apercu->setAlignment(Qt::AlignCenter);
    apercu->setSizePolicy(QSizePolicy::Expanding,  // <-- il peut rétrécir en largeur
                          QSizePolicy::Expanding);
    dispositionPanneauDroite->addWidget(apercu);

    QPushButton *boutonExporter = new QPushButton("Exporter le résultat", panneauDroite);
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
    m_listeBarreTraitements.append(entree);

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
    // Si le panneau n'existe pas encore, on ne fait rien
    if (!m_panneauFlux)
        return;

    // Construire la liste de traitements dans l'ordre de la barre
    QList<TraitementImage*> listeTraitements;

    for (int i = 0; i < m_listeBarreTraitements.size(); ++i) {
        const EntreeBarreTraitement &entree = m_listeBarreTraitements[i];

        // On mappe le nom affiché dans la barre vers l'objet traitement
        if (entree.nom == "Mosaïque" && m_traitementMosaique) {
            listeTraitements << m_traitementMosaique;
        } else if (entree.nom == "Flou" && m_traitementFlou) {
            listeTraitements << m_traitementFlou;
        } else if (entree.nom == "Contours" && m_traitementContours) {
            listeTraitements << m_traitementContours;
        }
        // Ici tu pourras rajouter d'autres :
        // else if (entree.nom == "Niveaux de gris" && m_traitementGris) { ... }
    }

    // Image source pour générer les aperçus des boîtes
    QImage imageSource;
    if (!m_lastPreview.isNull()) {
        imageSource = m_lastPreview.toImage();
    }

    // On demande au panneau de se reconstruire (boîtes + flèches)
    m_panneauFlux->definirOrdreTraitements(listeTraitements, imageSource);

    // Après avoir mis à jour les boites, on recalcule aussi l'image finale
    mettreAJourImageApresPipeline();

}



QImage MainWindow::executerPipeline(const QImage &imageSource)
{
    if (imageSource.isNull())
        return QImage();

    QImage image = imageSource;

    // On applique chaque traitement dans l'ordre de la barre
    for (const EntreeBarreTraitement &entree : std::as_const(m_listeBarreTraitements)) {
        TraitementImage *t = nullptr;

        if (entree.nom == "Mosaïque") {
            t = m_traitementMosaique;
        } else if (entree.nom == "Flou") {
            t = m_traitementFlou;
        } else if (entree.nom == "Contours") {
            t = m_traitementContours;
        }

        if (!t)
            continue;

        // 👉 ici on utilise TOUJOURS l'image précédente comme entrée
        // Pour l'instant on ne passe pas encore de paramètres (QVariantMap vide)
        image = t->appliquer(image, QVariantMap());
    }

    return image;
}





void MainWindow::mettreAJourImageApresPipeline()
{
    if (m_lastPreview.isNull()) {
        return; // pas encore d'image source
    }

    // Image d'entrée = dernière image chargée / capturée
    QImage source = m_lastPreview.toImage();

    // On lance le pipeline en chaîne
    QImage resultat = executerPipeline(source);

    if (!resultat.isNull()) {
        afficherImageDansPreview(QPixmap::fromImage(resultat));
    } else {
        // Si le pipeline renvoie vide (pas de traitements), on affiche l'image brute
        afficherImageDansPreview(m_lastPreview);
    }
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



        /* ====== BOUTON EXPORTER ====== */
        QPushButton#boutonExporter {
            background-color: #88421D;
            color: white;
            border-radius: 12px;       /* AVANT 18 */
            padding: 8px 12px;
            border: none;
            font-size: 13px;
            font-weight: 600;
        }

        QPushButton#boutonExporter:hover {
            background-color: #A85B2E;
        }
        QPushButton#boutonExporter:pressed {
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

