#ifndef UNISTD_H
    #include <unistd.h>
#endif

#ifndef PTRHEAD_H
    #include <pthread.h>
#endif

#ifndef TIME_H
    #include <time.h>
#endif

#ifndef GETOPT_H
    #include <getopt.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct dictList{
    char* word;
    struct dictList* next;
}DictList;

int main(){
    FILE *fp;
    char str[1000];

    fp = fopen("./input-handler/passwd.txt", "r");
    if (fp == NULL){
        printf("Could not open file");
        return 0;
    }

    DictList* head = NULL;

    while (fgets(str, 1000, fp) != NULL){

        DictList* node = (struct dictList *)malloc(sizeof(struct dictList));

        node->word = (char *)malloc(sizeof(char)*strlen(str));
        strncpy(node->word, str, strlen(str)-1);
        node->word[strlen(str)] = '\0';
        node->next = NULL;
        
        if( head == NULL ){
            head = node;
        }else{
            DictList* current = head;
            while( current->next ){
                current = current->next;
            }
            current->next = node;
        }
    }
    DictList * head3 = head;
    printf("\n\n");
    while (head3 != NULL){
        printf("%s\n", head3->word);
        head3 = head3->next;
    }
    printf("\n\n");

    DictList *passGuessed = NULL;
    DictList* head2 = head;
    int passwordIndex=4;

    if(passwordIndex == 1){
        head = head->next;

        head2->next = NULL;
    }else{
        //faccio -2 in quanto l'index parte da 1 e poi voglio fermarmi 1 posizione prima
        for (int i = 0; i < passwordIndex-2; i++){
            head2 = head2->next;
        }
        DictList *bridge = head2;
        head2 = head2->next;
        bridge->next = head2->next;
        head2->next = NULL;
        bridge = NULL;
        free(bridge);
    }

    if( passGuessed == NULL ){
            passGuessed = head2;
    }else{
        DictList* current = passGuessed;
        while( current->next ){
            current = current->next;
        }
        current->next = head2;
    }

    head2 = head;
    if(passwordIndex == 1){
        head = head->next;

        head2->next = NULL;
    }else{
        //faccio -2 in quanto l'index parte da 1 e poi voglio fermarmi 1 posizione prima
        for (int i = 0; i < passwordIndex-2; i++){
            head2 = head2->next;
        }
        DictList *bridge = head2;
        head2 = head2->next;
        bridge->next = head2->next;
        head2->next = NULL;
        bridge = NULL;
        free(bridge);
    }

    if( passGuessed == NULL ){
            passGuessed = head2;
    }else{
        DictList* current = passGuessed;
        while( current->next ){
            current = current->next;
        }
        current->next = head2;
    }

    printf("\n\n");
    while (head != NULL){
        printf("%s\n", head->word);
        head = head->next;
    }
    printf("\n\n");
    

    DictList *currentz = passGuessed;
    printf("\n\n");
    while (passGuessed != NULL){
        printf("%s\n", passGuessed->word);
        passGuessed = passGuessed->next;
    }
    printf("\n\n");

    FILE *f = fopen("./input-handler/marcus.txt", "w");
    if (f == NULL)
    {
        printf("Couldn't open file!\n");
        exit(1);
    }

    while (currentz != NULL){
        fprintf(f, "%s\n", currentz->word);
        currentz = currentz->next;
    }

    fclose(f);


    return 0;
}