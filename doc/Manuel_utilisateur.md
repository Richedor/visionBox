#  Manuel d’utilisation – VisionBox

Chaînage de traitements d’image – M1 EEA / TSI

---

## 1. Introduction

**VisionBox** est une application permettant de créer facilement une chaîne de traitements d’image en utilisant :

* une image chargée depuis un fichier,
* une image capturée depuis une webcam,
* ou des images extraites d’une vidéo.

L’utilisateur peut :

✔ ajouter des traitements,
✔ paramétrer chaque effet,
✔ visualiser le pipeline sous forme de boîtes connectées,
✔ afficher le résultat final,
✔ sauvegarder l’image,
✔ et enregistrer/recharger un workflow complet au format JSON.

Ce manuel décrit toutes les étapes d’utilisation.

---

## 2. Lancement de l’application

VisionBox peut être lancé de deux façons :

### 2.1 Via l'exécutable (Windows)

Depuis le dossier :

```
release/VisionBox.exe
```

Double-cliquez sur l’exécutable.
Aucune installation n’est nécessaire.

### 2.2 Via Qt Creator (compilation)

Si vous compilez le projet :

1. Ouvrir `CMakeLists.txt` dans Qt Creator.
2. Choisir un kit Qt 6 (MSVC recommandé).
3. Compiler puis exécuter.

---

## 3. Interface générale

<img width="1920" height="1032" alt="InterfaceGeneral" src="https://github.com/user-attachments/assets/df3009f2-0fcc-4832-9eca-3e0f0eb41a59" />

L’interface VisionBox se compose de plusieurs zones :

1. **Barre d’outils** (haut)

   * Charger image
   * Webcam
   * Vidéo
   * Exporter image
   * Exporter / importer workflow

2. **Aperçu principal** (centre)
   Affiche l’image source ou le résultat final du pipeline.

3. **Panneau de flux de traitements** (droite / en bas selon l’organisation)

   * Affiche les boîtes représentant les traitements
   * Les boîtes sont reliées par des flèches
   * Chaque boîte contient :

     * le nom du traitement
     * une miniature
     * un bouton **Paramètres**

4. **Barre de traitements** (bas)

   * Liste ordonnée des traitements ajoutés
   * Flèches gauche/droite pour réordonner
   * Croix pour supprimer un traitement

5. **Zone de logs**
   Résume :

   * l’acquisition
   * la liste des traitements
   * les paramètres actuels

---

## 4. Charger une image

VisionBox propose trois modes d’acquisition.

<img width="1104" height="632" alt="EcranAcquisition" src="https://github.com/user-attachments/assets/daa2be4b-3d4e-4d39-bcaf-e791ff71a197" />


---

### 4.1 Depuis un fichier image

1. Cliquer sur **“Charger image”**.
2. Sélectionner un fichier : `.png`, `.jpg`, `.bmp`, etc.
3. L’image apparaît dans l’aperçu.

**Remarque :**
L’image chargée devient la **source** du pipeline.

---

### 4.2 Depuis une webcam

<img width="642" height="512" alt="webcam" src="https://github.com/user-attachments/assets/2358f373-b900-4863-8eac-a89a1537bc00" />

1. Cliquer sur **“Webcam”**.
2. Une fenêtre dédiée s’ouvre.
3. Attendre que l’aperçu de la caméra s’affiche.
4. Cliquer sur **“Capturer”**.
5. L’image capturée devient l’image source.

La fenêtre se ferme automatiquement.

---

### 4.3 Depuis une vidéo

1. Cliquer sur **“Vidéo”**.
2. Choisir un fichier vidéo.
3. La vidéo se lance automatiquement dans l’aperçu.
4. Un bouton **Play/Pause** apparaît directement sur l’image.
5. L’image affichée est utilisée comme source pour les traitements.

---

## 5. Ajouter des traitements

En bas de l’interface, les traitements disponibles peuvent être ajoutés via :

* des boutons dédiés,
* un menu contextuel,
* un bouton “+” selon votre version de l’UI.

### Traitements disponibles :

* **Flou gaussien**
* **Mosaïque**
* **Inversion des couleurs**
* **Sepia**
* **Miroir**
* **Contours (Canny)**

### Lorsqu’un traitement est ajouté :

1. Une entrée apparaît dans la **barre de traitements**.
2. Une **boîte** correspondante est créée dans le panneau de flux.
3. Une miniature du traitement s'affiche.
4. Le résultat final est recalculé.

---

## 6. Réordonner ou supprimer des traitements

### 6.1 Réordonner

Dans la barre de traitements :

* Cliquer sur ❮ pour déplacer un traitement vers la gauche.
* Cliquer sur ❯ pour le déplacer vers la droite.

L’ordre du pipeline se met à jour en temps réel.

### 6.2 Supprimer

Cliquer sur **✖** dans la barre de traitements.
La boîte correspondante disparaît du pipeline et le résultat final est recalculé.

---

## 7. Paramétrer un traitement

Chaque boîte dispose d’un bouton **Paramètres**.

### Pour régler un traitement :

<img width="362" height="432" alt="ParametreContour" src="https://github.com/user-attachments/assets/f0970555-a4ca-4731-beb5-cd1d21ff92b3" />

1. Cliquer sur le bouton **Paramètres** dans la boîte.
2. Une fenêtre s’ouvre contenant :

   * une **prévisualisation** du résultat du traitement,
   * un ensemble de **sliders / champs numériques** spécifiques,
   * un bouton permettant d'appliquer ou fermer.

### Exemples de paramètres :

**Flou :** rayon (0 → 25)
**Mosaïque :** taille des cellules
**Contours :** seuils bas et haut
**Miroir :** mode horizontal / vertical
**Sepia :** intensité

Chaque modification met automatiquement à jour :

* l’aperçu de la fenêtre de réglage,
* la miniature dans la boîte,
* le résultat final.

---

## 8. Sauvegarder l’image finale

1. Cliquer sur **“Exporter image”**.
2. Choisir un format (.png, .jpg…).
3. L’image affichée dans l’aperçu est enregistrée.

---

## 9. Sauvegarder / Recharger un workflow (JSON)

VisionBox peut sauvegarder une session complète :

### Le fichier JSON contient :

* le type d’acquisition (fichier, webcam, vidéo),
* le chemin source,
* l’ordre des traitements,
* les paramètres de chaque traitement,
* le chemin de l’image finale.

### 9.1 Exporter un workflow

1. Cliquer sur **“Exporter workflow”**.
2. Choisir un dossier.
3. VisionBox crée :

   * un fichier `.json`,
   * l’image finale.

### 9.2 Importer un workflow

1. Cliquer **“Importer workflow”**.
2. Sélectionner un fichier `.json`.
3. VisionBox :

   * recharge l’image source,
   * reconstruit le pipeline complet,
   * réapplique tous les paramètres,
   * remet à jour l’affichage.

---

## 10. Mode vidéo

Lorsqu’une vidéo est chargée :

* VisionBox lit les frames via OpenCV,
* l’aperçu se met à jour automatiquement,
* un bouton Play/Pause permet d’arrêter ou relancer la lecture.

Les traitements sont appliqués sur **l’image en cours de lecture** pour l’aperçu, mais le workflow reste compatible avec la sauvegarde en JSON.

---

## 11. Zone de logs

La zone de logs affiche :

* la source de l’image,
* la liste des traitements,
* les paramètres courants,
* les informations d’import/export.

Les logs sont mis à jour automatiquement.

---

## 12. Problèmes courants

| Problème                     | Solution                                                                      |
| ---------------------------- | ----------------------------------------------------------------------------- |
| La webcam ne s’ouvre pas     | Vérifier qu’elle n’est pas utilisée ailleurs                                  |
| L’exécutable ne se lance pas | S’assurer que toutes les DLL Qt sont présentes (version packagée recommandée) |
| La vidéo ne lit pas          | Vérifier le codec OpenCV                                                      |
| L’image finale est noire     | Vérifier les paramètres extrêmes (ex : seuils Canny trop élevés)              |

---

## 13. À propos

* Projet universitaire
* Réalisé en équipe (traitements + interface + gestion projet)
* Développé avec Qt 6, C++17 et OpenCV 4.x

