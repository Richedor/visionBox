#pragma once

#include <QMainWindow>
#include <QImage>
#include <qboxlayout.h>

#include "panneau_flux_traitements.h"
#include "traitements/adaptateurs_qt/traitement_image.h"
#include <QStringList>
#include <QList>

#include <opencv2/opencv.hpp>

// déclarations avancées de futurs traitements
class TraitementFlou;
class TraitementMosaique;
class TraitementContours;

class QHBoxLayout;    // déclaration avancée
class QResizeEvent;   // pour la méthode resizeEvent

// représente un traitement dans la barre du bas ---
struct EntreeBarreTraitement
{
    QString nom;                 // Exemple : "Mosaïque", "Flou"
    QWidget *widgetBarre;        // Le petit conteneur visuel (chip) dans la barre
    TraitementImage *traitement; // instance propre à cette entrée
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onImageClicked();                       // clic sur l'icône "Image" (charger depuis le PC)
    void onWebcamClicked();                      // clic sur l'icône "Webcam"
    void onWebcamImageReady(const QImage &img);  // image reçue depuis le dialog webcam

    void exporterImageFinale();                  // export de l'image finale uniquement
    void exporterSessionComplete();              // export image + log JSON

    // appelé quand un traitement signale que ses paramètres ont changé
    void mettreAJourImageApresPipeline();        // recalcule et affiche l'image finale

private:
    Ui::MainWindow *ui;

    // Image courante en OpenCV (base de tous les traitements)
    cv::Mat m_imageCourante;

    // Image source originale (telle que chargée / capturée, sans traitements)
    QImage m_imageSource;

    //  infos sur l'acquisition pour le log
    QString m_typeAcquisition;   // "image_fichier", "webcam", etc.
    QString m_cheminSource;      // chemin de l'image si chargée depuis le disque

    // Dernier aperçu affiché (côté interface Qt)
    QPixmap m_lastPreview;

    QHBoxLayout *m_layoutBarreTraitement = nullptr;

    // panneau avec boites + flèches
    PanneauFluxTraitements *m_panneauFlux = nullptr;

    // modèle interne de la barre de traitements (ordre + widget associé)
    QList<EntreeBarreTraitement> m_listeBarreTraitements;

    // ordre ancien des traitements
    QStringList m_ordreTraitements;

    // --- Initialisation UI / style ---
    void initialiserPageAffichage();
    void appliquerStyleVisionBox();

    // --- Gestion de l'affichage ---
    void afficherImageDansPreview(const QPixmap &pix);
    void ajouterTraitementDansBarre(const QString &nom);

    // --- Gestion du pipeline de traitements ---
    void mettreAJourFluxDepuisBarre();                  // synchronise boites <-> barre
    QImage executerPipeline(const QImage &imageSource); // applique tous les traitements en chaîne

    void importerSessionDepuisJSON(const QString &fichier);
    // mise à jour de la zone de logs texte (panneau log)
    void mettreAJourLogsTexte();

    // --- Utilitaires de conversion OpenCV <-> Qt ---
    QImage matToQImage(const cv::Mat &mat);
    cv::Mat qImageToMat(const QImage &image);

protected:
    void resizeEvent(QResizeEvent *event) override;   // on gère le resize
};
