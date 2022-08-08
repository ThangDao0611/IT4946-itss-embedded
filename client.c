#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAXLINE 1024
#define PORT 3000
#define SHMSZ     4


int clientSocket;
char serverResponse[MAXLINE];
int *shm;
char *shm2;
// int currentVoltage;
char info[1000];
char systemInfo[1000];
int threshold;
int maxThreshold;

int *maxShm;

int kbhit();
int getch();
void showMenuDevices();
void showMenu();
void getResponse();
void makeCommand(char* command, char* code, char* param1, char* param2);
void showMenuAction(char *deviceName, int MODE_DEFAULT, int MODE_SAVING);
void getShareMemoryPointer(char * key_from_server);
void runDevice(int defaultVoltage, int savingVoltage, char* deviceName,int isSaving);
void stopDevice(char *deviceName);
void switchMode(char *deviceName, int newVoltage);
void getInfo(char * key_from_server);

int main(){
	getInfo("5678");
	strcpy(info, shm2);
	getInfo("9999");
	strcpy(systemInfo,shm2);
	char *token;
    token = strtok(systemInfo,"|");
	threshold = atoi(token);
	token = strtok(NULL,"|");
	maxThreshold = atoi(token);

	int shmid;
   	key_t key;
    
   	key = 2345;

    	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        	perror("shmget");
        	exit(1);
    	}

    	if ((maxShm = shmat(shmid, NULL, 0)) == (int *) -1) {
        	perror("shmat");
        	exit(1);
    	}

	struct sockaddr_in serverAddr;
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	while(1){
		showMenuDevices();
	}

	return 0;
}

void showMenuDevices(){
    int choice;
    char c;
	char *a[3];
	char *token;
	while (1) {
		choice = 0;
		printf("---------------Welcome----------------\n");
		printf("Please choose type of device to connect\n");
		printf("1. Television\t\t(1000|500 )\n");
		printf("2. Air Conditioner\t(3000|1000)\n");
		printf("3. Computer\t\t(700 |300 )\n");
		printf("4. Heater\t\t(2000|1500)\n");
		printf("5. Fan\t\t\t(500 |200 )\n");
		printf("6. Quit\n");
		printf("Your choice: ");
		while (choice == 0) {
			if(scanf("%d",&choice) < 1) {
					choice = 0;
			}
				if(choice < 1 || choice > 6) {
						choice = 0;
						printf("Invalid choice!\n");
						printf("Enter again: ");
				}
				while((c = getchar())!='\n') ;
		}

			switch (choice) {
			case 1:
				token = strtok(info,",");
				break;
			case 2:
				token = strtok(info,",");
				token = strtok(NULL,",");
				break;
			case 3:
				token = strtok(info,",");
				token = strtok(NULL,",");
				token = strtok(NULL,",");
				break;
			case 4:
				token = strtok(info,",");
				token = strtok(NULL,",");
				token = strtok(NULL,",");
				token = strtok(NULL,",");
				break;
			case 5:
				token = strtok(info,",");
				token = strtok(NULL,",");
				token = strtok(NULL,",");
				token = strtok(NULL,",");
				token = strtok(NULL,",");
				break;
			default:
				exit(0);
				break;
		}
		a[0] = strtok(token,"|");
		a[1] = strtok(NULL,"|");
		a[2] = strtok(NULL,"|");		
		showMenuAction(a[0],atoi(a[1]),atoi(a[2]));
	}
}

void showMenuAction(char *deviceName, int MODE_DEFAULT, int MODE_SAVING) {
	int choice;
	char c;
	while (1) {
		choice = 0;
		printf("---------------Welcome----------------\n");
		printf("Please choose an action:\n");
		printf("1. Run at normal mode \n");
		printf("2. Run at saving mode\n");
		printf("3. Turn off and quit\n");
		printf("Your choice: ");
		while (choice == 0) {
			if(scanf("%d",&choice) < 1) {
				choice = 0;
			}
			if(choice < 1 || choice > 4) {
				choice = 0;
				printf("Invalid choice!\n");
				printf("Enter again: ");
			}
			while((c = getchar())!='\n') ;
		}
		switch (choice) {
			case 1:
				deviceName = strtok(deviceName,"|");
				runDevice(MODE_DEFAULT,MODE_SAVING,deviceName,0);
				break;
			case 2:
				deviceName = strtok(deviceName,"|");
				runDevice(MODE_DEFAULT,MODE_SAVING,deviceName,1);
			break;
				default:
				exit(0);
		}
	}
}

void getResponse(){
	int n = recv(clientSocket, serverResponse, MAXLINE, 0);
	if (n == 0) {
		perror("The server terminated prematurely");
		exit(4);
	}
	serverResponse[n] = '\0';
}

int kbhit()
{
	struct timeval tv = { 0L, 0L };
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
	    int r;
	    unsigned char c;
	    if ((r = read(0, &c, sizeof(c))) < 0) {
	        return r;
	    } else {
	        return c;
	    }
}

void makeCommand(char* command, char* code, char* param1, char* param2){
        strcpy(command, code);
        strcat(command, "|");
        if (param1 != NULL) {
                strcat(command,param1);
                strcat(command,"|");
        }
        if (param2 != NULL) {
                strcat(command,param2);
                strcat(command,"|");
        }
}

void getShareMemoryPointer(char * key_from_server){
	int shmid;
        key_t key;
	key = atoi(key_from_server);

	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
	    perror("shmget");
	    exit(1);
	};
	if ((shm = shmat(shmid, NULL, 0)) == (int*) -1) {
		perror("shmat");
		exit(1);
	}
}

void getInfo(char * key_from_server){
	int shmid;
    key_t key;
	key = atoi(key_from_server);

	if ((shmid = shmget(key, 1000, 0666)) < 0) {
	    perror("shmget");
	    exit(1);
	}

	if ((shm2 = shmat(shmid, NULL, 0)) == (char*) -1) {
	    perror("shmat");
	    exit(1);
	}
}

void runDevice(int defaultVoltage, int savingVoltage, char *deviceName, int isSaving){
	char command[100];
	char response[100];
	char buffer[20];
	char param[20];
	int countDown;
	int currentVoltage;
	int switchFlag = 0;
	if(isSaving){
		strcat(deviceName,"|SAVING|");
		snprintf(buffer, 10, "%d", savingVoltage);
		currentVoltage = savingVoltage;
	} else {
		strcat(deviceName,"|NORMAL|");
		snprintf(buffer, 10, "%d", defaultVoltage);
		currentVoltage = defaultVoltage;
	}
	// snprintf(buffer, 10, "%d", voltage);
	makeCommand(command,"ON", deviceName,buffer);
	send(clientSocket, command, strlen(command), 0);
	getResponse();
	getShareMemoryPointer(serverResponse);
	countDown = 10;
	while (1) {
		if (*shm<= threshold){
			if(*maxShm <= maxThreshold && switchFlag == 1){
				printf("--------------------------------------\n");
				printf("Over-supply solved!\n");
				printf("The devices switch back to normal mode\n");
				deviceName = strtok(deviceName,"|");
				strcat(deviceName,"|NORMAL|");
				switchMode(deviceName,defaultVoltage);
				currentVoltage = defaultVoltage;
				isSaving = 0;
				switchFlag = 0;
			} else {
				countDown = 10;
				printf("--------------------------------------\n");
				if (isSaving) {
					printf("The current device is running with %dW (SAVING MODE)\nThe total supply currently is %dW\n Press enter to stop this device\n",currentVoltage,*shm);
				} else {
					printf("The current device is running with %dW (NORMAL MODE)\nThe total supply currently is %dW\n Press enter to stop this device\n",currentVoltage,*shm);
				}
			}
		}
		else if(*shm <= maxThreshold){
			if(*maxShm <= maxThreshold && switchFlag == 1){
				printf("The devices switch to normal mode\n");
				deviceName = strtok(deviceName,"|");
				strcat(deviceName,"|NORMAL|");
				switchMode(deviceName,defaultVoltage);
				currentVoltage = defaultVoltage;
				isSaving = 0;
				switchFlag = 0;
			} else {
				countDown = 10;
				printf("--------------------------------------\n");
				printf("Warning! The threshold is exceeded. The total supply currently is %dW\n",*shm);
				if (isSaving) {
					printf("The current device is running with %dW (SAVING MODE)\n Press enter to stop this device\n",currentVoltage);
				} else {
					printf("The current device is running with %dW (NORMAL MODE)\n Press enter to stop this device\n",currentVoltage);
				}
			
			}
		}
		else{
			if (!isSaving) {
				printf("--------------------------------------\n");
				printf("The threshold is exceeded. Switching to saving mode!\n");
				currentVoltage = savingVoltage;
				deviceName = strtok(deviceName,"|");
				strcat(deviceName,"|SAVING|");
				switchMode(deviceName, currentVoltage);
				isSaving = 1;
				switchFlag = 1;
			}
			if (*shm > maxThreshold) {
				printf("Maximum threshold is exceeded. A device will be turn off in %ds\n", countDown);
				countDown--;
				if(countDown < 0){
					countDown = 10;
					stopDevice(deviceName);
					break;
				}
			}
		}

		if (kbhit()) {
			stopDevice(deviceName);
			break;
		}
		sleep(1);
	}
}

void stopDevice(char *deviceName)
{
	char command[100];
	makeCommand(command,"STOP", deviceName, NULL);
	send(clientSocket, command, strlen(command), 0);
	getResponse();
}

void switchMode(char *deviceName, int newVoltage)
{
	char command[100];
	char buffer[20];
	snprintf(buffer, 10, "%d", newVoltage);
	makeCommand(command,"SWITCH", deviceName, buffer);
	send(clientSocket, command, strlen(command), 0);
	getResponse();
}
