/**
* @file pola-i.c
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

#include "pola-i.h"

int main (int argc, char** argv)
{
	// set the environnment to use polalib rather than libc
    setenv("LD_PRELOAD", "./lib/polalib.so", 1);
	// and tell the lib to use the interactive mode
    setenv("INTERACTIVE_MODE", "1", 1);
	setenv("WRITE_PERM", "", 1);
	setenv("READ_PERM", "", 1);

	int i;

    if (argc < 2)
    {
        printf("Usage : pola-i <command> [arg1] [arg2] ...\n");
        exit(-1);
    }
	// removes the "pola-i" from the command line
	for (i=1; i<argc; i++)
        argv[i-1] = argv[i];
    argv[i-1] = NULL;
    // executes the command
   	execvp(argv[0], argv);
    perror("Error while executing the command -> ");
   	
	return 0;
}
