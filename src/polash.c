/**
* @file polash.c
* @author Olivier Genser
* @version 1.0
* @brief Minimalist shell that executes function.
*
* @section LICENSE
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
* 
*/


#include "polash.h"


int setbin()
{
	char *tmp;
	tmp = malloc(1000*sizeof(char*));
	if (tmp == NULL)
	{
		perror("Malloc ->");
		exit(1);
	}
	memset(tmp,0,1000);
	
	// add the bin direcory from the project to the environment variable $PATH
	strcpy(tmp,getenv("PATH"));
	strcat(tmp,":");
	strcat(tmp, getenv("PWD"));
	strcat(tmp, "/bin");
	setenv("PATH",tmp,1);
	free(tmp);
	return 0;	
}


char** parser(char* buf)
{
	int i=0,j=0,k=0, decal=0, quote=0;
	char *tmp;
	char **cmd;
	cmd = malloc(nbargs*sizeof(char*));

	if (cmd == NULL)
	{
		perror("Malloc ->");
		exit(1);
	}

	tmp = malloc(1000*sizeof(char));
	if (tmp == NULL)
	{
		perror("Malloc ->");
		exit(1);
	}
	memset(tmp,0,1000);

	for (i=0; i<nbargs; i++)
	{
		cmd[i]=(char*)malloc(100*sizeof(char));
		if (cmd[i]==NULL)
		{
			perror("Malloc ->");
			exit(1);
		}
	}
	i=0;

	do		// this part is like strtok but keep the double quotes
	{
		// if we find a space or a '\0'
		if (buf[i]==' ' || buf[i]=='\0' )
		{
			if (quote)			// if we are between double qoutes, we keep the 
								// space
			{
				cmd[j][k]=' ';
				k++;
			}
			else				// otherwise we end the string
			{
				cmd[j][k]='\0';
				if (cmd[j][0]=='+')			// if a file is precedeed by a '+'
				{
					//we add it to the environnment containing write permissions
					setenv("WRITE_PERM",strcat(tmp,cmd[j]),1); 
					// and remove the file from the arguments
					j--;					
				}
				// if a file is surrounded by '< >'
				if (cmd[j][0]=='<' && cmd[j][k-1]=='>')		
				{
					cmd[j][k-1]='\0';
					// we remove those '< >'
					for (decal=0; decal<k; decal++)			
						cmd[j][decal]=cmd[j][decal+1];
					strcat(tmp,"+");
					//we add it to the environnment containing write permissions
					setenv("WRITE_PERM",strcat(tmp,cmd[j]),1);	
				}
				j++;
				k=0;

			}
		}
		else
		{			
			if (buf[i]=='\"')	// if there is a double quote we notice it
				quote=(quote+1)%2;
			cmd[j][k]=buf[i];
			k++;
		}
	}
	while (buf[i++] != '\0');
	cmd[j]=NULL;
	free(tmp);
	return cmd;
}



int main (int argc, char** argv)
{
	// set the environnment to use polalib rather than libc
	setenv("LD_PRELOAD", "./lib/polalib.so",1);

	int i, quit=0;
	pid_t pid;
	char *buf;
	char **cmd;
	
	// add the bin direcory from the project to the environment variable $PATH
	setbin();

	buf = malloc(1024*sizeof(char));
	if (buf == NULL)
	{
		perror("Malloc ->");
		exit(1);
	}
	memset(buf,0,1024);

	do
	{
		// set the environment for the polalib to know which files/directories 
		setenv("WRITE_PERM", "", 1);	// are allowed for writing

		buf[0]='\0';
		while (strlen(buf)<2)		//while there is nothing on the command line
		{
			printf("# ");
			fflush(stdin);

			if (fgets(buf, 1023, stdin)==NULL)
			{
				perror("Error on fgets ->");
				exit(-1);
			}
		}	

		buf[strlen(buf)-1]='\0';

		if (strcmp(buf, "exit") == 0)
		{
			quit=1;
			break;
		}

		setenv("POLASH_CMD", buf ,1);	// setting the environnment variable
										// for the library to check it
		cmd = parser(buf);		// parse the function to separate its arguments

		if ((pid = fork()) == -1)
		{
			perror("fork -> ");
		}
		else if (pid ==0)
		{
			execvp(cmd[0],cmd);			// executing the command
            perror("Error while executing the command -> ");
		} 
		else
			wait(NULL);	

		for (i=0; i<nbargs; i++)
			free(cmd[i]);
		free(cmd);
	}
	while (quit != 1);

	free(buf);
	return 0;
}

