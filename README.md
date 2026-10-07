# Highway Route Planner — Algorithms and Data Structures

[Italian version below](#italiano)

## English

### Overview

This project was developed for the **Algorithms and Data Structures Final Project 2022/2023**.

The program simulates a highway composed of service stations located at different distances from the beginning of the highway.

Each service station contains a fleet of electric rental vehicles. Every vehicle is characterized by its **range**, expressed in kilometers.

The main objective is to manage stations and vehicles dynamically and to find a route between two stations using the **minimum possible number of stops**.

---

## Features

The program supports the following operations:

- Add a new service station
- Remove an existing service station
- Add a vehicle to a station
- Scrap a vehicle from a station
- Plan a route between two stations

Each station is uniquely identified by its distance from the beginning of the highway.

During a trip, when the driver reaches a station, they can rent a new vehicle from that station.

The driver cannot change direction during the journey.

---

## Route Planning

Given a departure station and a destination station, the program searches for a route requiring the **minimum number of stops**.

A station can be reached from another station if the distance between them is less than or equal to the range of at least one vehicle available at the departure station.

If multiple routes require the same minimum number of stops, the route is selected according to the tie-breaking rule defined by the project specification, preferring stops closer to the beginning of the highway.

The problem is represented using a **graph**:

- Each station represents a node
- An edge represents the possibility of reaching another station
- A queue-based graph traversal is used to search for the route

---

## Data Structures

### `Station`

Represents a service station and stores:

- Distance from the beginning of the highway
- Station index
- Available vehicle ranges
- Maximum available vehicle range
- Edges to reachable stations

### `Edge`

Represents a connection between reachable stations.

### `Graph`

Represents the highway and contains all the service stations.

### `Queue`

FIFO data structure used during graph traversal and route planning.

---

## Supported Commands

### Add a station

```text
aggiungi-stazione <distance> <number-of-cars> <range-1> ... <range-n>
```

Example:

```text
aggiungi-stazione 10 3 100 200 300
```

Output:

```text
aggiunta
```

or:

```text
non aggiunta
```

if a station already exists at the specified distance.

---

### Remove a station

```text
demolisci-stazione <distance>
```

Output:

```text
demolita
```

or:

```text
non demolita
```

---

### Add a vehicle

```text
aggiungi-auto <station-distance> <vehicle-range>
```

Output:

```text
aggiunta
```

or:

```text
non aggiunta
```

---

### Scrap a vehicle

```text
rottama-auto <station-distance> <vehicle-range>
```

Output:

```text
rottamata
```

or:

```text
non rottamata
```

---

### Plan a route

```text
pianifica-percorso <departure> <destination>
```

If a route exists, the program prints all the stations in travel order.

Example:

```text
pianifica-percorso 20 50
```

Possible output:

```text
20 30 50
```

If no valid route exists:

```text
nessun percorso
```

Route planning does not modify the stations or their vehicle fleets.

---

## Implementation

The highway is stored as a dynamic collection of stations ordered by their distance from the beginning of the highway.

For each station, the program keeps track of the maximum vehicle range available. This information is used to determine which other stations can be reached.

During route planning, the required connections between stations are generated and a graph traversal is performed using a FIFO queue.

Dynamic memory management is implemented using:

```c
malloc()
realloc()
free()
```

---

## Compilation

The project is written in **C** and can be compiled using GCC:

```bash
gcc -Wall -Wextra -O2 -o autostrada main.c
```

Run the program with:

```bash
./autostrada
```

Input can also be redirected from a file:

```bash
./autostrada < input.txt
```

---

## Example

Input:

```text
aggiungi-stazione 20 4 5 10 15 25
aggiungi-stazione 30 1 40
aggiungi-stazione 45 1 30
aggiungi-stazione 50 2 20 25
pianifica-percorso 20 50
```

Output:

```text
aggiunta
aggiunta
aggiunta
aggiunta
20 30 50
```

---

## Main Constraints

- Station distances are unique
- Each station can contain up to **512 vehicles**
- The driver cannot change direction during a journey
- A new vehicle can be rented at every stop
- Route planning must minimize the number of stops
- In case of equal-length routes, the tie-breaking rule defined by the specification is applied

---

## Technologies and Concepts

- **Language:** C
- **Data structures:** Dynamic arrays, graphs, queues
- **Algorithms:** Graph traversal and shortest-path search
- **Memory management:** Dynamic allocation

---

## Author

Project developed for the **Algorithms and Data Structures** course — Academic Year **2022/2023**.

---

# Italiano

## Descrizione

Questo progetto è stato sviluppato per la **Prova Finale di Algoritmi e Strutture Dati 2022/2023**.

Il programma simula un'autostrada composta da stazioni di servizio situate a diverse distanze dall'inizio dell'autostrada.

Ogni stazione dispone di un parco di veicoli elettrici a noleggio. Ogni veicolo è caratterizzato dalla propria **autonomia**, espressa in chilometri.

L'obiettivo principale è gestire dinamicamente le stazioni e i relativi veicoli e trovare un percorso tra due stazioni utilizzando il **minor numero possibile di tappe**.

---

## Funzionalità

Il programma permette di:

- Aggiungere una nuova stazione
- Demolire una stazione esistente
- Aggiungere un veicolo a una stazione
- Rottamare un veicolo
- Pianificare un percorso tra due stazioni

Ogni stazione è identificata univocamente dalla propria distanza dall'inizio dell'autostrada.

Durante un viaggio, quando il conducente raggiunge una stazione, può noleggiare un nuovo veicolo appartenente al parco auto della stazione stessa.

Durante il viaggio non è possibile invertire il senso di marcia.

---

## Pianificazione del percorso

Date una stazione di partenza e una stazione di arrivo, il programma cerca un percorso che utilizzi il **minor numero possibile di tappe**.

Una stazione può essere raggiunta da un'altra se la distanza tra le due è minore o uguale all'autonomia di almeno uno dei veicoli disponibili nella stazione di partenza.

Se esistono più percorsi con lo stesso numero minimo di tappe, viene applicato il criterio di precedenza definito dalla specifica del progetto, privilegiando le tappe più vicine all'inizio dell'autostrada.

Il problema viene rappresentato tramite un **grafo**:

- Ogni stazione rappresenta un nodo
- Un arco rappresenta la possibilità di raggiungere un'altra stazione
- Una visita del grafo basata su una coda viene utilizzata per ricercare il percorso

---

## Strutture dati

### `Station`

Rappresenta una stazione dell'autostrada e contiene:

- Distanza dall'inizio dell'autostrada
- Indice della stazione
- Autonomie dei veicoli disponibili
- Autonomia massima disponibile
- Archi verso le stazioni raggiungibili

### `Edge`

Rappresenta un collegamento tra stazioni raggiungibili.

### `Graph`

Rappresenta l'intera autostrada e contiene l'insieme delle stazioni.

### `Queue`

Struttura FIFO utilizzata durante la visita del grafo e la ricerca del percorso.

---

## Comandi supportati

### Aggiunta di una stazione

```text
aggiungi-stazione <distanza> <numero-auto> <autonomia-1> ... <autonomia-n>
```

Esempio:

```text
aggiungi-stazione 10 3 100 200 300
```

Output:

```text
aggiunta
```

oppure:

```text
non aggiunta
```

se alla distanza specificata esiste già una stazione.

---

### Demolizione di una stazione

```text
demolisci-stazione <distanza>
```

Output:

```text
demolita
```

oppure:

```text
non demolita
```

---

### Aggiunta di un'automobile

```text
aggiungi-auto <distanza-stazione> <autonomia>
```

Output:

```text
aggiunta
```

oppure:

```text
non aggiunta
```

---

### Rottamazione di un'automobile

```text
rottama-auto <distanza-stazione> <autonomia>
```

Output:

```text
rottamata
```

oppure:

```text
non rottamata
```

---

### Pianificazione di un percorso

```text
pianifica-percorso <partenza> <arrivo>
```

Se esiste un percorso valido, vengono stampate le stazioni attraversate in ordine di percorrenza.

Esempio:

```text
pianifica-percorso 20 50
```

Possibile output:

```text
20 30 50
```

Se non esiste alcun percorso:

```text
nessun percorso
```

La pianificazione del percorso non modifica le stazioni o i relativi parchi auto.

---

## Implementazione

L'autostrada viene mantenuta come un insieme dinamico di stazioni ordinate in base alla loro distanza dall'inizio dell'autostrada.

Per ogni stazione viene mantenuta l'autonomia massima disponibile, utilizzata per determinare quali altre stazioni possono essere raggiunte.

Durante la pianificazione vengono costruiti i collegamenti necessari tra le stazioni e viene effettuata una visita del grafo utilizzando una struttura FIFO.

La gestione dinamica della memoria utilizza principalmente:

```c
malloc()
realloc()
free()
```

---

## Compilazione

Il progetto è scritto in **C** e può essere compilato utilizzando GCC:

```bash
gcc -Wall -Wextra -O2 -o autostrada main.c
```

Per eseguire il programma:

```bash
./autostrada
```

È inoltre possibile utilizzare un file come input:

```bash
./autostrada < input.txt
```

---

## Esempio

Input:

```text
aggiungi-stazione 20 4 5 10 15 25
aggiungi-stazione 30 1 40
aggiungi-stazione 45 1 30
aggiungi-stazione 50 2 20 25
pianifica-percorso 20 50
```

Output:

```text
aggiunta
aggiunta
aggiunta
aggiunta
20 30 50
```

---

## Vincoli principali

- Le distanze delle stazioni sono univoche
- Ogni stazione può contenere al massimo **512 veicoli**
- Durante un viaggio non è possibile invertire il senso di percorrenza
- Ad ogni tappa è possibile noleggiare un nuovo veicolo
- Il percorso deve utilizzare il minor numero possibile di tappe
- A parità di numero di tappe viene applicato il criterio di precedenza previsto dalla specifica

---

## Tecnologie e concetti utilizzati

- **Linguaggio:** C
- **Strutture dati:** Array dinamici, grafi, code
- **Algoritmi:** Visita di grafi e ricerca del cammino minimo
- **Gestione della memoria:** Allocazione dinamica

---

## Autore

Progetto sviluppato per il corso di **Algoritmi e Strutture Dati** — Anno Accademico **2022/2023**.
