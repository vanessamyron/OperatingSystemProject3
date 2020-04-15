#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    char** tokens;
    int numTokens;
} instruction;

struct BPB{	//Let me know if any of the numbers I got seem wrong and let me know
	unsigned short BPB_BytsPerSec;	//Offset = 11, Size = 2 Bytes (Should be 0x0200, this was given in the slides)
	unsigned char BPB_SecPerClus;	//Offset = 13, Size = 1 Byte	(I got 0x01)
	unsigned short BPB_RsvdSecCnt;	//Offset = 14, Size = 2 Bytes	(I got 0x0020 but I'm not  sure if this is right
	unsigned char BPB_NumFATs;	//Offset = 16, Size = 1 Byte	(I got 0x02)
	unsigned long BPB_FATSz32;	//OffSet = 32, Size = 4 Bytes	(I got 0x00200000)
	unsigned long BPB_RootClus;	//Offset = 36, Size = 4 Bytes	(I got 0x000003F1)
	unsigned long BPB_TotSec32;	//Offset = 44, Size = 4 Bytes	(I got 0x00000002)
} _attribute_((packed));

struct DirEntry{
	unsigned char DIR_name[11];
	unsigned char DIR_Attributes;
	unsigned char DIR_NTRes;
	unsigned char DIR_CrtTimeTenth);
	unsigned short DIR_CrtTime;
	unsigned short DIR_CrtDate;
	unsigned short DIR_LstAccDate;
	unsigned short DIR_FstClusHI;
	unsigned short DIR_WrtTime;
	unsigned short DIR_WrtDate;
	unsigned short DIR_FstClusLO;
	unsigned long DIR_FileSize;
	
} _attribute_((packed));

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);

int main() {
	

	
	//Variables for Intake
    char* token = NULL;
    char* temp = NULL;

    instruction instr;
    instr.tokens = NULL;
    instr.numTokens = 0;

    while (1) {
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
