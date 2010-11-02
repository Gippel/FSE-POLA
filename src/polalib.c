/**
* @file polalib.c
* @author Olivier Genser
* @version 1.0
* @brief Reimplementation of the open functions.
*
* @section LICENSE
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this library. If not, see <http://www.gnu.org/licenses/>.
* 
*/


#include "polalib.h"

/**
* Open primitive
*
* Sends back to the reimplemented functions depending on 
* the interactivity is set or not.
*
* @param pathname the name of the file to open
* @param flags the flags to open the file with
* @param mode options
* @return a descriptor on the file to open if it succeeds, -1 otherwise
*/
int open(const char* pathname, int flags, mode_t mode)
{
	if (getenv("INTERACTIVE_MODE") != NULL)
		return inter_open(pathname, flags, mode);
	else
		return non_inter_open(pathname, flags, mode);
}



/**
* Non interactive open function
*
* Reimplementation of the open function by checking if a file has been
* specifically allowed in reading/writing in the command line.
*
* @param pathname the name of the file to open
* @param flags the flags to open the file with
* @param mode options
* @return a descriptor on the file to open if it succeeds, -1 otherwise
*/
int non_inter_open(const char* pathname, int flags, mode_t mode)
{
	char* cmd;
	if ((flags & OPEN_FLAGS) == O_RDONLY)
	{
		// get the command line from the shell polash
		cmd = getenv("POLASH_CMD");			

		// if the file is not specified on the command line
		if (strstr(cmd,pathname) == NULL)	
		{
			fprintf(stderr,"> pola error : read access to %s is not authorized.\n",
				pathname);
			// puts an error message into the system log
			openlog("open :", LOG_NDELAY, LOG_AUTHPRIV);
			syslog(LOG_ERR|LOG_AUTHPRIV, "Non authorized access to %s by user %s",
				pathname, getenv("USERNAME"));
			closelog();
			errno = EACCES; 
			return -1;
		}
	}
	else if (((flags & OPEN_FLAGS) == O_WRONLY) || ((flags & OPEN_FLAGS) == O_RDWR))
	{
		// if the file is not in the writing permission list
		if (strstr(getenv("WRITE_PERM"),pathname) == NULL)
		{
			fprintf(stderr,"> pola error : write access to %s is not authorized.\n",
				pathname);
			// puts an error message into the system log
			openlog("open :", LOG_NDELAY, LOG_AUTHPRIV);
			syslog(LOG_ERR|LOG_AUTHPRIV, "Non authorized access to %s by user %s",
				pathname, getenv("USERNAME"));
			closelog();
			errno = EACCES;
			return -1;
		}
	}
	// if there was no problem, we call the open function into libc
	int (*open_libc)(const char* pathname, int flags, mode_t mode) = NULL;
	open_libc = dlsym(RTLD_NEXT, "open");
	return open_libc(pathname, flags, mode);
}



/**
* Interactive open function
*
* Reimplementation of the open function by asking if, for every files, the user
* wants to allow it in reading/writing.
*
* @param pathname the name of the file to open
* @param flags the flags to open the file with
* @param mode options
* @return a descriptor on the file to open if it succeeds, -1 otherwise
*/
int inter_open(const char* pathname, int flags, mode_t mode)
{
	int flag=-1;
	char rep=0;
	char* buf;
	
	if (((flags & OPEN_FLAGS) == O_RDONLY) || ((flags & OPEN_FLAGS) == O_RDWR))
	{
		// if the file has not already been authorized for the whole command
		if (strstr(getenv("READ_PERM"),pathname) == NULL)
		{
			do
			{
				fprintf(stdout,"> pola request : read access to %s [N/y/r] ? ",
					pathname);
				fflush(stdout);
				fscanf(stdin,"%c",&rep);

				switch(rep)
				{
					case 'y' :
					case 'Y' : 
						flag=O_RDONLY;
						break;
					case 'n' :
					case 'N' :
						fprintf(stderr,"> pola error : read access to %s is not authorized.\n",
							pathname);
						// puts an error message into the system log
						openlog("open :", LOG_NDELAY, LOG_AUTHPRIV);
						syslog(LOG_ERR|LOG_AUTHPRIV, "Non authorized access to %s by user %s",
							pathname, getenv("USERNAME"));
						closelog();
						errno = EACCES; 
						break;
					case 'r' :
					case 'R' :
						buf = malloc(1000*sizeof(char));
						if (buf == NULL)
						{
							perror("Malloc ->");
							exit(1);
						}
						memset(buf,0,1000);
						
						strcat(buf,"+");
						// adds the pathname to the environnment containing
						// reading permissions
						setenv("READ_PERM",strcat(buf,pathname),1);
						free(buf);
						flag=O_RDONLY;
						break;
					default :
						rep = 0;
				}
			}
			while (rep == 0);		
		}
	}
	if (((flags & OPEN_FLAGS) == O_WRONLY) || ((flags & OPEN_FLAGS) == O_RDWR))
	{
		// if the file has not already been authorized for the whole command
		if (strstr(getenv("WRITE_PERM"),pathname) == NULL)
		{
			do
			{
				fprintf(stdout,"> pola request : write access to %s [N/y/r] ? ",
					pathname);
				fflush(stdout);
				fscanf(stdin,"%c",&rep);

				switch(rep)
				{
					case 'y' :
					case 'Y' :
						flag=(flag==O_RDONLY)?O_RDWR:O_WRONLY;
						break;
					case 'n' :
					case 'N' :
						fprintf(stderr,"> pola error : write access to %s is not authorized.\n",
							pathname);
						// puts an error message into the system log
						openlog("open :", LOG_NDELAY, LOG_AUTHPRIV);
						syslog(LOG_ERR|LOG_AUTHPRIV, "Non authorized access to %s by user %s",
							pathname, getenv("USERNAME"));
						closelog();
						errno = EACCES; 
						break;
					case 'r' :
					case 'R' :
						buf = malloc(1000*sizeof(char));
						if (buf == NULL)
						{
							perror("Malloc ->");
							exit(1);
						}
						memset(buf,0,1000);
						
						strcat(buf,"+");	
						// adds the pathname to the environnment containing
						// writing permissions
						setenv("WRITE_PERM",strcat(buf,pathname),1);
						free(buf);
						flag=(flag==O_RDONLY)?O_RDWR:O_WRONLY;
						break;
					default :
						rep = 0;
				}
			}
			while (rep == 0);		
		}
	}
	if (flag == -1)
		return -1;
	// if there was no problem, we call the open function into libc
	int (*open_libc)(const char* pathname, int flags, mode_t mode) = NULL;
	open_libc = dlsym(RTLD_NEXT, "open");
	return open_libc(pathname, flag, mode);
}



/**
* Opendir primitive
*
* Sends back to the reimplemented functions depending on 
* the interactivity is set or not.
*
* @param dirname the name of the directory to open
* @return a descriptor on the direcory to open if it succeeds, NULL otherwise
*/
DIR *opendir(const char* dirname)
{
	if (getenv("INTERACTIVE_MODE") != NULL)
		return inter_opendir(dirname);
	else
		return non_inter_opendir(dirname);
}



/**
* Non interactive open function
*
* Reimplementation of the opendir function by checking if a directory has been
* specifically allowed in reading/writing in the command line.
*
* @param dirname the name of the directory to open
* @return a descriptor on the direcory to open if it succeeds, NULL otherwise
*/
DIR *non_inter_opendir(const char* dirname)
{
	// get the command line from the shell polash
	char* cmd = getenv("POLASH_CMD");	

	// if the directory's name doesn't figure one the command line
	if (strstr(cmd,dirname) == NULL)
	{
		fprintf(stderr,"> pola error : access to %s is not authorized.\n",dirname);
		openlog("opendir :",LOG_NDELAY,LOG_AUTHPRIV);
		syslog(LOG_ERR|LOG_AUTHPRIV,"Non authorized access to directory %s by user %s",
			dirname, getenv("USERNAME"));
		closelog();
		errno = EACCES;
	    return NULL;
	}
	// if there was no problem, call the opendir function into libc
	DIR *(*opendir_libc)(const char *nom) = NULL;
	opendir_libc = dlsym(RTLD_NEXT, "opendir");		
	return opendir_libc(dirname);
}



/**
* Interactive opendir function
*
* Reimplementation of the opendir function by asking if, for every directories, 
* the user wants to allow it in reading/writing.
* 
* @param dirname the name of the directory to open
* @return a descriptor on the direcory to open if it succeeds, NULL otherwise
*/
DIR *inter_opendir(const char* dirname)
{
	char rep=0;
	char* buf;

	// if the directory has not already been authorized for the whole command
	if (strstr(getenv("READ_PERM"),dirname) == NULL)	
	{
		do
		{
			fprintf(stdout,"> pola request : access to %s [N/y/r] ? ",dirname);
			fflush(stdout);
			fscanf(stdin,"%c",&rep);

			switch(rep)
			{
				case 'y' :
				case 'Y' :
					break;
				case 'n' :
				case 'N' :
					fprintf(stderr,"> pola error : access to %s is not authorized.\n",
						dirname);
					// puts an error message into the system log
					openlog("opendir :",LOG_NDELAY,LOG_AUTHPRIV);
					syslog(LOG_ERR|LOG_AUTHPRIV,"Non authorized access to directory %s by user %s",
						dirname, getenv("USERNAME"));
					closelog();
					errno = EACCES;
					return NULL;
				case 'r' :
				case 'R' :
					buf = malloc(1000*sizeof(char));
					if (buf == NULL)
					{
						perror("Malloc ->");
						exit(1);
					}
					memset(buf,0,1000);
					// adds the pathname to the environnment containing
					// reading permissions
					strcat(buf,"+");
					setenv("READ_PERM",strcat(buf,dirname),1);
					free(buf);
					break;
				default :
					rep = 0;
			}
		}
		while (rep == 0);	
	}
	
	// if there was no problem, call the opendir function into libc
	DIR *(*opendir_libc)(const char *nom) = NULL;
	opendir_libc = dlsym(RTLD_NEXT, "opendir");		
	return opendir_libc(dirname);
}

