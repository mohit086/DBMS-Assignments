#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pds.h"

// Include the header file that contains the struct declarations for parent and child records
#include "structs.h"

// Declare global variables for 10 parent and 10 child records
struct Shop **shop_list; //array of pointers to Shop structures
struct Product **product_list; //array of pointers to Product structures

void show_menu();
void setup_data();
void process_option(int option);
void process_option1();
void process_option2();

int main(){
    shop_list = malloc(10 * sizeof(struct Shop *)); //allocate memory
    product_list = malloc(10 * sizeof(struct Product *));
    int option;
    setup_data();
    do{
        show_menu();
        scanf("%d", &option);
        process_option(option);
    }
    while (option != 0);
    return 0;
}

void show_menu(){
    printf("\nLINKED DATA DEMO\n\n");
    printf("0. Exit\n");
    printf("1. Add linked data\n");
    printf("2. Get linked data\n");
    printf("\nEnter option: ");
}

void setup_data()
{
    pds_create("shops", "products");
    pds_open("shops", "products", sizeof(struct Shop), sizeof(struct Product));

    int n1 = 1; //parent keys from 1 to 10
    for (int i = 0; i < 10; i++){
        shop_list[i] = (struct Shop *)(malloc(sizeof(struct Shop)));
        char str[10];
        sprintf(str, "%d", n1);
        char name[10] = "shop_";
        strcat(name, str);
        strcpy(shop_list[i]->id,str);
        strcpy(shop_list[i]->name,name);
        put_rec_by_key(n1, shop_list[i]);
        n1++;
    }

    int n2 = 11; //child keys from 11 to 20
    for (int i = 0; i < 10; i++){
        product_list[i] = (struct Product *)(malloc(sizeof(struct Product)));
        char str[10];
        sprintf(str, "%d", n2);
        char name[10] = "product_";
        strcat(name, str);
        strcpy(product_list[i]->id,str);
        strcpy(product_list[i]->name,name);
        put_linked_rec_by_key(n2, product_list[i]);
        n2++;
    }
}

void process_option(int option){
    switch (option)
    {
    case 0:
        pds_close();
        break;
    case 1:
        process_option1();
        break;
    case 2:
        process_option2();
        break;
    default:
        printf("Invalid option\n");
        break;
    }
}

void process_option1(){
    int parent_key, child_key;
    printf("Enter parent key and child key for linking: \n");
    scanf("%d%d", &parent_key, &child_key);

    // Call appropriate PDS functions here to link the two records
    pds_link_rec(parent_key, child_key);
}

void process_option2(){
    int parent_key;
    printf("Enter parent key \n");
    scanf("%d", &parent_key);

    // Call appropriate PDS functions here to fetch array of child keys
    // Call your PDS functions to display the list of child keys retrieved for the given parent key
    // Display all field values of each child record in a nicely formatted manner

    int linked_keys_result[15];
    int result_set_size = 0;
    pds_get_linked_rec(parent_key, linked_keys_result, &result_set_size);

    for (int i = 0; i < result_set_size; i++){
        struct Product *p = malloc(sizeof(struct Product));
        get_linked_rec_by_key(linked_keys_result[i], p);
        printf("\nIndex: %d\n", linked_keys_result[i]);
        printf("ID: %s\n", p->id);
        printf("Name: %s\n", p->name);
    }
}
