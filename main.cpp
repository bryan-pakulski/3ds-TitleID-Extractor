#include <iostream>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>

using namespace std;

// All our functions
void loadCia(char *);
void getID(char *, long);

// Our text file to write to
ofstream titleFile;

// This function returns the title ID on a per file basis
void getID(char * mem, long size)
{
	// The hex value we are searching for is 43 54 52 2D 
	// (which is 'CTR-' we then need to get the next six bytes afterwards in order to extract our title id)
	// Just open up a .cia file in a hex editor and search for CTR- to see exactly what is happening
	//char* hex = '43 54 52 2D';
	unsigned short counter = 0x200;
	unsigned short value;
	
	for (int i = 0; i < size; i++)
	{
		// Get the first value for memory
		value = mem[i] << 8 | mem[i + 1];
		
		// We are looking for hex 43 54 52 2D with these switch statement
		switch (value & 0xFFFF)
		{
			case 0x4354:
				switch ((mem[i + 2] << 8 | mem[i + 3]))
				{
					case 0x522D:
						printf("Found CTR id on line %d\n", i);
						
						// Output hex values
						for (int j = 0; j < 10; j++)
						{
							printf("0x%X  ", mem[i + j]);
						}
						printf("\n");
						
						// This converts the hex value to a string which is printed and saved in our text file
						for (int j = 0; j < 10; j++)
						{
							int n;
							char buffer[2];
							n = sprintf(buffer,"%X", mem[i + j]);
							unsigned long x;
							x = strtoul(buffer, 0, 16);
							printf("%c", (char)x);
							// Add character to text file
							titleFile << (char)x;
						}
						titleFile << " | ";
						printf("\n\n");
				}
		}
	}

}

// This function loads a .cia file
void loadCia(char * filename)
{
	// Only run if the file is a .cia
	if (strstr(filename, ".cia"))
	{ 
		// Load cia
		printf("Loading cia: %s\n", filename);
		titleFile.precision(3);
		titleFile << "\n\n" << filename << "\n\n\t|";
	    FILE * cia = fopen(filename, "rb");
		
		// Get rom size
	    fseek(cia , 0 , SEEK_END);
	    long ciaSize = ftell(cia);
	    rewind(cia);
	    printf("Filesize: %d bytes\n\n", (int)ciaSize);
		
		// Load cia to buffer and allocate memory space for the rom
	    char * ciaBuffer = (char*)malloc(sizeof(char) * ciaSize); 
	    size_t result = fread (ciaBuffer, 1, ciaSize, cia);
		char * memory = new char[ciaSize];
	    
	    // Assign memory locations with buffer values
	    for (int i = 0; i < (int)ciaSize; i++)
	    {
	        memory[i] = ciaBuffer[i];
	    }
	    
	    // Get title id
	    getID(memory, ciaSize);
		
	    // Close file, free buffer and memory
	    delete[] memory;
	    fclose(cia);
	    free(ciaBuffer);
	}
}

// This function gets all cia files within a directory
void checkFolder(char * folder)
{
	DIR * dir;
	struct dirent * ent;
	if ((dir = opendir (folder)) != __null) {
	  while ((ent = readdir (dir)) != __null) {
	  	
	  	if (strstr(ent->d_name, ".cia"))
	    {
		    char str[256];
		    strcpy(str, folder);
		    strcat(str, "\\");
		    strcat(str, ent->d_name);
		    puts(str);
	    
	    	loadCia(str);	
		}
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  perror ("");
	}
}

int main(int argc, char** argv) {
	
	// Check if argument was supplied to the program
	if (argv[1] == __null)
	{
		printf("program usage: 3Tid <cia file>\n");
		return 0;
	}
	
	// Open text file
	titleFile.open ("titles.txt");
	titleFile << "CIA title list";
	
	for (int i = 1; i < argc; i++)
	{
		// Load file
		if (strstr(argv[i], ".cia"))
		{
			loadCia(argv[i]);
		}
		// Load folder
		else
		{
			checkFolder(argv[i]);
		}
		
	}
	
	
	// Close text file
	titleFile.close();
	
	system("pause");
	return 0;
}
