#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Station {
    int index;
    int distance;
    int* cars;
    int max_car;
    int num_cars;
    int max_car_deleted;
    int num_forward_edges;
    int num_incoming_edges;
    int *forward_edges;
    int *incoming_edges;
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
        printf("\nStation %d (distance: %d, max car:", i, station->distance);
        if(station->cars!=NULL){
            printf(" %d):", station->max_car);
        }
        printf("\nForward edges: ");
        if(station->forward_edges!=NULL){
            for (int j = 0; j < station->num_forward_edges; j++) {
            printf(" %d", station->forward_edges[j]);
            }
        }
        
        printf("\n");
        printf("Incoming edges: ");
        if(station->incoming_edges!=NULL){
            for (int j = 0; j < station->num_incoming_edges; j++) {
            printf(" %d", station->incoming_edges[j]);
            }
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
    highway->stations[0].max_car_deleted=0;
    highway->stations[0].num_forward_edges = 0;
    highway->stations[0].num_incoming_edges = 0;
    highway->stations[0].cars=NULL;
    highway->stations[0].forward_edges = NULL;
    highway->stations[0].incoming_edges = NULL;

    return highway;
}

void search_forward_edges(Graph *highway, Station *station){
    if(station->index== highway->numStations-1){
        return;
    }
    if(station->forward_edges!=NULL){
        free(station->forward_edges);
        station->forward_edges=NULL;
    }
    station->num_forward_edges=0;
    //printf("%d-> ",station->distance);
    for(int i=station->index+1; i<highway->numStations; i++){
        if(station->max_car>= abs(station->distance - highway->stations[i].distance)){
            station->num_forward_edges++;
            station->forward_edges= realloc(station->forward_edges, station->num_forward_edges*sizeof(int));
            station->forward_edges[station->num_forward_edges-1]= highway->stations[i].distance;
            //printf(" %d ",highway->stations[i].distance);
        }
        else{
            //printf("\n");
            return;
        }
    }
    //printf("\n"); 
}

void search_incoming_edges(Graph *highway, Station *station){

    if(station->index== highway->numStations-1){
        return;
    }
    if(station->incoming_edges!=NULL){
        free(station->incoming_edges);
        station->incoming_edges= NULL;
        }
    station->num_incoming_edges=0;
    //printf("%d-> ",station->distance);
    for(int i=station->index+1; i<highway->numStations; i++){
        if(highway->stations[i].max_car>= abs(station->distance - highway->stations[i].distance)){
            station->num_incoming_edges++;
            station->incoming_edges= realloc(station->incoming_edges, station->num_incoming_edges*sizeof(int));
            station->incoming_edges[station->num_incoming_edges-1]= highway->stations[i].distance;
            //printf(" %d ",highway->stations[i].distance);
        }
    }
    //printf("\n");
}

int add_station(Graph *highway, int distance, int *zero_is_a_station) {
    Station new_station;
    new_station.distance = distance;
    new_station.cars=NULL;
    new_station.num_cars = 0;
    new_station.max_car = 0;
    new_station.max_car_deleted= 0;
    new_station.num_forward_edges = 0;
    new_station.forward_edges = NULL;
    new_station.num_incoming_edges = 0;
    new_station.incoming_edges = NULL;
    new_station.index=0;

    if(distance!=0){
        int i=0;
        for (i = 0; i < highway->numStations; i++) {
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
        }

            new_station.index = i; 

            if(i>0 && i<highway->numStations) {
                highway->stations[i]= new_station;
            }

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
        if(*zero_is_a_station==1){
            return 0;
        }
        *zero_is_a_station=1;
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
    if (station->forward_edges[i] == check_station->distance) {
        return 1;
        }
    }
    return 0;
}

int edge_incoming_exists(Station *station, Station *check_station) {
    for (int i = 0; i < station->num_incoming_edges; i++) {
    if (station->incoming_edges[i] == check_station->distance) {
        return 1;
        }
    } 
    return 0;
}

int partition(int *edges, int low, int high) {
    int pivot = edges[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (edges[j] <= pivot) {
            i++;
            int temp = edges[i];
            edges[i] = edges[j];
            edges[j] = temp;
        }
    }
    int temp = edges[i + 1];
    edges[i + 1] = edges[high];
    edges[high] = temp;

    return (i + 1);
}

void quickSort(int *edges, int low, int high) {
    if (low < high) {
        int pi = partition(edges, low, high);
        quickSort(edges, low, pi-1);
        quickSort(edges, pi+1, high);
    }
}

void sort_edges(Station *station, int n, int forward) {
    int *edges = forward ? station->forward_edges : station->incoming_edges;
    quickSort(edges, 0, n - 1);
}


int delete_station(Graph *highway, int distance, int* zero_is_a_station) {
    if(highway->stations[highway->numStations-1].distance== distance){
        if(highway->stations[highway->numStations-1].cars!=NULL){
            free(highway->stations[highway->numStations-1].cars);
            highway->stations[highway->numStations-1].cars=NULL;
        }
        highway->numStations--;
        highway->stations= realloc(highway->stations, highway->numStations*sizeof(Station));
        return 1;
    }
    int deleted=0;
    for(int i=0; i<highway->numStations-1 ; i++){
        if(highway->stations[i].distance== distance){
            if(highway->stations[i].cars!=NULL){
                free(highway->stations[i].cars);
                highway->stations[i].cars=NULL;
            }
            highway->stations[i]= highway->stations[i+1];
            deleted= 1;
        }
        if(highway->stations[i].distance> distance && deleted){
            highway->stations[i]= highway->stations[i+1];
        }
    }
    if(deleted){
        highway->numStations--;
        highway->stations= realloc(highway->stations,highway->numStations*sizeof(Station));
        return 1;
    }
    return 0;
}

int delete_car(Graph *highway, int distance, int autonomy) {
    for(int i=0; i<highway->numStations; i++){
        if(highway->stations[i].distance== distance){
            if(highway->stations[i].num_cars==0 || highway->stations[i].cars==NULL){
                return 0;
            }
            for(int j=0; j<highway->stations[i].num_cars; j++){
                if(highway->stations[i].cars[j]== autonomy){
                    highway->stations[i].cars[j]= highway->stations[i].cars[highway->stations[i].num_cars-1];
                    highway->stations[i].num_cars--;
                    highway->stations[i].cars= realloc(highway->stations[i].cars, highway->stations[i].num_cars*sizeof(int));
                    highway->stations[i].max_car_deleted= 1;
                    return 1;
                }
            }
        }
    }
    return 0;
}


int add_car(Graph *highway, int distance, int* cars, int n_cars) {
    if(n_cars==0){
        return 1;
    }
    if(n_cars>512){
        return 0;
    }
    for(int i=0; i<highway->numStations; i++){
        if(highway->stations[i].distance==distance){
            if(highway->stations[i].num_cars==512){
                return 0;
            }
            highway->stations[i].cars= realloc(highway->stations[i].cars, (highway->stations[i].num_cars+n_cars)*sizeof(int));
            for(int j=0; j<n_cars; j++){
                highway->stations[i].num_cars++;
                highway->stations[i].cars[highway->stations[i].num_cars-1]= cars[j];
                if(cars[j]>highway->stations[i].max_car){
                    highway->stations[i].max_car= cars[j];
                    highway->stations[i].max_car_deleted=0;
                }
            }
            return 1;
        }
    }
    return 0;
}


int* plan_route(Graph* highway, int start_distance, int end_distance, int *num_stations, int* num_steps) {

    int forward = start_distance <= end_distance ? 1 : 0;
    int start=-1, end=-1;
    
    //assegno gli archi solo a chi mi serve
    int visit=-1;
    if(forward){
        for(int i=0; i<highway->numStations; i++){//scorro le stazioni
            if(start_distance== highway->stations[i].distance){
                start= highway->stations[i].index;
            }
            if(end_distance== highway->stations[i].distance){
                end= highway->stations[i].index;
                break;
            }
            if(highway->stations[i].distance>=start_distance && highway->stations[i].distance<=end_distance){
                highway->stations[i].forward_edges=0;
                if(highway->stations[i].max_car_deleted){//se necessario cerco la macchina massima
                    highway->stations[i].max_car=0;
                    for(int k=0; k< highway->stations[i].num_cars; k++){
                        if(highway->stations[i].cars[k]>highway->stations[i].max_car){
                            highway->stations[i].max_car= highway->stations[i].cars[k]; 
                        }
                    }
                    highway->stations[i].max_car_deleted= 0;
                }
                if(highway->stations[i].distance+highway->stations[i].max_car> visit){
                    search_forward_edges(highway, &highway->stations[i]);
                    visit= highway->stations[i].distance+highway->stations[i].max_car;
                }
            }
        }
    }else{
        int prec=-1;
        for(int i=0; i<highway->numStations; i++){//scorro le stazioni
            if(start_distance== highway->stations[i].distance){
                end= highway->stations[i].index;
                break;
            }
            if(end_distance== highway->stations[i].distance){
                start= highway->stations[i].index;
                prec= highway->stations[i].distance;
            }
            if(highway->stations[i].distance<=start_distance && highway->stations[i].distance>=end_distance){
                highway->stations[i].forward_edges=0;
                if(highway->stations[i].max_car_deleted){//se necessario cerco la macchina massima
                    highway->stations[i].max_car=0;
                    for(int k=0; k< highway->stations[i].num_cars; k++){
                        if(highway->stations[i].cars[k]>highway->stations[i].max_car){
                            highway->stations[i].max_car= highway->stations[i].cars[k]; 
                        }
                    }
                    highway->stations[i].max_car_deleted= 0;
                }
                if(prec>=0 && highway->stations[i].distance-highway->stations[i].max_car<=prec){
                    search_incoming_edges(highway, &highway->stations[i]);
                    prec= highway->stations[i].distance;
                }
                
            }
        }
    }
    
        
        if (start == -1 || end == -1 || start==end) {
            // One of the stations was not found
            return NULL;
        }
        return NULL;
    }
    /*
    // Initialize visited array and previous node array
    int* visited = (int*)malloc(highway->numStations * sizeof(int));
    int* prev = (int*)malloc(highway->numStations * sizeof(int));
    for (int i = 0; i < highway->numStations; i++) {
        visited[i] = 0;
        prev[i] = -1;
    }
    
    if(start > end){
        int temp= end;
        end = start; 
        start = temp;
    }
    // Create a queue and enqueue the start node
    Queue* queue = createQueue();
    enqueue(queue, start);
    
    visited[start] = 1;

    int pin=-1;

    if(forward && highway->stations[start].num_forward_edges>0){
        pin= start_distance <= end_distance ? highway->stations[start].forward_edges[highway->stations[start].num_forward_edges-1] : highway->stations[start].incoming_edges[highway->stations[start].num_incoming_edges-1];

    }
    if(!forward && highway->stations[start].num_incoming_edges>0){
        pin= start_distance <= end_distance ? highway->stations[start].forward_edges[highway->stations[start].num_forward_edges-1] : highway->stations[start].incoming_edges[highway->stations[start].num_incoming_edges-1];
    }
    

    while (!isEmpty(queue)) {
        int current = dequeue(queue);
        
        // Determine the edges to use based on the direction
        int* edges = forward ? highway->stations[current].forward_edges : highway->stations[current].incoming_edges;
        int num_edges = forward ? highway->stations[current].num_forward_edges : highway->stations[current].num_incoming_edges;
        // Iterate through all edges from the current station

        for (int i = 0; i < num_edges; i++) {
            int ind= find_station_index(highway, edges[i]);     
            if(ind==-1) continue;
            // If the station hasn't been visited yet
            if (visited[ind]==0) {
                
                if(pin!=-1 ){
                        
                        if(highway->stations[ind].forward_edges!=NULL && highway->stations[ind].forward_edges[highway->stations[ind].num_forward_edges-1]>pin && forward){
                                pin=highway->stations[ind].forward_edges[highway->stations[ind].num_forward_edges-1];
                                enqueue(queue, ind);
                                prev[ind] = current;
                        }
                        if(highway->stations[ind].incoming_edges!=NULL && highway->stations[ind].incoming_edges[highway->stations[ind].num_incoming_edges-1]>pin && !forward){
                                pin=highway->stations[ind].incoming_edges[highway->stations[ind].num_incoming_edges-1];
                                enqueue(queue, ind);
                                prev[ind] = current;
                        } 

                    }
                    
                    
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
                    continue;
            }
        }
    }
    free(queue->values);
    free(queue);
    
    free(visited);
    free(prev);
    return NULL;
}
*/
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
                while (car != NULL || i<num_cars) {
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
            if(distance<num_cars){
                for(int i=0; i<highway->numStations; i++){
                    if(highway->stations[i].forward_edges!=NULL){
                    free(highway->stations[i].forward_edges);
                    }
                }
            }
            int *route = plan_route(highway, distance, num_cars, &num_stations, &num_steps);
            if(route== NULL){
                printf("nessun percorso");
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
            
            for(int i=0; i<highway->numStations; i++){
                if(highway->stations[i].forward_edges!=NULL){
                    free(highway->stations[i].forward_edges);
                    highway->stations[i].forward_edges=NULL;
                }
                if(highway->stations[i].incoming_edges!=NULL){
                    free(highway->stations[i].incoming_edges);
                    highway->stations[i].incoming_edges=NULL;
                }
            }
            continue;
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
        //printf("sto liberando %d\n",highway->stations[i].distance);
        if(highway->stations[i].forward_edges!=NULL){free(highway->stations[i].forward_edges);}
        if(highway->stations[i].incoming_edges!=NULL){free(highway->stations[i].incoming_edges);}
        if(highway->stations[i].cars!=NULL){free(highway->stations[i].cars);}
    }
    if(highway->numStations>0){free(highway->stations);}
    free(highway);
    free(line);
    return 0;
}