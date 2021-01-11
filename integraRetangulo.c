#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "timer.h"

int nthreads, retangulo_indice_global;
double *areas;
double nRetangulos, a, b;
pthread_mutex_t mutex; //variavel de lock para exclusao mutua

void * tarefa (void *arg) {


  long int id = (long int ) arg;
  double retanguloLargura,offset_x,area_retangulo;

  retanguloLargura = (b - a)/nRetangulos;

  for(int i = id; i < nRetangulos; i += nthreads){
    
    offset_x = a + (i)*retanguloLargura;
    area_retangulo = retanguloLargura*((offset_x*offset_x)/((offset_x*offset_x*offset_x)+3));
    areas[i] = area_retangulo;
    printf("Thread %ld calculou o retângulo %d\n", id, i);

  }
  pthread_exit(NULL);
}

//funcao principal do programa
int main(int argc, char* argv[] ) {

  pthread_t *tid;
  double inicio, fim, delta,soma;


  if(argc<4){
      printf("Digite: %s <qtde de retangulos> <limite inferior> <limite superior> <nthreads>\n", argv[0]);
      return 1;
  }

  nRetangulos = strtod(argv[1], NULL);
  a = strtod(argv[2], NULL);
  b = strtod(argv[3], NULL);
  nthreads = atoi(argv[4]);
  areas = (double *) calloc( nRetangulos, sizeof(double) );
  if(areas ==NULL){puts("ERRO MALLOC");return 2;}

  // (a) Inicialização da estrutura de dados
  GET_TIME(inicio);
  //--inicilaiza o mutex (lock de exclusao mutua)
  pthread_mutex_init(&mutex, NULL);
  tid = (pthread_t*) malloc(nthreads*sizeof(pthread_t));
  if(tid ==NULL){puts("ERRO MALLOC");return 2;}

  GET_TIME(fim);

  delta = fim - inicio;
  printf("tempo de Inicialização da estrutura de dados %lf\n", delta);
  
  GET_TIME(inicio);
  for(long int i=0;i<nthreads; i++){
    if(pthread_create(tid+i, NULL, tarefa, (void*) i)){
      puts("ERRO --pthread create\n");
      return 3;
    }
  }

  //--espera todas as threads terminarem
  for (int thread=0; thread<nthreads; thread++) {
    if (pthread_join(tid[thread], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  }

  pthread_mutex_destroy(&mutex);
  for (int i=0; i<nRetangulos; i++) {
    soma += areas[i];
  }
  GET_TIME(fim);
  delta = fim - inicio;
  printf("tempo de criação das threads, execução da integração e termino de threads %lf\n", delta);

  GET_TIME(inicio);
  printf("resultado %lf", soma);
  GET_TIME(fim);
  delta = fim - inicio;
  printf("tempo de finalização do programa %lf\n", delta);

  pthread_exit(NULL);
  
}