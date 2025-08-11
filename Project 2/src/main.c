// solo_test_solver.c
// Complete, Pure C Implementation of Peg Solitaire Solver (7x7 Board)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_NODES 33
#define MAX_STACK 100000

typedef struct Node {
    char is_filled;
    struct Node* left;
    struct Node* right;
    struct Node* up;
    struct Node* down;
} Node;

typedef struct Board {
    Node* all_nodes[TOTAL_NODES];
} Board;

typedef struct Stack {
    Board* items[MAX_STACK];
    int top;
} Stack;

void push(Stack* s, Board* b) {
    if (s->top < MAX_STACK) {
        s->items[s->top++] = b;
    }
}

Board* pop(Stack* s) {
    return (s->top > 0) ? s->items[--s->top] : NULL;
}

int is_empty(Stack* s) {
    return s->top == 0;
}

int count_filled(Board* b) {
    int count = 0;
    for (int i = 0; i < TOTAL_NODES; i++) {
        if (b->all_nodes[i]->is_filled == 'F') count++;
    }
    return count;
}

int try_move(Board* board, Node* from, Node* over, Node* to) {
    if (from && over && to &&
        from->is_filled == 'F' &&
        over->is_filled == 'F' &&
        to->is_filled == 'E') {

        from->is_filled = 'E';
        over->is_filled = 'E';
        to->is_filled = 'F';
        return 1;
    }
    return 0;
}

Board* copy_board(Board* original) {
    Board* new_board = (Board*) malloc(sizeof(Board));
    Node* node_map[TOTAL_NODES];

    for (int i = 0; i < TOTAL_NODES; i++) {
        node_map[i] = (Node*) malloc(sizeof(Node));
        *node_map[i] = *original->all_nodes[i];
    }

    for (int i = 0; i < TOTAL_NODES; i++) {
        Node* orig = original->all_nodes[i];
        Node* copy = node_map[i];

        for (int j = 0; j < TOTAL_NODES; j++) {
            if (orig->left  == original->all_nodes[j]) copy->left  = node_map[j];
            if (orig->right == original->all_nodes[j]) copy->right = node_map[j];
            if (orig->up    == original->all_nodes[j]) copy->up    = node_map[j];
            if (orig->down  == original->all_nodes[j]) copy->down  = node_map[j];
        }

        new_board->all_nodes[i] = copy;
    }

    return new_board;
}

Board* init_board() {
    Board* b = (Board*) malloc(sizeof(Board));
    int index_map[7][7];
    int i, j, idx = 0;

    for (i = 0; i < 7; i++)
        for (j = 0; j < 7; j++)
            index_map[i][j] = -1;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            if ((i >= 2 && i <= 4) || (j >= 2 && j <= 4)) {
                Node* node = (Node*) malloc(sizeof(Node));
                node->is_filled = 'F';
                node->left = node->right = node->up = node->down = NULL;
                b->all_nodes[idx] = node;
                index_map[i][j] = idx++;
            }
        }
    }

    b->all_nodes[16]->is_filled = 'E'; 

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            int id = index_map[i][j];
            if (id == -1) continue;

            Node* n = b->all_nodes[id];
            if (i > 0 && index_map[i-1][j] != -1) n->up = b->all_nodes[index_map[i-1][j]];
            if (i < 6 && index_map[i+1][j] != -1) n->down = b->all_nodes[index_map[i+1][j]];
            if (j > 0 && index_map[i][j-1] != -1) n->left = b->all_nodes[index_map[i][j-1]];
            if (j < 6 && index_map[i][j+1] != -1) n->right = b->all_nodes[index_map[i][j+1]];
        }
    }

    return b;
}

void print_board(Board* b) {
    int i, j, index = 0;
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            if ((i >= 2 && i <= 4) || (j >= 2 && j <= 4)) {
                printf(" %c ", b->all_nodes[index++]->is_filled);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

int get_neighbor_index(int current, int delta_row, int delta_col) {
    int map[7][7], i, j, idx = 0;
    for (i = 0; i < 7; i++)
        for (j = 0; j < 7; j++)
            map[i][j] = -1;

    for (i = 0; i < 7; i++)
        for (j = 0; j < 7; j++)
            if ((i >= 2 && i <= 4) || (j >= 2 && j <= 4))
                map[i][j] = idx++;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            if (map[i][j] == current) {
                int ni = i + delta_row;
                int nj = j + delta_col;
                if (ni >= 0 && ni < 7 && nj >= 0 && nj < 7)
                    return map[ni][nj];
            }
        }
    }
    return -1;
}

void free_board(Board* b) {
    for (int i = 0; i < TOTAL_NODES; i++) {
        free(b->all_nodes[i]);
    }
    free(b);
}

int solve(Board* board) {
    Stack stack;
    stack.top = 0;

    int best_score = TOTAL_NODES;        
    int remaining = count_filled(board);
    int iteration = 0;
    int target_pin = 1;                 

    push(&stack, copy_board(board));

    
    int dr[4] = {-1, +1,  0,  0}; 
    int dc[4] = {0,  0, -1,  1};  

    while (!is_empty(&stack)) {
        Board* current = pop(&stack);
        remaining = count_filled(current);
        iteration++;
        printf("Iteration %i: There are %i pins remaining.\n", iteration, remaining);

        
        if (remaining <= target_pin) {
            print_board(current);
            free_board(current);
            break;
        }

       
    for (int i = 0; i < TOTAL_NODES; i++) {
        if (current->all_nodes[i]->is_filled != 'F')
            continue;

        for (int d = 0; d < 4; d++) {
            int over = get_neighbor_index(i, dr[d], dc[d]);
            int to   = get_neighbor_index(i, 2 * dr[d], 2 * dc[d]);

            if (over != -1 && to != -1) {
                Board* child = copy_board(current);
                if (try_move(child,
                            child->all_nodes[i],
                            child->all_nodes[over],
                            child->all_nodes[to])) {
                    push(&stack, child);
                } else {
                    free_board(child);
                }
            }
        }
    }


      
        free_board(current);
    }

   
    while (!is_empty(&stack)) {
        free_board(pop(&stack));
    }

    return iteration;
}

void print_board_indices() {
    int i, j, index = 0;
    printf("\nBoard Indexes:\n");
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            if ((i >= 2 && i <= 4) || (j >= 2 && j <= 4)) {
                printf("%2d ", index++);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
    printf("\n");
}


int main() {
    Board* board = init_board();

    printf("=== SOLO TEST ===\n");
    printf("Select a game mode:\n");
    printf("1 - Play! \n");
    printf("2 - Solve the problem\n");
    printf("Selection (1/2): ");

    int choice;
    scanf("%d", &choice);

    if (choice == 2) {
        printf("\nComputer is trying to solve the game...\n\n");

        int iteration_count = solve(board);
        printf("The computer solved the problem in %d iterations!\n", iteration_count);

        free_board(board);

        return 0;
    }
    else if (choice == 1)
    {
    // Game mode
    printf("\nThe indexes on the board are between 0-32.\n");
    printf("To move: Enter three indexes representing FROM/OVER/TO.\n");
    printf("Enter -1 to exit.\n\n");


    while (1) {
        print_board(board);
        print_board_indices();

        printf("Remaining peg count: %d\n", count_filled(board));

        if (count_filled(board) == 1) {
            printf("Congrats! You are a genius!\n");
            break;
        }

        int from, over, to;
        printf("Enter three indexes FROM/OVER/TO: ");
        scanf("%d", &from);
        if (from == -1) {
            printf("Exiting the game...\n");
            break;
        }
        scanf("%d %d", &over, &to);

        if (from < 0 || from >= TOTAL_NODES || over < 0 || over >= TOTAL_NODES || to < 0 || to >= TOTAL_NODES) {
            printf("Index error!\n");
            continue;
        }

        if (!try_move(board, board->all_nodes[from], board->all_nodes[over], board->all_nodes[to])) {
            printf("Invalid move.\n");
        }
    }
    return 0;
    }
    else
        return -1;


}
