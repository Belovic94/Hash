#include "hash.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#define TAM_INICIAL 101 //número primo.
#define PIVOTE1 378551
#define PIVOTE2 63689
#define F_CARGA 0.5

typedef enum{VACIO, BORRADO, DATO} estado_t;

typedef struct nodo nodo_t;
struct nodo{
  char* clave;
  void* dato;
  estado_t estado;
};

struct hash{
  hash_destruir_dato_t destructor;
  size_t capacidad; //tamaño del arreglo.
  size_t cantidad; //cantidad de lugares ocupados.
  size_t cant_borrados; //cantidad de lugares borrados.
  nodo_t* tabla;//arreglo de nodos.
};

struct hash_iter{
  const hash_t* hash;
  nodo_t* actual;
  nodo_t* anterior;
};

/* *****************************************************************
 *                      FUNCIONES AUXILIARES                       *
 * *****************************************************************/

//devuelve una posicion en el arreglo;
size_t hashing(size_t capacidad, const char *clave){
  size_t hash = 0, pivot1 = PIVOTE1, pivot2 = PIVOTE2;
  for(int i = 0; clave[i] != '\0'; i++){
    hash = hash * pivot1 + (size_t)clave[i];
    pivot1 *= pivot2;
  }
  return hash % capacidad;
}
//Determina si la clave pasada por parametro se encuentra en
//la posicion especificada por el indice.
bool misma_clave(const hash_t *hash, const char *clave, size_t index){
  return (hash->tabla[index].estado == DATO && strcmp(hash->tabla[index].clave, clave) == 0);
}
//Recibe una tabla de hash, una clave y un indice(posicion en el array).
//Aplica el Quadratic Probing.
//Devuelve la posicion en donde encontro un lugar vacio o la clave ingresada.
size_t manejar_colisiones(const hash_t* hash, const char *clave, size_t index){
  for(size_t i = 0; i < hash->capacidad ; i++){
    if(index % 2 == 0)
      index = (index - i*i) % hash->capacidad;
    else
      index = (index + i*i) % hash->capacidad;
    if(hash->tabla[index].estado == VACIO || misma_clave(hash, clave, index)){
      return index;
    }
  }
  return hash->capacidad + 1;
}
//Devuelve una posicion dentro del areglo correspondiente a una
//posicion vacia o una clave repetida. En caso contrario devuelve
//una posicion mayor al tamaño del arreglo.
size_t obtener_indice(const hash_t* hash, const char *clave){
  size_t index = hashing(hash->capacidad, clave);
  if(hash->tabla[index].estado == VACIO || misma_clave(hash, clave, index))
    return index;
  return manejar_colisiones(hash, clave, index);
}

void inicializar_tabla_hash(nodo_t *tabla, size_t capacidad){
  for(int i = 0; i < capacidad; i++){//coloco a todos los nodos en estado VACIO
    tabla[i].estado = VACIO;
  }
}
//Devuelve un array de nodos del tamaño especificado por parametro.
//En caso contrario, devuelve NULL.
nodo_t *crear_tabla_hash(size_t capacidad){
  nodo_t *tabla_hash = malloc(capacidad * sizeof(nodo_t));
  if(!tabla_hash)
    return NULL;
  inicializar_tabla_hash(tabla_hash, capacidad);
  return tabla_hash;
}
//Agrga los valores de una tabla existente a la recien creada.
//Devuelve true si puede hacerlo, si no devuelve false.
//Pre:el hash fue creado y la tabla no tiene que estar vacia.
bool llenar_tabla_nueva(hash_t *hash, nodo_t *tabla_anterior){
  size_t index;
  for(int i = 0; i < hash->capacidad/2; i++){
    if(tabla_anterior[i].estado == DATO){
      index = obtener_indice(hash, tabla_anterior[i].clave);
      if(index > hash->capacidad)
        return false;
      hash->tabla[index].dato = tabla_anterior[i].dato;
      hash->tabla[index].estado = DATO;
      hash->tabla[index].clave = malloc((strlen(tabla_anterior[i].clave) + 1) * sizeof(char));
      if(!hash->tabla[index].clave)
        return false;
      strcpy(hash->tabla[index].clave, tabla_anterior[i].clave);
      free(tabla_anterior[i].clave);
    }
  }
  return true;
}

bool hash_redimensionar(hash_t* hash){
  nodo_t *tabla_aux = hash->tabla;
  hash->tabla = crear_tabla_hash(hash->capacidad * 2);
  hash->capacidad *= 2;
  if(!llenar_tabla_nueva(hash, tabla_aux)){
    free(hash->tabla);
    hash->capacidad /= 2;
    hash->tabla = tabla_aux;
    return false;
  }
  free(tabla_aux);
  return true;
}
/* *****************************************************************
 *                     PRIMITIVAS TABLA  HASH                      *
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
  hash_t* hash = malloc(sizeof(hash_t));
  if(!hash){
    return NULL;
  }
  hash->tabla = crear_tabla_hash(TAM_INICIAL);
  if (!hash->tabla){
    free(hash);
    return NULL;
  }
  hash->capacidad = TAM_INICIAL;
  hash->cant_borrados = 0;
  hash->cantidad = 0;
  hash->destructor = destruir_dato;
  return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
  if(!hash || !clave){//Si la clave o hash son NULL, sale.
    return false;
  }
  //verifico si hay que redimensionar.
  if(((double)hash->cantidad / (double)hash->capacidad)  >= F_CARGA){
    if(!hash_redimensionar(hash))
      return false;
  }
  size_t index = obtener_indice(hash, clave);
  if(index > hash->capacidad)
    return false;
  if(misma_clave(hash, clave, index)){//se fija si en la posicion dada hay un dato
    if(hash->destructor)                //si es así lo borra.
      hash->destructor(hash->tabla[index].dato);
  }
  else{
    hash->tabla[index].clave = malloc((strlen(clave) + 1) * sizeof(char));
    if(!hash->tabla[index].clave)
      return false;
    strcpy(hash->tabla[index].clave, clave);
    hash->tabla[index].estado = DATO;
    hash->cantidad += 1;
  }
  hash->tabla[index].dato = dato;
  return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
  if(!hash || !clave)
    return NULL;
  size_t index = obtener_indice(hash, clave);
  if(hash->tabla[index].estado == VACIO)
    return NULL;
  hash->cant_borrados += 1;
  hash->tabla[index].estado = BORRADO;
  free(hash->tabla[index].clave);
  return hash->tabla[index].dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){
  if(!hash || !clave)
    return NULL;
  size_t index = obtener_indice(hash, clave);
  if(index > hash->capacidad || hash->tabla[index].estado == VACIO){
    return NULL;
  }
  return hash->tabla[index].dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
  if(!hash || !clave)
    return false;
  size_t index = obtener_indice(hash, clave);
  if(index > hash->capacidad || hash->tabla[index].estado == VACIO){
    return false;
  }
  return true;
}

size_t hash_cantidad(const hash_t *hash){
  if(!hash)
    return 0;
  return hash->cantidad - hash->cant_borrados;
}

void hash_destruir(hash_t *hash){
  for(int i = 0; i < hash->capacidad; i++){
    if(hash->tabla[i].estado == DATO){
      if(hash->destructor)
        hash->destructor(hash->tabla[i].dato);
      free(hash->tabla[i].clave);
    }
  }
  free(hash->tabla);
  free(hash);
}

/* *****************************************************************
 *                   PRIMITIVAS HASH ITERADOR                      *
 * *****************************************************************/
/*
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
*/
