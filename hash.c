#include "hash.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#define TAM_INICIAL 53; //numero primo.


typedef enum{VACIO, BORRADO, DATO} estado_t;

typedef nodo nodo_t;
struct nodo{
  char* clave;
  void* dato;
  estado_t estado;
};

struct hash{
  hash_destruir_dato_t* destructor;
  size_t capacidad; //tamaño del arreglo.
  size_t cantidad; //cantidad de lugares ocupados.
  nodo_t* tabla;//arreglo de nodos.
};

struct hash_iter{
  const hash_t* hash;
  nodo_t* actual;
  nodo_t* anterior;
};

/* *****************************************************************
 *                     PRIMITIVAS TABLA  HASH                      *
 * *****************************************************************/
//devuelve una posicion en el arreglo;
int hashing(size_t capacidad, char * clave){
  size_t valor = 0;
  for(int i = 0; clave[i] != '\0'; i++){
    valor += (size_t)clave[i];
  }
  return valor % capacidad;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
  hash_t* hash = malloc(sizeof(hash_t));
  if(!hash){
    return NULL;
  }
  hash->tabla = malloc(TAM_INICIAL * sizeof(nodo_t));
  if (!hash->tabla){
    free(hash);
    return NULL;
  }
  for(int i = 0; i < hash->capacidad; i++){
    hash->tabla.estado = VACIO;
  }
  hash->capacidad = TAM_INICIAL;
  hash->cantidad = 0
  return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
  if(!hash || !clave){
    return false;
  }
  size_t pos = hashing(hash->capacidad, clave);
  while(hash->tabla[pos].estado != VACIO){
    pos++;
  }
  hash->tabla[pos].dato = dato;
  strcpy(hash->tabla[pos].clave, clave);
  hash->table[pos].estado = DATO;
  hash->cantidad += 1;
  return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
  if(!hash || !clave){
    return NULL;
  }
  size_t pos = hashing(hash->capacidad, clave);
  while(hash->tabla[pos].estado != VACIO && !strcmp(hash->tabla[pos].clave, clave)){
    pos++;
  }
  if(hash->tabla[pos].estado == VACIO){
    return NULL;
  }
  hash->table[pos].estado = BORRADO;
  return hash->tabla[pos].dato;

}

void *hash_obtener(const hash_t *hash, const char *clave){

}

bool hash_pertenece(const hash_t *hash, const char *clave){

}

size_t hash_cantidad(const hash_t *hash){
  return hash->cantidad;
}

void hash_destruir(hash_t *hash){
  free(hash->tabla);
  free(hash);
}

/* *****************************************************************
 *                   PRIMITIVAS HASH ITERADOR                      *
 * *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash){

}


bool hash_iter_avanzar(hash_iter_t *iter){

}


const char *hash_iter_ver_actual(const hash_iter_t *iter){

}


bool hash_iter_al_final(const hash_iter_t *iter){

}

void hash_iter_destruir(hash_iter_t* iter){
  free(iter);
}
