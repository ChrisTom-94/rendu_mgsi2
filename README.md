# Rendu projets MGSI

Pour les tp j'ai voulu utiliser une petite librarie que j'ai developpé depuis quelque temps et qui me permet de faire des applications graphiques en OpenGL assez facilemen, le code est dans le dossier `SmartGL`.
Dans chaque dossiers de tp vous trouverez uniquement les classes necessaires pour le tp en question.

## Installation

> **Note:** Le seul projet qu'il ne faut pas compiler est le `TP_Automate (CIFS)`. Pour ce TP, il faut se placer à la racine du dossier et lancer le make

> **Note:** Pour compiler les projets, vous devez avoir `python` d'installé sur votre machine et vous devez vous trouver à la racine du projet (au même niveau que ce README.md).

Pour installer les projets taper la commande suivante:
```bash
python3 ./scripts/script.py build gmake

# Si jamais la commande ne passes pas, c'est que vous n'avez pas les droits d'execution sur le script. Pour cela, faites les commandes suivantes:
chmod +x ./scripts/script.py
chmod +x ./scripts/linux/premake5
```

Cette commande va vous generer les makefiles necessaires à chaque projet.

Ensuite, pour compiler les projets, il suffit de taper toujours depuis la racine:
```bash
make
```

SmartGL a quelques dependances qui peut prendre un peu de temps à compiler.

Une fois que vous avez fait la commande make, vous pouvez executer les projets avec la commande suivante:
```bash
python3 ./scripts/script.py run <nom_du_projet>

# Par exemple:
python3 ./scripts/script.py run TP1_Nurbs
python3 ./scripts/script.py run TP2_Nurbs
python3 ./scripts/script.py run TP_Deformations
python3 ./scripts/script.py run TP_GeometrieFractale
```
