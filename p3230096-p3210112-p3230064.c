#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "p3230096-p3210112-p3230064.h"
#include <pthread.h>
#include <unistd.h>

//variables to store the number of free employees of the pizzeria

int telephone_operators_free = number_of_telephone_operators;//variable to store the number of free telephone operators
int cooks_free = number_of_cooks;//variable to store the number of free cooks
int ovens_free = number_of_ovens;//variable to store the number of free ovens
int packers_free = number_of_packers;//variable to store the number of free packers
int deliverers_free = number_of_deliverers;//variable to store the number of free deliverers





int successful_counter = 0;//variable for the number of the successful orders
int unsuccessful_counter = 0;//variable for the number of the unsuccessful orders

int total_profit = 0;

//variables to store the number sold from each kind of  pizza
int margarita_counter = 0;
int pepperone_counter = 0;
int special_counter = 0;




//mutexes to lock the access to the above variables
pthread_mutex_t mutex_telephone_operators_free;
pthread_mutex_t mutex_cooks_free;
pthread_mutex_t mutex_ovens_free;
pthread_mutex_t mutex_packers_free;
pthread_mutex_t mutex_deliverers_free;

pthread_mutex_t mutex_sum_service_time;
pthread_mutex_t mutex_max_service_time;
pthread_mutex_t mutex_sum_cooling_time;
pthread_mutex_t mutex_max_cooling_time;
pthread_mutex_t mutex_screen;

//mutexes to lock the counters of the successful and unsuccessful orders
pthread_mutex_t mutex_unsuccessful_counter;
pthread_mutex_t mutex_successful_counter;

//mutexes to lock the counter of pizzas sold from each kind
pthread_mutex_t mutex_margarita_counter;
pthread_mutex_t mutex_pepperone_counter;
pthread_mutex_t mutex_special_counter;


pthread_mutex_t mutex_total_profit;


pthread_cond_t cond_telephone_operators = PTHREAD_COND_INITIALIZER;//condition variable for telephone oprators
pthread_cond_t cond_cooks = PTHREAD_COND_INITIALIZER;//condition variable for cooks
pthread_cond_t cond_ovens = PTHREAD_COND_INITIALIZER;//condition variable for ovens
pthread_cond_t cond_packers = PTHREAD_COND_INITIALIZER;//condition variable for packers
pthread_cond_t cond_deliverers = PTHREAD_COND_INITIALIZER;//condition variable for deliverers


pthread_cond_t cond_max_service_time=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_sum_service_time=PTHREAD_COND_INITIALIZER;

pthread_cond_t cond_max_cooling_time=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_sum_cooling_time=PTHREAD_COND_INITIALIZER;

int tc; // shows if a thread was created successfully
int sum_service_time=0; //sum time of service
int max_service_time=0; //max time of service
int sum_cooling_time=0; // sum pizza cooling time
int max_cooling_time=0; // max time of pizza cooling

typedef struct my_struct{
	int v1;
	int v2;
}my_struct;











void* customer_routine(void* arg) {
	
	 
    struct timespec start_service,end_service; //times of the start and the end of the service
    struct timespec end_preparation; // time at the end of the preparation
	
	my_struct* temp_struct =(my_struct*)arg;
	
	int order_id= temp_struct->v1;
	int seed=temp_struct->v2;
	tc=pthread_mutex_lock(&mutex_telephone_operators_free);//locking the variable telephone_operators_free
	
    clock_gettime(CLOCK_REALTIME, &start_service); // get the time at the start of the service
	
	while (telephone_operators_free == 0){//no telephone operators, the customer waits till a telephone operator is available
		  tc = pthread_cond_wait(&cond_telephone_operators, &mutex_telephone_operators_free);
	}
	
    telephone_operators_free--;
	pthread_cond_signal(&cond_telephone_operators); 
	pthread_mutex_unlock(&mutex_telephone_operators_free);// free the telephone operator
		
	int num_of_pizzas = rand_r(&seed) % number_orderhigh + 1;

	int num_of_margaritas = round(num_of_pizzas * propability_of_margarita);
	int num_of_pepperone = round(num_of_pizzas * propability_of_pepperone);
	int num_of_special = round(num_of_pizzas * propability_of_special);

	sleep(rand_r(&seed) % time_paymenthigh + 1); //time that passes till we see if the card is declined or not
		
	if (rand_r(&seed) % 20 +1  ==1) { //from 20 numbers, 1-20, we set 1 as the "failure number", in which case the order is declined (5% chance to get 1, as much as the other numbers)
		tc=pthread_mutex_lock(&mutex_screen);
		printf("Order with id %d got cancelled\n", order_id);
		tc=pthread_mutex_unlock(&mutex_screen);
		
		tc=pthread_mutex_lock(&mutex_unsuccessful_counter);
		unsuccessful_counter++;
		tc=pthread_mutex_unlock(&mutex_unsuccessful_counter);
		
		pthread_mutex_lock(&mutex_telephone_operators_free);
		telephone_operators_free++;
		
		pthread_cond_signal(&cond_telephone_operators);
		tc=pthread_mutex_unlock(&mutex_telephone_operators_free);
		
		pthread_exit(NULL);
	}//unsuccessfull order

	else {
		pthread_mutex_lock(&mutex_successful_counter);
		successful_counter++;
		pthread_mutex_unlock(&mutex_successful_counter);
		
		pthread_mutex_lock(&mutex_screen);
		printf("Order with id %d was added\n",order_id);
		
		pthread_mutex_unlock(&mutex_screen);
		
		pthread_mutex_lock(&mutex_total_profit);
		total_profit += num_of_margaritas * cost_of_margarita + num_of_pepperone * cost_of_pepperone + num_of_special * cost_of_special;
		pthread_mutex_unlock(&mutex_total_profit);

		pthread_mutex_lock(&mutex_margarita_counter);
		margarita_counter += num_of_margaritas;
		pthread_mutex_unlock(&mutex_margarita_counter);

		pthread_mutex_lock(&mutex_pepperone_counter);
		pepperone_counter += num_of_pepperone;
		pthread_mutex_unlock(&mutex_pepperone_counter);

		pthread_mutex_lock(&mutex_special_counter);
		special_counter += num_of_special;
		pthread_mutex_unlock(&mutex_special_counter);
		
		pthread_mutex_lock(&mutex_telephone_operators_free);
		telephone_operators_free++;
		
		pthread_cond_signal(&cond_telephone_operators);
		tc=pthread_mutex_unlock(&mutex_telephone_operators_free);
		
		
		
		
		
		pthread_mutex_lock(&mutex_cooks_free);
		while (cooks_free  == 0){//no free cooks, the pizzas wait to start getting prepared till a cook is available
		  tc = pthread_cond_wait(&cond_cooks, &mutex_cooks_free);
	    }
	    	
		//if (cooks_free > 0) {
		
		cooks_free--;

		tc = pthread_cond_signal(&cond_cooks);
		tc = pthread_mutex_unlock(&mutex_cooks_free); // free the cook
		
		
		sleep(num_of_pizzas * time_of_preparation); 
		
		tc = pthread_mutex_lock(&mutex_ovens_free);
		while (ovens_free<num_of_pizzas){//no ovens available, the pizzas wait to start getting baked
		  tc = pthread_cond_wait(&cond_ovens,&mutex_ovens_free);
	    }
	    	

		ovens_free -= num_of_pizzas;

		tc = pthread_cond_signal(&cond_cooks);
		tc = pthread_mutex_unlock(&mutex_ovens_free);
		

		sleep(time_of_bake);
		
		pthread_mutex_lock(&mutex_cooks_free);
		cooks_free++;
		
		pthread_cond_signal(&cond_cooks);
		tc=pthread_mutex_unlock(&mutex_cooks_free);
		
		struct timespec start_cooling; 
		clock_gettime(CLOCK_REALTIME, &start_cooling); // get the time when the pizza gets out of the oven

		tc = pthread_mutex_lock(&mutex_packers_free);
		while (packers_free==0){//no available packers, the pizzas wait to start getting prepared for delivery
		  tc = pthread_cond_wait(&cond_packers,&mutex_packers_free);
	    }
	    
		packers_free--;
		
		pthread_cond_signal(&cond_packers);
		tc = pthread_mutex_unlock(&mutex_packers_free);
		
		
		
		tc = pthread_mutex_lock(&mutex_ovens_free);
		ovens_free += num_of_pizzas;
		
		tc = pthread_cond_signal(&cond_ovens);
		pthread_mutex_unlock(&mutex_ovens_free);
		

		sleep(num_of_pizzas * time_of_pack);
		tc=pthread_mutex_lock(&mutex_packers_free);
		packers_free++;
		
		tc = pthread_cond_signal(&cond_packers);
		tc = pthread_mutex_unlock(&mutex_packers_free);
		
		
		clock_gettime(CLOCK_REALTIME, &end_preparation); // get the time at the end of the preparation
		unsigned int preparation_time = end_preparation.tv_sec - start_service.tv_sec;
		printf("The preparation time of the order with id %d is %d minutes\n" , order_id, preparation_time);
		
        
        
		pthread_mutex_lock(&mutex_deliverers_free);
		
		while(deliverers_free==0){
				tc = pthread_cond_wait(&cond_deliverers,&mutex_deliverers_free);
				
				
				
		}		
				
		deliverers_free--;
		tc=pthread_cond_signal(&cond_deliverers);
		tc=pthread_mutex_unlock(&mutex_deliverers_free);
		
		int deliveryTime=rand_r(&seed) % time_orderhigh + 1;
		sleep(deliveryTime); // time to deliver the pizza
		clock_gettime(CLOCK_REALTIME, &end_service); // get the time at the end of the service, this is the time when the pizzas are delivered and the order is completed
		sleep(deliveryTime); // time for the deliverer to return back to the store
		tc=pthread_mutex_lock(&mutex_deliverers_free);
		deliverers_free++;
		
		tc=pthread_cond_signal(&cond_deliverers);
		tc=pthread_mutex_unlock(&mutex_deliverers_free);
		
		unsigned int service_time = end_service.tv_sec - start_service.tv_sec; //the service time is the time in which the order is delivered(end_service time) - the time where the customer spoke with the telephone operator and made the order(start_service time)
		printf("The service time of the order with id %d is %d minutes\n" , order_id, service_time);
		tc=pthread_mutex_lock(&mutex_max_service_time);
		if(service_time>max_service_time){
			max_service_time=service_time;
		}
		
		tc=pthread_cond_signal(&cond_max_service_time);
		tc=pthread_mutex_unlock(&mutex_max_service_time);
		
		tc=pthread_mutex_lock(&mutex_sum_service_time);
	    
	    sum_service_time+= service_time;
	    tc=pthread_mutex_unlock(&mutex_sum_service_time);
	    unsigned int cooling_time = end_service.tv_sec - start_cooling.tv_sec; //the cooling time is the time in which the order is delivered(end_service time) - the time where the order has been packed by the packer and is waiting to be delivered (start_cooling time)
	    
	    tc=pthread_mutex_lock(&mutex_max_cooling_time);
	    if(cooling_time>max_cooling_time){
	    	max_cooling_time=cooling_time;
	    	
	    }
	    tc=pthread_mutex_unlock(&mutex_max_cooling_time);
	    
	    
	    tc=pthread_mutex_lock(&mutex_sum_cooling_time);
	    sum_cooling_time+= cooling_time;
	    tc=pthread_mutex_unlock(&mutex_sum_cooling_time);
	}




	return NULL;

}//end of customer_routine



int main(int argc, char* arg[]) {



		

		int number_of_customers = atoi(arg[1]);//number of customers
		int seed = atoi(arg[2]);////the seed for generating random numbers
		//with atoi we convert an array of characters to an integer

		pthread_t threads[number_of_customers];//we make an array of threads
		
		int order_ids[number_of_customers];
		
		my_struct structs[number_of_customers];
		
		

		//we initialize the mutexes for the variables of the free employees

		pthread_mutex_init(&mutex_telephone_operators_free, NULL);
		pthread_mutex_init(&mutex_cooks_free, NULL);
		pthread_mutex_init(&mutex_ovens_free, NULL);
		pthread_mutex_init(&mutex_packers_free, NULL);
		pthread_mutex_init(&mutex_deliverers_free, NULL);

		//we initialize the mutexes for the variables of the successful and unsuccessful orders
		pthread_mutex_init(&mutex_successful_counter, NULL);
		pthread_mutex_init(&mutex_unsuccessful_counter, NULL);


		pthread_mutex_init(&mutex_margarita_counter, NULL);
		pthread_mutex_init(&mutex_pepperone_counter, NULL);
		pthread_mutex_init(&mutex_special_counter, NULL);

		pthread_mutex_init(&mutex_total_profit, NULL);
		
		pthread_mutex_init(&mutex_sum_service_time,NULL);
		pthread_mutex_init(&mutex_max_service_time,NULL);
		pthread_mutex_init(&mutex_sum_cooling_time,NULL);
		pthread_mutex_init(&mutex_max_cooling_time,NULL);
		
		
		
		pthread_mutex_init(&mutex_screen, NULL);

		for (int i = 0; i < number_of_customers; i++) {

			
			order_ids[i]=i+1;
			structs[i].v1=order_ids[i];
			structs[i].v2=seed;
			
			
			if (pthread_create(&threads[i], NULL, customer_routine, (void*)&order_ids[i]) != 0) {//we check if the thread was created correctly
				perror("Failed to create thread\n");
				return 1;
			}


			sleep(rand_r(&seed) % time_orderhigh + 1);//time waiting for the next customer to call



		}


		for (int i = 0; i < number_of_customers; i++) {
			if (pthread_join(threads[i], NULL) != 0) {//we check if the thread was joined correctly
				perror("Error joining thread\n");
				return 1;
			}


		}
		
		//print the revenue, total number of pizzas of each kind that were sold and the number of successful/unsuccessful orders, 1st bullet
		printf("The overal revenue is %d $\n",total_profit);
		
		
		printf("%d margarita pizzas were sold\n",margarita_counter);
		printf("%d pepperone pizzas were sold\n",pepperone_counter);
		printf("%d special pizzas were sold\n",special_counter);
		
		
		printf("There were %d successful orders \n",successful_counter);
		printf("There were %d unsuccessful orders \n",unsuccessful_counter);
		
		//print average and maximum service time for successful orders, 2nd bullet
		printf("The average service time is %.2f minutes\n",(float) sum_service_time/successful_counter);
        printf("The maximum service time is %d minutes\n",max_service_time);
        
        //print average and maximum cooling time for successful orders, 3rd bullet
        printf("The average cooling time is %.2f minutes\n", (float)sum_cooling_time/successful_counter);
		printf("The maximun cooling time is %d minutes\n", max_cooling_time);
		
		pthread_mutex_destroy(&mutex_telephone_operators_free);
		pthread_mutex_destroy(&mutex_cooks_free);
		pthread_mutex_destroy(&mutex_ovens_free);
		pthread_mutex_destroy(&mutex_packers_free);
		pthread_mutex_destroy(&mutex_deliverers_free);


		pthread_mutex_destroy(&mutex_unsuccessful_counter);
		pthread_mutex_destroy(&mutex_successful_counter);

		pthread_mutex_destroy(&mutex_margarita_counter);
		pthread_mutex_destroy(&mutex_pepperone_counter);
		pthread_mutex_destroy(&mutex_special_counter);
		
		pthread_mutex_destroy(&mutex_sum_service_time);
		pthread_mutex_destroy(&mutex_max_service_time);
		pthread_mutex_destroy(&mutex_sum_cooling_time);
		pthread_mutex_destroy(&mutex_max_cooling_time);

		pthread_mutex_destroy(&mutex_total_profit);
   
      //destroy the mutexes to free the memory that was occupied by them


		return 0;

} //end of main


