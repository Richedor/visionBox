## 1. Objectif du projet

Développer une application C++ avec interface graphique permettant :

* d’acquérir une image :

  * depuis un fichier (png / jpg / bmp / tiff),
  * depuis une webcam,
  * ou depuis une vidéo (optionnel mais présent dans le code),
* de construire une **chaine de traitements d’image** (pipeline) en enchaînant plusieurs opérations OpenCV,
* d’afficher le résultat final et de **sauvegarder l’image**,
* d’afficher le pipeline sous forme de **boîtes reliées par des flèches**, chacune cliquable pour ouvrir une fenêtre de paramétrage,
* de **sauvegarder / recharger** une session complète (acquisition + pipeline + paramètres) dans un fichier JSON.

Le projet est découpé en deux grandes parties :

1. **Cœur de traitement & pipeline** (groupe “traitements”)
2. **Interface graphique & interactions utilisateur** (groupe “UI / Qt”)

---

## 2. Choix techniques

* **Langage** : C++ (norme compatible Qt 6, MSVC / CMake).
* **Interface graphique** : Qt 6 (QMainWindow, QWidget, QDialog, layouts…).
* **Traitement d’image** : OpenCV (cv::Mat, VideoCapture…).
* **OS cible** : Windows 10/11 (mais code largement portable).
* **Build system** : CMake généré par Qt Creator.
* **Format de sauvegarde des sessions** : JSON (QJsonDocument, QJsonObject, QJsonArray).

---

## 3. Architecture globale

### 3.1 Vue d’ensemble (modules)

* **Module Application / Contrôleur**

  * `MainWindow` (fichiers `mainwindow.h/.cpp/.ui`)
  * `main.cpp`
* **Module Affichage du pipeline**

  * `PanneauFluxTraitements` (`panneau_flux_traitements.h/.cpp`)
  * `BoiteTraitement` (`boite_traitement.h/.cpp`)
* **Module Acquisition**

  * `WebcamDialog` (`webcamdialog.h/.cpp`)
  * Gestion vidéo dans `MainWindow` (m_videoCapture, QTimer…)
* **Module Traitements (cœur OpenCV)**

  * Dossier `traitements/core/` :
    `flou.*`, `mosaique.*`, `sepia.*`, `invertcolor.*`, `miroir.*`, `contours.*`
* **Module Adaptateurs Qt des traitements**

  * Dossier `traitements/adaptateurs_qt/` :
    `traitement_image.*` (interface),
    `adaptateur_flou.*`, `adaptateur_mosaique.*`,
    `adaptateur_sepia.*`, `adaptateur_invertcolor.*`,
    `adaptateur_miroir.*`, `adaptateur_contours.*`
  * `traitements/traitement_factory.h`
* **Utilitaires UI**

  * `ClickableLabel` (`clickablelabel.h`)
  * `miroir.h` (utile pour quelques helpers)

---

## 4. Détail des classes et responsabilités

### 4.1 Application principale : `MainWindow`

**Fichiers** : `mainwindow.h`, `mainwindow.cpp`, `mainwindow.ui`
**Rôle** : point central de l’application. Gère :

* l’acquisition (fichier, webcam, vidéo),
* la liste des traitements dans la barre du bas,
* le lancement du pipeline,
* la mise à jour des boîtes dans `PanneauFluxTraitements`,
* l’export image / JSON et l’import de sessions.

#### 4.1.1 Attributs principaux

* Interface Qt :

  * `Ui::MainWindow *ui;`
* Gestion acquisition :

  * `QString m_typeAcquisition;`

    * valeurs typiques : `"image_fichier"`, `"webcam"`, `"video_fichier"`.
  * `QString m_cheminSource;` (chemin de l’image ou de la vidéo).
  * `QImage m_imageSource;` (image originale).
  * `QPixmap m_lastPreview;` (dernier résultat affiché).
* Gestion pipeline (modèle logique de la barre de traitement) :

  * ```cpp
    struct EntreeBarreTraitement {
        QString nom;                 // "Mosaïque", "Flou", etc.
        QWidget *widgetBarre;        // le "chip" dans la barre
        TraitementImage *traitement; // instance dédiée
    };
    ```
  * `QList<EntreeBarreTraitement> m_listeBarreTraitements;`
  * `QHBoxLayout *m_layoutBarreTraitement;` (layout des “chips” en bas)
* Vidéo :

  * `cv::VideoCapture m_videoCapture;`
  * `QTimer m_videoTimer;`
  * `QString m_videoPath;`
  * `bool m_videoPaused = false;`
  * `bool m_videoLoop = true;`
  * `QPushButton *m_btnVideoPlayPause = nullptr;`
* Autres :

  * pointeur vers le panneau de boîtes : `PanneauFluxTraitements *m_panneauFluxTraitements;`

#### 4.1.2 Méthodes publiques / slots principaux

* **Construction / destruction**

  * `MainWindow(QWidget *parent = nullptr);`
  * `~MainWindow();`

* **Acquisition**

  * `void on_actionChargerImage_triggered();`
    (ou équivalent via lambda) → `chargerImage(const QString &path);`
  * `void onWebcamClicked();` → ouvre `WebcamDialog`.
  * `void onVideoClicked();` → ouvre un fichier vidéo.
  * `void onVideoFrameTimeout();` → lit une frame de `m_videoCapture`.
  * `void onVideoPlayPauseClicked();`
    (gestion de `m_videoPaused` + timer).

* **Gestion de la barre de traitements**

  * `void construireBarreTraitements();`
    (initialisation, récupération du layout dans l’UI).
  * `void ajouterTraitementDansBarre(const QString &nom);`

    * crée un `QWidget *wrapper` (chip) + boutons `<`, `x`, `>`,
    * instancie le `TraitementImage` associé (via `nom`),
    * ajoute une `EntreeBarreTraitement` à `m_listeBarreTraitements`,
    * déclenche `mettreAJourFluxDepuisBarre()`.
  * `void deplacerTraitementDansBarre(QWidget *wrapper, int delta);`

    * modifie l’ordre dans `m_listeBarreTraitements` et dans le layout,
    * puis `mettreAJourFluxDepuisBarre();`
  * suppression d’un traitement : via bouton “x” connecté à un lambda qui :

    * retire l’entrée de `m_listeBarreTraitements`,
    * supprime `wrapper`,
    * appelle `mettreAJourFluxDepuisBarre();`

* **Pipeline & affichage**

  * `QImage executerPipeline(const QImage &imageSource);`
    applique tous les `TraitementImage` dans l’ordre :

    ```cpp
    for (const EntreeBarreTraitement &entree : m_listeBarreTraitements) {
        TraitementImage *t = entree.traitement;
        if (!t) continue;
        image = t->appliquer(image, QVariantMap());
    }
    ```
  * `void mettreAJourFluxDepuisBarre();`
    construit la liste de `TraitementImage*` et appelle :

    ```cpp
    m_panneauFluxTraitements->definirOrdreTraitements(liste, m_imageSource);
    ```
  * `void mettreAJourImageApresPipeline();`

    * relance `executerPipeline(m_imageSource)`,
    * affiche le résultat dans `ui->labelPreview`,
    * met à jour `m_lastPreview`.

* **Logs & export**

  * `void mettreAJourLogsTexte();`

    * remplit un `QTextEdit` (nommé `"zoneLogs"`) avec :

      * infos sur l’acquisition,
      * liste des traitements,
      * paramètres courants.
  * `void exporterImageFinale();`

    * sauvegarde `m_lastPreview` en PNG / JPG.
  * `void exporterSessionComplete();`

    * crée un dossier d’export,
    * sauvegarde l’image finale,
    * écrit un JSON avec :

      * acquisition (`type`, `source`),
      * chemin de l’image finale,
      * `pipeline[]` de traitements :

        ```json
        {
          "id": "flou_gaussien",
          "nom": "Flou",
          "params": { ... }
        }
        ```
  * `void importerSessionDepuisJSON(const QString &fichier);`

    * lit le JSON,
    * reconstruit la barre de traitements,
    * recharge les paramètres via `appliquerParametresDepuis(...)` de chaque `TraitementImage`.

* **Divers**

  * `void afficherImageDansPreview(const QPixmap &pix);`
  * `void recentrerBoutonVideo();` (recentrage du bouton Play/Pause quand la fenêtre est redimensionnée).
  * `void resizeEvent(QResizeEvent *event) override;` (appelle `recentrerBoutonVideo()`).

---

### 4.2 Affichage du pipeline

#### 4.2.1 `PanneauFluxTraitements`

**Fichiers** : `panneau_flux_traitements.h/.cpp`
**Rôle** : afficher les boîtes de traitements et dessiner les flèches entre elles.

**Attributs** :

* `QList<TraitementImage*> m_traitements;`
* `QList<BoiteTraitement*> m_boites;`
* `QImage m_imageSource;`

**Méthodes principales** :

* `void definirOrdreTraitements(const QList<TraitementImage*> &liste, const QImage &imageSource);`

  * supprime les anciennes `BoiteTraitement`,
  * met à jour `m_traitements` et `m_imageSource`,
  * crée une `BoiteTraitement` par traitement, positionnée automatiquement (x, y),
  * connecte le signal `BoiteTraitement::demandeOuvertureParametres(BoiteTraitement*)` à `ouvrirFenetreParametres(...)`.

* `void paintEvent(QPaintEvent *event) override;`

  * dessine des flèches entre les boîtes (utilisation de `QPainter`, `QLineF`, `QPolygonF`).

* `QSize sizeHint() const override;`

  * propose une taille par défaut (utile si intégré dans une `QScrollArea`).

* `void ouvrirFenetreParametres(BoiteTraitement *boite);`

  * affiche un `QDialog` :

    * avec une zone d’aperçu (`QLabel`),
    * un bouton Play/Pause superposé pour la vidéo,
    * le widget de paramètres retourné par `TraitementImage::creerEditeurParametres(...)`,
  * connecte le signal `parametresModifies()` du traitement pour :

    * recalculer l’aperçu,
    * mettre à jour la miniature dans la `BoiteTraitement`.

#### 4.2.2 `BoiteTraitement`

**Fichiers** : `boite_traitement.h/.cpp`
**Rôle** : représenter un traitement (une “step”) dans le pipeline sous forme de boîte colorée.

**Attributs** :

* `TraitementImage *m_traitement;`
* `QLabel *m_etiquetteTitre;` (nom du traitement)
* `QLabel *m_etiquetteApercu;` (miniature de sortie)
* `QPushButton *m_boutonParametres;`
* Gestion du drag :

  * `bool m_enDeplacement = false;`
  * `QPoint m_positionSourisLocal;`

**Comportement** :

* **UI** :

  * fond coloré (`background-color:#88421D; border-radius:15px;`),
  * titre centré, mini image, bouton “Paramètres”.
* **Drag & Drop interne** :

  * `mousePressEvent()` → mémorise la position relative du clic,
  * `mouseMoveEvent()` → déplace la boîte dans les limites du parent,
  * `mouseReleaseEvent()` → arrête le déplacement.
* **Ouverture des paramètres** :

  * clic sur le bouton “Paramètres” -> émet un signal (ex. `demandeOuvertureParametres(BoiteTraitement*)`) vers `PanneauFluxTraitements`.

---

### 4.3 Gestion de l’acquisition : `WebcamDialog`

**Fichiers** : `webcamdialog.h/.cpp`
**Rôle** : dialog modal pour capturer une image fixe depuis une webcam.

**Attributs** :

* `cv::VideoCapture m_cap;`
* `cv::Mat m_lastFrame;`
* `QTimer m_timer;`
* `QLabel *m_view;` (affichage de l’image)
* `QPushButton *m_btnCapture;`
* `QVBoxLayout *m_layout;`

**Méthodes** :

* `explicit WebcamDialog(QWidget *parent = nullptr);`
* `~WebcamDialog();`
* `void demarrerCapture();` (ouvre la webcam, démarre le timer)
* `void arreterCapture();`
* `void onTimeoutFrame();` (lit une frame, met à jour `m_view`)
* `QImage matToQImage(const cv::Mat &mat);`
  (conversion OpenCV → QImage)
* la valeur retournée par `exec()` et a récupération de la dernière image permettent à `MainWindow` de mettre à jour `m_imageSource`.

---

### 4.4 Interface de traitement générique : `TraitementImage`

**Fichiers** : `traitements/adaptateurs_qt/traitement_image.h`
**Rôle** : interface pure, indépendante du type de traitement, utilisée par `MainWindow` et `PanneauFluxTraitements`.

```cpp
class TraitementImage : public QObject
{
    Q_OBJECT
public:
    virtual QString nom() const = 0;
    virtual QImage appliquer(const QImage &entree,
                             const QVariantMap &parametres) = 0;
    virtual QImage genererApercu(const QImage &entree,
                                 const QVariantMap &parametres) = 0;
    virtual QWidget* creerEditeurParametres(QWidget *parent = nullptr) = 0;

    virtual QString idTraitement() const = 0;               // "flou_gaussien", "mosaique", ...
    virtual QVariantMap lireParametresCourants() const = 0; // pour log / export
    virtual void appliquerParametresDepuis(const QVariantMap &p) = 0;

signals:
    void parametresModifies();
};
```

---

### 4.5 Adaptateurs Qt des traitements

Pour chaque traitement, on a :

* une **implémentation core** en OpenCV (`traitements/core/*.h/.cpp`),
* un **adaptateur Qt** dérivant de `TraitementImage` (`traitements/adaptateurs_qt/*.h/.cpp`).

Exemple pour le flou :

#### 4.5.1 Cœur flou (`traitements/core/flou.h/.cpp`)

* `struct ParametresFlou { int rayonPixels = 5; };`
* `cv::Mat appliquerFlouCore(const cv::Mat &entree, const ParametresFlou &params);`

#### 4.5.2 Adaptateur flou (`adaptateur_flou.h/.cpp`)

* Classe `AdaptateurFlou : public TraitementImage`
* Attributs :

  * `int m_rayonMin = 0;`
  * `int m_rayonMax = 25;`
  * `QVariantMap m_parametresCourants;` (clé `"rayonPixels"`)
* Méthodes clés :

  * `QString nom() const override;` → `"Flou"`
  * `QString idTraitement() const override;` → `"flou_gaussien"`
  * `QImage appliquer(...)` :

    * convertit `QImage` → `cv::Mat`,
    * appelle `appliquerFlouCore`,
    * reconvertit en `QImage`.
  * `QWidget* creerEditeurParametres(QWidget *parent)` :

    * crée un petit widget avec `QSlider` / `QSpinBox`,
    * met à jour `m_parametresCourants`,
    * émet `parametresModifies()`.

Même schéma pour :

* `AdaptateurMosaique` (`idTraitement() = "mosaique"`)
* `AdaptateurSepia` (`"sepia"`)
* `AdaptateurInvertColor` (`"invertcolor"`)
* `AdaptateurMiroir` (`"miroir"`)
* `AdaptateurContours` (`"contours"`)

#### 4.5.3 Factory de traitements

**Fichier** : `traitements/traitement_factory.h`

```cpp
inline TraitementImage* creerTraitementDepuisId(const QString &id) {
    if (id == "flou_gaussien") return new AdaptateurFlou();
    if (id == "mosaique")      return new AdaptateurMosaique();
    if (id == "sepia")         return new AdaptateurSepia();
    if (id == "invertcolor")   return new AdaptateurInvertColor();
    if (id == "miroir")        return new AdaptateurMiroir();
    if (id == "contours")      return new AdaptateurContours();
    return nullptr;
}
```

Utilisé lors de l’import JSON pour reconstruire le pipeline.

---

### 4.6 Utilitaires

#### 4.6.1 `ClickableLabel`

**Fichier** : `clickablelabel.h`
**Rôle** : simplifier la détection de clic sur un `QLabel` (pour icônes webcam / vidéo).

```cpp
class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget* parent = nullptr) : QLabel(parent) {}
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event) override {
        emit clicked();
    }
};
```

---

## 5. Répartition par équipes (logique projet)

Pour coller à la consigne :

### Groupe 1 – Chaîne de traitements & paramétrage

* Dossier `traitements/core/`
* Dossier `traitements/adaptateurs_qt/`
* `traitements/traitement_factory.h`
* Partie “pipeline” dans `MainWindow` :

  * `EntreeBarreTraitement`,
  * `m_listeBarreTraitements`,
  * `executerPipeline(...)`,
  * `exporterSessionComplete()`, `importerSessionDepuisJSON(...)`,
  * `mettreAJourLogsTexte()`.

### Groupe 2 – Interface graphique & UX

* `mainwindow.ui`, `mainwindow.cpp` (menus, boutons, layout, label preview, icônes webcam/vidéo)
* `PanneauFluxTraitements` (dessin des flèches, gestion de la fenêtre de paramètres)
* `BoiteTraitement` (boîtes déplaçables, aperçu, bouton paramètres)
* `WebcamDialog` (fenêtre webcam)
* `ClickableLabel`
