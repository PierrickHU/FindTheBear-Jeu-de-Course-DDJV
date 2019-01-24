#Find the bear - Jeu de Course

Jeu Produit pour la session d'Automne du DDJV de l'Universite de Sherbrooke.

Equipe "The Inclusives Jellyfishes" : 
        Guillaume BUCHLE
        Iannick LANGEVIN
        Alexis DUSSIER
        Pierrick HUMBERT
        Redha Maouly-Vilatimo
        
DDJV - cohorte 14
2018/2019

Pour tester le prototype, il faut compiler le projet en Debug.

Pour cette version finale, nous avons implémenté les déplacements du véhicule , un circuit , différents items à récupérer, trois caméra ainsi que des collisions adaptées au différents objets qui se collisionent.
Pour ce qui est de DirectX, nous avons dans notre jeu une skybox, un menu et un HUD, plusieurs billboards. Pour ce projet nous devions implémenter des technique HLSL. Nous avons donc fait le choix d'implémenter le shadow mapping, le normal mapping, un effet de Blur
et pour finir de l'alpha blending.


Pour ce qui est de l'importation des objets, nous avons  importés plusieurs objets 3D  : un nuage, des arbres, des ours, des sabliers et des caisses bonus. 


Pour controler le véhicule (le nuage), il faut utiliser les touches "WASD". W pour avancer, S pour reculer puis A et D pour tourner a gauche et a droite en avancant ou en reculant. 
Lorsque le joueur arrête d'avancer ou de reculer le véhicule va progressivement décélérer.
Lorsque le joueur décide de reculer juste apres avoir avancé le véhicule va d'abord décélerer avant de reculer.
L'oscillation du véhicule est voulue, elle a pour but de simuler le comportement de notre nuage (véhicule prévu).

Il existe un menu d'entrée et un menu pause permettant de changer de difficulté même si dans la version actuelle les différentes difficultées proposées n'ont aucun impact sur le jeu.

Nous avons donc trois caméras implémentées dans le jeu. Celle  définie par défaut va suivre le véhicule et donc les inputs du joueur.

Cette 1ere caméra va se replacer derriere le joueur lorsqu'il va tourner. 

La 2eme caméra (FreeCam) est plus utile pour débuger et observer différents problemes. Pour l'activer il faut appuyer la touche "2". On peut ensuite la diriger avec "WASD" (pour modifier la position) et les flèches (pour modifier la direction).

Une fois sur la 2eme caméra, il suffit d'appuyer sur la touche "1" pour repasser sur la caméra du joueur.

La 3eme caméra est la caméra 1ère personne. Pour l'activer il faut appuyer sur la toucher "3".

Le circuit à grandement été amélioré depuis le volet 2. Maintenant notre terrain est beaucoup plus grand, et à une forme circulaire. Au centre de notre terrain, il y a un chateau traversé par un tunnel. Nous avons également fait apparaitre un grand nombre
d'arbre sur le terrain. Les différentes textures du terrain sont mélangés grâce à la technique d'alpha blending. Il faut également savoir que le dénivelé du terrain est généré aléatoirement.

Pour ce qui est du jeu en lui même, pour gagner il faut récupérer les 8 oursons qui sont dispersés sur la carte avant le temps impartie. Lorsque que l'on récupère les caisses Bonus la vitesse du véhicule augmente.
Pour récupérer du temps , il faut passer en mode cauchemars. Pour passer dans ce mode il faut appuyer sur la barre espace. Dans ce monde, il est possible de récupérer des fioles de temps qui rajoute 20 secondes au chrono.
Mais attention, si dans ce mode le joueur touche un mur, un arbre ou la limite du terrain il perd la partie. A noter que le joueur ne peut pas récupérer les oursons dans le monde des cauchemars.
Au bout de 20 secondes le joueur retourne automatiquement dans le monde "normal" des rêves.




## Description du format des données

Les objets importés sont en .obj.

Les textures importés sont en .dds. 

## Utilisation de bibliotheques exterieures 

Pour importer les .obj nous utilisons la librairie Assimp.
