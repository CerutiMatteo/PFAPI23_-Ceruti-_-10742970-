
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CARS 512
typedef struct Station {
    int index;
    int distance;
    int cars[MAX_CARS];
    int num_cars;
    int num_forward_edges;
    int num_incoming_edges;
    struct Station *forward_edges;
    struct Station *incoming_edges;
} Station;

typedef struct Graph {
    int numStations;
    Station *stations;
} Graph;

typedef struct Queue {
    int *values;
    int size;
    int capacity;
}Queue;

Queue *createQueue();
void enqueue(Queue *queue, int value);
int dequeue(Queue *queue);

Graph *init_highway();
int add_station(Graph *highway, int distance);
void check_and_create_edges(Graph *highway, Station *new_station);
void your_edges(Graph *highway, Station *new_station);
int get_max_car (Station *station);
int edge_exists(Station *station, Station * target);
int lower_bound(Station *stations, int numStations, int value);
int upper_bound(Station *stations, int numStations, int value);
int add_car(Graph *highway, int distance, int autonomy);
int scrap_car(Graph *highway, int distance, int autonomy);
int* plan_route_a(Graph *highway, int start, int end, int* num_stations, int* num_steps);
int* plan_route_b(Graph *highway, int start, int end, int* num_stations, int* num_steps);
int delete_station(Graph *highway, int distance);
int delete_car(Graph *highway, int distance, int autonomy);
void sort_edges(Station *station, int n, int forward);
void printQueue(const Queue* queue);
int index_of_a_distance(Graph *highway, int distance);

// HELPER
int find_station_index(Graph* highway, int distance) {
    for (int i = 0; i < highway->numStations; i++) {
        if (highway->stations[i].distance == distance) {
            return i;
        }
    }
    return -1;  // If no station with the given distance was found
}


void print_graph(Graph* highway) {
    for (int i = 0; i < highway->numStations; i++) {
        Station* station = &highway->stations[i];
        printf("\nStation %d (distance: %d, max car: %d):\n", i, station->distance, get_max_car(station));
        printf("Forward edges: ");
        for (int j = 0; j < station->num_forward_edges; j++) {
            printf(" %d", station->forward_edges[j].distance);
        }
        printf("\n");
        printf("incoming edges: ");
        for (int j = 0; j < station->num_incoming_edges; j++) {
            printf(" %d", station->incoming_edges[j].distance);
        }
        printf("\n");
    }
}


Queue *createQueue() {
    Queue *queue = malloc(sizeof(Queue));
    queue->values = malloc(sizeof(int));
    queue->size = 0;
    queue->capacity = 1;
    return queue;
}

void enqueue(Queue *queue, int value) {
    if (queue->size == queue->capacity) {
        queue->capacity *= 2;
        queue->values = realloc(queue->values, queue->capacity * sizeof(int));
    }
    queue->values[queue->size] = value;
    queue->size++;
}

int dequeue(Queue *queue) {
    int value = queue->values[0];
    int i;
    for (i = 0; i < queue->size - 1; i++) {
        queue->values[i] = queue->values[i + 1];
    }
    queue->size--;
    return value;
}

int isEmpty(Queue *queue) {
    return queue->size == 0;
}


Graph *init_highway() {
    Graph *highway = malloc(sizeof(Graph));
    highway->numStations = 1;
    highway->stations = malloc(sizeof(Station));
    highway->stations[0].distance = 0;
    highway->stations[0].num_cars = 0;
    highway->stations[0].num_forward_edges = 0;
    highway->stations[0].num_incoming_edges = 0;
    return highway;
}

void check_and_create_edges(Graph *highway, Station *new_station) {
    int i;
    int max_car=0;
    for (i = 0; i < highway->numStations; i++) {
    
        //forward edges
        max_car = get_max_car(&highway->stations[i]);    
        if (max_car >= abs(new_station->distance - highway->stations[i].distance) &&
            highway->stations[i].index != new_station->index && !edge_exists(&highway->stations[i], new_station)) {
            if (new_station->distance > highway->stations[i].distance) {
                highway->stations[i].num_forward_edges++;
                highway->stations[i].forward_edges = realloc(highway->stations[i].forward_edges, highway->stations[i].num_forward_edges * sizeof(Station));
                highway->stations[i].forward_edges[highway->stations[i].num_forward_edges - 1] = *new_station;
            }
        }
        max_car = get_max_car(new_station);
        if(max_car >= abs(new_station->distance - highway->stations[i].distance) && new_station->distance > highway->stations[i].distance &&
            highway->stations[i].index != new_station->index && !edge_exists(&highway->stations[i], new_station)){
            highway->stations[i].num_incoming_edges++;
            highway->stations[i].incoming_edges = realloc(highway->stations[i].incoming_edges, highway->stations[i].num_incoming_edges * sizeof(Station));
            highway->stations[i].incoming_edges[highway->stations[i].num_incoming_edges - 1] = *new_station;
            
        }

    }
    
    for (i = 0; i < highway->numStations; i++) {
        max_car = get_max_car(new_station);
        if (max_car>= abs(new_station->distance - highway->stations[i].distance) &&
            new_station->index != highway->stations[i].index && !edge_exists(new_station, &highway->stations[i])) {
            if (new_station->distance < highway->stations[i].distance) {
                // Aggiungi arco forward a new_station
                new_station->num_forward_edges++;
                new_station->forward_edges = realloc(new_station->forward_edges, new_station->num_forward_edges * sizeof(Station));
                new_station->forward_edges[new_station->num_forward_edges - 1] = highway->stations[i];
            }
        }
        max_car = get_max_car(&highway->stations[i]);
        if (max_car >= abs(new_station->distance - highway->stations[i].distance) && new_station->distance > highway->stations[i].distance &&
            highway->stations[i].index != new_station->index && !edge_exists(new_station, &highway->stations[i])){
                new_station->num_incoming_edges++;
                new_station->incoming_edges = realloc(new_station->incoming_edges, new_station->num_incoming_edges * sizeof(Station));
                new_station->incoming_edges[new_station->num_incoming_edges - 1] = highway->stations[i];
            }
    
    }
}

int add_station(Graph *highway, int distance) {
    int i;
    for (i = 0; i < highway->numStations; i++) {
        if (highway->stations[i].distance == distance) {
            return 0;  // Station already exists
        }
        if (highway->stations[i].distance > distance) {
            break;  // Found the position where to insert the new station
        }
    }

    // Increase the number of stations and reallocate memory
    highway->numStations++;
    highway->stations = realloc(highway->stations, highway->numStations * sizeof(Station));

    // Shift all stations from i onwards one position to the right
    for (int j = highway->numStations - 1; j > i; j--) {
        highway->stations[j] = highway->stations[j - 1];
        highway->stations[j].index = j;  // Update index
    }

    // Insert the new station at position i
    highway->stations[i].index = i;
    highway->stations[i].distance = distance;
    highway->stations[i].num_cars = 0;
    highway->stations[i].num_forward_edges = 0;
    highway->stations[i].num_incoming_edges = 0;
    highway->stations[i].forward_edges = NULL;//malloc(sizeof(Station));
    highway->stations[i].incoming_edges = NULL;//malloc(sizeof(Station));

    // Check and create edges for the new station
    check_and_create_edges(highway, &highway->stations[i]);

    return 1;
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

int edge_exists(Station *station, Station *check_station) {//ATTENZIONE
    for (int i = 0; i < station->num_forward_edges; i++) {
        if (station->forward_edges[i].distance == check_station->distance) {
            return 1;
        }
    }
    for (int i = 0; i < station->num_incoming_edges; i++) {
        if (station->incoming_edges[i].distance == check_station->distance) {
            return 1;
        }
    }
    return 0;
}

void sort_edges(Station *station, int n, int forward){
    int temp=0;
    for(int i=0; i<n-1; i++){
        for(int j=i+1; j<n; j++){
            if(forward){//1
                if(station->forward_edges[i].distance > station->forward_edges[j].distance){
                    temp= station->forward_edges[i].distance;
                    station->forward_edges[i].distance= station->forward_edges[j].distance;
                    station->forward_edges[j].distance= temp;
                }
            }else{//0
                if(station->incoming_edges[i].distance > station->incoming_edges[j].distance){
                    temp= station->incoming_edges[i].distance;
                    station->incoming_edges[i].distance= station->incoming_edges[j].distance;
                    station->incoming_edges[j].distance= temp;
                }
            }
        }
    }
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

int delete_station(Graph *highway, int distance) {
    // Find the station index
    int index = find_station_index(highway, distance);
    if (index == -1) {
        return 0;  // Station does not exist
    }

    // Remove the station from the edges of all other stations
    for (int i = 0; i < highway->numStations; i++) {
        if (i == index) continue;  // Skip the station being deleted

        // Check forward edges
        for (int j = 0; j < highway->stations[i].num_forward_edges; j++) {
            if (highway->stations[i].forward_edges[j].distance == distance) {
                // Shift all edges after the current one to the left
                for (int k = j; k < highway->stations[i].num_forward_edges - 1; k++) {
                    highway->stations[i].forward_edges[k] = highway->stations[i].forward_edges[k + 1];
                }
                highway->stations[i].num_forward_edges--;
                highway->stations[i].forward_edges = realloc(highway->stations[i].forward_edges, highway->stations[i].num_forward_edges * sizeof(Station));
                break;
            }
        }

        // Check incoming edges
        for (int j = 0; j < highway->stations[i].num_incoming_edges; j++) {
            if (highway->stations[i].incoming_edges[j].distance == distance) {
                // Shift all edges after the current one to the left
                for (int k = j; k < highway->stations[i].num_incoming_edges - 1; k++) {
                    highway->stations[i].incoming_edges[k] = highway->stations[i].incoming_edges[k + 1];
                }
                highway->stations[i].num_incoming_edges--;
                highway->stations[i].incoming_edges = realloc(highway->stations[i].incoming_edges, highway->stations[i].num_incoming_edges * sizeof(Station));
                break;
            }
        }
    }

    // Shift all stations after the one to be deleted to the left
    for (int i = index; i < highway->numStations - 1; i++) {
        highway->stations[i] = highway->stations[i + 1];
        highway->stations[i].index = i;  // Update index
    }

    // Decrease the number of stations and reallocate memory
    highway->numStations--;
    highway->stations = realloc(highway->stations, highway->numStations * sizeof(Station));

    return 1;
}

int delete_car(Graph *highway, int distance, int autonomy) {
    // Find the station index
    int index = find_station_index(highway, distance);
    if (index == -1) {
        return 0;  // Station does not exist
    }

    Station *station = &highway->stations[index];

    // Find the car in the station's cars
    int car_index = -1;
    for (int i = 0; i < station->num_cars; i++) {
        if (station->cars[i] == autonomy) {
            car_index = i;
            break;
        }
    }

    if (car_index == -1) {
        return 0;  // Car does not exist
    }

    // Check if the car to be deleted is the max car
    if (autonomy == get_max_car(station)) {
        // Find the second max car
        int second_max_car = 0;
        for (int i = 0; i < station->num_cars; i++) {
            if (station->cars[i] > second_max_car && station->cars[i] != autonomy) {
                second_max_car = station->cars[i];
            }
        }

        // Remove edges that cannot be reached with the second max car
        for (int i = 0; i < station->num_forward_edges; i++) {
            if (abs(station->forward_edges[i].distance - station->distance) > second_max_car) {
                // Shift all edges after the current one to the left
                for (int j = i; j < station->num_forward_edges - 1; j++) {
                    station->forward_edges[j] = station->forward_edges[j + 1];
                }
                station->num_forward_edges--;
                station->forward_edges = realloc(station->forward_edges, station->num_forward_edges * sizeof(Station));
                i--;  // Check the new edge at the current index
            }
        }
        for (int i = 0; i < highway->numStations; i++) {
            for(int j=0; j < highway->stations[i].num_incoming_edges; j++){
                if(highway->stations[i].incoming_edges[j].distance==highway->stations[index].distance && 
                    abs(highway->stations[i].distance-highway->stations[index].distance) >second_max_car){
                        highway->stations[i].incoming_edges[j] = highway->stations[i].incoming_edges[highway->stations[i].num_incoming_edges - 1]; 
                    }
                
            }
            highway->stations[i].num_incoming_edges--;
            highway->stations[i].incoming_edges = realloc(highway->stations[i].incoming_edges, highway->stations[i].num_incoming_edges * sizeof(Station));
        }



    }

    // Shift all cars after the one to be deleted to the left
    for (int i = car_index; i < station->num_cars - 1; i++) {
        station->cars[i] = station->cars[i + 1];
    }

    // Decrease the number of cars
    station->num_cars--;

    return 1;
}


int add_car(Graph *highway, int distance, int car) {
    int i;
    for (i = 0; i < highway->numStations; i++) {
        if (highway->stations[i].distance == distance) {
            if (highway->stations[i].num_cars == MAX_CARS) {
                return 0;//quando la stazione ha 512 macchine 
            }
            highway->stations[i].cars[highway->stations[i].num_cars] = car;
            highway->stations[i].num_cars++;

            int max_car = get_max_car(&highway->stations[i]);

            // If the added car is the new max car, update the edges
            if (car == max_car) {
                // Memorizza la stazione con la nuova macchina
                Station *new_station = &highway->stations[i];

                for (int j = 0; j < highway->numStations; j++) {
                    if (car >= abs(new_station->distance - highway->stations[j].distance) &&
                        new_station->index != highway->stations[j].index && !edge_exists(new_station, &highway->stations[j])) {
                        if (new_station->distance < highway->stations[j].distance) {
                            // Aggiungi arco forward a new_station
                            new_station->num_forward_edges++;
                            new_station->forward_edges = realloc(new_station->forward_edges, new_station->num_forward_edges * sizeof(Station));
                            new_station->forward_edges[new_station->num_forward_edges - 1] = highway->stations[j];
                        }
                        if (new_station->distance > highway->stations[j].distance) {
                            // Aggiungi arco incoming alle stazioni
                            highway->stations[j].num_incoming_edges++;
                            highway->stations[j].incoming_edges = realloc(highway->stations[j].incoming_edges, highway->stations[j].num_incoming_edges * sizeof(Station));
                            highway->stations[j].incoming_edges[highway->stations[j].num_incoming_edges - 1] = *new_station;
                        }
                    }
                }
            }
            return 1;
        }
    }
    return 0;
}


int* plan_route_a(Graph* highway, int start_distance, int end_distance, int *num_stations, int* num_steps) {
    
    for(int i=0; i<highway->numStations; i++){
        sort_edges(&highway->stations[i],highway->stations[i].num_forward_edges,1);
        //printf("%d->%d   ",highway->stations[i].distance,highway->stations[i].index);
    }

    // Convert distances to indices
    int start = find_station_index(highway, start_distance);
    int end = find_station_index(highway, end_distance);

    if (start == -1 || end == -1 || start==end) {
        // One of the stations was not found
        return NULL;
    }
    
    // Initialize visited array and previous node array
    int* visited = (int*)malloc(highway->numStations * sizeof(int));
    int* prev = (int*)malloc(highway->numStations * sizeof(int));
    for (int i = 0; i < highway->numStations; i++) {
        visited[i] = 0;
        prev[i] = -1;
    }

    // Create a queue and enqueue the start node
    Queue* queue = createQueue();
    enqueue(queue, start);
    visited[start] = 1;

    while (!isEmpty(queue)) {
        int current = dequeue(queue);
        //printf("%d->\n",highway->stations[current].distance);
        // Determine the edges to use based on the direction
        Station* edges =highway->stations[current].forward_edges;
        int num_edges =highway->stations[current].num_forward_edges;
        
        // Iterate through all edges from the current station
        for (int i = 0; i < num_edges; i++) {
            int ind= find_station_index(highway, edges[i].distance);//PER RICAVARE CORRETTAMENTE L'INDICE DI UNA STAZIONE LA CUI DISTANZA è NOTA
            if(ind==-1) continue;
            // If the station hasn't been visited yet
            if (visited[ind]==0) {
                //printf("sto visitando %d", edges[i].distance);
                enqueue(queue, ind);
                //printQueue(queue);
                
                visited[ind] = 1;
                prev[ind] = current;
                //printf("PREV: %d\n",highway->stations[current].distance);
                *num_stations = *num_stations + 1; 

                // If we have reached the end station
                if (ind == end) {
                    
                    free(queue);
                    free(visited);

                    // Create a list to store the path
                    int* path = (int*)malloc((*num_stations) * sizeof(int));
                    int current_station = end;
                    int path_index = *num_stations-1;
                
                    *num_steps=0;
                    

                    // Follow the path from the end to the start
                    while (current_station != -1) {
                        *num_steps=*num_steps + 1;
                        path[path_index] = highway->stations[current_station].distance;
                        current_station = prev[current_station];
                        path_index--;
                    }
                    
                    free(prev);
                    return path;  
                }
            }
        }
    }

    free(queue);
    free(visited);
    free(prev);
    return NULL;
}

int* plan_route_b(Graph* highway, int start_distance, int end_distance, int *num_stations, int* num_steps) {
    
    for(int i=0; i<highway->numStations; i++){
        sort_edges(&highway->stations[i],highway->stations[i].num_forward_edges,1);
        //printf("%d->%d   ",highway->stations[i].distance,highway->stations[i].index);
    }

    // Convert distances to indices
    int start = find_station_index(highway, start_distance);
    int end = find_station_index(highway, end_distance);

    if (start == -1 || end == -1 || start==end) {
        // One of the stations was not found
        return NULL;
    }
    
    // Initialize visited array and previous node array
    int* visited = (int*)malloc(highway->numStations * sizeof(int));
    int* prev = (int*)malloc(highway->numStations * sizeof(int));
    for (int i = 0; i < highway->numStations; i++) {
        visited[i] = 0;
        prev[i] = -1;
    }

    // Create a queue and enqueue the start node
    Queue* queue = createQueue();
    enqueue(queue, end);
    visited[end] = 1;

    while (!isEmpty(queue)) {
        int current = dequeue(queue);
        //printf("%d->\n",highway->stations[current].distance);
        // Determine the edges to use based on the direction
        Station* edges =highway->stations[current].incoming_edges;
        int num_edges =highway->stations[current].num_incoming_edges;
        
        // Iterate through all edges from the current station
        for (int i = 0; i < num_edges; i++) {
            int ind= find_station_index(highway, edges[i].distance);//PER RICAVARE CORRETTAMENTE L'INDICE DI UNA STAZIONE LA CUI DISTANZA è NOTA
            if(ind==-1) continue;
            // If the station hasn't been visited yet
            if (visited[ind]==0) {
                //printf("sto visitando %d", edges[i].distance);
                enqueue(queue, ind);
                //printQueue(queue);
                
                visited[ind] = 1;
                prev[ind] = current;
                //printf("PREV: %d\n",highway->stations[current].distance);
                *num_stations = *num_stations + 1; 

                // If we have reached the end station
                if (ind == start) {
                    
                    free(queue);
                    free(visited);

                    // Create a list to store the path
                    int* path = (int*)malloc((*num_stations) * sizeof(int));
                    int current_station = start;
                    int path_index = 0;
                
                    *num_steps=0;
                    

                    // Follow the path from the end to the start
                    while (current_station != *num_stations) {
                        *num_steps=*num_steps + 1;
                        path[path_index] = highway->stations[current_station].distance;
                        current_station = prev[current_station];
                        path_index++;
                    }
                    
                    free(prev);
                    return path;  
                }
            }
        }
    }

    free(queue);
    free(visited);
    free(prev);
    return NULL;
}


void printQueue(const Queue* queue) {
    if (queue == NULL || queue->values == NULL || queue->size == 0) {
        printf("La coda è vuota.\n");
        return;
    }

    printf("Contenuto della coda: ");
    for (int i = 0; i < queue->size; i++) {
        printf("%d ", queue->values[i]);
    }
    printf("\n");
}

int index_of_a_distance(Graph *highway, int distance){
    for(int i=0; i< highway->numStations; i++){
        if(highway->stations[i].distance==distance){
            return i;
        }
    }
    return -1;
}

//SISTEMARE GLI INDICI 
//SISTEMARE IL RIORDINAMENTO DEGLI ARRAY DEGLI ARCHI 



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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
                car = strtok(NULL, " ");
                int a=0;
                while (car != NULL) {
                    if(a){}
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
            int num_stations = 0; int num_steps = 0; int* route;
            if(num_stations < num_steps){
                route = plan_route_a(highway, distance, num_cars, &num_stations, &num_steps);
                if(route== NULL){printf("nessun percorso\n");}
                else{
                    for (int i = num_stations-num_steps; i < num_stations; i++) {
                        printf("%d ", route[i]);
                    }
                }
            }else if(num_stations > num_steps){
                route = plan_route_b(highway, distance, num_cars, &num_stations, &num_steps);
                if(route== NULL){printf("nessun percorso\n");}
                else{
                    for (int i = 0; i < num_steps; i++) {
                        printf("%d ", route[i]);
                    }
                }
            }else{
                printf("nessun percorso\n");
            }
            printf("\n");
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