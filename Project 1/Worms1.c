#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct WormPart
{
    int x, y;
};

struct Node
{
    struct WormPart *data;
    struct Node *next;
    struct Node *previous;
};

struct DoublyList
{
    struct Node *head;
    struct Node *tail;
    int elemcount;
};

void addFront(struct DoublyList *list, struct WormPart *new_element)
{
    struct Node *newnode = (struct Node *)malloc(sizeof(struct Node));
    newnode->data = new_element;
    newnode->next = list->head;
    newnode->previous = NULL;

    if (list->head != NULL)
        list->head->previous = newnode;

    list->head = newnode;
    list->elemcount++;

    if (list->elemcount == 1)
        list->tail = newnode;
}

void addBack(struct DoublyList *list, struct WormPart *new_element)
{
    struct Node *newnode = (struct Node *)malloc(sizeof(struct Node));
    newnode->data = new_element;
    newnode->next = NULL;
    newnode->previous = list->tail;

    if (list->tail != NULL)
        list->tail->next = newnode;

    list->tail = newnode;
    list->elemcount++;

    if (list->elemcount == 1)
        list->head = newnode;
}

void removeList(struct DoublyList *list)
{
    struct Node *current = list->head;
    while (current != NULL)
    {
        struct Node *temp = current;
        current = current->next;
        free(temp->data);
        free(temp);
    }
    free(list);
}

struct DoublyList *create_worm(char *filename)
{
    struct DoublyList *worm = (struct DoublyList *)malloc(sizeof(struct DoublyList));
    worm->head = NULL;
    worm->tail = NULL;
    worm->elemcount = 0;

    FILE *infile = fopen(filename, "r");

    if (infile == NULL)
    {
        perror("File open failed");
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), infile))
    {
        int x, y;
        sscanf(line, "%d %d", &x, &y);

        struct WormPart *new_part = (struct WormPart *)malloc(sizeof(struct WormPart));
        new_part->x = x;
        new_part->y = y;

        addFront(worm, new_part);
    }

    fclose(infile);
    return worm;
}

void split_and_replace(struct DoublyList **wormfield, int *wormcount, int index, struct Node *hit_node)
{
    struct DoublyList *old_worm = wormfield[index];

    struct DoublyList *part1 = (struct DoublyList *)malloc(sizeof(struct DoublyList));
    part1->head = NULL;
    part1->tail = NULL;
    part1->elemcount = 0;

    struct DoublyList *part2 = (struct DoublyList *)malloc(sizeof(struct DoublyList));
    part2->head = NULL;
    part2->tail = NULL;
    part2->elemcount = 0;

    struct Node *iter = old_worm->head;

    while (iter != NULL && iter != hit_node)
    {
        struct WormPart *newpart = (struct WormPart *)malloc(sizeof(struct WormPart));
        newpart->x = iter->data->x;
        newpart->y = iter->data->y;
        addBack(part1, newpart);
        iter = iter->next;
    }

    iter = hit_node->next;
    while (iter != NULL)
    {
        struct WormPart *newpart = (struct WormPart *)malloc(sizeof(struct WormPart));
        newpart->x = iter->data->x;
        newpart->y = iter->data->y;
        addBack(part2, newpart);
        iter = iter->next;
    }

    removeList(old_worm);

    if (part1->elemcount > 0)
    {
        wormfield[index] = part1;
    }
    else
    {
        wormfield[index] = NULL;
        removeList(part1);
    }

    if (part2->elemcount > 0)
    {
        wormfield[*wormcount] = part2;
        (*wormcount)++;
    }
    else
    {
        removeList(part2);
    }
}

void attack_worms(struct DoublyList *wormfield[], int *wormcount, int x, int y)
{
    for (int i = 0; i < *wormcount; i++)
    {
        if (wormfield[i] == NULL)
            continue;

        struct Node *current = wormfield[i]->head;
        while (current != NULL)
        {
            if (current->data->x == x && current->data->y == y)
            {
                printf("\n You hitted Worm %d at (%d,%d) is already hitten and removed!\n\n\n", i, x, y);

                if (current->previous != NULL)
                {
                    struct Node *prev = current->previous;
                    if (prev->previous)
                        prev->previous->next = current;
                    else
                        wormfield[i]->head = current;
                    current->previous = prev->previous;
                    free(prev->data);
                    free(prev);
                    wormfield[i]->elemcount--;
                }

                if (current->next != NULL)
                {
                    struct Node *next = current->next;
                    current->next = next->next;
                    if (next->next)
                        next->next->previous = current;
                    else
                        wormfield[i]->tail = current;
                    free(next->data);
                    free(next);
                    wormfield[i]->elemcount--;
                }

                split_and_replace(wormfield, wormcount, i, current);
                return;
            }
            current = current->next;
        }
    }
    printf("\n You miss No worm founded at (%d,%d)\n\n\n", x, y);
}

int main()
{
    struct DoublyList *wormfield[100];
    int wormcount = 0;

    char *filenames[4] = {"worms/worm1.txt", "worms/worm2.txt", "worms/worm3.txt", "worms/worm4.txt"};
    for (int i = 0; i < 4; i++)
    {
        wormfield[wormcount++] = create_worm(filenames[i]);
    }

    while (1)
    {
        int option;
        printf("------------------------------------\n");
        printf("|1- View the worms\n");
        printf("|2- Attack\n");
        printf("|3- Quit\n");
        printf("------------------------------------\n");
        printf("Please enter an action: ");
        scanf("%d", &option);

        if (option == 1)
        {
            printf("Worm List\n");
            for (int i = 0; i < wormcount; i++)
            {
                if (wormfield[i] == NULL)
                    continue;

                printf("Worm %d ", i);
                struct Node *wormnode_ptr = wormfield[i]->head;
                while (wormnode_ptr != NULL)
                {
                    printf("(%d,%d) ", wormnode_ptr->data->x, wormnode_ptr->data->y);
                    wormnode_ptr = wormnode_ptr->next;
                }
                printf("\n");
            }
        }
        else if (option == 2)
        {
            printf("Enter coordinates to attack x, y:\n");

            int x, y;

            if (scanf("%d, %d", &x, &y) != 2)
            {
                printf("Invalid input! Please enter two integers separated by , and space.\n");
                while (getchar() != '\n')
                    ; 
                continue;
            }

            attack_worms(wormfield, &wormcount, x, y);
        }
        else if (option == 3)
        {
            break;
        }
        else
        {
            printf("Wrong Input!\n");
        }
    }

    return 0;
}
