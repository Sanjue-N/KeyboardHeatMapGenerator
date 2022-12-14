/*
author- Nymeria Becker 
licence GPLv3


program generates out of an input file
	 which has 1 file location per line
an output files for destrebution of Keys presset in promill, based on the ASCII values ofte charn (accepted non ASCII chars ß, ä, ö, ü, Ä, Ö, Ü)
for the Keyboard layouts:
de-qwertz
neo2
us-qwerty
us-dvorak
us-programmer-dvorak

the ouput files are for a ISO-keymatrix nubres up to 999 seperatertby a tab
esc	F1	F2 .... F12
numberrow
tab	Keyrow1
capslock	Keyrow2
leftShift keyrow3
leftStrg	...

*/
#include <stdio.h>
#include <string.h>


static unsigned long countUpASCII[256];		//spaces for 14-20 set for ß, ä, ö, ü, Ä, Ö, Ü
char pathFileCurrent[1024 * 1024];
static unsigned long keyBoard[6][14];



/*
reads in all input chars and increments the element of countUpASCII at the place of its ASCII Value
*/

int fillTable(char* pathLocationFile){
	FILE *inputLocationFile = fopen(pathLocationFile, "r");
	FILE *inputCurrentFile;
	int readChar = EOF;			
	int flagEOF = 1;
	unsigned long count = 0;

	while(fgets(pathFileCurrent, 1024 * 1024, inputLocationFile) != NULL){			//read in next file location
	
		pathFileCurrent[strlen(pathFileCurrent) - 1] = 0;
		inputCurrentFile = fopen(pathFileCurrent, "r");
		if(inputCurrentFile == NULL){
			printf("fillTable inputCurrentFile is null; pathFileCurrent = %s\n", pathFileCurrent);
			return(-1);
		}

		while((readChar  = fgetc(inputCurrentFile)) != EOF && flagEOF){
							//handle special cases aka multibyte chars
			if(readChar == 0b11000011){
				if((readChar = fgetc(inputCurrentFile)) != EOF){
					if(readChar == 0b10011111){					//german ß 
						countUpASCII[14]++;
//						printf("ß");
					}else if(readChar == 0b10100100){			//german ä
						countUpASCII[15]++;
//						printf("ä");
					}else if(readChar == 0b10110110){			//german ö
						countUpASCII[16]++;
//						printf("ö");
					}else if(readChar == 0b10111100){			//german ü
						countUpASCII[17]++;
//						printf("ü");
					}else if(readChar == 0b10000100){			//german Ä
						countUpASCII[18]++;
//						printf("Ä");
					}else if(readChar == 0b10010110){			//german Ö
						countUpASCII[19]++;
//						printf("Ö");
					}else if(readChar == 0b10011100){			//german Ü
						countUpASCII[20]++;
//						printf("Ü");
					}	
					
				}else
					flagEOF = 0;
			}else{
				countUpASCII[readChar]++;
			}
					
		}
		fclose(inputCurrentFile);
		
		printf("file read: %lu : %s\n", ++count, pathFileCurrent);		
	}
	
	printf("all files read\n");
	return 0;
}

int resetKeys(){
	for(int i = 0; i < 14; i++){
		for(int j = 0; j < 6; j++){
			keyBoard[j][i] = 0;	
		}
	}
	keyBoard[5][13] = 0;
	return 0;
}

int heatMap(){
	long double value = 0;
	for(int i = 0; i < 6; i++){
		for(int j = 0; j < 14; j++ ){
			if(keyBoard[i][j] > value)
				value = keyBoard[i][j];
		}
	}
	value /= 1000;
	
	for(int i = 0; i < 6; i++){
		for(int j = 0; j < 14; j++ ){
			keyBoard[i][j] /= value;
			if(keyBoard[i][j] == 1000)
				keyBoard[i][j]--;
		}
	}
	keyBoard[5][13] = value*1000;
	return 0;
}
 

int fillKeys(int keyLayout[][6], char layoutName[]) {
	resetKeys();
	//printf("rShift accurencen %lu at %s\n",keyBoard[4][12], layoutName);
					
	for(int i = 0; i < 130; i++){		//ignoring ESC
		if(i > 64 && i < 91){				//upper case letter
			keyBoard[keyLayout[i+32][0]][keyLayout[i+32][1]] += countUpASCII[i];
			keyBoard[4][0] += countUpASCII[i];
		}else{
			if(keyLayout[i][0] == 0 && keyLayout[i][1] == 0 && countUpASCII[i])
				printf("detected char without key: %i\tamount:%lu\n", i, countUpASCII[i]);		//for checking the not recognised ASCII-code
			keyBoard[keyLayout[i][0]][keyLayout[i][1]] += countUpASCII[i];
			if(keyLayout[i][2] != 0)
				keyBoard[keyLayout[i][2]][keyLayout[i][3]] += countUpASCII[i];
			if(i > 13 && i < 21 && keyLayout[i][4] != 0)
				keyBoard[keyLayout[i][4]][keyLayout[i][5]] += countUpASCII[i];
		}
	}	
	keyBoard[4][0] /= 2;
	keyBoard[4][12] += keyBoard[4][0];
	if( !strcmp(layoutName,"neo")){
		keyBoard[3][0] /= 2;
		keyBoard[3][12] += keyBoard[3][0];

		keyBoard[4][1] /= 2;
		keyBoard[5][4] += keyBoard[4][1];
	}

	heatMap();
	
	if(keyBoard[0][0] != 0)
		printf("unused but detected chars for %s: %lu\n", layoutName, keyBoard[0][0]); 
	return keyBoard[0][0];
}

int output(char layoutName[]){
	char temp[50];
	
	strcpy(temp, layoutName);
	strcat(temp, "LayoutKeypresses.txt");
	FILE *output = fopen( temp, "w");
	for(int i = 1; i < 6; i++){
		for(int j = 0; j < 14; j++){
			fprintf(output, "%lu\t", keyBoard[i][j]);
		}	
		fprintf(output, "\n");	
	}
	return fclose(output);
}

/*
matix layout is per element up to 3 tuples of row and collum so {row_0, collum_0, row_1, collum_1, row_2, collum_2}
row 0 are F-keys
collum 0 are the left most keys on an ISO-standert Keyboard
	
*/

int compileOutput() {
	int keyLayoutDe[][6] =	{
						 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//00							
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{1,13,0,0},	{2,0,0,0},		//05         /b/t		
							{3,13,0,0},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{1,11,0,0},		//10   /n       ß
							{3,11,0,0},	{3,10,0,0},	{2,11,0,0},	{3,11,4,0,0,0},	{3,10,4,0,0,0},//15	ä ö ü Ä Ö 
							{2,11,4,0,0,0},{0,0,0,0},{0,0,0,0},	{0,0,0,0}, 	{0,0,0,0},	 	//20	Ü
																		
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//25	
							{0,0,0,0}, 	{0,0,0,0}, 	{5,3,0,0}, 	{1,1,4,0}, 	{1,2,4,0}, 		//30	space ! " 
							{3,12,0,0},	{1,4,4,0}, 	{1,5,4,0}, 	{1,6,4,0}, 	{3,12,4,0}, 	//35	# $ % & ' 
							{1,8,0,0}, 	{1,9,0,0}, 	{2,12,4,0},	{2,12,0,0},	{4,9,0,0}, 		//40	( ) * + ,
							{4,11,0,0},	{4,10,0,0},	{1,7,4,0}, 	{1,10,0,0}, {1,1,0,0},		//45	- . / 0 1 	
																		
							{1,2,0,0}, 	{1,3,0,0}, 	{1,4,0,0}, 	{1,5,0,0}, 	{1,6,0,0},		//50	2 3 4 5 6 
							{1,7,0,0}, 	{1,8,0,0}, 	{1,9,0,0}, 	{4,10,4,0},	{4,9,4,0},		//55	7 8 9 :	;
							{4,1,0,0}, 	{1,10,4,0},	{4,1,4,0}, 	{1,11,4,0},	{2,1,5,4}, 		//60	< = > ? @
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//65	A B C D E
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//70	F G H I J
																		
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//75	K L M N O
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//80	P Q R S T 
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//85	U V W X Y
							{0,0,0,0}, 	{1,8,5,4}, 	{1,11,5,4},	{1,9,5,4}, 	{1,0,0,0}, 		//90	Z [ \ ] ^
							{4,11,4,0},	{1,12,4,0},	{3,1,0,0}, 	{4,6,0,0}, 	{4,4,0,0},		//95	_ ` a b c
																		
							{3,3,0,0}, 	{2,3,0,0}, 	{3,4,0,0}, 	{3,5,0,0}, 	{3,6,0,0},		//100	d e f g h
							{2,8,0,0}, 	{3,7,0,0}, 	{3,8,0,0}, 	{3,9,0,0}, 	{4,8,0,0}, 		//105	i j k l m 		
							{4,7,0,0}, 	{2,9,0,0}, 	{2,10,0,0},	{2,1,0,0}, 	{2,4,0,0},		//110	n o p q r
							{3,2,0,0}, 	{2,5,0,0}, 	{2,7,0,0}, 	{4,5,0,0}, 	{2,2,0,0}, 		//115	s t u v w
							{4,3,0,0}, 	{4,2,0,0}, 	{2,6,0,0}, 	{1,7,5,4}, 	{4,1,5,4},		//120	x y z { |
																		
							{1,10,5,4},	{2,12,5,4},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}		//125	} ~
							}; 																//130			
										//code 65-90 are 97-122 plus shift
	fillKeys(keyLayoutDe, "de");
	output("de");

	int keyLayoutNeo[][6] =	{
						 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//00							
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{1,13,0,0},	{2,0,0,0},		//05         /b/t		
							{3,13,0,0},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{2,11,0,0},		//10   /n       ß
							{4,4,0,0},	{4,3,0,0},	{4,2,0,0},	{4,4,4,0,0,0},{4,3,4,0,0,0},//15	ä ö ü Ä Ö 
							{4,2,4,0,0,0},{0,0,0,0},{0,0,0,0},	{0,0,0,0}, 	{0,0,0,0},	 	//20	Ü
																		
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//25	
							{0,0,0,0}, 	{0,0,0,0}, 	{5,3,0,0}, 	{2,6,3,0}, 	{4,9,3,0}, 		//30	space ! " 
							{4,2,3,0},	{4,3,3,0}, 	{4,8,3,0}, 	{2,10,3,0},	{4,10,3,0}, 	//35	# $ % & ' 
							{3,7,3,0}, 	{3,8,3,0}, 	{3,5,3,0},	{4,7,3,0},	{4,9,0,0}, 		//40	( ) * + ,
							{3,9,3,0},	{4,10,0,0},	{3,2,3,0}, 	{5,3,4,1},	{4,8,4,1},		//45	- . / 0 1 	
																		
							{4,9,4,1},	{4,10,4,1},	{3,8,4,1}, 	{3,9,4,1},	{3,10,4,1},		//50	2 3 4 5 6 
							{2,8,4,1}, 	{2,9,4,1}, 	{2,10,4,1},	{3,10,3,0},	{4,11,3,0},		//55	7 8 9 :	;
							{2,7,3,0}, 	{2,9,3,0},	{2,8,3,0}, 	{3,6,3,0},	{3,11,3,0},		//60	< = > ? @
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//65	A B C D E
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//70	F G H I J
																		
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//75	K L M N O
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//80	P Q R S T 
							{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//85	U V W X Y
							{0,0,0,0}, 	{2,3,3,0}, 	{3,1,3,0},	{2,4,3,0}, 	{2,5,3,0}, 		//90	Z [ \ ] ^
							{2,2,3,0},	{4,6,3,0},	{3,3,0,0}, 	{4,7,0,0}, 	{2,4,0,0},		//95	_ ` a b c
																		
							{3,10,0,0},	{3,4,0,0}, 	{2,9,0,0}, 	{2,8,0,0}, 	{2,7,0,0},		//100	d e f g h
							{3,2,0,0}, 	{4,11,0,0},	{2,6,0,0}, 	{2,3,0,0}, 	{4,8,0,0}, 		//105	i j k l m 		
							{3,7,0,0}, 	{3,5,0,0}, 	{4,5,0,0},	{2,10,0,0},	{3,8,0,0},		//110	n o p q r
							{3,6,0,0}, 	{3,9,0,0}, 	{3,1,0,0}, 	{4,5,0,0}, 	{2,5,0,0}, 		//115	s t u v w
							{2,1,0,0}, 	{3,11,0,0},	{4,6,0,0}, 	{3,3,3,0}, 	{4,4,3,0},		//120	x y z { |
																		
							{3,4,3,0},	{4,5,3,0},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}		//125	} ~
							}; 																//130			
										//code 65-90 are 97-122 plus shift
	fillKeys(keyLayoutNeo, "neo");
	output("neo");






	int keyLayoutUs[][6] =	{
							 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//00							
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{1,13,0,0},	{2,0,0,0},		//05         /b/t		
								{3,13,0,0},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{3,2,3,2},		//10   /n       ß
								{3,1,2,3},	{2,9,2,3},	{2,7,2,3},	{3,1,2,3,4,0},{2,9,4,0,2,3},//15	ä ö ü Ä Ö 
								{2,7,4,0,2,3},{0,0,0,0},{0,0,0,0},	{0,0,0,0}, 	{0,0,0,0},	 	//20	Ü											
									
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//25	
								{0,0,0,0}, 	{0,0,0,0}, 	{5,3,0,0}, 	{1,1,4,0}, 	{3,11,4,0},		//30	space ! " 
								{1,3,4,0},	{1,4,4,0}, 	{1,5,4,0}, 	{1,7,4,0}, 	{3,11,0,0}, 	//35	# $ % & ' 
								{1,9,4,0}, 	{1,10,4,0}, {1,8,4,0},	{1,12,4,0},	{4,9,0,0}, 		//40	( ) * + ,
								{1,11,0,0},	{4,10,0,0},	{4,11,0,0},	{1,10,0,0}, {1,1,0,0},		//45	- . / 0 1 	
																			
								{1,2,0,0}, 	{1,3,0,0}, 	{1,4,0,0}, 	{1,5,0,0}, 	{1,6,0,0},		//50	2 3 4 5 6 
								{1,7,0,0}, 	{1,8,0,0}, 	{1,9,0,0}, 	{3,10,4,0},	{3,10,0,0},		//55	7 8 9 :	;
								{4,9,4,0}, 	{1,12,0,0},	{4,10,4,0},	{4,11,4,0},	{1,2,4,0}, 		//60	< = > ? @
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//65	A B C D E
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//70	F G H I J
																			
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//75	K L M N O
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//80	P Q R S T 
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//85	U V W X Y
								{0,0,0,0}, 	{2,11,0,0}, {3,12,0,0},	{2,11,0,0},	{1,6,4,0}, 		//90	Z [ \ ] ^
								{1,11,4,0},	{1,0,0,0},	{3,1,0,0}, 	{4,6,0,0}, 	{4,4,0,0},		//95	_ ` a b c
																			
								{3,3,0,0}, 	{2,3,0,0}, 	{3,4,0,0}, 	{3,5,0,0}, 	{3,6,0,0},		//100	d e f g h
								{2,8,0,0}, 	{3,7,0,0}, 	{3,8,0,0}, 	{3,9,0,0}, 	{4,8,0,0}, 		//105	i j k l m 		
								{4,7,0,0}, 	{2,9,0,0}, 	{2,10,0,0},	{2,1,0,0}, 	{2,4,0,0},		//110	n o p q r
								{3,2,0,0}, 	{2,5,0,0}, 	{2,7,0,0}, 	{4,5,0,0}, 	{2,2,0,0}, 		//115	s t u v w
								{4,3,0,0}, 	{2,6,0,0}, 	{4,2,0,0}, 	{2,11,4,0},	{3,12,4,0},		//120	x y z { |
																			
								{2,12,4,0},	{1,0,4,0},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}		//125	} ~
								}; 																//130			
											//code 65-90 are 97-122 plus shift
		fillKeys(keyLayoutUs, "us");
		output("us");

		int keyLayoutDevorak[][6] =	{
							 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//00							
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{1,13,0,0},	{2,0,0,0},		//05         /b/t		
								{3,13,0,0},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{3,10,3,10},	//10   /n       ß
								{3,1,3,3},	{3,4,3,3},	{3,2,3,3},	{3,1,3,3,4,0},{3,4,4,0,3,3},//15	ä ö ü Ä Ö 
								{3,2,4,0,3,3},{0,0,0,0},{0,0,0,0},	{0,0,0,0}, 	{0,0,0,0},	 	//20	Ü											
									
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//25	
								{0,0,0,0}, 	{0,0,0,0}, 	{5,3,0,0}, 	{1,1,4,0}, 	{2,1,4,0},		//30	space ! " 
								{1,3,4,0},	{1,4,4,0}, 	{1,5,4,0}, 	{1,7,4,0}, 	{2,1,0,0},	 	//35	# $ % & ' 
								{1,9,4,0}, 	{1,10,4,0}, {1,8,4,0},	{1,12,4,0},	{2,2,0,0}, 		//40	( ) * + ,
								{3,11,0,0},	{2,3,0,0},	{2,11,0,0},	{1,10,0,0}, {1,1,0,0},		//45	- . / 0 1 	
																			
								{1,2,0,0}, 	{1,3,0,0}, 	{1,4,0,0}, 	{1,5,0,0}, 	{1,6,0,0},		//50	2 3 4 5 6 
								{1,7,0,0}, 	{1,8,0,0}, 	{1,9,0,0}, 	{4,2,4,0},	{4,2,0,0},		//55	7 8 9 :	;
								{2,1,4,0}, 	{2,12,0,0},	{2,1,4,0},	{2,11,4,0},	{1,2,4,0}, 		//60	< = > ? @
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//65	A B C D E
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//70	F G H I J
																			
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//75	K L M N O
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//80	P Q R S T 
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//85	U V W X Y
								{0,0,0,0}, 	{1,11,0,0}, {3,12,0,0},	{1,11,0,0},	{1,6,4,0}, 		//90	Z [ \ ] ^
								{3,11,4,0},	{1,0,0,0},	{3,1,0,0}, 	{4,7,0,0}, 	{2,8,0,0},		//95	_ ` a b c
																			
								{3,6,0,0}, 	{3,3,0,0}, 	{2,6,0,0}, 	{2,7,0,0}, 	{3,7,0,0},		//100	d e f g h
								{3,5,0,0}, 	{4,4,0,0}, 	{4,5,0,0}, 	{2,10,0,0},	{4,8,0,0}, 		//105	i j k l m 		
								{3,9,0,0}, 	{3,2,0,0}, 	{2,4,0,0},	{4,3,0,0}, 	{2,9,0,0},		//110	n o p q r
								{3,10,0,0},	{3,8,0,0}, 	{3,4,0,0}, 	{4,10,0,0},	{4,9,0,0}, 		//115	s t u v w
								{4,6,0,0}, 	{2,5,0,0}, 	{4,11,0,0},	{1,11,4,0},	{3,12,4,0},		//120	x y z { |
																			
								{1,12,4,0},	{1,0,4,0},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}		//125	} ~
								}; 																//130			
											//code 65-90 are 97-122 plus shift
		fillKeys(keyLayoutDevorak, "dvorak");
		output("dvorak");

		int keyLayoutProgDevorak[][6] =	{
							 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//00							
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{1,13,0,0},	{2,0,0,0},		//05         /b/t		
								{3,13,0,0},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{3,10,3,10},	//10   /n       ß
								{3,1,3,3},	{3,4,3,3},	{3,2,3,3},	{3,1,3,3,4,0},{3,4,4,0,3,3},//15	ä ö ü Ä Ö 
								{3,2,4,0,3,3},{0,0,0,0},{0,0,0,0},	{0,0,0,0}, 	{0,0,0,0},	 	//20	Ü											
									
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//25	
								{0,0,0,0}, 	{0,0,0,0}, 	{5,3,0,0}, 	{1,11,0,0},	{4,2,4,0},		//30	space ! " 
								{1,12,4,0},	{1,0,0,0}, 	{1,1,4,0}, 	{1,1,0,0}, 	{4,2,0,0},	 	//35	# $ % & ' 
								{1,5,0,0}, 	{1,8,0,0},	{1,7,0,0},	{1,9,0,0},	{2,2,0,0}, 		//40	( ) * + ,
								{3,11,0,0},	{2,3,0,0},	{2,11,0,0},	{1,7,4,0},	{1,5,4,0},		//45	- . / 0 1 	
																			
								{1,8,4,0}, 	{1,4,4,0}, 	{1,9,4,0}, 	{1,3,4,0}, 	{1,10,4,0},		//50	2 3 4 5 6 
								{1,2,4,0}, 	{1,11,4,0},	{1,6,4,0}, 	{2,1,4,0},	{2,1,0,0},		//55	7 8 9 :	;
								{2,1,4,0}, 	{2,6,0,0},	{2,1,4,0},	{2,11,4,0},	{2,12,4,0},		//60	< = > ? @
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//65	A B C D E
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//70	F G H I J
																			
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0},		//75	K L M N O
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//80	P Q R S T 
								{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}, 		//85	U V W X Y
								{0,0,0,0}, 	{1,2,0,0},	{3,12,0,0},	{1,10,0,0},	{2,12,4,0},		//90	Z [ \ ] ^
								{3,11,4,0},	{1,12,4,0},	{3,1,0,0}, 	{4,7,0,0}, 	{2,8,0,0},		//95	_ ` a b c
																			
								{3,6,0,0}, 	{3,3,0,0}, 	{2,6,0,0}, 	{2,7,0,0}, 	{3,7,0,0},		//100	d e f g h
								{3,5,0,0}, 	{4,4,0,0}, 	{4,5,0,0}, 	{2,10,0,0},	{4,8,0,0}, 		//105	i j k l m 		
								{3,9,0,0}, 	{3,2,0,0}, 	{2,4,0,0},	{4,3,0,0}, 	{2,9,0,0},		//110	n o p q r
								{3,10,0,0},	{3,8,0,0}, 	{3,4,0,0}, 	{4,10,0,0},	{4,9,0,0}, 		//115	s t u v w
								{4,6,0,0}, 	{2,5,0,0}, 	{4,11,0,0},	{1,5,0,0},	{3,12,4,0},		//120	x y z { |
																			
								{1,4,0,0},	{1,0,4,0},	{0,0,0,0}, 	{0,0,0,0}, 	{0,0,0,0}		//125	} ~
								}; 																//130			
											//code 65-90 are 97-122 plus shift
		fillKeys(keyLayoutProgDevorak, "progDvorak");
		output("progDvorak");


	
	return 0;
}

int main(int argumentCount, char** arguments){
//	printf("start main with argument %s\n", arguments[1]);
	if(fillTable(arguments[1]) != 0){
		perror("an error accured during fillTable");
		return -1;
	}

	compileOutput();

	printf("done\n");

	return 0;
}
