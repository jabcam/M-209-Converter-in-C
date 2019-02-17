//
//  main.c
//  M209
//	Speedier version with bits in variables instead of ints in arrays
//
//  Created by Michael Josefsson on 2018-12-25.
//  Copyright © 2018 Michael Josefsson. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define NUM_WHEELS 	 6
#define NUM_BARS 	27

#define SPEEDTEST

#ifdef SPEEDTEST
#define RUNLEN 1000000L
#endif

// wheel order: [0] is 17 pin, rightmost in machine
//int wheel[NUM_WHEELS] = 		{  0,  0,  0,  0,  0,  0};

// Internal numbering is by 'index, N',
// a int where A->Z is mapped to 0->25
// Ascii numbering is used in interface from
// input and and to output


//int message[] ="THEZMESSAGE";
//int message[] = "MWRASWBZHBO";
//// These are matching texts!
char message[] = "HIGHERZOPTIMIZATIONZLEVELSZPERFORMZMOREZGLOBALZTRANSFORMATIONSZONZTHEZPROGRAMZANDZAPPLYZMOREZEXPENSIVEZANALYSISZALGORITHMSZINZORDERZTOZGENERATEZFASTERZANDZMOREZCOMPACTZCODEZTHEZPRICEZINZCOMPILATIONZTIMEZANDZTHERESULTINGZIMPROVEMENTZINZEXECUTIONZTIMEZBOTHZDEPENDZONZTHEZPARTICULARZAPPLICATIONZANDZTHEZHARDWAREZENVIRONMENTZYOUZSHOULDZEXPERIMENTZTOZFINDZTHEZBESTZLEVELZFORZYOURZAPPLICATION";

#ifdef SPEEDTEST
char output[RUNLEN];
#else
char output[10000];
#endif

char coded_output[] = "YVPSAJUDSOKAXWKQJUANLGTBYLLBUZLEDYGNAXKAWGPKVVUMXQWMLCCMHDHLSMSHBPNYBWGVCEGKXRPWMMKXETHKJFPSDVPIVVAHWFMKSNBCPGAGDVNBPGREEITWMLBPKZUJCEEFKMIWIHGTJQXZMVJZJNJATTKPEUDXOCGTGJMKGVOBRBCBIBLYZNDZMCBFQZJFAAKQGBAJBJDQSCCMRDJQHXCUFHTTYWKXRQCGYVKAVPODZEPTWEHDDHCAADPHJCGWEKFHNTMBLZPGMEKLSVFLZUFXWIJGOFMAQSONDFYWYDXRWQAMRGSHXSLGYNZTLMASLBCBXHXKTGXBOZEFAAHWBNHUFXIYJMITLUPGFDLNCANAIWXAPTVQULEULOWCDQ";

int main(int argc, const char * argv[]) {
	
//int active_lever[6];	// connecting levers,  inactive=0

//INITIAL SETTING START============================================
//active pins on wheel[i]
//int wheel_pin[6][26] = {"ABQ", "ABCD", "", "", "", ""};

char wheel_pin[NUM_WHEELS][26] = {
	"ABDHKNOQ",		// 17
	"BDEFHIMNPS",	// 19
	"CEFHIMNPSTU",	// 21
	"ABGHJRSTUVX",	// 23
	"DEGJKLORSUX",	// 25
	"ABDHIKMSTVW"};	// 26

// re-map letters above to actual pins
// active_pin[i] = { 1, 0, 1, ... } <=> AC.. but with wheel offset
// clear all pin before populating
	int active_pin[NUM_WHEELS][26];
	int c, i,n;
	for( i = 0; i != NUM_WHEELS; i++)
		for( c = 0; c != 26; c++){
			active_pin[i][c] = 0;
		}
//#wheel 0/17, 1/19, 2/21, 3/23, 4/25, 5/26
	for( i = 0; (c = wheel_pin[0][i++]) != '\0';	active_pin[0][((c-'A') + 7) % 17] = 1);
	for( i = 0; (c = wheel_pin[1][i++]) != '\0';	active_pin[1][((c-'A') + 8) % 19] = 1);
	for( i = 0; (c = wheel_pin[2][i++]) != '\0';	active_pin[2][((c-'A') + 9) % 21] = 1);
	for( i = 0; (c = wheel_pin[3][i++]) != '\0';) // or -'B' if c=='X'
		if(c == 'X')
			active_pin[3][((c-'B') + 10) % 23] = 1;
		else
			active_pin[3][((c-'A') + 10) % 23] = 1;
	for( i = 0; (c = wheel_pin[4][i++]) != '\0';) // or -'B' if c in 'X','Y','Z'
		if(c == 'X' || c == 'Y' || c == 'Z')
			active_pin[4][((c-'B') + 11) % 25] = 1;
		else
			active_pin[4][((c-'A') + 11) % 25] = 1;
	for( i = 0; (c = wheel_pin[5][i++]) != '\0';	active_pin[5][((c-'A') + 11) % 26] = 1);

// For new structure, map the arrays above to individual bit-patterns
// activ_pinX={b31, b30, b29, b28,..., b4, b3, b2, b1, b0}
// where a '1'-bit is in position Y as in active_pin[X][Y]=1 above
	unsigned long active_pin0 = 0UL; // bit 3 is active on wheel 17
	unsigned long active_pin1 = 0UL; // bit 4 is active on wheel 19
	unsigned long active_pin2 = 0UL; // bit 4 is active on wheel 21
	unsigned long active_pin3 = 0UL; // bit 4 is active on wheel 23
	unsigned long active_pin4 = 0UL; // bit 4 is active on wheel 25
	unsigned long active_pin5 = 0UL; // bit 4 is active on wheel 26
	
	// map wheel[0][pin0-16] to active_pin0 bits0-16
	for( i = 0; i < 17;	i++){	active_pin0 |= active_pin[0][i]<<i;	};
	for( i = 0; i < 19;	i++){	active_pin1 |= active_pin[1][i]<<i;	};
	for( i = 0; i < 21;	i++){	active_pin2 |= active_pin[2][i]<<i;	};
	for( i = 0; i < 23;	i++){	active_pin3 |= active_pin[3][i]<<i;	};
	for( i = 0; i < 25;	i++){	active_pin4 |= active_pin[4][i]<<i;	};
	for( i = 0; i < 26;	i++){	active_pin5 |= active_pin[5][i]<<i;	};

//cage_bars,
// max 2 active lugs in each row
// En aktiv lever är aktiv för ett helt varv av cage
// 27 bommar. vardera påverkas av EN aktiv pinne det varvet.
// Om överlappande pinnar påverkas bara EN
static int	cage_bar[27][6]= {
//   6  5  4  3  2  1	<-- actual machine markings i.e. backwards!
	{1, 1, 0, 0, 0, 0}, //0
	{1, 0, 0, 0, 0, 0}, //1
	{0, 0, 1, 0, 0, 0}, //2
	{0, 1, 1, 0, 0, 0}, //3
	{1, 0, 1, 0, 0, 0}, //4
	{0, 1, 1, 0, 0, 0}, //5
	{0, 1, 0, 1, 0, 0},	//6
	{0, 0, 0, 1, 0, 0},	//7
	{0, 0, 0, 1, 0, 0},	//8
	{0, 0, 0, 1, 0, 0},	//9
	{0, 0, 0, 1, 0, 0},	//10
	{0, 0, 0, 0, 1, 0},	//11
	{0, 0, 0, 0, 1, 0},	//12
	{0, 0, 0, 0, 1, 0},	//13
	{0, 0, 0, 0, 1, 0},	//14
	{0, 0, 0, 0, 1, 0},	//15
	{0, 0, 0, 0, 1, 0},	//16
	{0, 0, 0, 0, 1, 0},	//17
	{0, 0, 0, 0, 0, 1},	//18
	{0, 0, 0, 0, 0, 1},	//19
	{0, 0, 0, 0, 0, 1},	//20
	{0, 0, 0, 0, 0, 1},	//21
	{0, 0, 0, 0, 0, 1},	//22
	{0, 0, 0, 0, 0, 1},	//23
	{0, 0, 0, 0, 0, 1},	//24
	{0, 0, 0, 0, 0, 1},	//25
	{0, 0, 0, 0, 0, 1}	//26
};

	// preprocess cage for new machine, expand into register.
	// Array above is turned into 0, 0xff- u_int64
	// new versions in u_int64_t cage[27]
	volatile u_int64_t cage[27];
	
	for( int i = 0; i < NUM_BARS;i++){
		cage[i] = 0UL; // first clear
		// then populate
		for(int n = NUM_WHEELS-1; n > -1; n--){
			cage[i]<<=8;	// dummy for pipelining
			if(cage_bar[i][n]){	cage[i] |= 0xff; } else { cage[i] |= 0x00;}
		}
		//printf("cage[%i]= \t%l016x\n", i, cage[i]);
	}
	
//// preprocess cage for speed
// column sum of single cage lugs
//	int single_bar[NUM_WHEELS] = { 0, 0, 0, 0, 0, 0 };
// index of cage bars with double lugs
//	int double_bar[NUM_BARS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//	int sum;
//
//	// find singles and doubles
//	int j = 0;		// index in double_bar
//	for (i = 0; i < NUM_BARS; i++){
//		sum = 0;
//		// find each bar
//		for( n = 0; n < NUM_WHEELS; n++){
//			sum += cage_bar[i][n];	// sum lugs
//		}
//		// only sum == 1 or sum == 2 possible
//		switch(sum){
//			case 1:	// was just one 1, then safe to add it
//				for( n = 0; n < NUM_WHEELS; n++){
//					single_bar[n] += cage_bar[i][n];
//				};
//				break;
//			case 2:	// copy this bar to list of double_bars
//					double_bar[j++]= i ;	// index of bar in cage with double lugs
//				break;
//			default:	printf("ERROR IN CAGE\n");
//		}
//
//	}
//	int double_bar_size = j;
//	// Now we have two arrays:
//	// single_bar = { 0, 3, 0, 2, 2, 1}; with sum of single lugs for each column
//	// double_bar[0..j][] = copies of (at most all) cage_bar with two lugs active
//	// cage_bars with NO lugs are discarded from now on
	
// END OF SETUP

	
// CODING / DECODING
u_int64_t wheels = 0UL;

clock_t start = clock();

#ifdef SPEEDTEST
for(unsigned long k = 0; k < RUNLEN; k++){
#else
for(unsigned long k = 0UL; k < strlen(message); k++){
#endif

// masks for wheels 00|00|26|25|23|21|19|17
#define W17		0xff
#define W19 	0xff00
#define W21 	0xff0000
#define W23 	0xff000000
#define W25 	0xff00000000
#define W26 	0xff0000000000
	

	//bump wheels
	wheels += 0x0000010101010101UL;

	int W17addr = ((wheels >>  0) & 0xff);
	int W19addr = ((wheels >>  8) & 0xff);
	int W21addr = ((wheels >> 16) & 0xff);
	int W23addr = ((wheels >> 24) & 0xff);
	int W25addr = ((wheels >> 32) & 0xff);
	int W26addr = ((wheels >> 40) & 0xff);


	if(W17addr == 17){ wheels = wheels & ~W17; W17addr = ((wheels & W17)>>0);}
	if(W19addr == 19){ wheels = wheels & ~W19; W19addr = ((wheels & W19)>>8);}
	if(W21addr == 21){ wheels = wheels & ~W21; W21addr = ((wheels & W21)>>16);}
	if(W23addr == 23){ wheels = wheels & ~W23; W23addr = ((wheels & W23)>>24);}
	if(W25addr == 25){ wheels = wheels & ~W25; W25addr = ((wheels & W25)>>32);}
	if(W26addr == 26){ wheels = wheels & ~W26; W26addr = ((wheels & W26)>>40);}


// Map to old wheels structure for printing
//	wheel[0]= W17addr;
//	wheel[1]= W19addr;
//	wheel[2]= W21addr;
//	wheel[3]= W23addr;
//	wheel[4]= W25addr;
//	wheel[5]= W26addr;
//
//Print wheels
//		for(int n = NUM_WHEELS-1; n > -1; n--){
//			if (n == 3 || n == 4){
//				if (wheel[n] >= ('W'-'A'))
//					printf("%c", wheel[n]+'B');
//				else
//					printf("%c", wheel[n]+'A');
//
//			} else {
//				printf("%c", wheel[n]+'A');
//			}
//		}
//		printf("\n");
//	

	// For the letters of the wheels above after one step forward
	// find out if each wheel's pins are in active position or not.
	// active position means active_lever = 1
	// active_lever is this wheel's index
	// e.g. if pin 5 is active on wheel then wheel[n] is 5, and active_lever = 1
	
	u_int64_t active_lev = 0L;	// active_lever

	active_lev |= 0x1 & (active_pin5 >> W26addr);
	active_lev <<= 8;
	active_lev |= 0x1 & (active_pin4 >> W25addr);
	active_lev <<= 8;
	active_lev |= 0x1 & (active_pin3 >> W23addr);
	active_lev <<= 8;
	active_lev |= 0x1 & (active_pin2 >> W21addr);
	active_lev <<= 8;
	active_lev |= 0x1 & (active_pin1 >> W19addr);
	active_lev <<= 8;
	active_lev |= 0x1 & (active_pin0 >> W17addr);
//	printf("\tWheels>>0 \t%llx\n",  (wheels >>0 ) & 0xff);
//	printf("TOTAL Active_lev %llx\n", active_lev);
//	printf("\tTOTAL Wheels %llX\n",wheels);


//	 Drum-lugs give offset
//
//	 a) a bar can be active or inactive
//	 b) an active bar increases offset by one
//	 c) a bar is active if lug corresponds to active pin on drum
//	 d) sufficient if ONE lug matches ITS active pin
//	 e) order of evaluation of matching is irrelevant
//	 f) Note: NO active lugs => no shifts
//	 g) Note: MORE active => higher mean
//	 h) Note: ALL active in one column => offset = 27 (maximum)
	
	// Cases:
	// active_lev = 00 01 01 00 00 00
	// cage[n]    = 01 00 01 01 00 00
	// and'em     = 00 00 01 00 00 00 !=0 --> offset increase by one
	// if and-result is ==0 then no increment
	u_int8_t offset = 0;
	for(n = 0; n < NUM_BARS; n++){	// run through all 27 bars
		if (active_lev & cage[n]) offset++;

	}
	
// original dont touch
//	for(i = 0; i < double_bar_size ; i++){ //för alla double bars
//		for(n = 0; n < NUM_WHEELS; n++){
//			if (active_lever[n] && cage_bar[double_bar[i]][n]){
//				offset++;	// max en påverkan av varje bar
//				break;
//			}
//		}
//	}
//
//	for(n = 0; n < NUM_WHEELS; n++){ // add single bars
//		if (active_lever[n] && single_bar[n]){
//			offset += single_bar[n];
//		}
//	}
//

#ifdef SPEEDTEST
	u_int8_t p = 'A'-offset;
#else
	u_int8_t p = message[k]-offset;
#endif
	while (p < 'A') p += 26;
	output[k] = 155-p;
	}


#ifdef SPEEDTEST
	clock_t end = clock();
	float secs = (float)(end - start) / CLOCKS_PER_SEC;

	printf("\nTime: %f ms\t\tSpeed %ld chars/sec",1000*secs, (long)(RUNLEN/secs));
	printf("\nMessage length = %lu chars\n", RUNLEN);

#else
	printf("CODE OUTPUT =");
	for(i = 0; i < strlen(output); i++){
		//if (i % 5 == 0) printf(" ");
		printf("%c",output[i]);
	}
	
	//printf("\nTime: %f ms\t\tSpeed %ld chars/sec",1000*secs, (long)(strlen(message) /secs));
	printf("\nMessage length = %lu chars\n", strlen(message));
	printf("Strcmp says %s", strcmp(coded_output, output)==0 ? "MATCH\n" : "NOMATCH!\n");
#endif
	return 0;
}



