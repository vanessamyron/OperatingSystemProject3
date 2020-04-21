#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
//#include <bits/stdc++.h>


typedef struct
{
    char** tokens;
    int numTokens;
} instruction;

typedef struct{
	unsigned char DIR_name[11];
	unsigned char DIR_Attributes;
	unsigned char DIR_NTRes;
	unsigned char DIR_CrtTimeTenth;
	unsigned short DIR_CrtTime;
	unsigned short DIR_CrtDate;
	unsigned short DIR_LstAccDate;
	unsigned short DIR_FstClusHI;
	unsigned short DIR_WrtTime;
	unsigned short DIR_WrtDate;
	unsigned short DIR_FstClusLO;
	unsigned long DIR_FileSize;
	
} __attribute__((packed)) DirEntry;

typedef struct {
	unsigned char name[100];
	unsigned int curr_clust_num;
	char curr_clust_path[50];
	char curr_path[50][100];
	int curr;
}__attribute__((packed)) ENVIR;

// Global Variables
ENVIR environment;
int f;

unsigned short BPB_BytsPerSec;	
unsigned char BPB_SecPerClus;	
unsigned short BPB_RsvdSecCnt;	
unsigned char BPB_NumFATs;	
unsigned int BPB_TotSec32;
unsigned int BPB_FATSz32;	
unsigned int BPB_RootClus;	

int FirstDataSector;


//Functions 

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
int firstSectorOfCluster(unsigned int clusterNum);
int size(char* arg, int image);
void create(char* arg, int image);
void mkdir(char* arg, int image);
int nextEmptyClus(int image);
int cd(char *name, int directoryClust);
void ls(int image, unsigned int clusNum);

int main() {
	
	
	int f = open("fat32.img", O_RDWR);
	pread(f, &BPB_BytsPerSec, 2, 11);
	pread(f, &BPB_SecPerClus, 1, 13);
	pread(f, &BPB_RsvdSecCnt, 2, 14);
	pread(f, &BPB_NumFATs, 1, 16);
	pread(f, &BPB_TotSec32, 4, 32);
	pread(f, &BPB_FATSz32, 4, 36);
	pread(f, &BPB_RootClus, 4, 44);
	

	



	// to include the cluster_num and clust_name to the curr path
	int curr_cluster = 0;
	environment.curr = 0;
	char *pathName = "/";
	environment.curr_clust_num = BPB_RootClus;
	strcpy((char *)environment.name, pathName);
	strcpy(environment.curr_path[environment.curr], pathName);

	environment.curr_clust_path[environment.curr] = BPB_RootClus;
	++environment.curr;

	//calculate the FirstDataSector
	FirstDataSector = BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz32);
	
	
		//Variables for Intake
	char* token = NULL;
	char* temp = NULL;

	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;
	
	

	while (1) {
		// for the current environment info
		char *user = getenv("USER");
		char *machine = getenv("MACHINE");
		char *pwd = getenv("PWD");
			
		printf("%s@%s:", user, machine);

			int j = 0;
		while(j < environment.curr)
		{
			printf("%s/", j == 0 ? "" : environment.curr_path[j]);
			j++;
		}
		printf("> ");


		// loop reads character sequences separated by whitespace
		do {
			//scans for next token and allocates token var to size of scanned token
			scanf("%ms", &token);
			temp = (char*)malloc((strlen(token) + 1) * sizeof(char));

			int i;
			int start = 0;

			for (i = 0; i < strlen(token); i++) {
				//pull out special characters and make them into a separate token in the instruction
				if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&') {
					if (i-start > 0) {
						memcpy(temp, token + start, i - start);
						temp[i-start] = '\0';
						addToken(&instr, temp);
					}

					char specialChar[2];
					specialChar[0] = token[i];
					specialChar[1] = '\0';

					addToken(&instr,specialChar);

					start = i + 1;
				}
			}

			if (start < strlen(token)) {
				memcpy(temp, token + start, strlen(token) - start);
				temp[i-start] = '\0';
				addToken(&instr, temp);
			}

			//free and reset variables
			free(token);
			free(temp);

			token = NULL;
			temp = NULL;
		} while ('\n' != getchar());    //until end of line is reached

		addNull(&instr);
		
		//Add functions here
		
		
		
		if(strcmp(instr.tokens[0], "exit") == 0){
			if(close(f) != 0){
				printf("There was a problem close fat32.img\n");
			}
			else{
				clearInstruction(&instr);
				printf("Successfully Exited\n");
			}
			break;
		}

		if(strcmp(instr.tokens[0], "info") == 0) {
			printf("%s%hu\n", "Bytes per sector: ", BPB_BytsPerSec);
			printf("%s%u\n", "Sectors per cluster: ", BPB_SecPerClus);	
			printf("%s%hu\n", "Reserved sector count: ", BPB_RsvdSecCnt);	
			printf("%s%u\n", "Number of FATs: ", BPB_NumFATs);	
			printf("%s%u\n", "Total sectors: ", BPB_TotSec32);	
			printf("%s%u\n", "FAT size: ", BPB_FATSz32);
			printf("%s%u\n", "Root Cluster: ", BPB_RootClus);
		}
		
		if(strcmp(instr.tokens[0], "size") == 0) {
			int s;
			s = size(instr.tokens[1], f);
			if(s >= 0)
				printf("%u\n", s);  
		}
		
		
		if(strcmp(instr.tokens[0], "create") == 0){
			create(instr.tokens[1], f);
		}

		if(strcmp(instr.tokens[0], "mkdir") == 0){
			mkdir(instr.tokens[1], f);
		}
		
		clearInstruction(&instr);
	}
	
	
	
	



    return 0;
}

void addToken(instruction* instr_ptr, char* tok)
{
    //extend token array to accomodate an additional token
    if (instr_ptr->numTokens == 0)
        instr_ptr->tokens = (char**) malloc(sizeof(char*));
    else
        instr_ptr->tokens = (char**) realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

    //allocate char array for new token in new slot
    instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok)+1) * sizeof(char));
    strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

    instr_ptr->numTokens++;
}

void addNull(instruction* instr_ptr)
{
    //extend token array to accomodate an additional token
    if (instr_ptr->numTokens == 0)
        instr_ptr->tokens = (char**)malloc(sizeof(char*));
    else
        instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

    instr_ptr->tokens[instr_ptr->numTokens] = (char*) NULL;
    instr_ptr->numTokens++;
}

void printTokens(instruction* instr_ptr)
{
    int i;
    printf("Tokens:\n");
    for (i = 0; i < instr_ptr->numTokens; i++) {
        if ((instr_ptr->tokens)[i] != NULL)
            printf("%s\n", (instr_ptr->tokens)[i]);
    }
}

void clearInstruction(instruction* instr_ptr)
{
    int i;
    for (i = 0; i < instr_ptr->numTokens; i++)
        free(instr_ptr->tokens[i]);

    free(instr_ptr->tokens);

    instr_ptr->tokens = NULL;
    instr_ptr->numTokens = 0;
}

// Obtain the first sector of clusters 
int firstSectorOfCluster(unsigned int clusterNum)
{
	int firstSectorOfClust= (((clusterNum -2) * BPB_SecPerClus) + FirstDataSector) * BPB_BytsPerSec;
	return firstSectorOfClust;

}


int size(char* arg, int image){
	
	DirEntry tempDir;
	unsigned int x = environment.curr_clust_num;	//First cluster we check
	unsigned int byteOffSet;
	unsigned char firstBit;
	
	
	char* space = " ";
	int i;
	for(i = strlen(arg); i < 11; i++){
		strcat(arg, space);
	}
	
	
	while(x < 0x0FFFFFF8){
		
		byteOffSet = BPB_BytsPerSec * (FirstDataSector + ( x - 2) * BPB_SecPerClus);
		
		int i;
		
		do{

			pread(image, &tempDir, sizeof(DirEntry), byteOffSet);	//intake entire dir entry
			if(tempDir.DIR_Attributes != 15){
				int j;
				for(j = 0; j < 11; j++){
					printf("%c", tempDir.DIR_name[j]);
				}
				printf("\n");
				
				//Print size if name matches of directory matches arg
				if(strncmp(tempDir.DIR_name, arg, 11) == 0)
					return tempDir.DIR_FileSize;
		}
			byteOffSet += 32;
		}while(tempDir.DIR_name[0] != 0);
		
		
		pread(image, &x, 4, BPB_RsvdSecCnt * BPB_BytsPerSec + x * 4);	//Should read the value at the current cluster number and make it the new curr cluster number
		
	}
	
	printf("File does not exist\n");
	return -1;
	
}

void create(char* arg, int image){
	
	
	DirEntry temp;
	unsigned int eofValue = 0x0FFFFFF8;
	unsigned int tempClus;
	
	tempClus = nextEmptyClus(image);
	
	//printf("%s%i\n", "EmptyClus Found: ", tempClus);
	
	//Write to empty clus number EOFValue
	pwrite(image, &eofValue, 4, BPB_RsvdSecCnt * BPB_BytsPerSec + tempClus * 4);
	
	int i;
	for(i = 0; i < strlen(arg); i++){
		temp.DIR_name[i] = arg[i];
	}
	
	//Pad name with spaces
	for(i = strlen(arg); i < 11; i++){
		temp.DIR_name[i] = 32;
	}
	
	unsigned int byteOffSet;
	byteOffSet = BPB_BytsPerSec * (FirstDataSector + ( tempClus - 2) * BPB_SecPerClus);
	temp.DIR_FstClusLO = 0xFFFF & byteOffSet;
	temp.DIR_FstClusHI = (byteOffSet >> 16) & 0xFFFF;
	temp.DIR_FileSize = 0;
	
	//Putting the actual directory entry into the directory (What if the directory is full?)
	
	
	byteOffSet = BPB_BytsPerSec * (FirstDataSector + ( environment.curr_clust_num - 2) * BPB_SecPerClus);
	DirEntry itr;
	
	//Read throught the directory until an unnocupied space is found
	while(1){
		pread(image, &itr, sizeof(itr), byteOffSet);
		if(itr.DIR_name[0] == 0){
			//Write new file into empty space
			pwrite(image, &temp, sizeof(temp), byteOffSet);
			return;
		}
		byteOffSet += 32;
	};
}

void mkdir(char* arg, int image){
	
	
	DirEntry temp;
	unsigned int eofValue = 0x0FFFFFF8;
	unsigned int tempClus;
	
	tempClus = nextEmptyClus(image);
	
	//printf("%s%i\n", "EmptyClus Found: ", tempClus);
	
	//Write to empty clus number EOFValue
	pwrite(image, &eofValue, 4, BPB_RsvdSecCnt * BPB_BytsPerSec + tempClus * 4);
	
	int i;
	for(i = 0; i < strlen(arg); i++){
		temp.DIR_name[i] = arg[i];
	}
	
	//Pad name with spaces
	for(i = strlen(arg); i < 11; i++){
		temp.DIR_name[i] = 32;
	}
	
	 
	
	//Putting the actual directory entry into the directory (What if the directory is full?)
	
	unsigned int byteOffSet;
	byteOffSet = BPB_BytsPerSec * (FirstDataSector + ( tempClus - 2) * BPB_SecPerClus);
	
	temp.DIR_FstClusLO = 0xFFFF & byteOffSet;
	temp.DIR_FstClusHI = (byteOffSet >> 16) & 0xFFFF;
	temp.DIR_Attributes = 0x10;
	temp.DIR_FileSize = 0;
	byteOffSet = BPB_BytsPerSec * (FirstDataSector + ( environment.curr_clust_num - 2) * BPB_SecPerClus);
		
	DirEntry itr;
	
	//Read throught the directory until an unnocupied space is found
	while(1){
		pread(image, &itr, sizeof(itr), byteOffSet);
		if(itr.DIR_name[0] == 0){
			//Write new file into empty space
			pwrite(image, &temp, sizeof(temp), byteOffSet);
			break;
		}
		byteOffSet += 32;
	};
	
	byteOffSet = BPB_BytsPerSec * (FirstDataSector + ( environment.curr_clust_num - 2) * BPB_SecPerClus);
	
	//Create parent direntry and enter values to cluster
	DirEntry parent;
	
	strncpy(parent.DIR_name, "..         ", 11);
	parent.DIR_FstClusLO = 0xFFFF & byteOffSet;
	parent.DIR_FstClusHI = (byteOffSet >> 16) & 0xFFFF;
	parent.DIR_Attributes = 0x10;
	parent.DIR_FileSize = 0;
	byteOffSet = BPB_BytsPerSec * (FirstDataSector + ( tempClus - 2) * BPB_SecPerClus);
	pwrite(image, &parent, sizeof(parent), byteOffSet);
	
	//Insert current directory into cluster
	strncpy(temp.DIR_name, ".          ", 11);
	pwrite(image, &temp, sizeof(temp), byteOffSet + 32);
}

void ls(int image, unsigned int clusNum){
	unsigned int byteOffSet;
	byteOffSet = BPB_BytsPerSec * (FirstDataSector + ( clusNum - 2) * BPB_SecPerClus);
	DirEntry tempDir;
	
	do{

		pread(image, &tempDir, sizeof(DirEntry), byteOffSet);	//intake entire dir entry
		if(tempDir.DIR_Attributes != 15){
			int j;
			for(j = 0; j < 11; j++){
				if(tempDir.DIR_name[j] != ' ')
					printf("%c", tempDir.DIR_name[j]);
			}
			printf(" ");
			
		}
		byteOffSet += 32;
	}while(tempDir.DIR_name[0] != 0);
	printf("\n");
}



int nextEmptyClus(int image){
	
	unsigned int tempClus = BPB_RootClus;
	unsigned int clusValue;
	
	do{
		tempClus++;
		
		
		pread(image, &clusValue, 4, BPB_RsvdSecCnt * BPB_BytsPerSec + tempClus * 4);
		//printf("%s%u%s%u\n", "Cluster Number: ", tempClus, "     Value in cluster", clusValue);
	}while(clusValue != 0);
	
	return tempClus;
	
}

