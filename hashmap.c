#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) 
{
    if (map == NULL || key == NULL) return;

    // Verificar si hay que agrandar el mapa
    if ((float)map->size / map->capacity > 0.7) {
        enlarge(map);
    }

    long index = hash(key, map->capacity);
    long originalIndex = index;

    while (1) {
        // Casilla vacía
        if (map->buckets[index] == NULL) {
            map->buckets[index] = createPair(key, value);
            map->size++;
            map->current = index;
            return;
        }

        // Casilla eliminada: reutilizar
        if (map->buckets[index]->key == NULL) {
            free(map->buckets[index]);
            map->buckets[index] = createPair(key, value);
            map->size++;
            map->current = index;
            return;
        }

        // Clave ya existe → no insertar
        if (strcmp(map->buckets[index]->key, key) == 0) {
            return;
        }

        // Avanzar circularmente
        index = (index + 1) % map->capacity;

        if (index == originalIndex) return; // Vuelta completa
    }

}

void enlarge(HashMap * map) 
{
if (map == NULL) return;

    Pair ** old_buckets = map->buckets;
    long old_capacity = map->capacity;

    map->capacity *= 2;
    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *)); 
    map->size = 0;

    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
        } else if (old_buckets[i] != NULL) {
            free(old_buckets[i]); // liberar pares inválidos
        }
    }

    free(old_buckets);
    enlarge_called = 1; // no borrar (testing purposes)
}

    

HashMap * createMap(long capacity) 
{
    HashMap * map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) return NULL;

    // Reservar memoria para los buckets (array de Pair*)
    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }

    // Inicializar los valores del mapa
    map->size = 0;
    map->capacity = capacity;
    map->current = -1;

    return map;
}

void eraseMap(HashMap * map,  char * key) {    


}

Pair * searchMap(HashMap * map,  char * key) 
{ 
    if (map == NULL || key == NULL) return NULL;

    long index = hash(key, map->capacity);
    long originalIndex = index;

    while (map->buckets[index] != NULL) {
        // Si el par es válido y coincide la clave
        if (map->buckets[index]->key != NULL && strcmp(map->buckets[index]->key, key) == 0) {
            map->current = index;
            return map->buckets[index];
        }

        // Avanza al siguiente índice (forma circular)
        index = (index + 1) % map->capacity;

        // Si dimos toda la vuelta → no está
        if (index == originalIndex) break;
    }

    // No se encontró
    return NULL;
}

Pair * firstMap(HashMap * map) 
{
    if (map == NULL) return NULL;

    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }

    return NULL; 
}

Pair * nextMap(HashMap * map) 
{
    if (map == NULL) return NULL;

    long i = map->current + 1;

    while (i < map->capacity) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
        i++;
    }
    return NULL;
}
