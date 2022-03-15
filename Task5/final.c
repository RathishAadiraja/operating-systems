#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h> 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


#define PHIL_COUNT 5

int semid = -1;

int total_times[PHIL_COUNT-1];
int eat_time[PHIL_COUNT-1];
int think_time[PHIL_COUNT-1];

//int phil_arr[PHIL_COUNT-1];

void generate_random_numbers(){

	for(int i = 0; i < PHIL_COUNT; i++){

		total_times[i] = (rand()%(6))+1;
		eat_time[i] = (rand()%(5))+1;
		think_time[i] = (rand()%(4))+1;
	}

}


void grab_forks(int left_fork_id);
void put_away_forks(int left_fork_id);

void set_sem_op(int left_fork_id, int sem_op);

void grab_forks(int left_fork_id) { return set_sem_op(left_fork_id, -1); }
void put_away_forks(int left_fork_id) { return set_sem_op(left_fork_id, 1); }



void set_sem_op(int left_fork_id, int sem_op)
{

    struct sembuf sops[2];

    sops[0].sem_num = left_fork_id;
    sops[1].sem_num = (left_fork_id + 1) % PHIL_COUNT;


    sops[0].sem_op = sem_op;
    sops[0].sem_flg = 0;

    sops[1].sem_op = sem_op;
    sops[1].sem_flg = 0;


    if (semop(semid, sops, 2) == -1)
    {
        if(sem_op == -1){
        	printf("grabbing forks failed. \n");
        }else{
        	printf("putting away forks failed. \n");
        }
        
    }
}

int philosopher(int id)
{
    printf("Philosopher[%i] created with pid[%i]\n", id, getpid());
    generate_random_numbers();
    sleep(3);

    //phil_arr[id] = getpid();
    int no_of_times = total_times[id];
    
    printf("Philosopher[%i]------------------------------ can eat total of '%i' times\n", id, no_of_times);
    bool cond = true;

    while(cond)
    {
        
	    if(no_of_times > 0){
	
	        printf("Philosopher[%i] trying to grab forks...\n", id);
	        grab_forks(id);
	        

	        printf("Philosopher[%i] grabbed------------------------------[FORK_%i] and [FORK_%i]\n", id, id, (id + 1) % PHIL_COUNT);
	        sleep(eat_time[id]);

	        printf("Philosopher[%i] trying to put away forks...\n", id);
	        put_away_forks(id);
	        no_of_times--;

	        printf("Philosopher[%i] puts away----------------------------[FORK_%i] and [FORK_%i] and lets other philosopher eat\n", id, id, (id + 1) % PHIL_COUNT);
	        sleep(think_time[id]);

	    }else{

	    	cond = false;
	    	printf("Philosopher[%i] puts away----------------------------[FORK_%i] and [FORK_%i] permanently.------He finished eating.\n", id, id, (id + 1) % PHIL_COUNT);
	    	sleep(24);
	        
	    	break;
	    }
    }

}

int main(int argc, const char * argv[])
{

    printf("Total number of philosophers-----%i\n\n", PHIL_COUNT);


    semid = semget(IPC_PRIVATE, PHIL_COUNT, 0644 | IPC_CREAT);


    if (semid == -1)
    {
        printf("Creating semaphores failed");
        return 1;
    }


    for (int i = 0; i < PHIL_COUNT; ++i)
    {
        if (semctl(semid, i, SETVAL, 1) == -1)
        {
            printf("Controlling created semaphores failed");
            return 1;
        }
    }


    for (int i = 0; i < PHIL_COUNT; ++i)
    {
        pid_t process_id = fork();

        if (process_id == -1)
        {
            printf("Fork operation failed. Cannot create child processes.");
            return 1;
        }

        if (process_id == 0){
        	return philosopher(i);
       
        }
            
    }


    int term_count = 0;

	while(1){

		int temp_pid = wait(NULL);

		if(temp_pid == -1){
			
			break;

		} else{

			printf("Philosopher[%d] execution completed\n", temp_pid);
			term_count++;
			
		}
	}

	printf("There are no more running processes\n");
	printf("Number of exit codes recieved: %d\n", term_count);


    if (semctl (semid, 0, IPC_RMID, 1) < 0){
        printf("Destroying semaphores failed");
    }

    return 0;
}
