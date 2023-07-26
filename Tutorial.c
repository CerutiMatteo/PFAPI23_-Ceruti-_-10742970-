#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tutorial.h"


int main() {
    Graph *highway = init_highway();

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        char command[20];
        int distance, num_cars;
        int num_read = sscanf(line, "%s %d %d", command, &distance, &num_cars);

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
                printf("Station added successfully\n");
            } else {
                printf("Station already exists\n");
            }
        } else if (strcmp(command, "demolisci-stazione") == 0 && num_read >= 2) {
            // Implement the demolish_station function
            continue;
        } else if (strcmp(command, "aggiungi-auto") == 0 && num_read >= 3) {
            int success = add_car(highway, distance, num_cars);
            if (success) {
                printf("Car added successfully at distance %d with autonomy of %d\n", distance, num_cars);
            } else {
                printf("Failed to add car\n");
            }
        } else if (strcmp(command, "rottama-auto") == 0 && num_read >= 3) {
            // Implement the scrap_car function
            continue;
        } else if (strcmp(command, "pianifica-percorso") == 0) {
            int num_stations = 0;
            int *route = plan_route(highway, distance, num_cars, &num_stations);
            for (int i = 0; i < num_stations; i++) {
                printf("%d ", route[i]);
            }
            printf("\n");

        } else if (strcmp(command, "printa-grafo") == 0){
            print_graph(highway);
        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    free(line);
    return 0;
}
