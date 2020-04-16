#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ls.h"

typedef struct
{
    char** tokens;
    int numTokens;
} instruction;

typedef struct {	//Let me know if any of the numbers I got seem wrong and let me know
	//added the rest of the BPB structure 
	unsigned char BS_jmpBoot[3];
	unsigned char BS_Oem[8];

    
	unsigned short BPB_BytsPerSec;	//Offset = 11, Size = 2 Bytes (Should be 0x0200, this was given in the slides)
	unsigned char BPB_SecPerClus;	//Offset = 13, Size = 1 Byte	(I got 0x01)
	unsigned short BPB_RsvdSecCnt;	//Offset = 14, Size = 2 Bytes	(I got 0x0020 but I'm not  sure if this is right
	unsigned char BPB_NumFATs;	//Offset = 16, Size = 1 Byte	(I got 0x02)
	unsigned long BPB_FATSz32;	//OffSet = 32, Size = 4 Bytes	(I got 0x00200000)
	unsigned long BPB_RootClus;	//Offset = 36, Size = 4 Bytes	(I got 0x000003F1)
	unsigned long BPB_TotSec32;	//Offset = 44, Size = 4 Bytes	(I got 0x00000002)
	unsigned char BPB_MediaD; 
	unsigned short BPB_SecPerTrk;
	unsigned short BPB_NumofHeads;
	unsigned short BPB_HiddenSect;

	unsigned short BPN_FSVer;
	unsigned short BPB_FSInfo;
	unsigned short BPB_BkBootSec;
	unsigned char BPB_Reserved[12];
	unsigned char BS_DrvNum;
	unsigned char BS_Reserve1;
	unsigned char BS_BootSig;
	unsigned int BS_VollD;
	unsigned char BS_VolLab[11];
	unsigned char BS_FileSystemType[8];
	

} __attribute__((packed))BPB;

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
FILE * image;
BPB BootSec;
int FirstDataSector;


//Functions 

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
void listDirectory(unsigned int current_clust_num);
int firstSectorOfCluster(unsigned int clusterNum);
void ls(int current_clust_num, const char *pathDir);

int main() {
image = fopen("fat32.img", "r+b");
fread(&BootSec,sizeof(BPB),1,image);

// to include the cluster_num and clust_name to the curr path
int curr_cluster = 0;
environment.curr = 0;
char *pathName = "/";
environment.curr_clust_num = BootSec.BPB_RootClus;
strcpy((char *)environment.name, pathName);
strcpy(environment.curr_path[environment.curr], pathName);

environment.curr_clust_path[environment.curr] = BootSec.BPB_RootClus;
++environment.curr;

//calculate the FirstDataSector
FirstDataSector = BootSec.BPB_RsvdSecCnt + (BootSec.BPB_NumFATs * BootSec.BPB_FATSz32);
	
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
        printf("Please enter an instruction: ");


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
            clearInstruction(&instr);
			break;
		}

        if(strcmp(instr.tokens[0], "size") == 0) {
			/*
            FILE *sizeFile;
            if (sizeFile = (fopen(instr.tokens[1], "r"))) {
                fseek(sizeFile, 0L, SEEK_END);
                int size = ftell(sizeFile);
                printf("%d\n", size);   //Need to know how size is displayed
                fseek(sizeFile, 0L, SEEK_SET);
                fclose(sizeFile);
            }
			else{
				printf("%s\n", "Error: File does not exist");
			}
			*/
        }

        clearInstruction(&instr);
    }
	 if(strcmp(instr.tokens[0], "ls") == 0)
	{
	 if(instr.tokens[1] != NULL && strcmp(instr.tokens[1], ".") !=0)
	{	
		 
		if(strcmp(instr.tokens[1], "..") == 0)
		{
			if(environment.curr -2 != 0)
			{
			 listDirectory(BootSec.BPB_RootClus);
			}
	else		
		listDirectory(environment.curr_clust_path[environment.curr-2]);
			//listDirectory(BootSec.BPB_RootClus);
					
		}
	else 	
		{// I would need the cd function for this 
		//lisDir(environment.curr_clus, instr.tokens[1]);
		}
	}
		else 
		{
			listDirectory(environment.curr_clust_num);
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
int firstSectorOfClust= (((clusterNum -2) * BootSec.BPB_SecPerClus) + FirstDataSector) * BootSec.BPB_BytsPerSec;
	return firstSectorOfClust;

}

void listDirectory(unsigned int current_clust_num)
{

int clusterNumber;
DirEntry tempDir;
unsigned int start = 0;
while(1)
{
        start = 0;
        while(BootSec.BPB_BytsPerSec > start*sizeof(tempDir))
        {
          int offset = firstSectorOfCluster(current_clust_num) + start *sizeof(tempDir);


                // the file pointer will be moved to the starting position of the root dir.
                fseek(image, offset, SEEK_SET);

                //the root dir. should be read to a new buff.
                fread(&tempDir, 1,BootSec.BPB_BytsPerSec, image);

                // print the directories and file names
                if(strcmp((char *)tempDir.DIR_name,"") !=0)
                {
			printf("%s\n", tempDir.DIR_name);

		start++;
		
      		 }
	}	


}
}

void ls(int current_clust_num, const char *pathDir)
{
// for . and ..
if(strcmp(pathDir, ".") == 0)
{
	listDirectory(current_clust_num);
}


}
