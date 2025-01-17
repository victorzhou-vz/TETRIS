#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <SDL2/SDL.h>
#include <signal.h>

#define BUF_SIZE 1024
#define PORT 30000
#define TILE_SIZE 30
#define ROWS 20
#define COLUMNS 10
#define WINDOW_WIDTH (COLUMNS * TILE_SIZE)
#define WINDOW_HEIGHT (ROWS * TILE_SIZE)

int running = 1;
int sock;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);

// Couleurs des pièces
SDL_Color colors[] = {
    {0, 0, 0, 255},    // Vide (noir)
    {255, 0, 0, 255},  // I (rouge)
    {0, 255, 0, 255},  // O (vert)
    {0, 0, 255, 255},  // T (bleu)
    {255, 255, 0, 255},// L (jaune)
    {255, 0, 255, 255},// J (magenta)
    {0, 255, 255, 255},// S (cyan)
    {255, 165, 0, 255} // Z (orange)
};

// Pointeurs SDL
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

// Fonction pour dessiner la grille
void draw_grid(int grid[ROWS][COLUMNS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            SDL_Color color = colors[grid[i][j]];
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

            SDL_Rect tile = {
                j * TILE_SIZE,
                i * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };
            SDL_RenderFillRect(renderer, &tile);

            // Contours des cases
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderDrawRect(renderer, &tile);
        }
    }
}

// Fonction de gestion du signal SIGINT (Ctrl+C)
void handle_sigint(int sig) {
    running = 0;
}

// Fonction principale du jeu
void play_game() {
    char buffer[BUF_SIZE];
    int grid[ROWS][COLUMNS] = {0};
    SDL_Event event;

    while (running) {
        // Gérer les événements SDL
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                break;
            }
        }

        // Recevoir les données du serveur
        int n = recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (n <= 0) {
            if (!running) break; // Sortir si on reçoit un signal SIGINT
            printf("Connexion au serveur perdue.\n");
            running = 0;
            break;
        }

        buffer[n] = '\0';
        printf("ÉTAT DU TOUR : %s\n", buffer);

        // Identifier le type de message reçu
        int bool_tour = 0;
        if (strstr(buffer, "C'est votre tour") != NULL) {
            bool_tour = 1;
        }
        if (strstr(buffer, "GAME OVER") != NULL) {
            printf("GAME OVER, FIN DE LA PARTIE\n");
            printf("reset de la partie\n");
            // Réinitialiser l'état local de la grille
            memset(grid, 0, sizeof(grid));
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fond noir
            SDL_RenderClear(renderer);
            draw_grid(grid);
            SDL_RenderPresent(renderer);
            continue; // Continuer pour attendre le nouvel état du jeu
        }
        if (strstr(buffer, "Grille actuelle :") != NULL) {
            // Parser la grille envoyée par le serveur
            char *start = strstr(buffer, "Grille actuelle :") + strlen("Grille actuelle :\n");
            char *line = strtok(start, "\n");
            int row = 0;
            while (line != NULL && row < ROWS) {
                for (int col = 0; col < COLUMNS; col++) {
                    switch (line[col]) {
                        case 'I': grid[row][col] = 1; break;
                        case 'O': grid[row][col] = 2; break;
                        case 'T': grid[row][col] = 3; break;
                        case 'L': grid[row][col] = 4; break;
                        case 'J': grid[row][col] = 5; break;
                        case 'S': grid[row][col] = 6; break;
                        case 'Z': grid[row][col] = 7; break;
                        default: grid[row][col] = 0; break;
                    }
                }
                line = strtok(NULL, "\n");
                row++;
            }
            // Redessiner la grille
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fond noir
            SDL_RenderClear(renderer);
            draw_grid(grid);
            SDL_RenderPresent(renderer);
        }
        if (bool_tour == 1) {
            printf("Votre action :\n> ");
            fgets(buffer, BUF_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);
        }
    }
}

int main(int argc, char **argv) {
    // Capturer le signal SIGINT (Ctrl+C)
    signal(SIGINT, handle_sigint);

    // Vérification des arguments
    if (argc != 2) {
        printf("Usage : %s [adresse IP du serveur]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Création du socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur de création du socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Adresse IP invalide");
        exit(EXIT_FAILURE);
    }

    // Envoyer un message de connexion au serveur
    const char *connection_message = "Connexion client";
    sendto(sock, connection_message, strlen(connection_message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Connecté au serveur.\n");

    // Initialisation SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Création de la fenêtre
    window = SDL_CreateWindow("Tetris Client",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Création du renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Lancer le jeu
    play_game();

    // Nettoyage SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    close(sock);
    return 0;
}
