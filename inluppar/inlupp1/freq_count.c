#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

#define BUFFER_SIZE 100

static void process_word(ioopm_hash_table_t *ht, char *word)
{
    elem_t result = ioopm_hash_table_lookup(ht, string_elem(word));

    if (result.i != 0)
    {
        result.i++;
        ioopm_hash_table_insert(ht, string_elem(word), result);
    }
    else
    {
        ioopm_hash_table_insert(ht, string_elem(word), int_elem(1));
    }
}

static void process_file(char *filename, ioopm_hash_table_t *ht)
{
    FILE *f = fopen(filename, "r");

    if (f == NULL)
    {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return;
    }

    char word[BUFFER_SIZE];

    while (fscanf(f, "%99s", word) == 1)
    {
        process_word(ht, word);
    }

    fclose(f);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s file1 ... filen\n", argv[0]);
        return 1;
    }

    ioopm_hash_table_t *ht = ioopm_hash_table_create(
        ioopm_string_hash,
        ioopm_string_eq
    );

    for (int i = 1; i < argc; i++)
    {
        process_file(argv[i], ht);
    }

    ioopm_hash_table_destroy(ht);

    return 0;
}
