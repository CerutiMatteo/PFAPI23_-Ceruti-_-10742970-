#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Station {
    int index;
    int distance;
    int* cars;
    int max_car;
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
int add_station(Graph *highway, int distance, int *zero_is_a_station);
void your_edges(Graph *highway, Station *new_station);
int get_max_car (Station *station);
int edge_forward_exists(Station *station, Station * target);
int edge_incoming_exists(Station *station, Station * target);
int lower_bound(Station *stations, int numStations, int value);
int upper_bound(Station *stations, int numStations, int value);
int add_car(Graph *highway, int distance, int* autonomy,int n_cars);
int scrap_car(Graph *highway, int distance, int autonomy);
int* plan_route(Graph *highway, int start, int end, int* num_stations, int* num_steps);
int delete_station(Graph *highway, int distance, int *zero_is_a_station);
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
        printf("Incoming edges: ");
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
    highway->stations[0].index=0;
    highway->stations[0].distance = 0;
    highway->stations[0].num_cars = 0;
    highway->stations[0].max_car=0;
    highway->stations[0].num_forward_edges = 0;
    highway->stations[0].num_incoming_edges = 0;
    highway->stations[0].cars=NULL;
    highway->stations[0].forward_edges = NULL;
    highway->stations[0].incoming_edges = NULL;

    return highway;
}


int add_station(Graph *highway, int distance, int *zero_is_a_station) {
    Station new_station;
    new_station.distance = distance;
    new_station.cars=NULL;
    new_station.num_cars = 0;
    new_station.max_car = 0;
    new_station.num_forward_edges = 0;
    new_station.forward_edges = NULL;
    new_station.num_incoming_edges = 0;
    new_station.incoming_edges = NULL;
    new_station.index=0;

    if(distance!=0){
        int i=0;
        for (i = 0; i < highway->numStations; i++) {
            if(highway->stations[i].distance < distance){//sistemo gli archi delle stazioni prima della nuova
                if(get_max_car(&highway->stations[i])>=abs(highway->stations[i].distance - distance) && !edge_forward_exists(&highway->stations[i], &new_station)){
                    highway->stations[i].num_forward_edges++;
                    highway->stations[i].forward_edges = realloc(highway->stations[i].forward_edges, highway->stations[i].num_forward_edges * sizeof(Station));
                    highway->stations[i].forward_edges[highway->stations[i].num_forward_edges - 1] = new_station;
                }
            }
            if(highway->stations[i].distance == distance){return 0;}//ritorno 0 se già esiste
            if(highway->stations[i].distance > distance ){//memorizzo l'indice della nuova stazione
                highway->numStations++;
                highway->stations=realloc(highway->stations, highway->numStations*sizeof(Station));
                break;
            }
            
        }

        if(i<highway->numStations){//sistemo gli archi delle stazioni dopo quella nuova e faccio slittare
            for (int k = highway->numStations-1; k > i; k--){
            highway->stations[k] = highway->stations[k-1];//gli passo la stazione che precedente e cambio l'indice
            highway->stations[k].index = k;
            if(get_max_car(&highway->stations[k])>=abs(highway->stations[k].distance - distance) && !edge_incoming_exists(&new_station, &highway->stations[k] )){
                    new_station.num_incoming_edges++;
                    new_station.incoming_edges = realloc(new_station.incoming_edges, new_station.num_incoming_edges * sizeof(Station));
                    new_station.incoming_edges[new_station.num_incoming_edges - 1] = highway->stations[k]; 
                }
        }

            new_station.index = i; 

            if(i>0 && i<highway->numStations) {highway->stations[i]= new_station;}

            return 1; 
        }

        if(i==highway->numStations){//se invece sono arrivato in fondo al ciclo for inserisco la nuova stazione in coda
            highway->numStations++;
            highway->stations=realloc(highway->stations, highway->numStations*sizeof(Station));
            new_station.index = i; 
            new_station.distance = distance;
            new_station.num_cars = 0;
            new_station.max_car= 0;
            new_station.num_forward_edges = 0;
            new_station.num_incoming_edges = 0;
            new_station.forward_edges = NULL;
            new_station.incoming_edges = NULL;

            if(i>0 && i<highway->numStations) {highway->stations[i]= new_station;}

            return 1; 
        }
        

    }
    if(distance==0){
        if(*zero_is_a_station==1){return 0;}
        *zero_is_a_station=1;

        for(int i=0; i<highway->numStations; i++){
            if(new_station.max_car > highway->stations[i].distance && !edge_incoming_exists(&highway->stations[0], &highway->stations[i])){
                highway->stations[0].num_forward_edges++;
                highway->stations[0].forward_edges = realloc(highway->stations[0].forward_edges, highway->stations[0].num_forward_edges * sizeof(Station));
                highway->stations[0].forward_edges[highway->stations[0].num_forward_edges - 1] = highway->stations[i]; 
            }
        }

        return 1;
    }
    return 0;
    
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

int edge_forward_exists(Station *station, Station *check_station) {
    
    for (int i = 0; i < station->num_forward_edges; i++) {
    if (station->forward_edges[i].distance == check_station->distance) {
        return 1;
        }
    }
    return 0;
}

int edge_incoming_exists(Station *station, Station *check_station) {
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
            if(forward){
                if(station->forward_edges[i].distance > station->forward_edges[j].distance){
                    temp= station->forward_edges[i].distance;
                    station->forward_edges[i].distance= station->forward_edges[j].distance;
                    station->forward_edges[j].distance= temp;
                }
            }else{
                if(station->incoming_edges[i].distance > station->incoming_edges[j].distance){
                    temp= station->incoming_edges[i].distance;
                    station->incoming_edges[i].distance= station->incoming_edges[j].distance;
                    station->incoming_edges[j].distance= temp;
                }
            }
        }
    }
}


int delete_station(Graph *highway, int distance, int* zero_is_a_station) {
    if(distance==0){
        *zero_is_a_station=0;
        free(highway->stations[0].forward_edges);
        free(highway->stations[0].incoming_edges);
        highway->stations = NULL;
        highway->stations[0].distance = 0;
        highway->stations[0].num_cars = 0;
        highway->stations[0].max_car = 0;
        highway->stations[0].num_forward_edges = 0;
        highway->stations[0].num_incoming_edges = 0;
        return 1;
    }
    // Find the station index
    int index = find_station_index(highway, distance);
    if (index == -1) {
        return 0;  // Station does not exist
    }

    // Remove the station from the edges of all other stations
    for (int i = 0; i < highway->numStations; i++) {
        if (i == index) continue;  // Skip the station being deleted
        if(highway->stations[i].max_car >= abs(highway->stations[i].distance-highway->stations[index].distance)){//controllo solo chi ha l'arco
            // Check forward edges
            if(highway->stations[i].distance < highway->stations[index].distance){
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
        }
    }

    //free edges
    if(highway->stations[index].num_forward_edges>0){free(highway->stations[index].forward_edges);}
    if(highway->stations[index].num_incoming_edges>0){free(highway->stations[index].incoming_edges);}
    if(highway->stations[index].num_cars>0){free(highway->stations[index].cars);}
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
    if (autonomy == station->max_car) {
        // Find the second max car
        station->max_car = 0;
        for (int i = 0; i < station->num_cars; i++) {
            if (station->cars[i] > station->max_car && i!=car_index) {
                station->max_car = station->cars[i];
            }
        }

        // Remove edges that cannot be reached with the second max car
        for (int i = 0; i < station->num_forward_edges; i++) {
            if (abs(station->forward_edges[i].distance - station->distance) > station->max_car) {
                // Shift all edges after the current one to the left
                for (int j = i; j < station->num_forward_edges - 1; j++) {
                    station->forward_edges[j] = station->forward_edges[j + 1];
                }
                station->num_forward_edges--;
                station->forward_edges = realloc(station->forward_edges, station->num_forward_edges * sizeof(Station));
                i--;  // Check the new edge at the current index
            }
        }

        for (int i = 0; i < station->num_incoming_edges; i++) {
            if (abs(station->incoming_edges[i].distance - station->distance) > station->max_car) {
                // Shift all edges after the current one to the left
                for (int j = i; j < station->num_incoming_edges - 1; j++) {
                    station->incoming_edges[j] = station->incoming_edges[j + 1];
                }
                station->num_incoming_edges--;
                station->incoming_edges = realloc(station->incoming_edges, station->num_incoming_edges * sizeof(Station));
                i--;  // Check the new edge at the current index
            }
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


int add_car(Graph *highway, int distance, int* cars, int n_cars) {
    int i=0;
    for (i = 0; i < highway->numStations; i++) {
        if (highway->stations[i].distance == distance) {
            if (highway->stations[i].num_cars == 512) {
                return 0;//quando la stazione ha 512 macchine 
            }
            highway->stations[i].cars=realloc(highway->stations[i].cars, (highway->stations[i].num_cars+n_cars)*sizeof(int));
            for(int j=0; j<n_cars; j++){
                highway->stations[i].num_cars++;
                highway->stations[i].cars[highway->stations[i].num_cars-1] = cars[j];
                if(cars[j] > highway->stations[i].max_car){
                    highway->stations[i].max_car= cars[j];
                    Station *new_station = &highway->stations[i];
                    for (int k = 0; k < highway->numStations; k++) {
                        if (cars[j] >= abs(new_station->distance - highway->stations[k].distance) &&
                            new_station->index != highway->stations[k].index) {
                            if (new_station->distance < highway->stations[k].distance && !edge_forward_exists(new_station, &highway->stations[k])) {
                                // Aggiungi arco forward a new_station
                                new_station->num_forward_edges++;
                                new_station->forward_edges = realloc(new_station->forward_edges, new_station->num_forward_edges * sizeof(Station));
                                new_station->forward_edges[new_station->num_forward_edges - 1] = highway->stations[k];
                            } else if (new_station->distance > highway->stations[k].distance && !edge_incoming_exists(&highway->stations[k],new_station )) {
                                // Aggiungi arco incoming a stazione corrente
                                highway->stations[k].num_incoming_edges++;
                                highway->stations[k].incoming_edges = realloc(highway->stations[k].incoming_edges, highway->stations[k].num_incoming_edges * sizeof(Station));
                                highway->stations[k].incoming_edges[highway->stations[k].num_incoming_edges - 1] = *new_station;
                            }
                        }
                    }
                }
            }
            return 1;
        }
    }
    return 0;
}


int* plan_route(Graph* highway, int start_distance, int end_distance, int *num_stations, int* num_steps) {
    
    for(int i=0; i<highway->numStations; i++){
        sort_edges(&highway->stations[i],highway->stations[i].num_forward_edges,1);
        sort_edges(&highway->stations[i],highway->stations[i].num_incoming_edges,0);
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

    int forward = start_distance <= end_distance ? 1 : 0;
    
    if(start > end){
        int temp= end;
        end = start; 
        start = temp;
    }
    // Create a queue and enqueue the start node
    Queue* queue = createQueue();
    enqueue(queue, start);
    
    visited[start] = 1;

    while (!isEmpty(queue)) {
        int current = dequeue(queue);
        //printf("%d->\n",highway->stations[current].distance);
        // Determine the edges to use based on the direction
        Station* edges = forward ? highway->stations[current].forward_edges : highway->stations[current].incoming_edges;
        int num_edges = forward ? highway->stations[current].num_forward_edges : highway->stations[current].num_incoming_edges;
        
        // Iterate through all edges from the current station
        for (int i = 0; i < num_edges; i++) {
            int ind= find_station_index(highway, edges[i].distance);
            
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
                    
                    free(queue->values);
                    free(queue);
                    free(visited);

                    // Create a list to store the path
                    *num_stations = *num_stations + 1;
                    int* path = (int*)malloc((*num_stations) * sizeof(int));
                    int current_station = end;
                    int path_index = *num_stations-1;
                
                    *num_steps= 0;
                    

                    // Follow the path from the end to the start
                    while (current_station != -1) {
                        *num_steps= *num_steps+1;
                        if(path_index>=0){
                            path[path_index] = highway->stations[current_station].distance; 
                        }
                        current_station = prev[current_station];
                        path_index--;
                    }
                    free(prev);
                    return path;  
                }
            }
        }
    }
    free(queue->values);
    free(queue);
    
    free(visited);
    free(prev);
    return NULL;
}

//MAIN
int main() {
    Graph *highway = init_highway();

    char *line = NULL;
    size_t len = 0;

    int zero_is_a_station=0;
    
    //CICLO
    while (getline(&line, &len, stdin) != -1) {
        char command[20];
        int distance, num_cars;
        int num_read = sscanf(line, "%s %d %d", command, &distance, &num_cars);

        //AGGIUNGI STAZIONE
        if (strcmp(command, "aggiungi-stazione") == 0 && num_read >= 3) {
            if(zero_is_a_station==1 && distance==0){
                printf("non aggiunta\n");
            }
            char *autonomies = strchr(line, '\n');
            if (autonomies != NULL) {
                *autonomies = '\0';  // Replace the newline character with a null terminator
            }
            autonomies = strchr(line, ' ') + 1;
            autonomies = strchr(autonomies, ' ') + 1;  // Skip past the distance and number of cars

            int success = add_station(highway, distance, &zero_is_a_station);
            if (success) {
                char *car = strtok(autonomies, " ");
                car = strtok(NULL, " ");
                int i=0;
                int* cars= (int*)malloc(num_cars*sizeof(int));
                while (car != NULL) {
                    cars[i] = atoi(car);
                    i++;
                    car = strtok(NULL, " ");
                }
                add_car(highway, distance, cars, i);
                free(cars);
                printf("aggiunta\n");//printf("Station added successfully\n");
            } else {
                printf("non aggiunta\n");//printf("Station already exists\n");
            }
            continue;
        }

        //DEMOLISCI STAZIONE
        else if (strcmp(command, "demolisci-stazione") == 0 && num_read >= 2) {
            if(distance==0 && zero_is_a_station==0){
                printf("non demolita\n");
                continue;
            }
            if(delete_station(highway, distance, &zero_is_a_station)){
                printf("demolita\n");
            }else{
                printf("non demolita\n");
            }
            continue;
        } 
        
        //AGGIUNGI AUTO 
        else if (strcmp(command, "aggiungi-auto") == 0 && num_read >= 3) {
            int success = add_car(highway, distance, &num_cars, 1);
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
            int num_stations = 0; int num_steps = 0;
            int *route = plan_route(highway, distance, num_cars, &num_stations, &num_steps);
            if(route== NULL){
                printf("nessun percorso\n");
                continue;
            }
            
            //printf("NUM STATIONS: %d\n", num_stations);
            //printf("STEPS: %d\n",num_steps);
            if(distance<num_cars){
                for (int i = num_stations-num_steps; i < num_stations; i++) { 
                    if(i>=0){
                        printf("%d", route[i]);
                        if(i!=num_stations-1){
                            printf(" ");
                        } 
                    }           
                }
            }
            if(distance>num_cars){
                
                for (int i = num_stations-1 ; i >= num_stations-num_steps; i--) { 
                    if(i>=0){
                        printf("%d", route[i]);
                        if(i!=num_stations-num_steps){
                            printf(" ");
                        } 
                    }           
                }
            }
            
            free(route);
            printf("\n");   
        }

        //PRINTA GRAFO
        else if (strcmp(command, "printa-grafo") == 0){
            print_graph(highway);
        } else {
            printf("Unknown command: %s\n", command);
        }
    }
    //FREE
    for(int i=0; i<highway->numStations; i++){
        if(highway->stations[i].num_forward_edges>0){free(highway->stations[i].forward_edges);}
        if(highway->stations[i].num_incoming_edges>0){free(highway->stations[i].incoming_edges);}
        if(highway->stations[i].num_cars>0){free(highway->stations[i].cars);}
    }
    if(highway->numStations>0){free(highway->stations);}
    free(highway);
    free(line);
    return 0;
}