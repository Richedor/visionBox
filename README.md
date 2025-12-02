# VisionBox – Projet C++ / Qt / OpenCV

Chaînage de traitements d’image 
<br>

##  Technologies utilisées

<p align="left">
  <img src="https://img.shields.io/badge/Qt%206-41cd52?logo=qt&logoColor=white&style=for-the-badge" />
  <img src="https://img.shields.io/badge/C%2B%2B17-00599C?logo=cplusplus&logoColor=white&style=for-the-badge" />
  <img src="https://img.shields.io/badge/OpenCV-5C3EE8?logo=opencv&logoColor=white&style=for-the-badge" />
  <img src="https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=white&style=for-the-badge" />
  <img src="https://img.shields.io/badge/GitHub-181717?logo=github&logoColor=white&style=for-the-badge" />
  <img src="https://img.shields.io/badge/JSON-000000?logo=json&logoColor=white&style=for-the-badge" />
  <img src="https://img.shields.io/badge/Windows%20EXE-0078D6?logo=windows&logoColor=white&style=for-the-badge" />
</p>

<br>

##  Description

**VisionBox** est une application permettant :

* de charger une image depuis un fichier, une webcam ou une vidéo,
* d’enchaîner plusieurs traitements OpenCV (flou, mosaïque, sepia, contours, etc.),
* de visualiser ces traitements sous forme de **boîtes connectées**,
* de paramétrer chaque traitement,
* d’afficher puis **sauvegarder** le résultat final,
* de **sauvegarder / recharger** un workflow complet en JSON.

Projet réalisé dans le cadre du **M1 EEA / TSI – Programmation C++**.

<br>

##  Contenu important du dépôt

```
doc/
    Cahier_des_charges.md
    Manuel_utilisateur.pdf
    presentationVisionBox.pdf
    modelisationVisionBox.pdf

release/
    VisionBox.exe      → Application Windows prête à l’emploi
```

<br>

##  Lancer l’application

###  Méthode 1 : Exécutable Windows (recommandé)

Télécharger simplement :

```
release/VisionBox.exe
```

Aucune installation nécessaire (exécutable autonome).

###  Méthode 2 : Compilation

* Qt 6
* CMake
* OpenCV 4.x

Ouvrir `CMakeLists.txt` avec Qt Creator, configurer un kit, puis compiler.

<br>

##  Fonctionnalités clés

* Chargement image / webcam / vidéo
* Pipeline de traitements modifiable (ajout, suppression, déplacement)
* Paramétrage des effets
* Aperçu en temps réel
* Export de l’image finale
* Sauvegarde / rechargement JSON du workflow

<br>

##  Documentation utile

* Qt 6 : [https://doc.qt.io/qt-6/qimage.html](https://doc.qt.io/qt-6/qimage.html)
* Qt Docs : [https://doc.qt.io/](https://doc.qt.io/)
* OpenCV : [https://docs.opencv.org/4.x/](https://docs.opencv.org/4.x/)

<br>

##  Travail en équipe

* **Interface graphique** (Qt Widgets, affichage, boîtes)
* **Traitements OpenCV & chaîne de traitement**
* **Gestion de projet : Git, GitHub, packaging, documentation**

<br>
