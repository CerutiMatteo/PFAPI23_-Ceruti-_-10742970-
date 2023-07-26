#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CARS 512


typedef struct Station {
    int distance;
    int cars[MAX_CARS];
    int num_cars;
    int num_edges;
    int *edges;
} Station;

typedef struct Graph {
    int numStations;
    Station *stations;
} Graph;

Graph *init_highway() {
    Graph *highway = malloc(sizeof(Graph));
    highway->numStations = 1;
    highway->stations = malloc(sizeof(Station));
    highway->stations[0].distance = 0;
    highway->stations[0].num_cars = 0;
    highway->stations[0].num_edges = 0;
    return highway;
}

int add_station(Graph *highway, int distance) {
    int i;
    for (i = 0; i < highway->numStations; i++) {
        if (highway->stations[i].distance == distance) {
            return 0;
        }
    }
    highway->numStations++;
    highway->stations = realloc(highway->stations, highway->numStations * sizeof(Station));// Riallocare l'array delle stazioni in base al nuovo numero di stazioni
    highway->stations[highway->numStations - 1].distance = distance;
    highway->stations[highway->numStations - 1].num_cars = 0;
    highway->stations[highway->numStations - 1].num_edges = 0;

    //Update of edges:
    for(int i=0; i<highway->numStations; i++){
        if(highway->stations[i].distance-get_max_car(&highway->stations[i]) < distance &&
        highway->stations[i].distance+get_max_car(&highway->stations[i]) > distance){
            highway->stations[i].num_edges++;
            highway->stations[i].edges[highway->stations[i].num_edges-1]= distance;
        }
    }

    return 1;
}

void delete_station(Graph *highway, int distance) {
    for (int i = 0; i < highway->numStations; i++) {
        if (highway->stations[i].distance == distance) {
            highway->stations[i] = highway->stations[highway->numStations - 1];//spostiamo l'ultima stazione nell'array sopra di questa stazione
            highway->numStations--;
            highway->stations = realloc(highway->stations, highway->numStations * sizeof(Station));// siallocare l'array delle stazioni in base al nuovo numero di stazioni
            printf("demolita\n");
            //Update of edges
            for(int i=0; i< highway->numStations; i++){
                if(highway->stations[i].distance-get_max_car(&highway->stations[i]) <= distance 
                && highway->stations[i].distance+get_max_car(&highway->stations[i]) >= distance){
                    highway->stations[i].num_edges--;
                    for(int j; j<highway->stations[i].num_edges+1; j++){
                        if(highway->stations[i].edges[j] == distance){
                            highway->stations[i].edges[j]= highway->stations[i].edges[highway->stations[i].num_edges];
                        }
                    }
                }
            }
            return;
        }
    }
    printf("non demolita\n");
}

void destroy_car(Graph *highway, int distance, int autonomy){
    int i=0;
    if(highway->numStations==0){
        printf("non rottamata\n");
        return;
    }
    for (i = 0; i < highway->numStations; i++){
        if (highway->stations[i].distance == distance){
            for (int j = 0; j < highway->stations[i].num_cars; j++){
                if(highway->stations[i].cars[j]==autonomy){
                    highway->stations[i].cars[j]= highway->stations[i].cars[highway->stations[i].num_cars-1];//spostiamo ultima autonomia al posto di quella da eliminare
                    highway->stations[i].cars[highway->stations[i].num_cars-1]=0;//setto a 0 l'ultima autonomia (rimuovo la macchina)
                    highway->stations[i].num_cars--;// decremento il numero di macchine nella stazione
                    printf("rottamata\n");
                    if(autonomy>get_max_car(&highway->stations[i])){//se rottamo l'auto con autonomia massima:
                        int max_car=get_max_car(&highway->stations[i]);
                        int start = lower_bound(highway->stations, highway->numStations, distance - max_car);
                        int end = upper_bound(highway->stations, highway->numStations, distance + max_car);
                        int cont_edges=0;
                        for (int j = start; j < end; j++) {
                            if (edge_exists(&highway->stations[i], j)) {
                                cont_edges++;
                                highway->stations[i].edges[cont_edges-1] = j;
                            }
                        }
                        highway->stations[i].num_edges=cont_edges;
                        highway->stations[i].edges = realloc(highway->stations[i].edges, highway->stations[i].num_edges * sizeof(int));
                    }
                    return;
                }
            }
            
        }
    }
    printf("non rottamata\n");
}

void plan_route(Graph *highway, int distance_start, int distance_end){
    if (distance_start==distance_end){
        printf("%d\n",distance_start);
        return;
    }
    //...
}

int get_max_car (Station *station) {
    int max = 0;
    int i;
    for (i = 0; i < station->num_cars; i++) {
        if (station->cars[i] > max) {
            max = station->cars[i];
        }
    }
    return max;
}

int edge_exists(Station *station, int target) {
    for (int i = 0; i < station->num_edges; i++) {
        if (station->edges[i] == target) {
            return 1;
        }
    }
    return 0;
}

int lower_bound(Station *stations, int numStations, int value) {
    int left = 0;
    int right = numStations;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (stations[mid].distance < value) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

int upper_bound(Station *stations, int numStations, int value) {
    int left = 0;
    int right = numStations;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (stations[mid].distance <= value) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

int add_car(Graph *highway, int distance, int car) {//aggiunge una macchina e nel caso sistema gli archi 
    int i;
    for (i = 0; i < highway->numStations; i++) {
        if (highway->stations[i].distance == distance) {
            if (highway->stations[i].num_cars == MAX_CARS) {
                return 0;
            }
            highway->stations[i].cars[highway->stations[i].num_cars] = car;
            highway->stations[i].num_cars++;

            // create an edge between the current station and all the other stations reachable from the current station
            int j;
            int max_car = get_max_car(&highway->stations[i]);
            int start = lower_bound(highway->stations, highway->numStations, distance - max_car);
            int end = upper_bound(highway->stations, highway->numStations, distance + max_car);
            for (j = start; j < end; j++) {
                if (!edge_exists(&highway->stations[i], j)) {
                    highway->stations[i].num_edges++;
                    highway->stations[i].edges = realloc(highway->stations[i].edges, highway->stations[i].num_edges * sizeof(int));
                    highway->stations[i].edges[highway->stations[i].num_edges - 1] = j;
                }
            }

            return 1;
        }
    }
    return 0;
}

int main() {
    Graph *highway = init_highway();

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        char command[20];
        int distance, num_cars;
        int num_read = sscanf(line, "%s %d %d", command, &distance, &num_cars); //??

        //AGGIUNGI STAZIONE
        if (strcmp(command, "aggiungi-stazione") == 0 && num_read >= 3) {
            char *autonomies = strchr(line, '\n');
            if (autonomies != NULL) {
                *autonomies = '\0';  // Replace the newline character with a null terminator
            }
            autonomies = strchr(line, ' ') + 1; //autonomies punta al primo indirizzo dopo lo spazio 
            autonomies = strchr(autonomies, ' ') + 1;  // Skip past the distance and number of cars; punta al primo indirizzo dopo il secondo spazio(numero di autonomie)

            int success = add_station(highway, distance);
            if (success) {
                char *car = strtok(autonomies, " ");//line suddivisa dagli spazi, conta anche il numero di macchine?
                while (car != NULL) {
                    int autonomy = atoi(car);
                    add_car(highway, distance, autonomy);
                    car = strtok(NULL, " ");//passando NULL a strtok riparte da dove si era interrotto
                }
                printf("aggiunta\n");
            } else {
                printf("non aggiunta\n");
            }

        //DEMOLISCI STAZIONE
        } else if (strcmp(command, "demolisci-stazione") == 0 && num_read >= 2) {
            delete_station(highway, distance);
            continue;

        //AGGIUNGI AUTO 
        } else if (strcmp(command, "aggiungi-auto") == 0 && num_read >= 3) {
            int car;
            sscanf(strchr(line, ' ') + 1, "%d", &car);
            int success = add_car(highway, distance, car);
            if (success) {
                printf("Car added successfully\n");
            } else {
                printf("Failed to add car\n");
            }

        //ROTTAMA AUTO 
        } else if (strcmp(command, "rottama-auto") == 0 && num_read >= 3) {
            int autonomy= num_cars;
            destroy_car(highway, distance, num_cars);
            continue;

        //PIANIFICA PERCORSO
        } else if (strcmp(command, "pianifica-percorso") == 0 && num_read >= 2) {
            int distance_start=distance, distance_end=num_cars;
            plan_route(highway, distance_start, distance_end);
            continue;

        //UNKNOWN COMMAND
        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    free(line);
    return 0;
}