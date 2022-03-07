#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 8 // Dimension of the board 8x8
#define P 2 // Size of char for the pieces (O for the player, M for the machine)

typedef struct node {
    char board[N][N]; //-127, 128
    struct node *children[N];
    int n_children;
    double value;
} Node;

int gameFinish(char board[N][N], char piece[P]);

double heuristic(int row, int col, char board[N][N], char mpiece[P], char hpiece[P]){
    int i, j;
    double value = 10000;
    if (gameFinish(board, mpiece) == 1) return value;
    if (gameFinish(board, hpiece) == 1) value -= 5000;
    else {
        for (j=col; j<N-1; j++) {
            if (board[row][j+1] == *hpiece) value -= 100;
            else break;
        }
        for (j=col; j>0; j--) {
            if (board[row][j-1] == *hpiece) value -= 100;
            else break;
        }
        for (i=row; i<N-1; i++) {
            if (board[i+1][col] == *hpiece) value -= 100;
            else break;
        }
    }
    return value;
}
void copyBoard(char board2[N][N], char board[N][N]) {
    for (int i=0; i<N; i++) for (int j=0; j<N; j++) board2[i][j] = board[i][j];
}
// Returns '1' if the column passed is full, '0' if not.
int columnFull(int column, char board[N][N]) {
    if (board[0][column] != ' ') return 1;
    return 0;
}
// Return the number of children of a node
int numberChildrens(char board[N][N]) {
    int num_childrens = 0;
    for (int column=0; column<N; column++) {
        if (board[0][column] == ' ') num_childrens += 1;
    }
    return num_childrens;
}
// Puts the player piece in the correct row depend on the column selected, return the row
int throwPiece(int column, char board[N][N], char piece[P]) {
    int i;
    if (columnFull(column, board) == 1) {
        printf("\nERROR: Selected column is full! \nSelect another column: ");
        return -1;
    }
    for (i = N-1; i>=0; i--) {
        if (board[i][column] == ' ') {
            board[i][column] = *piece;
            break;
        }
    }
    return i;
}
Node *createNode(Node *parent, int numChild, int level) {
    char mpiece[P] = "M"; char hpiece[P] = "O";
    int row;
    Node *p = malloc(sizeof(Node));
    copyBoard(p->board, parent->board); //Copies a board onto another
    if (level<2) {
        throwPiece(numChild, p->board, mpiece);
        p->n_children = numberChildrens(p->board);
    }
    else {
        row = throwPiece(numChild, p->board, hpiece);
        p->value = heuristic(row, numChild, p->board, mpiece, hpiece);
        p->n_children = 0;
    }
    return p;
}
void createChildren(Node *parent, int level) {
    for (int i=0; i<N; i++) {
        if (columnFull(i, parent->board) == 1) parent->children[i] = NULL;
        else parent->children[i] = createNode(parent, i, level);
    }
}
//We consider root node already created and children already set.
void createTree(Node *root) {
    createChildren(root, 1);
    for (int i=0; i<N; i++) if (root->children[i] != NULL) createChildren(root->children[i], 2);
}
double firstValue(Node *n) {
    double value;
    for (int j=0; j<N; j++) if (n->children[j] != NULL) value = n->children[j]->value;
    return value;
}
int MINIMAX(char board[N][N]) {
    double min, max; int i, j, index;
    Node *root = malloc(sizeof(Node));
    copyBoard(root->board, board);
    root->n_children = numberChildrens(root->board);
    createTree(root);
    // min
    for (i=0; i<N; i++) {
        if (root->children[i] != NULL) {
            min = firstValue(root->children[i]);
            for (j=0; j<N; j++) {
                if (root->children[i]->children[j] != NULL) {
                    if (root->children[i]->children[j]->value < min) min = root->children[i]->children[j]->value;
                }
                free(root->children[i]->children[j]);
            }
        root->children[i]->value = min;
        }
    }
    // max
    for (i=0; i<N; i++) if (root->children[i] != NULL) {
            max = root->children[i]->value;
            index = i;
            break;
    }
    for (i=0; i<N; i++) {
        if (root->children[i] != NULL) {
            if (root->children[i]->value > max) {
                max = root->children[i]->value;
                index = i;
            }
        }
        free(root->children[i]);
    }
    free(root);
    return index+1;
}
// Prints in the console the actual state of the board.
void showBoard(char board[N][N]) {
    printf("\n  1   2   3   4   5   6   7   8 \n");
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) printf("| %c ", board[i][j]);
        printf("|\n");
    }
}
// Initialize the board filling it with spaces (' ').
void initBoard(char board[N][N]) {
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) board[i][j] = ' ';
    }
}
// Calculate if some of the players has win
int gameFinish(char board[N][N], char piece[P]) {
    int sum = 0; int i, j, e;
    // Horizontal
    for (i=N-1; i>=0; i--) {
        for (j=0; j<N; j++) {
            if (board[i][j] == *piece) sum += 1;
            else sum = 0;
            if (sum == 4) return 1;
        }
    }
    // Vertical
    sum = 0;
    for (i=0; i<N; i++) {
        for (j=N-1; j>=0; j--) {
            if (board[j][i] == *piece) sum += 1;
            else sum = 0;
            if (sum == 4) return 1;
        }
    }
    // Diagonal
    // First diagonals, from left to right and up to down
    for (e=0; e<N-3; e++) {
        sum = 0;
        for (i=0,j=e; j<N; i++, j++) {
            if (board[i][j] == *piece) sum += 1;
            else sum = 0;
            if (sum == 4) return 1;
        }
    }
    for (e=1; e<N-3; e++) {
        sum = 0;
        for (i=e, j=0; i<N; i++, j++) {
            if (board[i][j] == *piece) sum += 1;
            else sum = 0;
            if (sum == 4) return 1;
        }
    }
    // Second diagonals, from left to right and down to up
    for (e=N-1; e>2; e--) {
        sum = 0;
        for (i=e, j=0; i>=0; i--, j++) {
            if (board[i][j] == *piece) sum += 1;
            else sum = 0;
            if (sum == 4) return 1;
        }
    }
    for (e=1; e<N-3; e++) {
        sum = 0;
        for (i=N-1,j=e; j<N; i--, j++) {
            if (board[i][j] == *piece) sum += 1;
            else sum = 0;
            if (sum == 4) return 1;
        }
    }
    return 0;
}
int playAgain() {
    char again[N];
    printf("\nDo you want to play again? (y/-) ");
    scanf("%7s", again);
    if (*again == 'y') return 1;
    return 0;
}
int fullBoard(char board[N][N]) {
    int numColsFull = 0;
    for (int j=0; j<N; j++) if (board[0][j] != ' ') numColsFull += 1;
    if (numColsFull == N) {
        printf("######### It's a draw!! #########");
        return 1;
    }   return 0;
}
int main() {
    int finish, column, row = 0;
    int player_games_win, machine_games_win = 0;
    char board[N][N];
    char piece_player1[P] = "O"; char piece_player2[P] = "M";

    initBoard(board);
    printf("\n########### CONNECT-4 ###########\n");
    do {// Player 1
        showBoard(board);
        printf("\nPlayer 1 (O) \nSelect a column to throw the piece: ");
        do {
            scanf("%i", &column);
            if (column <1 || column >8) {
                printf("\nERROR: Selected column out of range, should be between 1-8. \nTry again: ");
                row = -1;
            }
            else row = throwPiece(column-1, board, piece_player1);
        } while (row == -1);
        if (gameFinish(board, piece_player1) == 1) {
            printf("\n######### Player wins!! #########");
            player_games_win += 1;
            showBoard(board);
            if (playAgain() == 1) initBoard(board);
            else finish = 1;
        }
        else {
            if (fullBoard(board) == 1) break;
            // Machine
            showBoard(board);
            printf("\n(M) The machine is choosing the column to throw the piece...\n");
            column = MINIMAX(board);
            printf("Column choosed: %i \n", column);
            row = throwPiece(column-1, board, piece_player2);
            if (gameFinish(board, piece_player2) == 1) {
                printf("\n######### Machine wins!! #########");
                machine_games_win += 1;
                showBoard(board);
                if (playAgain() == 1) initBoard(board);
                else finish = 1;
            }
            else if (fullBoard(board) == 1) break;
        }
    } while (finish == 0);

    printf("\n\tCOUNTER OF WINS\n-------------------------------\n    Player: %i\tMachine: %i\n", player_games_win, machine_games_win);
    return 0;
}
