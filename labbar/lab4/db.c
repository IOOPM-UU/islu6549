#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "../lib/string_utils.h"
#include <ctype.h>

typedef struct item_t {
    char *Name;
    char *Desc;
    int Price;
    char *Shelf;
} item_t;

void print_item(item_t *item);
item_t make_item(char *name, char *desc, int price, char *shelf);
item_t input_item();

void edit_db(item_t *db, int db_size);
void list_db(item_t *db, int db_size);

char *magick(char *array_1[],
             char *array_2[],
             char *array_3[],
             size_t array_length);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    char *array1[] = { "Väldigt", "Lite", "Ingen" };
    char *array2[] = { "bra", "dålig", "medioker" };
    char *array3[] = { "pryl", "uppgift", "anställd" };

    if (argc < 2) {
        printf("Usage: %s number\n", argv[0]);
    } else {
        item_t db[16]; // Array med plats för 16 varor
        int db_size = 0; // Antalet varor i arrayen just nu

        int items = atoi(argv[1]); // Antalet varor som skall skapas

        if (items > 0 && items <= 16) {
            for (int i = 0; i < items; ++i) {
                // Läs in en vara, lägg till den i arrayen, öka storleksräknaren
                item_t item = input_item();
                db[db_size] = item;
                ++db_size;
            }
        } else {
            puts("Sorry, must have [1-16] items in database.");
            return 1; // Avslutar programmet!
        }

        
        for (int i = db_size; i < 16; ++i) {
            char *name = magick(array1, array2, array3, 3); // TODO: Lägg till storlek
            char *desc = magick(array1, array2, array3, 3); // TODO: Lägg till storlek
            int price = rand() % 200000;
            char shelf[] = { rand() % ('Z'-'A') + 'A',
                            rand() % 10 + '0',
                            rand() % 10 + '0',
                            '\0' };
            item_t item = make_item(name, desc, price, strdup(shelf));

            db[db_size] = item;
            ++db_size;
        }
        list_db(db, db_size);

        edit_db(db, db_size);
        list_db(db, db_size);
    }
    return 0;
}

void print_item(item_t *item) {
    printf("Name: %s\n", item->Name);
    printf("Description: %s\n", item->Desc);
    printf("Price: %.2f\n", item->Price / 100);
    printf("Shelf: %s\n", item->Shelf);
}

item_t make_item(char *name, char *desc, int price, char *shelf){
    item_t item;

    item.Name = name;
    item.Desc = desc;
    item.Price = price;
    item.Shelf = shelf;

    return item;
}

item_t input_item() {
    char *name = ask_question_string("name?");
    char *desc = ask_question_string("description?");
    int price = ask_question_int("price?");
    char *shelf = ask_question_shelf("shelf?");

    item_t item = make_item(name, desc, price, shelf);

    return item;
}

void edit_db(item_t *db, int db_size) {
    char *input_edit;
REDO:
    int input_user = ask_question_int("What is the index of the item?") - 1;
    if (input_user >= db_size || input_user < 0) {
        printf("Enter a functioning index value, being between %d and %d ", 0, db_size);
        goto REDO;
    }

    if (db[input_user].Name == NULL) {
        printf("%d is an empty index. do you want to edit index %d?", input_user, input_user);
        input_edit = ask_question_string("Yes or no");
    } else {
        print_item(&db[input_user]);
        input_edit = ask_question_string("You want to change that item? Yes or no.");
    }
    
    if (strcmp(input_edit, "YES") == 0) {
        db[input_user] = input_item();
    }
}

void list_db(item_t *db, int db_size) {
    for(int i = 0; i < db_size; i++) {
        printf("%d: %s\n", (i +1), db[i].Name);
    }
}

char *magick(char *array_1[], char *array_2[], char *array_3[], size_t array_length) {
    char buf[64];
 
    int r1 = rand() % array_length;
    int r2 = rand() % array_length;
    int r3 = rand() % array_length;

    snprintf(buf, sizeof(buf), "%s %s %s", array_1[r1], array_2[r2], array_3[r3]);
    
    return strdup(buf);
}

