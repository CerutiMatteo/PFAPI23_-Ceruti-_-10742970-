#define MAX_CARS 512

typedef struct Station {
    int index;
    int distance;
    int cars[MAX_CARS];
    int num_cars;
    int num_forward_edges;
    int num_backward_edges;
    struct Station *forward_edges;
    struct Station *backward_edges;
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
int get_max_car (Station *station);
int edge_exists(Station *station, Station * target);
int lower_bound(Station *stations, int numStations, int value);
int upper_bound(Station *stations, int numStations, int value);
int add_car(Graph *highway, int distance, int autonomy);
int scrap_car(Graph *highway, int distance, int autonomy);
int* plan_route(Graph *highway, int start, int end, int* num_stations);
int delete_station(Graph *highway, int distance);
int delete_car(Graph *highway, int distance, int autonomy);

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
        printf("Station %d (distance: %d, max car: %d):\n", i, station->distance, get_max_car(station));
        printf("Forward edges: ");
        for (int j = 0; j < station->num_forward_edges; j++) {
            printf(" %d", station->forward_edges[j].distance);
        }
        printf("\n");
        printf("Backward edges: ");
        for (int j = 0; j < station->num_backward_edges; j++) {
            printf(" %d", station->backward_edges[j].distance);
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
    highway->stations[0].num_backward_edges = 0;
    return highway;
}

void check_and_create_edges(Graph *highway, Station *new_station) {
    int i;
    for (i = 0; i < highway->numStations; i++) {
        int max_car = get_max_car(&highway->stations[i]);
        if (max_car >= abs(new_station->distance - highway->stations[i].distance) &&
            highway->stations[i].index != new_station->index) {
            if (new_station->distance > highway->stations[i].distance &&
                !edge_exists(&highway->stations[i], new_station)) {
                highway->stations[i].num_forward_edges++;
                highway->stations[i].forward_edges = realloc(highway->stations[i].forward_edges, highway->stations[i].num_forward_edges * sizeof(Station));
                highway->stations[i].forward_edges[highway->stations[i].num_forward_edges - 1] = *new_station;
            } else if (new_station->distance < highway->stations[i].distance &&
                       !edge_exists(&highway->stations[i], new_station)) {
                highway->stations[i].num_backward_edges++;
                highway->stations[i].backward_edges = realloc(highway->stations[i].backward_edges, highway->stations[i].num_backward_edges * sizeof(Station));
                highway->stations[i].backward_edges[highway->stations[i].num_backward_edges - 1] = *new_station;
            }
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
    highway->stations[i].num_backward_edges = 0;
    highway->stations[i].forward_edges = malloc(sizeof(Station));
    highway->stations[i].backward_edges = malloc(sizeof(Station));

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

int edge_exists(Station *station, Station *check_station) {
    for (int i = 0; i < station->num_forward_edges; i++) {
        if (&station->forward_edges[i] == check_station) {
            return 1;
        }
    }
    for (int i = 0; i < station->num_backward_edges; i++) {
        if (&station->backward_edges[i] == check_station) {
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
            if (highway->stations[i].forward_edges[j].index == index) {
                // Shift all edges after the current one to the left
                for (int k = j; k < highway->stations[i].num_forward_edges - 1; k++) {
                    highway->stations[i].forward_edges[k] = highway->stations[i].forward_edges[k + 1];
                }
                highway->stations[i].num_forward_edges--;
                highway->stations[i].forward_edges = realloc(highway->stations[i].forward_edges, highway->stations[i].num_forward_edges * sizeof(Station));
                break;
            }
        }

        // Check backward edges
        for (int j = 0; j < highway->stations[i].num_backward_edges; j++) {
            if (highway->stations[i].backward_edges[j].index == index) {
                // Shift all edges after the current one to the left
                for (int k = j; k < highway->stations[i].num_backward_edges - 1; k++) {
                    highway->stations[i].backward_edges[k] = highway->stations[i].backward_edges[k + 1];
                }
                highway->stations[i].num_backward_edges--;
                highway->stations[i].backward_edges = realloc(highway->stations[i].backward_edges, highway->stations[i].num_backward_edges * sizeof(Station));
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

        for (int i = 0; i < station->num_backward_edges; i++) {
            if (abs(station->backward_edges[i].distance - station->distance) > second_max_car) {
                // Shift all edges after the current one to the left
                for (int j = i; j < station->num_backward_edges - 1; j++) {
                    station->backward_edges[j] = station->backward_edges[j + 1];
                }
                station->num_backward_edges--;
                station->backward_edges = realloc(station->backward_edges, station->num_backward_edges * sizeof(Station));
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


int add_car(Graph *highway, int distance, int car) {
    int i;
    for (i = 0; i < highway->numStations; i++) {
        if (highway->stations[i].distance == distance) {
            if (highway->stations[i].num_cars == MAX_CARS) {
                return 0;
            }
            highway->stations[i].cars[highway->stations[i].num_cars] = car;
            highway->stations[i].num_cars++;

            int max_car = get_max_car(&highway->stations[i]);

            // If the added car is the new max car, update the edges
            if (car == max_car) {

                for (int j = 0; j < highway->numStations; j++) {
                    if (highway->stations[j].distance != distance &&
                        max_car >= abs(distance - highway->stations[j].distance) &&
                        !edge_exists(&highway->stations[i], &highway->stations[j])) {
                        if (highway->stations[j].distance > distance) {
                            highway->stations[i].forward_edges = realloc(highway->stations[i].forward_edges, (highway->stations[i].num_forward_edges + 1) * sizeof(Station));
                            highway->stations[i].forward_edges[highway->stations[i].num_forward_edges] = highway->stations[j];
                            highway->stations[i].num_forward_edges++;
                        } else {
                            highway->stations[i].backward_edges = realloc(highway->stations[i].backward_edges, (highway->stations[i].num_backward_edges + 1) * sizeof(Station));
                            highway->stations[i].backward_edges[highway->stations[i].num_backward_edges] = highway->stations[j];
                            highway->stations[i].num_backward_edges++;
                        }
                    }
                }
            }

            return 1;
        }
    }
    return 0;
}

int* plan_route(Graph* highway, int start_distance, int end_distance, int *num_stations) {
    // Convert distances to indices
    int start = find_station_index(highway, start_distance);
    int end = find_station_index(highway, end_distance);

    if (start == -1 || end == -1) {
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

    // Determine the direction of the travel
    int forward = start_distance <= end_distance ? 1 : 0;

    while (!isEmpty(queue)) {
        int current = dequeue(queue);

        // Determine the edges to use based on the direction
        Station* edges = forward ? highway->stations[current].forward_edges : highway->stations[current].backward_edges;
        int num_edges = forward ? highway->stations[current].num_forward_edges : highway->stations[current].num_backward_edges;

        // Iterate through all edges from the current station
        for (int i = 0; i < num_edges; i++) {
            Station* edge = &edges[i];

            // If the station hasn't been visited yet
            if (!visited[edge->index]) {
                enqueue(queue, edge->index);
                visited[edge->index] = 1;
                prev[edge->index] = current;
                *num_stations = *num_stations + 1;

                // If we have reached the end station
                if (edge->index == end) {
                    free(queue);
                    free(visited);

                    // Create a list to store the path
                    int* path = (int*)malloc((*num_stations) * sizeof(int));
                    int current_station = end;
                    int path_index = *num_stations - 1;

                    // Follow the path from the end to the start
                    while (current_station != -1) {
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


