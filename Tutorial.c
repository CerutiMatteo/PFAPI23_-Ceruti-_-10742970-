#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tutorial.h"


int main() {
    Graph *highway = init_highway();

    char *line = NULL;
    size_t len = 0;
    
    //CICLO
    while (getline(&line, &len, stdin) != -1) {
        char command[20];
        int distance, num_cars;
        int num_read = sscanf(line, "%s %d %d", command, &distance, &num_cars);

        //AGGIUNGI STAZIONE
        if (strcmp(command, "aggiungi-stazione") == 0 && num_read >= 3) {
            char *autonomies = strchr(line, '\n');
            if (autonomies != NULL) {
                *autonomies = '\0';  // Replace the newline character with a null terminator
            }
            autonomies = strchr(line, ' ') + 1;
            autonomies = strchr(autonomies, ' ') + 1;  // Skip past the distance and number of cars

            int success = add_station(highway, distance);
            if (success) {
                char *car = strtok(autonomies, " ");
                while (car != NULL) {
                    int autonomy = atoi(car);
                    add_car(highway, distance, autonomy);
                    car = strtok(NULL, " ");
                }
                printf("aggiunta\n");//printf("Station added successfully\n");
            } else {
                printf("non aggiunta\n");//printf("Station already exists\n");
            }
        }

        //DEMOLISCI STAZIONE
        else if (strcmp(command, "demolisci-stazione") == 0 && num_read >= 2) {
            if(delete_station(highway, distance)){
                printf("demolita\n");
            }else{
                printf("non demolita\n");
            }
            continue;
        } 
        
        //AGGIUNGI AUTO 
        else if (strcmp(command, "aggiungi-auto") == 0 && num_read >= 3) {
            int success = add_car(highway, distance, num_cars);
            if (success) {
                printf("aggiunta\n");//printf("Car added successfully at distance %d with autonomy of %d\n", distance, num_cars);
            } else {
                printf("non aggiunta\n");//printf("Failed to add car\n");
            }
            continue;
        } 

        //ROTTAMA AUTO
        else if (strcmp(command, "rottama-auto") == 0 && num_read >= 3) {
            if(delete_car(highway, distance, num_cars)){
                printf("rottamata\n");
            }
            else{
                printf("non rottamata\n");
            }
            continue;
        } 
        
        //PIANIFICA PERCORSO
        else if (strcmp(command, "pianifica-percorso") == 0) {
            int num_stations = 0;
            int *route = plan_route(highway, distance, num_cars, &num_stations);
            if(route== NULL){
                printf("nessun percorso\n");
            }else{
                for (int i = 0; i <= num_stations; i++) {
                    if((route[i]>= distance && route[i]<= num_cars) || (route[i]<= distance && route[i]>= num_cars)){
                        printf("%d ", route[i]);
                    }
            }
            printf("\n");
            }
        }

        //PRINTA GRAFO
        else if (strcmp(command, "printa-grafo") == 0){
            print_graph(highway);
        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    free(line);
    return 0;
}
