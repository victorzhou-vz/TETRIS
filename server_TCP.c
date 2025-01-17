
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define ROWS 20
#define COLUMNS 10
#define MAX_PLAYERS 2
#define BUF_SIZE 1024
#define PORT 30000

typedef struct {
    int x, y;          // Position de la pièce
    int shape[4][4];   // Forme de la pièce
    char type;         // Type de la pièce (I, O, T, L, J, S, Z)
} Piece;

typedef struct {
    int sock;
    char name[BUF_SIZE];
    int score;
    int is_turn;
} Player;

int grid[ROWS][COLUMNS] = {0}; // Grille du jeu
Player players[MAX_PLAYERS];   // Joueurs connectés
int current_player = 0;        // Joueur actuel
Piece current_piece;           // Pièce active
Piece pieces[7];               // Liste des pièces possibles

void init_pieces() {
    int I[4][4] = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    int O[4][4] = {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    int T[4][4] = {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    int L[4][4] = {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    int J[4][4] = {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    int S[4][4] = {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    int Z[4][4] = {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    
    memcpy(pieces[0].shape, I, sizeof(I)); pieces[0].type = 'I';
    memcpy(pieces[1].shape, O, sizeof(O)); pieces[1].type = 'O';
    memcpy(pieces[2].shape, T, sizeof(T)); pieces[2].type = 'T';
    memcpy(pieces[3].shape, L, sizeof(L)); pieces[3].type = 'L';
    memcpy(pieces[4].shape, J, sizeof(J)); pieces[4].type = 'J';
    memcpy(pieces[5].shape, S, sizeof(S)); pieces[5].type = 'S';
    memcpy(pieces[6].shape, Z, sizeof(Z)); pieces[6].type = 'Z';
}

void init_grid() {
    memset(grid, 0, sizeof(grid));
}

void spawn_piece() {
    int index = rand() % 7;
    memcpy(current_piece.shape, pieces[index].shape, sizeof(pieces[index].shape));
    current_piece.type = pieces[index].type;
    current_piece.x = COLUMNS / 2 - 2;
    current_piece.y = 0;
}

int can_move(int new_y, int new_x) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (current_piece.shape[i][j]) {
                int grid_y = new_y + i;
                int grid_x = new_x + j;
                if (grid_y >= ROWS || grid_x < 0 || grid_x >= COLUMNS || grid[grid_y][grid_x]) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void place_piece() {
    int type_value;

    // Associer un entier au type de pièce
    switch (current_piece.type) {
        case 'I': type_value = 1; break;
        case 'O': type_value = 2; break;
        case 'T': type_value = 3; break;
        case 'L': type_value = 4; break;
        case 'J': type_value = 5; break;
        case 'S': type_value = 6; break;
        case 'Z': type_value = 7; break;
        default: type_value = 0; break;
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (current_piece.shape[i][j]) {
                grid[current_piece.y + i][current_piece.x + j] = type_value;
            }
        }
    }
}

void send_to_all(const char *message) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (players[i].sock != 0) {
            send(players[i].sock, message, strlen(message), 0);
        }
    }
}

void reset_game() {
    init_grid();
    spawn_piece();
    current_player = 0;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].is_turn = (i == 0); // Le premier joueur commence
    }
}

void line_complete() {
    int j = 0;
    for (int i = 0; i < ROWS; i++)
    {
        j = 0 ; 
        while(grid[i][j] != 0 && j < COLUMNS){
            j++;
        }
        if( j == COLUMNS){
            for (int k = 0; k < COLUMNS; k++)
            {
                grid[i][k] = 0; 
            }
            if(i > 3){
                for (int r = i; r > 3; r--)
                {
                    for (int c = 0; c < COLUMNS; c++)
                    {
                        grid[r][c] = grid[r-1][c];
                    }
                }
            }
        }
    }
}
/*
void send_game_state() {
    char buffer[BUF_SIZE];
    snprintf(buffer, BUF_SIZE, "Grille actuelle :\n");

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            char cell = grid[i][j] ? '#' : '.';
            strncat(buffer, &cell, 1);
        }
        strncat(buffer, "\n", 1);
    }

    snprintf(buffer + strlen(buffer), BUF_SIZE - strlen(buffer), "\nPièce actuelle : %c\n", current_piece.type);
    send_to_all(buffer);
}*/
void send_game_state() {
    char buffer[BUF_SIZE];
    int offset = 0;

    // Ajouter l'entête
    offset += snprintf(buffer + offset, BUF_SIZE - offset, "Grille actuelle :\n");
    line_complete();
    // Ajouter l'état actuel de la grille
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            char cell; //= grid[i][j] ? '#' : '.';
            switch(grid[i][j]){
                case 1 : cell = 'I' ; break ; 
                case 2 : cell = 'O' ; break ;
                case 3 : cell = 'T' ; break ;
                case 4 : cell = 'L' ; break ;
                case 5 : cell = 'J' ; break ;
                case 6 : cell = 'S' ; break ;
                case 7 : cell = 'Z' ; break ;
                default : cell = '.' ; break ;
            }

            if (offset < BUF_SIZE - 1) {
                buffer[offset++] = cell;
            }
        }
        if (offset < BUF_SIZE - 1) {
            buffer[offset++] = '\n';
        }
    }

    // Ajouter la pièce active
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (current_piece.shape[i][j]) {
                int grid_y = current_piece.y + i;
                int grid_x = current_piece.x + j;

                if (grid_y >= 0 && grid_y < ROWS && grid_x >= 0 && grid_x < COLUMNS) {
                    // Calculer la position dans le buffer
                    int pos = grid_y * (COLUMNS + 1) + grid_x + strlen("Grille actuelle :\n");
                    if (pos < BUF_SIZE) {
                        buffer[pos] = current_piece.type;
                    }
                }
            }
        }
    }
    int game_over=-1;
    for (int j = 0; j < COLUMNS; j++)
    {

        if(grid[3][j] != 0){

            game_over=1;
            break;
        }
        
    }
    /*
    if (game_over == 1) {
        // Envoyer un message de GAME OVER à tous les clients
        strncat(buffer, "\nGAME OVER! La partie est terminée. La partie va redémarrer...\n", BUF_SIZE - strlen(buffer) - 1);
        send_to_all(buffer);
        printf("GAME OVER: La partie va être réinitialisée.\n");
        // Réinitialiser le jeu
        reset_game();
        // Envoyer l'état initial du jeu après réinitialisation
        send_game_state();
    } else {
        // Terminer la chaîne
        buffer[offset] = '\0';

        // Envoyer aux clients
        snprintf(buffer + strlen(buffer), BUF_SIZE - strlen(buffer), "\nPièce actuelle : %c\n", current_piece.type);
        printf("Buffer envoyé :\n%s\n", buffer);

        send_to_all(buffer);
    }*/

    
    if(game_over == 1){
        
        //strncat(buffer, "\nGAME OVER! La partie est terminée.\n", BUF_SIZE - strlen(buffer) - 1);
        strncat(buffer, "\nGAME OVER! La partie est terminée. La partie va redémarrer...\n", BUF_SIZE - strlen(buffer) - 1);
        printf("GAME OVER: FIN DE LA PARTIE.\n");
        //buffer[offset] = '\0';
        send_to_all(buffer);
        //snprintf(buffer + strlen(buffer), BUF_SIZE - strlen(buffer), "\nPièce actuelle : %c\n", current_piece.type);
        reset_game();
        
        send_game_state();


    }else{

        // Terminer la chaîne
        buffer[offset] = '\0';


        // Envoyer aux clients
        snprintf(buffer + strlen(buffer), BUF_SIZE - strlen(buffer), "\nPièce actuelle : %c\n", current_piece.type);
        //for (int i = 0; i < MAX_PLAYERS; i++) {
            //if (players[i].sock != 0) {
                //send(players[i].sock, buffer, strlen(buffer), 0);
            //}
        //}
        printf("Buffer envoyé :\n%s\n", buffer);

        send_to_all(buffer);
        //send_to_all(buffer);
    }
    
    
}


void drop_piece() {
    if (can_move(current_piece.y + 1, current_piece.x)) {
        current_piece.y++;
    } else {
        place_piece();
        spawn_piece();
    }
}

int can_rotate(int new_shape[4][4], int y, int x) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (new_shape[i][j]) {
                int grid_y = y + i;
                int grid_x = x + j;

                // Vérifier les limites de la grille
                if (grid_y < 0 || grid_y >= ROWS || grid_x < 0 || grid_x >= COLUMNS) {
                    return 0;
                }

                // Vérifier les collisions
                if (grid[grid_y][grid_x]) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void rotate_piece(Piece *piece) {
    int new_shape[4][4];

    // Transposer la matrice et inverser l'ordre des colonnes
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            new_shape[j][3 - i] = piece->shape[i][j];
        }
    }

    // Vérifier si la rotation est valide
    if (can_rotate(new_shape, piece->y, piece->x)) {
        // Appliquer la rotation
        memcpy(piece->shape, new_shape, sizeof(new_shape));
    }
}


void handle_player_turn(Player *player) {
    char buffer[BUF_SIZE];
    //send_game_state();
    
    snprintf(buffer, BUF_SIZE, "C'est votre tour, %s !\nEntrez une action (left, right, rotate, drop) :\n", player->name);
    send(player->sock, buffer, strlen(buffer), 0);
    //send(player->sock, buffer, strlen(buffer), 0);

    /*for (int i = 0; i < MAX_PLAYERS; i++) {
        if (i != current_player && players[i].sock != 0) {
            snprintf(buffer, BUF_SIZE, "C'est le tour de %s. Veuillez patienter.\n", player->name);
            send(players[i].sock, buffer, strlen(buffer), 0);
        }
    }*/

    int n = recv(player->sock, buffer, BUF_SIZE - 1, 0);
    if (n <= 0) {
        printf("%s s'est déconnecté.\n", player->name);
        close(player->sock);
        player->sock = 0;
        return;
    }

    buffer[n] = '\0';
    printf("Action reçue de %s : %s\n", player->name, buffer);

    if (strcmp(buffer, "left") == 0 && can_move(current_piece.y, current_piece.x - 1)) {
        current_piece.x--;
    } else if (strcmp(buffer, "right") == 0 && can_move(current_piece.y, current_piece.x + 1)) {
        current_piece.x++;
    } else if (strcmp(buffer, "rotate") == 0) {
        // Rotation non implémentée ici
        rotate_piece(&current_piece);
    } else if (strcmp(buffer, "drop") == 0) {
        while (can_move(current_piece.y + 1, current_piece.x)) {
            current_piece.y++;
        }
        place_piece();
        spawn_piece();
    }
}

void run_game() {
    struct timespec ts = {0, 500 * 1000000}; // Délai entre les ticks du jeu (500 ms)

    while (1) {
        nanosleep(&ts, NULL);
        drop_piece();
        send_game_state(); // Envoyer la grille à tous les joueurs

        // Vérifier si c'est le tour du joueur actif
        Player *player = &players[current_player];
        if (player->sock != 0 && player->is_turn) {
            // Envoyer un message au joueur actif pour qu'il joue
            char buffer[BUF_SIZE];
            /*snprintf(buffer, BUF_SIZE, "C'est votre tour, %s !\nEntrez une action (left, right, rotate, drop):\n", player->name);
            send(player->sock, buffer, strlen(buffer), 0);*/

            // Envoyer un message d'attente aux autres joueurs
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (i != current_player && players[i].sock != 0) {
                    snprintf(buffer, BUF_SIZE, "C'est le tour de %s. Veuillez patienter.\n", player->name);
                    send(players[i].sock, buffer, strlen(buffer), 0);
                }
            }

            // Attendre la réponse du joueur
            handle_player_turn(player);

            // Passer au joueur suivant
            player->is_turn = 0;
            current_player = (current_player + 1) % MAX_PLAYERS;
            players[current_player].is_turn = 1;
        }
        
    }
}


int main() {
    srand(time(NULL));

    // Mélange : appeler rand() plusieurs fois avant de l'utiliser
    for (int i = 0; i < 5; i++) {
        rand();
    }
    init_pieces();
    init_grid();
    spawn_piece();

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur de création du socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur lors du bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, MAX_PLAYERS) < 0) {
        perror("Erreur lors de l'écoute");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d...\n", PORT);

    int num_players = 0;
    while (num_players < MAX_PLAYERS) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Erreur lors de l'acceptation");
            continue;
        }

        players[num_players].sock = client_sock;
        snprintf(players[num_players].name, BUF_SIZE, "Joueur %d", num_players + 1);
        players[num_players].score = 0;
        players[num_players].is_turn = (num_players == 0);
        num_players++;

        printf("%s connecté.\n", players[num_players - 1].name);
    }

    printf("Tous les joueurs sont connectés. Début de la partie !\n");



    run_game();
    
    

    close(server_sock);
    return 0;
}