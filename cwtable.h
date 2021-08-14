/*****************************************************
**	cwtable.h - morse code conversion table
*****************************************************/

#define EEND 0
#define DIT 1
#define DAH 2
#define WB 3
#define PACK4(a,b,c,d) (a<<6 | b<<4 | c<<2 | d)

unsigned const char PROGMEM chtable[] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '.', '/', '?', ' ', EEND
};

unsigned const char PROGMEM cwtable[] = {
	PACK4(DAH, DAH, DAH, DAH), PACK4(DAH, EEND, EEND, EEND),  //0
	PACK4(DIT, DAH, DAH, DAH), PACK4(DAH, EEND, EEND, EEND),  //1
	PACK4(DIT, DIT, DAH, DAH), PACK4(DAH, EEND, EEND, EEND),  //2
	PACK4(DIT, DIT, DIT, DAH), PACK4(DAH, EEND, EEND, EEND),  //3
	PACK4(DIT, DIT, DIT, DIT), PACK4(DAH, EEND, EEND, EEND),  //4
	PACK4(DIT, DIT, DIT, DIT), PACK4(DIT, EEND, EEND, EEND),  //5
	PACK4(DAH, DIT, DIT, DIT), PACK4(DIT, EEND, EEND, EEND),  //6
	PACK4(DAH, DAH, DIT, DIT), PACK4(DIT, EEND, EEND, EEND),  //7
	PACK4(DAH, DAH, DAH, DIT), PACK4(DIT, EEND, EEND, EEND),  //8
	PACK4(DAH, DAH, DAH, DAH), PACK4(DIT, EEND, EEND, EEND),  //9
	PACK4(DIT, DAH, EEND, EEND), EEND,    //A
	PACK4(DAH, DIT, DIT, DIT), EEND,      //B
	PACK4(DAH, DIT, DAH, DIT), EEND,      //C
	PACK4(DAH, DIT, DIT, EEND), EEND,     //D
	PACK4(DIT, EEND, EEND, EEND), EEND,   //E
	PACK4(DIT, DIT, DAH, DIT), EEND,      //F
	PACK4(DAH, DAH, DIT, EEND), EEND,     //G
	PACK4(DIT, DIT, DIT, DIT), EEND,      //H
	PACK4(DIT, DIT, EEND, EEND), EEND,    //I
	PACK4(DIT, DAH, DAH, DAH), EEND,      //J
	PACK4(DAH, DIT, DAH, EEND), EEND,     //K
	PACK4(DIT, DAH, DIT, DIT), EEND,      //L
	PACK4(DAH, DAH, EEND, EEND), EEND,    //M
	PACK4(DAH, DIT, EEND, EEND), EEND,    //N
	PACK4(DAH, DAH, DAH, EEND), EEND,     //O
	PACK4(DIT, DAH, DAH, DIT), EEND,      //P
	PACK4(DAH, DAH, DIT, DAH), EEND,      //Q
	PACK4(DIT, DAH, DIT, EEND), EEND,     //R
	PACK4(DIT, DIT, DIT, EEND), EEND,     //S
	PACK4(DAH, EEND, EEND, EEND), EEND,   //T
	PACK4(DIT, DIT, DAH, EEND), EEND,     //U
	PACK4(DIT, DIT, DIT, DAH), EEND,      //V
	PACK4(DIT, DAH, DAH, EEND), EEND,     //W
	PACK4(DAH, DIT, DIT, DAH), EEND,      //X
	PACK4(DAH, DIT, DAH, DAH), EEND,      //Y
	PACK4(DAH, DAH, DIT, DIT), EEND,      //Z
	PACK4(DIT, DAH, DIT, DAH), PACK4(DIT, DAH, EEND, EEND),   // Period
  PACK4(DAH, DIT, DIT, DAH), PACK4(DIT, EEND, EEND, EEND),  // Forward slash
  PACK4(DIT, DIT, DAH, DAH), PACK4(DIT, DIT, EEND, EEND),   // Question mark
	PACK4(WB, EEND, EEND, EEND), EEND,    //Word space
};
