# TP1 Courbes et repére de Frenet

## Fichiers

Le fichier `TP1_Nurbs.cpp` est le fichier d'entrée del'application.

Dans le dossier src, vous trouverez les fichiers suivants :
- **Editor** : Interface utilisateur de l'application.
- **Renderer** : Responsable de l'affichage de la scène.
- **BSplineCurve** : Classe pour encapsuler une courbe de B-Spline.


## Utilisation

Il est possible d'intéragir avec l'application en utilisant la souris et le clavier, ainsi que l'interface UI.
La caméra fonctionne avec un control de type "ArcBall", qui vous permet d'orbiter autour de la scène en utilisant le click droit de la souris.
Pour la manipulation de la courbe, séléctionnez un point de control avec le click gauche, ensuite appuier sur la touche `G` pour déplacer le point de control et voir la courbe évoluer en temps réel.

### Touches disponibles


> **Ces touches ne fonctionnent que lorsqu'il y a un point de control séléctionné.**

- `G` : Déplacer le point de control séléctionné
- `E` : Ajouter un point de control à partir du point de control séléctionné
- `ESC` : Annuler le déplacement du point de control
- `SUPPR` : Supprimer le point de control séléctionné
