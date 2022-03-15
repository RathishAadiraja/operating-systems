#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>


#define NUM_CHILD 6
#define WITH_SIGNALS

#ifdef WITH_SIGNALS

bool mark = false;

static void handle_interrupt(int sig){

	printf("Interrupt signal recieved by parent[%d]\n",getpid());
	mark = true;

}

static void handle_terminate(int sig){

	printf("Terminate signal recieved by child[%d]\n",getpid());
	mark = true;

}

static void force_ignore(int sig){

    //Force ignoring any signal dummy function
}

#endif

int main(void){

	printf("Total number of child process to be created: %d\n", NUM_CHILD);
	

	for (int i = 0; i < NUM_CHILD; i++){

		printf("Child-%d\n", i+1);


		int created[NUM_CHILD];

		int num = fork();	


		if(num == 0){

			#ifdef WITH_SIGNALS

			signal(SIGINT, SIG_IGN);

			struct sigaction child_sa;
			child_sa.sa_handler = &handle_terminate;
			child_sa.sa_flags = SA_NODEFER;
			sigemptyset(&child_sa.sa_mask);


			sigaction(SIGTERM, &child_sa, NULL);

			#endif



			created[i] = getpid();
			printf("Child process[%d] created by parent[%d]\n", getpid(),getppid());
			sleep(10);
			
			exit(0);

		}	

		#ifdef WITH_SIGNALS

		if(num > 0){

			for (int l = 1; l < NSIG; l++){

				signal(l, force_ignore);

			}

			signal(SIGCHLD, SIG_DFL);

			struct sigaction sa;
			sa.sa_handler = &handle_interrupt;
			sa.sa_flags = SA_NODEFER;
			sigemptyset(&sa.sa_mask);


			sigaction(SIGINT, &sa, NULL);

	

			if(mark){

				sleep(1);

				int length2;
				length2 = sizeof(created)/sizeof(int);

				for(int k = 0; k < length2; k++){

					kill(created[i], SIGTERM);

				}

				printf("process[%d] interrupted, terminating all existing child processes......\n", getpid() );

				i = NUM_CHILD;


			}
			

		}


		#endif


		if(num == -1){

			int length;
			length = sizeof(created)/sizeof(int);

			for(int k = 0; k < length; k++){

				kill(created[i], SIGTERM);

			}
			printf("Fork process failed, process finished by SIGTERM signal\n");

			exit(1);
		}

		sleep(1);
		
	}

	int term_count = 0;

	while(1){

		int temp_pid = wait(NULL);

		if(temp_pid == -1){
			
			break;

		} else{

			printf("Child process[%d] execution completed\n", temp_pid);
			term_count++;
		}
	}

	printf("There are no more child processes\n");
	printf("Number of exit codes recieved: %d\n", term_count);

	#ifdef WITH_SIGNALS

	for (int l = 1; l < NSIG; l++){

		signal(l, SIG_DFL);

	}

	#endif

	return 0;
	
	
}
