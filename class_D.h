// demux.cpp header: Ali Taatian May 27, 2021
#include "windows.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream.h"
#include "fstream.h"

typedef  unsigned char byte; 



class TSpacket
{
public:
	//parameters:
	short PID;
	byte pusi ;
	byte continuity_counter ;
	byte pointer_field;
	byte * TSbuffer;
	BYTE is_read;//the initialization value is 0

	//functions:
	TSpacket();
	~TSpacket();
	bool readTS_fromfile(FILE *);//it reads a complete TS packet (188 bytes) from file and assigns 
	//each of (required) packet fields . It returns true if it is ok
};

class PAT_SEC
{
public:
	PAT_SEC();
	~PAT_SEC();
	void fillfromTS(TSpacket*);//according to pusi value and the parameters of TS and section ,fills the section
	bool PAT_is_made();//returns true if the section is complete ,it assigns (required) fields
	bool PAT_CRCcheck();//returns true if CRC check is ok


	byte * secB;//starts from secB[0]
	int sec_pointer;//shows how many bytes of section has been read(if one byte is read ,it is 1!
	int seclen;//shows the entire length of section

	short transport_stream_id;
	BYTE version_number;
	short program_number1;
	short program_map_PID1;  

};

class PMT_SEC
{
public:
	PMT_SEC();
	~PMT_SEC();
	void fillfromTS(TSpacket*);//according to pusi value and the parameters of TS and section ,fills the section
	bool PMT_is_made();//returns true if the section is complete
	bool PMT_CRCcheck();
	short getE_PID();//returns elementary PID

	byte * secB;//starts from secB[0]
	int sec_pointer;//shows how many bytes of section has been read(if one byte is read ,it is 1!
	int seclen;//shows the entire length of section

	short program_number;
	BYTE version_number;
	BYTE stream_type1;
	short elementary_PID1;

};

class Data_sec
{
public:
	Data_sec();
	~Data_sec();
	bool fillfromTS(TSpacket*);//according to pusi value and the parameters of TS and section ,fills the section
	bool SEC_is_made();//returns true if the section is complete
	bool SEC_CRCcheck();

	void FillToFile(FILE *);//it writes the data of the section to file

	byte * secB;//starts from secB[0]
	int sec_pointer;//shows how many bytes of section has been read(if one byte is read ,it is 1!
	int seclen;//shows the entire length of section

	
	BYTE MAC_6;
	BYTE MAC_5;
	BYTE MAC_4;
	BYTE MAC_3;
	BYTE MAC_2;
	BYTE MAC_1;

};

class Program
{
public:
	Program();
	~Program();
	void set(PAT_SEC *);//sets all the required field of the program
	short getPMT_PID();//returns PMT PID of the program

	short program_map_PID;
	BYTE number_of_elementary_streams;
	//we can add further parameters!
};