# TP Deformations

La seule chose un peu tricky dans ce TP est que j'ai voulu faire du deferred shading. Donc le code du renderer est un peu lourd.
J'utilise un GBuffer pour appliquer les transformations sur les normales et les positions des fragments et stocker les résultats dans des textures. Ensuite, je les utilise dans la pass de shading pour calculer la couleur finale.
Toutes les intéractions possibles sont dans l'interface UI.

## Fichiers

- `TP_Deformations.cpp` : fichier principal
- src
    - **Editor** : Gestion de l'interface graphique
    - **Renderer** : Gestion de l'affichage
- shaders
    - **twist.glsl** : shader pour la déformation par torsion
    - **pinch.glsl** : shader pour la déformation par pincement
    - **blinn-phong.glsl** : shader pour le shading
