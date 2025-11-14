#pragma once

#include <QMainWindow>
#include <QImage>
#include <qboxlayout.h>

#include "panneau_flux_traitements.h"
#include "traitement_image.h"
#include <QStringList>
#include <QList>


// déclarations avancées de futurs traitements
class TraitementFlou;
class TraitementMosaique;
class TraitementContours;


class QHBoxLayout;    // déclaration avancée
class QResizeEvent;   // pour la méthode resizeEvent

// représente un traitement dans la barre du bas ---
struct EntreeBarreTraitement
{
    QString nom;           // Exemple : "Mosaïque", "Flou"
    QWidget *widgetBarre;  // Le petit conteneur visuel (chip) dans la barre
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
    void onImageClicked();                  // clic sur l'icône "Image"
    void onWebcamClicked();                 // clic sur l'icône "Webcam"
    void onWebcamImageReady(const QImage &img); // image reçue depuis le dialog webcam

private:
    Ui::MainWindow *ui;

    QPixmap m_lastPreview;              // <--- on mémorise la dernière image
    QHBoxLayout *m_layoutBarreTraitement = nullptr;

    // panneau avec boites + flèches
    PanneauFluxTraitements *m_panneauFlux = nullptr;

    // instances de traitements (pointeurs génériques vers TraitementImage)
    TraitementImage *m_traitementMosaique = nullptr;
    TraitementImage *m_traitementFlou     = nullptr;
    TraitementImage *m_traitementContours = nullptr;

    // modèle interne de la barre de traitements (ordre + widget associé)
    QList<EntreeBarreTraitement> m_listeBarreTraitements;

    // ordre anciens des traitements
    QStringList m_ordreTraitements;


    void initialiserPageAffichage();
    void appliquerStyleVisionBox();
    void afficherImageDansPreview(const QPixmap &pix);
    void ajouterTraitementDansBarre(const QString &nom);


    void mettreAJourFluxDepuisBarre();   // <--- fonction qui synchronise boites <-> barre
    void mettreAJourImageApresPipeline();      // recalcule l'image finale
    QImage executerPipeline(const QImage &imageSource); // applique tous les traitements en chaîne


protected:
    void resizeEvent(QResizeEvent *event) override;   // <--- on gère le resize

};
