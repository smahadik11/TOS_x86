#include "kernel.h"

#define TRAIN_ID "20\0"
#define NONE 0
#define ONE_OR_TWO 1
#define	THREE 3
#define FOUR 4
#define CLOCKWISE 7
#define ANTI_CLOCKWISE 13
#define TRUE 1
#define FALSE 0

static WINDOW train_window =  {0, 0, 80, 8, 0, 0, ' '};
static WINDOW shell_wnd = {0,9,61,16,0,0,0xDC};

int sleep_ticks = 10;//These ticks are used between consecutive commands only.
int train_config = 0;//holds the configuration value
int result = 0;//holds the result of zamboni search
/*=====================================================================================================================================================*/
/**
 * This function creates a Train Process.
 */
void init_train(WINDOW* wnd);
/**
 * String Concatenation Function
 * checks for the EOL
 * s1 [Base String 1]
 * s2 [String to be added at end]
 */
void my_strcat(char*s1,char*s2);
/**
 * This function sends the command to the COM port.
 *  cmd is the command which has to be passed.
 *  buff_len is the input buffer length.
 *  in_buff stores the return value from the COM port.
 */
void tsend(char* cmd, int buff_len, char* in_buff);
/*
 * This function sends the command string bufferto the COM port.
 *  string stores the command which has to be passed.
*/
void send_command(char* string);
/*
* This function continuously polls the contact for the presence of 
* the train or wagon or zamboni. It accepts contact number as the input 
* and gives back the 0 (absence) or 1(presence).
*/
int probe(char* sw);
/*
Change the speed of a vehicle. 
Format => L#S#{CR}
parameter: speed, 0-5 
 */
void speed_control(char* no);
/**
 * This function initializes the outer loop switches.
 */
void init_switch(char*,char*);
/**
 * This function reverses the direction of the train.
 */
void reverse();
/**
 * This function clears the train buffer.
 */
void clear_train_buffer();
/**
 * This function detects the presence of zamboni on the contacts.
 */
int detect_zamboni();
/**
 * This function checks and returns the configuration of the train.
 */
int check_config();
/**
 * This function run the config one/two of the train without zamboni.
 * Basically train detects the wagon ,goes to it and takes back along 
 * with it to its original position.
 */
void run_config1_no_zam();
/**
 * This function run the config three of the train without zamboni.
 */
void run_config3_no_zam();
/**
 * This function run the config four of the train without zamboni.
 */
void run_config4_no_zam();
/**
 * This function run the config one of the train with zamboni.
 * Basically train detects the wagon and zamboni,goes to wagon by avoiding zamboni
 * and takes back along with it to its original position.
 */
void config1_with_zam();
/**
 * This function run the config two of the train with zamboni.
 */
void config2_with_zam();
/**
 * This function run the config three of the train with zamboni.
 */
void config3_with_zam();
/**
 * This function run the config four of the train with zamboni.
 */
void config4_with_zam();
/*======================================================================================================================================================*/
void send_command(char* string){


	char command[15];
	command[0] = '\0';
	char temp[10];
	int len;
	COM_Message msg;
	my_strcat(command,string);
	my_strcat(command,"\015");
	
	msg.output_buffer =command;
   	msg.len_input_buffer = len;
   	msg.input_buffer = temp;
   	
   	sleep(sleep_ticks);
   	send(com_port,&msg);

}
void speed_control(char* no)
{
	char cmd[6];
	cmd[0] = 'L';
	cmd[1] = '\0';
	my_strcat(cmd,TRAIN_ID);
	cmd[3] = 'S';
	cmd[4] = no;
	cmd[5] = '\0';
	send_command(cmd);
}
void reverse()
{
	char cmd[5];
	cmd[0] = 'L';
	cmd[1] = '\0';
	my_strcat(cmd,TRAIN_ID);
	cmd[3] = 'D';
	cmd[4] = '\0';
	send_command(cmd);
}
void clear_train_buffer()
{
	char cmd[2];
	cmd[0] = 'R';
	cmd[1] = '\0';
	send_command(cmd);
}

int probe(char *contact)
{
	char val[3];
	int len = 3;
	char input[3];
	clear_train_buffer();
	val[0] = 'C';
	val[1] = '\0';
	my_strcat(val,contact);
	sleep(sleep_ticks);//delay reqd between two consecutive commands
	tsend(val,3,input);
	if(input[1]=='1')
	{
	return 1;
	}
	else
	{
	return 0;
	}
}

void set_switch(char* switchNum, char* RorG){
	char cmd[5];

	cmd[0] = 'M';
	cmd[1] = switchNum;
	cmd[2] = RorG;
	cmd[3] = '\0';
	send_command(cmd);
}

void init_switches(){
	set_switch('4','G');
	set_switch('5','G');
	set_switch('8','G');
	set_switch('9','R');
	set_switch('1','G');
	
}
void my_strcat(char* s1, char* s2)
{
	while (*s1!='\0') s1++;
	while (*s2!='\0') {
		*s1 = *s2;
		s1++;
		s2++;
	}
	*s1='\0';
}

void tsend(char* cmd, int buff_len, char* in_buff)
	{
			
		COM_Message msg;
		char store_command[20];
		store_command[0] = '\0';
		my_strcat(store_command, cmd);
		my_strcat(store_command, "\015");		// Concatenating "Enter" character in the end of the string.
		msg.output_buffer = store_command;
		msg.len_input_buffer = buff_len;
		msg.input_buffer = in_buff; 
		sleep(sleep_ticks); 				//add delay
		send(com_port, &msg);				// Using TOS API send();
	}
	
int detect_zamboni()
{
	int i;
	int found = FALSE;
	for(i=0;i<60;i++){ 
		sleep(sleep_ticks);
			if(probe("4")){
				found = TRUE;
				break;
			}
		}

	for(i=0;i<60 && found;i++){
		
			sleep(sleep_ticks);
			if(probe("7")){
				return CLOCKWISE;
			}
			sleep(sleep_ticks);
			if(probe("3")){
				return ANTI_CLOCKWISE;
			}
		
	}
	return 0;
}
int check_config()
{
	train_config = 0;
	if((probe("8") == 1) && (probe("2") == 1))
	{
		train_config = ONE_OR_TWO;
	}
	else if((probe("5") == 1) && (probe("11") == 1))
	{
		train_config = THREE;
	}
	else if((probe("5") == 1) && (probe("16") == 1))
	{
		train_config = FOUR;
	}
	else 
	{
		train_config = NONE;
	}
	return train_config;
}

void run_config1_no_zam()
{
	set_switch('6','R');
	set_switch('5','R');
	set_switch('4','R');
	set_switch('3','G');
	speed_control('4');
	while(!probe("1"));
	speed_control('0');	
	reverse();
	speed_control('4');
	while(!probe("8"));
	speed_control('0');	
	wprintf(&train_window,"Train with Wagon has reached to the destination!!\n");
}
void run_config3_no_zam()
{
	set_switch('4','R');
	set_switch('3','R');	
	speed_control('5');	
	set_switch('5','R');
	set_switch('6','G');
	while(!probe("9"));
	set_switch('5','G');
	while(!probe("12"));
	speed_control('0');
	reverse();
	set_switch('7','R');
	speed_control('5');
	while(!probe("6"));
	speed_control('0');
	reverse();
	speed_control('5');
	while(!probe("5"));
	speed_control('0');
	wprintf(&train_window,"Train with Wagon is back to pavillion!!!\n");
}
void run_config4_no_zam()
{

	set_switch('4','R');
	set_switch('3','R');	
	speed_control('5');	
	while(!probe("6"));
	set_switch('4','G');
	speed_control('0');
	reverse();
	speed_control('5');
	while(!probe("14"));
	speed_control('0');
	reverse();
	set_switch('9','G');	
	speed_control('5');
	while(probe("14"));//check if the track is not 14
	sleep(5); //sleep till train reaches on track 16 near the wagon
	speed_control('0');// to catch the wagon lower the speed (dead end)
	reverse();
	speed_control('5');
	while(!probe("6"));
	set_switch('4','R');
	speed_control('3');//To return to home lower the speed (dead end)
	while(!probe("5"));
	speed_control('0');
	wprintf(&train_window,"Train with Wagon is back to pavillion!!!\n");
}
void config1_with_zam()
{
	wprintf(&train_window,"TRAIN CONFIGURATION 1 WITH ZAMBONI\n");
	set_switch('7','R');
	set_switch('2','R');
	while(!probe("10"));
	set_switch('1','R');
	wprintf(&train_window,"Zamboni is Trapped\n");
	run_config1_no_zam();
}

void config2_with_zam()
{
	wprintf(&train_window,"TRAIN CONFIGURATION 2 WITH ZAMBONI\n");
	set_switch('7','R');
	set_switch('2','R');
	while(!probe("3"));
	set_switch('8','R');
	set_switch('1','R');
	wprintf(&train_window,"Zamboni is Trapped\n");
	run_config1_no_zam();
}
void config3_with_zam()
{
	wprintf(&train_window,"TRAIN CONFIGURATION 3 WITH ZAMBONI\n");
	set_switch('4','R');
	set_switch('3','R');	
	while(!probe("10"));
	speed_control('5');	
	set_switch('5','R');
	set_switch('6','G');
	while(!probe("9"));
	set_switch('5','G');
	set_switch('2','R');
	while(!probe("12"));
	speed_control('0');
	reverse();
	set_switch('7','R');
	while(!probe("13"));
	speed_control('5');
	while(!probe("6"));
	speed_control('0');
	sleep(sleep_ticks);
	speed_control('5');
	while(!probe("6"));
	speed_control('0');
	reverse();
	speed_control('4');
	while(!probe("5"));
	speed_control('0');
	wprintf(&train_window,"Train with Wagon is back to pavillion!!!\n");

}

void  config4_with_zam()
{
	wprintf(&train_window,"TRAIN CONFIGURATION 4 WITH ZAMBONI\n");
	while(!probe("4"));	
	set_switch('4','R');
	set_switch('3','R');	
	speed_control('5');	
	while(!probe("6"));
	set_switch('4','G');
	speed_control('0');
	reverse();
	speed_control('5');
	while(!probe("14"));
	speed_control('5');
	while(!probe("3"));
	while(!probe("14"));
	speed_control('0');
	reverse();
	set_switch('9','G');	
	speed_control('4');
	while(probe("14"));
	sleep(120);
	speed_control('0');
	reverse();
	while(!probe("14"));		
	speed_control('5');
	while(!probe("4"));
	speed_control('0');
	while(!probe("6"));
	set_switch('4','R');
	speed_control('3');
	while(!probe("5"));
	speed_control('0');
	set_switch('4','G');
	wprintf(&train_window,"Train with Wagon is back to pavillion!!!\n");
	
}

void train_process(PROCESS self, PARAM param)
{
	clear_window(&train_window);
	wprintf(&train_window,"Starting Train Application\n");
	wprintf(&train_window,"Initializing Switches\n");
	init_switches();
	wprintf(&train_window,"Initialization successful\n");
	wprintf(&train_window,"Searching for Zamboni...\n");
	init_switches();
	result = detect_zamboni();
	if(result){
		wprintf(&train_window,"Zamboni Found\n");
		if(result==7){
			wprintf(&train_window,"Zamboni Is rotating Clock-Wise\n");	
		}else if(result==13){
			wprintf(&train_window,"Zamboni Is rotating Anti Clock-Wise\n");	
		}
	}
	else 
		wprintf(&train_window,"Zamboni not found\n");	

	train_config = check_config();
	switch(train_config){
		case NONE:
			wprintf(&train_window,"TRAIN CONFIGURATION IS NOT FOUND\n");
			break;
		case ONE_OR_TWO:
			if(result == 0) 
			{
				wprintf("result = %d\n",result);				
				wprintf(&train_window,"TRAIN CONFIGURATION 1 or 2 WITHOUT ZAMBONI\n");
				run_config1_no_zam();
			}
			break;
		case THREE:
			if(result == 0)
			{
				wprintf(&train_window,"TRAIN CONFIGURATION 3 WITHOUT ZAMBONI\n");
				run_config3_no_zam();
			} 
			break;
		case FOUR:
			if(result == 0)
			{
				wprintf(&train_window,"TRAIN CONFIGURATION 4 WITHOUT ZAMBONI\n");
				run_config4_no_zam();
			}
			break;
	}
	if(result == CLOCKWISE && train_config == ONE_OR_TWO)
		config1_with_zam();		
	else if((result == ANTI_CLOCKWISE && train_config == ONE_OR_TWO))
		config2_with_zam();
	else if(train_config == THREE)
		config3_with_zam();
	else if(train_config == FOUR)
		config4_with_zam();
	wprintf(&train_window,"Closing Train Application\n");
	result = 0;
	train_config = 0;
	remove_ready_queue(active_proc);
	resign();
	while(1);
}


void init_train(WINDOW* wnd)
{
	create_process(train_process,5, 0, "Train Process");
	clear_window(&train_window);
	resign();
}
