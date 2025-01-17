VERSION TCP :

TETRIS 2 joueurs avec Sockets TCP : 

Pour pouvoir compiler et exécuter le jeu, il faut tout d'abord installer la bibliothèque SDL2 :

- Pour Mac : brew install sdl2 
- Pour Linux : sudo apt-get install libsdl2-dev

Une fois cette bibliothèque installée, il faut récupérer les chemins vers les headers et la bibliothèque.

Pour cela, effectuer la commande suivante dans un terminal : "sdl2-config --cflags" cela renvoie une réponse du type "-I/..." 
Pour récupérer le chemin vers la librairie il faut effectuer la commande suivante : "sdl2-config --libs" cela renvoie une réponse du type "-L/... -lSDL2"

Pour compiler voici les commandes à entrer dans le terminal : 

gcc -o server_TCP server_TCP.c -I/.../include -L/.../lib -lSDL2
gcc -o client_TCP client_TCP.c -I/.../include -L/.../lib -lSDL2

Mettre à la place du "-I/.../include" le chemin obtenu avec "sdl2-config --cflags"
Mettre à la place du "-L/.../lib" le chemin obtenu avec "sdl2-config --libs"

Il y a aussi le Makefile pour pouvoir automatiser la compilation : 
il faut juste remplacer dans la variable "LDFLAGS" les chemins et mettre les chemins obtenus avec "sdl2-config --cflags" et "sdl2-config --libs" précedemment 


Pour l'exécution voici les commandes à entrer dans le terminal : 
./server_TCP 
./client_TCP IP_du_server

Il faut exécuter client_TCP avec l'IP du server :
Pour récupérer l'IP du server, il faut entrer la commande "ifconfig" dans un terminal et trouver l'IP du server.

Il faut d'abord exécuter server_TCP : le message suivant s'affiche "Serveur en écoute sur le port 30000..."
Le server attend la connextion des 2 joueurs pour lancer la partie de TETRIS.

Il faut ensuite exécuter la commande "./client_TCP IP_du_server" dans un 2ème terminal ou sur un autre PC pour connecter le 1er client.
Refaire la même chose pour connecté le 2ème client. 

Dans le terminal de chaque client le message suivant s'affiche "Connecté au serveur."
Dans le terminal du server, le message suivant s'affiche "Joueur 1 connecté." quand le 1er client se connecte 
Puis "Joueur 2 connecté" et "Tous les joueurs sont connectés. Début de la partie !" quand le 2ème client est connecté   

Le jeu TETRIS se lance, pour chaque client une fenetre SDL s'ouvre montrant l'état actuel de la grille de TETRIS et dans le terminal de chaque client il y a aussi l'état actuel du tour qui s'affiche ainsi que la pièce actuelle.
Le joueur à qui c'est le tour doit rentrer une action parmi les suivantes : left, right, rotate ou drop 
L'autre joueur à un message d'attente.
Puis le jeu se joue ensuite à tour de rôle.

Les pièces tombent d'une ligne à chaque instruction des clients, à part pour l'instruction "drop" où dans ce cas la pièce descend directement.
Pour l'instruction rotate, si la pièce à l'espace nécessaire autour d'elle pour tourner alors la pièce tourne et descend d'une ligne sinon la pièce ne tourne pas et descend d'une ligne.
Pour les instructions "left" et "right", si la pièce peut aller à gauche ou à droite alors la pièce descend et se déplace d'une colone à gauche ou à droite sinon (cas sur les bords de la grille) la pièce dscend juste d'une ligne.

La limite de la grille de TETRIS est la ligne juste en dessous du spawn (l'appartition) de la pièce.
Dès qu'une pièce touche cette ligne alors la partie est finie "GAME OVER" et le jeu se relance.
Quand le jeu se relance la main est toujours donné au joueur 1 (client 1).

Si une ligne est remplie de cases de couleurs (ligne complète) alors la ligne disparaît et les blocs se trouvant dans les lignes au-dessus de la ligne complète tombe tous d'une ligne (d'une case).
Même chose si plusieurs lignes sont complètes en même temps.

Pour cette version du jeu avec Sockets TCP, pour fermer le server et les clients il faut suivre les étapes suivantes :

- D'abord fermer le server avec Control+C 
- Fermer ensuite les clients avec Control+C et entrée

(Pour la version UDP il faut d'abord fermer les clients avec Control+C et entrée, puis ensuite fermer le server mais cela est expliqué dans la partie "Version UDP")

##########################################################################
VERSION UDP :

TETRIS 2 joueurs avec Sockets UDP : 

Pour pouvoir compiler et exécuter le jeu, il faut tout d'abord installer la bibliothèque SDL2 :

- Pour Mac : brew install sdl2 
- Pour Linux : sudo apt-get install libsdl2-dev

Une fois cette bibliothèque installée, il faut récupérer les chemins vers les headers et la bibliothèque.

Pour cela, effectuer la commande suivante dans un terminal : "sdl2-config --cflags" cela renvoie une réponse du type "-I/..." 
Pour récupérer le chemin vers la librairie il faut effectuer la commande suivante : "sdl2-config --libs" cela renvoie une réponse du type "-L/... -lSDL2"

Pour compiler voici les commandes à entrer dans le terminal : 

gcc -o server_UDP server_UDP.c -I/.../include -L/.../lib -lSDL2
gcc -o client_UDP client_UDP.c -I/.../include -L/.../lib -lSDL2

Mettre à la place du "-I/.../include" le chemin obtenu avec "sdl2-config --cflags"
Mettre à la place du "-L/.../lib" le chemin obtenu avec "sdl2-config --libs"

Il y a aussi le Makefile pour pouvoir automatiser la compilation : 
il faut juste remplacer dans la variable "LDFLAGS" les chemins et mettre les chemins obtenus avec "sdl2-config --cflags" et "sdl2-config --libs" précedemment 


Pour l'exécution voici les commandes à entrer dans le terminal : 
./server_UDP 
./client_UDP IP_du_server

Il faut exécuter client_UDP avec l'IP du server :
Pour récupérer l'IP du server, il faut entrer la commande "ifconfig" dans un terminal et trouver l'IP du server.


Il faut d'abord exécuter server_UDP : le message suivant s'affiche "Serveur en écoute sur le port 30000..."
Le server attend la connextion des 2 joueurs pour lancer la partie de TETRIS.

Il faut ensuite exécuter la commande "./client_UDP IP_du_server" dans un 2ème terminal ou sur un autre PC pour connecter le 1er client.
Refaire la même chose pour connecté le 2ème client. 

Dans le terminal de chaque client le message suivant s'affiche "Connecté au serveur."
Dans le terminal du server, le message suivant s'affiche "Joueur 1 connecté." quand le 1er client se connecte 
Puis "Joueur 2 connecté" et "Tous les joueurs sont connectés. Début de la partie !" quand le 2ème client est connecté   

Le jeu TETRIS se lance, pour chaque client une fenetre SDL s'ouvre montrant l'état actuel de la grille de TETRIS et dans le terminal de chaque client il y a aussi l'état actuel du tour qui s'affiche ainsi que la pièce actuelle.
Le joueur à qui c'est le tour doit rentrer une action parmi les suivantes : left, right, rotate ou drop 
L'autre joueur à un message d'attente.
Puis le jeu se joue ensuite à tour de rôle.

Les pièces tombent d'une ligne à chaque instruction des clients, à part pour l'instruction "drop" où dans ce cas la pièce descend directement.
Pour l'instruction rotate, si la pièce à l'espace nécessaire autour d'elle pour tourner alors la pièce tourne et descend d'une ligne sinon la pièce ne tourne pas et descend d'une ligne.
Pour les instructions "left" et "right", si la pièce peut aller à gauche ou à droite alors la pièce descend et se déplace d'une colone à gauche ou à droite sinon (cas sur les bords de la grille) la pièce dscend juste d'une ligne.

La limite de la grille de TETRIS est la ligne juste en dessous du spawn (l'appartition) de la pièce.
Dès qu'une pièce touche cette ligne alors la partie est finie "GAME OVER" et le jeu se relance.
Quand le jeu se relance la main est toujours donné au joueur 1 (client 1).

Si une ligne est remplie de cases de couleurs (ligne complète) alors la ligne disparaît et les blocs se trouvant dans les lignes au-dessus de la ligne complète tombe tous d'une ligne (d'une case).
Même chose si plusieurs lignes sont complètes en même temps.

Pour cette version du jeu avec Sockets UDP, pour fermer le server et les clients il faut suivre les étapes suivantes :

- D'abord fermer le client à qui c'est le tour avec Control+C et Entrée
- Ensuite fermer le 2ème client avec Control+C et Entrée
- Fermer ensuite le server avec Control+C 

