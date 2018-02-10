#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "producto.h"
#include "cola.h"
#include "stack.h"
#include "ListaEnlazada.c"
#include "configuracion.h"
#include "simulacion.c"
#include <time.h>
#include <math.h>

void menu();

void interactiva(ListaEnlazada *Carrito,ColaCarrito *BandaT, Stack *Pila, ListaEnlazada *Bolsas);

void automatica(ListaEnlazada *Carrito);

void moveralabanda(ListaEnlazada *Carrito, ColaCarrito *BandaT, int *volumen);

int procesamiento(Producto *Producto,int *volumen);

void embolsar(Stack *Pila, ListaEnlazada *Bolsa);



int main (){
    int opcion;
    while(1){
        menu();
        while(1){
            printf("Ingrese una opcion: ");
            scanf("%d", &opcion);
            if ( opcion == 1 ){
                srand(time(NULL));
                ListaEnlazada *Carrito;
                ColaCarrito *BandaT;
                Stack *Pila;
				ListaEnlazada *Bolsas;
                Carrito = malloc(sizeof(ListaEnlazada));
                BandaT = malloc(sizeof(ColaCarrito));
                Pila = malloc(sizeof(Stack));
				Bolsas = malloc(sizeof(ListaEnlazada));
                Inicialize(Carrito);
                iniciarcola(BandaT);
                initialization(Pila);
				Inicialize(Bolsas);
                cantidaddecarritos = rand();
                Productos = LeerProductos();
                generarcarrito(Carrito);
                if ( modalidad == "interactiva" ) interactiva(Carrito,BandaT,Pila, Bolsas);
                else{
                    automatica(Carrito);
                } 
            }
            else if ( opcion == 2 ){
                configuracion();
            }
            else if (opcion == 3){
                return 0;
            }
            else{
                printf("Introdujo una opcion erronea");
            }
            break;
        }
    }
}

void menu(){
    printf("1. Ejecutar la Simulacion\n");
    printf("2. Cambiar las opciones de la simulacion\n");
    printf("3. Salir del programa\n");
}

void interactiva(ListaEnlazada *Carrito,ColaCarrito *BandaT, Stack *Pila, ListaEnlazada *Bolsas){
	int tiempo = 0;
	int tiempoprocesamiento = 0;
	int tiempoinicio = 0;
	int facturacion = 0;
	int volumenbt = 0;
	ListaEnlazada *Bolsa;
	struct Producto *Producto;
	Bolsa = malloc(sizeof(ListaEnlazada));
	Inicialize(Bolsa);
	printf("Presiona Enter para iniciar la simulacion");
	fflush(stdin);
	getchar();
	printf("----Tiempo transcurrido: %d s.----\n",tiempo);
	printf("Lista de elementos en el carrito: ");
	imprimirlista(Carrito);
	printf("Lista de elementos en la banda transportadora: ");
	imprimircola(BandaT);
	printf("Lista de elementos en el area de embolsado: ");
	show(Pila);
	printf("Lista de elementos embolsados: \n"); //PENEDIENTE
	imprimirlistabolsa(Bolsas);
	do{
		if (Carrito->head != NULL){ //NUEVO
			moveralabanda(Carrito,BandaT,&volumenbt);
		}
		if (BandaT->nraiz != NULL){ // NUEVO
			if((BandaT->nraiz->Dato->Peso+AreaPila(Pila) <= maxareaembolsado)|| 
			(BandaT->nraiz->Dato->Peso > maxareaembolsado && Pila->size == 0)){
				if ((tiempo - tiempoprocesamiento) == tiempoinicio){
					tiempoinicio = tiempo;
					if(BandaT->nraiz->Dato->Peso <= maxbolsa && tiempo > 0){
						volumenbt -= BandaT->nraiz->Dato->Peso;
						push(Pila,extraernodocola(BandaT));
					}else{
						if(tiempo > 0){
							volumenbt -= BandaT->nraiz->Dato->Peso;
							addelementlist(Bolsas, extraernodocola(BandaT));
						}
					}
					Producto= BandaT->nraiz->Dato;
					tiempoprocesamiento = Producto->Complejidad / velocidadcajera;
					printf("--------TIEMPO PROCESAMIENTO: %d seg.----------\n",tiempoprocesamiento);
				}
			}else{;
				tiempoinicio++;
			}
		}
		else{ // NUEVO
			if ((tiempo - tiempoprocesamiento) == tiempoinicio){
				if(Producto->Peso <= maxbolsa && tiempo > 0) push(Pila,Producto);
				else{
					if(tiempo > 0) addelementlist(Bolsas, Producto);
				}
			}
		} // NUEVO FIN
		if (Pila->head!=NULL && tiempo % velocidadembolsador == 0){
			embolsar(Pila,Bolsa);
			ListaEnlazada *BolsaAuxiliar;
			BolsaAuxiliar = malloc(sizeof(Bolsa));
			memcpy(BolsaAuxiliar, Bolsa, sizeof(Bolsa));
			anadirbolsa(Bolsas, BolsaAuxiliar);
			free(Bolsa);
			Bolsa = malloc(sizeof(ListaEnlazada));
			Inicialize(Bolsa);
		}
		if(tiempo>0){
			printf("----Tiempo transcurrido: %d s.----\n",tiempo);
			printf("Lista de elementos en el carrito: ");
			imprimirlista(Carrito);
			printf("\nLista de elementos en la banda transportadora: ");
			imprimircola(BandaT);
			printf("\nLista de elementos en el area de embolsado: ");
			show(Pila);
			printf("\nLista de elementos embolsados: ");
			imprimirlistabolsa(Bolsas);
		}
		fflush(stdin);
		printf("\nPresiona Enter para continuar la simulacion: ");
		getchar();
		tiempo ++;
	}while (1); // Establecer una nueva condicion de parada. En la primera iteracion puede
	//pasar que la banda se quede vacia y la pila tambien.
}

void moveralabanda(ListaEnlazada *Carrito, ColaCarrito *BandaT, int *volumen){
	// DESPUES VERIFICAR SI EL OBJETO SUPERA LA CAPACIDAD DE LA BANDA
	while (Carrito->head->Dato->Peso + *volumen <= maxbt && Carrito->head != NULL){
		struct Producto *objeto = removeelementlist(Carrito);
		insertarnodocola(objeto, BandaT);
		*volumen += objeto->Peso;
		if (Carrito->head == NULL){ //NUEVO
			break;
		}
	}
	if(BandaT->nraiz == NULL && Carrito->head->Dato->Peso > maxbt){
		struct Producto *objeto = removeelementlist(Carrito);
		insertarnodocola(objeto, BandaT);
	}
}

void embolsar(Stack *Pila, ListaEnlazada *Bolsa){ //PASAR DE BANDA DE TRANSBORDADORA
	int volumen = 0;
	while (1){
		if (Pila->head == NULL){
			break;
		}
		else if (Pila->head->item->Peso + volumen <= maxbolsa){
			struct Producto *objeto = pop(Pila);
			addelementlist(Bolsa,objeto);
			volumen += objeto->Peso;
		}
		else{
			break;
		}
	}
}

void automatica(ListaEnlazada *Carrito){
}
