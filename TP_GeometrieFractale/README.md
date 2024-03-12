# TP Montagnes fractales

Malheureusement, lorsque j'ai commencé le TP, je ne suis pas parti sur l'idée de pouvoir afficher la mêmme montagne avec différentes itérations. Je construis une nouvelle montagne à chaque fois qu'un paramètre change. Il est possible de constater que l'algorithme fonctionne en passant en mode wireframe et en observant les montagnes générées à chaque changement d'itération.
Toutes les intéractions possibles sont dans l'interface UI.

## Fichiers

- `TP_GeometrieFractale.cpp` : fichier principal
- src
    - **Editor** : Gestion de l'interface graphique
    - **Renderer** : Gestion de l'affichage
    - **FractalMountain** : Génération de la montagne fractale
- shaders
    - **mountain.glsl** : shader simple pour le rendu de la montagne
    - **sea.glsl** : shader simple pour le rendu de l'eau
