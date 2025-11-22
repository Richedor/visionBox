# Instructions pour l’équipe Traitement

*(document officiel du projet VisionBox)*

Ce document explique **de A à Z** comment développer et intégrer un nouveau traitement dans VisionBox.
Il est destiné à l’équipe en charge de l’implémentation des traitements OpenCV et des adaptateurs Qt.

Veuillez lire entièrement ce document avant de commencer, car tout y est précisé.

---

# 1. Objectif général

L’objectif de l’équipe Traitement est de produire, pour chaque nouveau traitement :

1. Le **cœur OpenCV** du traitement (algorithme image → image)
2. L’**adaptateur Qt**, qui connecte ce traitement à l’interface VisionBox, gère les paramètres, affiche les sliders, et permet à la boîte d’évoluer selon les réglages.

Pour chaque traitement, vous me livrez **quatre fichiers** + **un bloc de code**.
Je m’occupe ensuite personnellement de l’intégration finale dans VisionBox.

---

# 2. Où travailler dans le projet

Dans le dépôt VisionBox, vous devez travailler uniquement dans :

```
VisionBox/
  traitements/
    core/                    <-- Vous développez ici les traitements OpenCV
    adaptateurs_qt/         <-- Vous développez ici les adaptateurs Qt
```

Vous ne devez **rien modifier ailleurs**, y compris :

* mainwindow
* affichage des boîtes
* interface globale
* CMake
* structure du projet

Je gère tout ce qui concerne l’UI.

---

# 3. Ce que vous devez produire pour chaque traitement

Pour un traitement nommé ici “MonExempleDeTraitement” (remplacez par votre vrai nom), vous devez fournir :

### 3.1. Dans `traitements/core/`

* `monexempledetraitement.h`
* `monexempledetraitement.cpp`

### 3.2. Dans `traitements/adaptateurs_qt/`

* `adaptateur_monexempledetraitement.h`
* `adaptateur_monexempledetraitement.cpp`

### 3.3. Dans **le fichier .h de l’adaptateur**

Vous devez ajouter **un commentaire contenant le code destiné à la factory**, par exemple :

```cpp
/*
A coller dans traitement_factory.h :

#include "adaptateurs_qt/adaptateur_monexempledetraitement.h"

if (id == "monexempledetraitement")
    return new AdaptateurMonExempleDeTraitement();
*/
```

Je ferai personnellement ce copier-coller.

---

# 4. Règles fondamentales à respecter

Pour que VisionBox fonctionne correctement, vos traitements doivent respecter strictement les règles suivantes :

### 4.1. Signature obligatoire du traitement

Le traitement OpenCV doit être écrit sous la forme :

```cpp
cv::Mat appliquerTraitementCore(
    const cv::Mat &entree,
    const ParametresMonTraitement &params
);
```

### 4.2. L’entrée ne doit jamais être modifiée

Toujours :

```cpp
cv::Mat sortie = entree.clone();
```

### 4.3. Traitement “stateless”

Votre traitement ne doit **pas conserver d’état d’une image à l’autre**.

Pas de :

* static dans la fonction
* variables globales
* buffers persistants

Ce point garantit la compatibilité vidéo.

### 4.4. Paramètres simples

Tous les paramètres doivent être des types simples :

* int
* double
* float
* bool
* éventuellement cv::Scalar

### 4.5. Aucun affichage, aucun accès disque dans le core

Pas de :

* `imshow`
* `waitKey`
* `imwrite`
* `cout` non contrôlé

Le traitement doit juste transformer une image en une autre.

---

# 5. Détails complets sur les fichiers à produire

## 5.1. Fichier `core/monexempledetraitement.h`

Ce fichier déclare :

* la structure des paramètres du traitement
* la fonction principale OpenCV

Voici le modèle, que vous devez modifier selon vos besoins :

```cpp
/*
    FICHIER : monexempledetraitement.h
    UTILITE :
    - Déclare la structure ParametresMonExempleDeTraitement
    - Déclare la fonction appliquerMonExempleDeTraitementCore
*/

#pragma once
#include <opencv2/opencv.hpp>

// Structure des paramètres du traitement.
// Vous pouvez ajouter autant de paramètres simples que nécessaire.
struct ParametresMonExempleDeTraitement
{
    int forceTraitement = 50;   // Exemple paramètre 1
    int seuil = 128;            // Exemple paramètre 2
    bool activerOption = false; // Exemple paramètre 3

    // Pour ajouter un paramètre :
    // - déclarer ici sa variable
    // - lui donner une valeur par défaut
};

// Déclaration de la fonction principale du traitement OpenCV.
cv::Mat appliquerMonExempleDeTraitementCore(
    const cv::Mat &entree,
    const ParametresMonExempleDeTraitement &params);
```

---

## 5.2. Fichier `core/monexempledetraitement.cpp`

Voici le modèle complet :

```cpp
/*
    FICHIER : monexempledetraitement.cpp
    UTILITE :
    - Implémente la fonction OpenCV du traitement.
*/

#include "monexempledetraitement.h"

cv::Mat appliquerMonExempleDeTraitementCore(
        const cv::Mat &entree,
        const ParametresMonExempleDeTraitement &params)
{
    if (entree.empty())
        return entree;

    cv::Mat sortie = entree.clone();

    // --------------------------------------------------------------
    // Exemple d'utilisation des paramètres.
    // Vous devez remplacer ce bloc par votre propre algorithmique.
    // --------------------------------------------------------------

    int force = std::clamp(params.forceTraitement, 0, 100);
    double sigma = (force / 100.0) * 10.0;

    if (sigma > 0.0)
        cv::GaussianBlur(entree, sortie, cv::Size(0, 0), sigma);

    if (params.activerOption)
        cv::threshold(sortie, sortie, params.seuil, 255, cv::THRESH_BINARY);

    return sortie;
}
```

---

# 6. Adaptateurs Qt

L’adaptateur Qt relie votre traitement OpenCV à l’interface VisionBox.

Il doit :

* lire les paramètres depuis `QVariantMap`
* convertir QImage ↔ cv::Mat
* créer les sliders, checkbox, champs, etc.
* appeler votre fonction OpenCV

## 6.1. Fichier `adaptateur_monexempledetraitement.h`

Vous devez :

* modifier le nom du traitement
* modifier l’identifiant
* déclarer les paramètres dans `m_parametresCourants`
* ajouter les bornes pour les sliders (min/max)

Le modèle fourni est déjà complet et commenté.

---

## 6.2. Fichier `adaptateur_monexempledetraitement.cpp`

Ce fichier contient :

* les conversions QImage ↔ Mat (à garder telles quelles)
* l’application du traitement
* la création des widgets de paramètres

Tout est déjà commenté ligne par ligne.

Pour chaque nouveau paramètre, vous devez :

1. L’ajouter dans `m_parametresCourants`
2. Ajouter un widget (slider, checkbox, bouton…) dans `creerEditeurParametres`
3. Récupérer sa valeur dans `appliquer()`

Tous ces endroits sont clairement indiqués.

---

# 7. Ajouter plusieurs paramètres (méthode officielle)

Pour ajouter un nouveau paramètre nommé `facteur` :

### 7.1. Dans `ParametresMonExempleDeTraitement` :

```cpp
double facteur = 1.5;
```

### 7.2. Dans `monexempledetraitement.cpp` :

```cpp
sortie = sortie * params.facteur;
```

### 7.3. Dans `adaptateur_monexempledetraitement.h` :

```cpp
double m_facteurMin = 0.0;
double m_facteurMax = 5.0;
```

### 7.4. Dans le constructeur :

```cpp
m_parametresCourants["facteur"] = 1.5;
```

### 7.5. Dans `creerEditeurParametres()` :

```cpp
QSlider *sldFacteur = new QSlider(Qt::Horizontal);
sldFacteur->setRange(0, 500);
sldFacteur->setValue(150);

connect(sldFacteur, &QSlider::valueChanged, this, [&](int v){
    m_parametresCourants["facteur"] = v / 100.0;
    emit parametresModifies();
});
layout->addWidget(sldFacteur);
```

### 7.6. Dans `appliquer()` :

```cpp
p.facteur = parametres.value("facteur").toDouble();
```

---

# 8. Obligations de test (OpenCV)

Vous devez **obligatoirement** tester votre traitement avec un petit projet Visual Studio simple, sans Qt.

Créez un projet contenant :

* `main.cpp`
* vos deux fichiers du dossier `core/`
* OpenCV

Exemple :

```cpp
#include <opencv2/opencv.hpp>
#include "monexempledetraitement.h"

int main()
{
    cv::Mat img = cv::imread("entree.jpg");
    if (img.empty())
        return 1;

    ParametresMonExempleDeTraitement p;
    p.forceTraitement = 80;
    p.seuil = 100;
    p.activerOption = true;

    cv::Mat out = appliquerMonExempleDeTraitementCore(img, p);
    cv::imwrite("sortie.jpg", out);

    return 0;
}
```

Ceci permet de vérifier rapidement :

* que votre traitement fonctionne
* que vos paramètres sont pris en compte
* qu'il n’y a pas d’erreur OpenCV

---

# 9. Compatibilité vidéo

Votre traitement est **automatiquement compatible vidéo**, rien à faire.

VisionBox appliquera votre fonction image → image sur chaque frame d’une vidéo.

---

# 10. Ce que vous devez me livrer

Pour chaque traitement, vous me rendez :

1. `core/montraitement.h`
2. `core/montraitement.cpp`
3. `adaptateurs_qt/adaptateur_montraitement.h`
4. `adaptateurs_qt/adaptateur_montraitement.cpp`
5. À la fin du `.h` de l’adaptateur :
   le bloc de code pour la factory
   (je m’occupe de le copier)

Une fois ces fichiers reçus, j’intègre le traitement dans VisionBox.

---
