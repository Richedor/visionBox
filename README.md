# VisionBox – Application de chaînage de traitements d'image  

VisionBox est une application modulaire permettant d’acquérir une image (fichier, webcam, vidéo), d’enchaîner plusieurs traitements OpenCV, de visualiser le pipeline sous forme de boîtes reliées et d’exporter le résultat final ou le workflow complet.

Ce projet a été réalisé en C++/Qt6 avec OpenCV dans le cadre du module **Programmation C++** – Université de Bourgogne.

---

##  Fonctionnalités principales

- Chargement d’image (PNG, JPG, BMP)
- Capture via webcam
- Lecture vidéo + Play/Pause
- Construction d’une chaîne de traitements (flou, mosaïque, contours, etc.)
- Boîtes déplaçables reliées par des flèches
- Paramétrage individuel des traitements
- Export de l’image finale
- Sauvegarde / rechargement d’une session complète (JSON)

---

##  Installation & exécution

### Prérequis
- **Qt 6** (Qt Creator recommandé)
- **OpenCV** (déjà configuré dans le projet)
- Windows 10/11

### Lancer l’application
1. Ouvrir Qt Creator  
2. `Fichier` → `Ouvrir un projet...`  
3. Sélectionner `CMakeLists.txt`  
4. Choisir un kit (ex. *Desktop Qt 6.x MSVC*)  
5. Compiler et exécuter (`▶`)

---

##  Structure du dépôt

```

VisionBox/
│
├─ src/                      # Code source complet
├─ images/                   # Ressources graphiques
├─ documentation/            # Documentation du projet
│   ├─ cahier_des_charges.pdf
│   ├─ manuel_utilisateur.pdf
│   └─ presentation.pdf
├─ docs/captures/            # Captures d’écran de l’application
└─ README.md                 # Présent fichier

```

---

##  Documentation complète

Toute la documentation se trouve dans :  
 **`/documentation/`**

Elle contient :
- 📌 Cahier des charges  
- 📌 Manuel utilisateur détaillé  
- 📌 Présentation PowerPoint / PDF

---

##  Auteurs

Année 2025–2026.

---

##  Licence
Projet académique – utilisation non commerciale.
```
