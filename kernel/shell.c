#include <kernel.h>
#define MAX_BUFLEN 60

static WINDOW shell_wnd = {0,12,61,13,0,0,0xDC};
static WINDOW train_window=  {0, 0, 80, 8, 0, 0, ' '};

char command[MAX_BUFLEN];//hold the command characters
int cmdLegnth = 0;//cmd legnth
int i =0;//for command buffer index tracking

void echo_shell(char *ptr);
void about_shell(char *data);
void clear_shell(char *data);
void print_help();
void help_shell(char *data);
void process_print_shell(char *data);
void clear_buff();
int filter_spacechar(char *data);
void execute_cmd();
void ShellProcess(PROCESS self,PARAM param);
void init_shell();

/**
 * This function echos whatever written in shell window  .
 * para ptr holds the data
 */
void echo_shell(char *ptr)
{
 	wprintf(&shell_wnd,ptr);
	wprintf(&shell_wnd,"\n");
}
/**
 * This function prints the information about the operating system and
 the author of the shell window code.
 *
 */
void about_shell(char *data)
{
	if(*data !='\0')
	{
		if(!(filter_spacechar(data)))
			wprintf(&shell_wnd,"Train Operating System Shell V1.0-- Author:Sudha\n");
			else
			wprintf(&shell_wnd,"\nInvalid command: Type help for more info\n");
	}
		else
		wprintf(&shell_wnd,"Train Operating System Shell-- Author:Sudha\n");
 }
/**
 * This function Clears the shell window  .
 * wnd has the dimesions of the window.
 */
void clear_shell(char *data)
{
    if(*data !='\0')
	{
		if(!(filter_spacechar(data)))
		{
			clear_window(&shell_wnd); //API for clear window
			wprintf(&shell_wnd,"WELCOME TO THE TOS SHELL\n");
		}
		else
			wprintf(&shell_wnd,"\nInvalid command: Type help for more info\n");
 	}
	else
	{
		clear_window(&shell_wnd); //API for clear window
		wprintf(&shell_wnd,"WELCOME TO THE TOS SHELL\n");
	}
}
void print_help() //Api for help command
{
 	wprintf(&shell_wnd,"Welcome to HELP\n");
 	wprintf(&shell_wnd,"Following are the list of commands that you may use:\n");
 	wprintf(&shell_wnd,"help                  - for displaying help information\n");
 	wprintf(&shell_wnd,"about                 - Author information\n");
 	wprintf(&shell_wnd,"ps                    - list all processes\n");
 	wprintf(&shell_wnd,"cls                   - clear window\n");
 	wprintf(&shell_wnd,"echo {string}         - Echo the string passed\n");
	wprintf(&shell_wnd,"train	          - Initialize train setting\n");
	wprintf(&shell_wnd,"run		          - Starts the train\n");
	wprintf(&shell_wnd,"stop	          - Stops the train\n");
}
/**
 * This function shows the commands supported by the shell. 
 */
void help_shell(char *data)//data parameter passed  to check if any 
{							//unwanted chars or spaces are typed after the command
    if(*data !='\0')
	{
		if(!(filter_spacechar(data)))
			print_help();
		else
			wprintf(&shell_wnd,"\nInvalid command: Type help for more info\n");
 	}
	else
		print_help();//API for Help
}

void start_train()
{
	init_train(&train_window);
}

void train_run()
{
	char temp;	
	tsend("L20S4",0, &temp);
}

void train_stop()
{
	char temp;	
	tsend("L20S0",0, &temp);
}
/**
  * This function prints all the process created in Tos
  */
void process_print_shell(char *data)
{
	if(*data !='\0')
	{
		if(!(filter_spacechar(data)))
			print_all_processes(&shell_wnd);
		else
			wprintf(&shell_wnd,"\nInvalid command: Type help for more info\n");
	}
	else
 	print_all_processes(&shell_wnd);
}
/**
  * This function clears the command buffer
  */
void clear_buff()
{
	int i;
	for ( i= 0; i < (MAX_BUFLEN-1); ++i)
	{
		command[i] = '\0';
	}

}
/**
  * This function check for unneccessary characters and spaces after the command word
  and if only spaces are present then allow the command to work by returning 0 and if any 
  invalid char is present then returns 1.
  */
int filter_spacechar(char *data)
{
	int i = 0;
	for(i=0;i<(MAX_BUFLEN-cmdLegnth);i++)
	{
		if(data[i] ==' ')
		{ 
			continue;
		}
		if((data[i] != '\0') && (data[i] != '\n') && (data[i] != '\t')&&(data[i] !=' ')) 
		{
			return 1;
		}
		return 0;
    }
	return 0;
}
/**
 * This function runs the commands supported by the shell. 
 */
void execute_cmd()
{
	char* cmdData;
	char* cmdName;
	int k;
	cmdName = command;
	for(k=0;k<(MAX_BUFLEN-1);k++)//identify command and data part by detecting first space char
	{
	  if(command[k] == ' ')
	  { 
		cmdData = &command[k+1]; //assign pointter to data parameter
 		cmdLegnth = k; 
		break;
	  }
        cmdData = &command[i];//if only command no data parameter e.g help
 	}
	if((k_memcmp(cmdName,"echo",cmdLegnth)) == 0)
		{echo_shell(cmdData);}
	else if(!(k_memcmp(cmdName,"about",cmdLegnth)))
		{about_shell(cmdData);}
	else if(!(k_memcmp(cmdName,"cls",cmdLegnth)))
		{clear_shell(cmdData);}
	else if(!(k_memcmp(cmdName,"help",cmdLegnth)))
		{help_shell(cmdData);}
	else if(!(k_memcmp(cmdName,"ps",cmdLegnth)))
		{process_print_shell(cmdData);}
	else if(!(k_memcmp(cmdName,"train",cmdLegnth)))
	{
		start_train();
	}
	else if(!(k_memcmp(cmdName,"run",cmdLegnth)))
	{
		//train_run();
		speed_control('4');
	}
	else if(!(k_memcmp(cmdName,"stop",cmdLegnth)))
	{
//		train_stop();
		speed_control('0');
	}
	else if(!(k_memcmp(cmdName,"reverse",cmdLegnth)))
	{
		reverse();
	}
	else 
	   wprintf(&shell_wnd,"Invalid command: Type help for more info\n");
	clear_buff();
}
/**
 * This is a shell process which reads the characters(commands) from the keyboard
 and execuutes them.
 */
void ShellProcess(PROCESS self,PARAM param)//Shel Process
{
	char ch;
	Keyb_Message msg;
	clear_window(kernel_window);//new
	clear_window(&shell_wnd);// Clearing the shell window.		
	clear_buff();
	while(1)
	{
		wprintf(&shell_wnd,"WELCOME TO THE TOS SHELL\n");
		output_char(&shell_wnd,'#');
		while(1)
		{
			msg.key_buffer = &ch;
			send(keyb_port,&msg);
			switch(ch)
			{
				case 13:			//pressed Enter key
					command[i]='\0';
					wprintf(&shell_wnd,"\n");
					if(i!=0) // excute command only when there is data in buffer
					{
						execute_cmd();
					}
					i=0;
					cmdLegnth = 0;
					clear_buff();
					output_char(&shell_wnd,'#');
				break;
				case 8:				//press backspace key
				    if(i==0)
				    {
						continue;
				    }
				    command[i] = '\0';
				    i--;
				    cmdLegnth--;
				    wprintf(&shell_wnd,"%c",ch);
				break;
				default:			//otherwise read characters
					if(i<58)			//check for overflow
					{
						if(ch != ' ')
					 	{
					        command[i] = ch;
							cmdLegnth++;	
					    	i++;
					    }
					    else if((cmdLegnth != 0)&&(ch == ' ')) // to ignore spaces at initial of command
					 	{
					        command[i] = ch;
							i++;
						}
					 wprintf(&shell_wnd,"%c",ch);
					}
		     	break;
	     	}//end of switch
		}//end of while 2
	}//end of while 1
}

void init_shell()
{
    create_process(ShellProcess,5,0,"Shell");	//create shell process with priority 6
    resign();   
}
