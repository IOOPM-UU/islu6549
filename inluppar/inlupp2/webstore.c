#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "UI_Func.h"
#include "string_utils.h"
#include "webstore.h"

static bool correct_type(char *input) {
    if (strlen(input) != 1)
    {
        return false;
    }
    if(strchr("AaLlDdEeSsPpCcRr+-=OoUuQq", *input) != NULL) {
        return true;
    } else {
        return false;
    }
}  

char ask_question_menu(char *question) {
    answer_t answer = ask_question(question, correct_type, string_to_answer_t);
    char choice = toupper((unsigned char)answer.string_value[0]);
    free(answer.string_value);
    return choice;
}

void event_loop(ioopm_hash_table_t *ht) {

    while(true) {
        char menu = ask_question_menu(
                                    "(A) Add merchandise\n"
                                    "(L) List merchandise\n"
                                    "(D) Remove merhcandise\n"
                                    "(E) Edit merchandise\n"
                                    "(S) Show stock\n"
                                    "(P) Replenish\n"
                                    "(C) Create cart\n"
                                    "(R) Remove cart\n"
                                    "(+) Add to cart\n"
                                    "(-) Remove from cart\n"
                                    "(=) Calculate cost\n"
                                    "(O) Checkout\n"
                                    "(U) Undo\n"
                                    "(Q) Quit\n");
        switch (menu)
        {
        case 'A': {
            // add merch function
            char *name = ask_question_line("Name: ");
            char *description = ask_question_line("Description: ");
            int price = ask_question_int("Price: ");

            ioopm_merch_add(ht, name, description, price, NULL);
            break;
        }
        
        case 'L': {
            // list merch function
            merch_display(ht);
            break;
        }
        
        case 'D': {
            // Remove merchandise
            char *name = ask_question_line("Name of merchandise to remove: ");
            merch_remove(ht, name);
            free(name);
            break;
        }
        
        case 'E': {
            // Edit merchandise
            char *old_name = ask_question_line("Name of merchandise to edit: ");
            char *new_name = ask_question_line("New name: ");
            char *new_description = ask_question_line("New description: ");
            int new_price = ask_question_int("New price: ");

            merch_edit(ht, old_name, new_name, new_description, new_price);

            free(old_name);
            free(new_name);
            free(new_description);
            break;
        }
        
        case 'S': {
            // Show stock
            char *name = ask_question_line("Name of merchandise:");
            ioopm_stock_show(ht, name);
            free(name);
            break;
        }
        
        case 'P': {
            // Replenish
            char *name = ask_question_line("Name of merchandise:");
            char *aisle = ask_question_shelf("Shelf (a letter followed by numbers, e.g. A25):");
            int amount = ask_question_int("Amount to add:");

            ioopm_stock_replenish(ht, name, aisle, amount);

            free(name);
            free(aisle);
            break;
        }
        
        case 'C': {
            // Create cart
            ioopm_cart_create();
            break;
        }
        
        case 'R':
            // Remove
            break;
        
        case '+': {
            // Add to cart
            int cart_id = ask_question_int("Cart id:");
            char *name = ask_question_line("Name of merchandise:");
            int quantity = ask_question_int("Quantity:");

            ioopm_cart_add_item(ht, cart_id, name, quantity);
            free(name);
            break;
        }
        
        case '-': {
            // Remove from cart
            int cart_id = ask_question_int("Cart id:");
            char *name = ask_question_line("Name of merchandise:");
            int quantity = ask_question_int("Quantity to remove:");

            ioopm_cart_remove_item(cart_id, name, quantity);
            free(name);
            break;
        }
        
        case '=':
            // Calculate cost
            break;
        
        case 'O':
            // Checkout
            break;
        
        case 'U':
            // Undo
            break;
        
        case 'Q': {
            // Quit
            ioopm_program_exit(ht);
            break;
        }
            
        default:
            printf("no\n");
        }
    }
}

int main(void) {

    ioopm_hash_table_t ht;
    ioopm_hash_table_init(&ht);

    event_loop(&ht);

    return 0;
}