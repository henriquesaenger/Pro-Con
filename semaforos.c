#include <stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//Nome: Henrique Neitzel Saenger

//O código roda, mas ainda não funciona pra todos os casos.
//Quando tenho mais threads produtoras que consumidoras 
//ou quando se tem o mesmo número de threads, ele funciona.
//No caso de ter mais threads consumidoras que produtoras ele 
//ainda não funciona


void* prod(void* arg);
void* cons(void* arg);

#define TAM 20			//tamanho máximo de itens
#define CON 5			//numero de threads consumidoras
#define PRO 5			//numero de threads produtoras
#define ITM 3			//itens produzidos





//SEÇÃO CRÍTICA
sem_t vazio, cheio;
pthread_mutex_t m= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c= PTHREAD_COND_INITIALIZER;
int  buff[TAM];
int nbelem;
int cont_prod;			//variável de controle para verificar se o numero de produtores é maior que consumidores



//MAIN
int main(){

nbelem=0;
cont_prod=0;

if(ITM>0){
	for(int i=0;i<ITM;i++){
		buff[i]=rand()%100;
		nbelem++;
	}
}

pthread_t produtor[PRO], consumidor[CON];
pthread_mutex_init(&m, NULL);
pthread_cond_init(&c, NULL);
sem_init(&vazio, 0, TAM);
sem_init(&cheio, 0, -1);



for(int i=0;i<PRO;i++){
	pthread_create(&produtor[i], NULL, prod, NULL);
}
for(int j=0;j<CON;j++){
	pthread_create(&consumidor[j], NULL, cons, NULL);	
}
for(int i=0;i<PRO;i++){
	pthread_join(produtor[i], NULL);	
}
for(int j=0;j<CON;j++){
	pthread_join(consumidor[j], NULL);	
}


//destrói mutex e semáforo
pthread_mutex_destroy(&m);
sem_destroy(&vazio);
sem_destroy(&cheio);

return 0;
}



//PRODUTOR
void* prod(void *arg){
	int n=rand()%100;
	sem_wait(&vazio);
	pthread_mutex_lock(&m);
	buff[nbelem]=n;
	nbelem++;
	printf("\nO número %d foi produzido", n);
	pthread_cond_signal(&c);
	pthread_mutex_unlock(&m);
	cont_prod++;
	sem_post(&cheio);
}


//CONSUMIDOR
void* cons(void *arg){
	if(nbelem == 0 && cont_prod!=PRO){			//se o número de elementos chegar a 0 e ainda não tiver feito todas as operações de produção, ele espera produzir um valor para poder consumir
		pthread_cond_wait(&c, &m);
	}
	if(cont_prod==PRO){							//se tiver mais operações de consumir do que de produzir, ele consome dos itens pré-colocados 
		pthread_mutex_lock(&m);
		int n= buff[nbelem-1];
		nbelem--;
		printf("\nO número %d foi consumido", n);
		pthread_mutex_unlock(&m);	
	}
	else{										//caso contrário ele consome um item da pilha normalmente
		sem_wait(&cheio);					
		pthread_mutex_lock(&m);
		int n= buff[nbelem-1];
		nbelem--;
		pthread_mutex_unlock(&m);
		printf("\nO número %d foi consumido", n);
		sem_post(&vazio);
	}	
}