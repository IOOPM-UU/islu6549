#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "../lib/string_utils.h"
#include <ctype.h>

typedef struct {
    char *name;
    char *desc;
    float price;
    char *shelf;
} item_t;

void event_loop(item_t *database, int db_size);
item_t make_item(char *name, char *desc, float price, char *shelf);
void print_item(item_t *item);
void list_db(item_t *items, int amount);
void print_menu();
char ask_question_menu(char *question);
void add_item_to_db(item_t *database, int db_size);
void remove_item_from_db(item_t *database, int db_size);
bool corr_men_form(char *input);
item_t input_item();
char *magick(char *array_1[], char *array_2[], char *array_3[], size_t array_length);
void edit_db(item_t *db, int db_size);



int main(void) {
    srand(time(NULL));
    //Medieval 3, Greatest game to be!
    
    item_t db[16] = {0};
    int db_size = 16;

    event_loop(db, db_size);
    return 0;
}

void event_loop(item_t *database, int db_size) {
    bool run = true;
    while(run) {
        char menu = ask_question_menu("[L]ägga till en vara\n"
                                  "[T]a bort en vara\n"
                                  "[R]edigera en vara\n"
                                  "Ån[g]ra senaste ändringen\n"
                                  "Lista [h]ela varukatalogen\n"
                                  "[A]vsluta\n");
        switch (menu)
        {
        case 'L':
            add_item_to_db(database, db_size);
            break;
        
        case 'T':
            remove_item_from_db(database, db_size);
            break;
        
        case 'R':
            edit_db(database, db_size);
            break;
        
        case 'G':
            printf("remove or add instead dumbass\n");
            break;
        
        case 'H':
            list_db(database, db_size);
            break;
        
        case 'A':
            run = false;
            break;
            
        default:
            printf("no\n");
        }
    }
}

void add_item_to_db(item_t *database, int db_size) {
    item_t item = input_item();
    for (int i = 0; i < db_size; i++) {
        if(database[i].name == NULL) {
            database[i] = item;
            return;
        }
    }
    printf("no more space for you!\n");
}

void remove_item_from_db(item_t *database, int db_size) {
    list_db(database, db_size);
REDO_DIS:
    char *user_input = ask_question_string("item name what is?");
    for (int i = 0; i < db_size; i++) {
        if(database[i].name != NULL &&
           strcmp(database[i].name, user_input) == 0) {
            database[i].name = NULL;
            return;
        }
    } 
    printf("cant find shit!\n\n");
    goto REDO_DIS;
}

void print_menu() {
    printf("[L]ägga till en vara\n"
            "[T]a bort en vara\n"
            "[R]edigera en vara\n"
            "Ån[g]ra senaste ändringen\n"
            "Lista [h]ela varukatalogen\n"
            "[A]vsluta\n");
}

char ask_question_menu(char *question) {
    answer_t answer = ask_question(question, corr_men_form, string_to_answer_t);
    return toupper(*answer.string_value);
}

bool corr_men_form(char *input) {
    if (strlen(input) != 1)
    {
        return false;
    }
    if(strchr("LlTtRrGgHhAa", *input) != NULL) {
        return true;
    } else {
        return false;
    }
}


item_t make_item(char *name, char *desc, float price, char *shelf) {
    item_t item = { .name = name, 
                    .desc = desc, 
                    .price = price, 
                    .shelf = shelf };

    return item;
}

void print_item(item_t *item) {
    printf("Name: %s\n", item->name);
    printf("Desc: %s\n", item->desc);
    printf("Price: %.2f\n", item->price);
    printf("Shelf: %s\n", item->shelf);
}

void list_db(item_t *items, int amount) {
    for (int i = 0; i < amount; i++) {
        printf("%d: %s\n", i + 1, items[i].name);
    }
}

item_t input_item() {
    char *name = ask_question_string("name?");
    char *desc = ask_question_string("description?");
    float price = ask_question_float("price?");
    char *shelf = ask_question_shelf("shelf?");   
         
    item_t item = make_item(name, desc, price, shelf);

    return item;
}

char *magick(char *array_1[], char *array_2[], char *array_3[], size_t array_length) {
    char buf[64];
 
    int r1 = rand() % array_length;
    int r2 = rand() % array_length;
    int r3 = rand() % array_length;

    snprintf(buf, sizeof(buf), "%s-%s, %s", array_1[r1], array_2[r2], array_3[r3]);
    
    return strdup(buf);
}

void edit_db(item_t *db, int db_size) {
REDO:
    int user_input = ask_question_int("What is the index of the item?") - 1;
    if (user_input >= db_size || user_input < 0) {
        printf("No no no. Too big! Or to small! I don't know. You know.");
        goto REDO;
    }

    print_item(&db[user_input]);
    char *user_input_2 = ask_question_string("You, yeah you, wanna change, that item? Perchance! yes or no.");
    if (strcmp(user_input_2, "yes") == 0) {
        db[user_input] = input_item();
    }
}
