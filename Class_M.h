// mux.cpp header: Ali Taatian May 27, 2021
#include "windows.h"
#include "stdio.h"
#include "stdlib.h"

#include "iostream.h"
#include "fstream.h"

extern		int Number_of_PIDs;//keeps the number of different PIDs in the stream
extern		short * PIDarray;
extern		BYTE * PIDcounter;
extern		int Mytsnumber;
extern		FILE * pfile;
extern		FILE * wfile;
extern		bool enough;



typedef  unsigned char byte; 

const short dataPID=0x0101;
const short PAT_PID=0;
const short NIT_PID=0x0010;
const short PMT_PID=0x0102;
const short SDT_PID=0x0011;

const short TSNumber=20;//maximum number of TS packet producing
const short MAXPID=100;//maximum number of different PIDs
const short SI_sending_interval=10;


class Data_buffer
{

	//Functions:
public:
	bool check_for_data();//Cheks if there is any data in buffer or not,if not ,latch the data 
	// from source and if can not returns false
	Data_buffer();//constructor
	~Data_buffer();//destructor

private:
    int take_bytes_from_file(int number_of_bytes);//fills buffer from file
	//returns the location of pointer in buffer
	//(i.e. end of the data),if no data is in access,returns 0


	//Parameters:
public:
	byte * buffer; //starts from buffer[0]
	int buff_pointer;//holds the amount of data in buffer
};


class Data_sec
{

	//Parameters:
	byte table_id;
	byte section_syntax_indicator;
	byte private_indicator;
	short section_length;
	byte MAC6;
	byte MAC5;
	byte payload_scrambling_control;
	byte address_scrambling_control;
	byte LLC_SNAP_flag;
	byte current_next_indicator;
	byte section_number;
	byte last_section_number;
	byte MAC4;
	byte MAC3;
	byte MAC2;
	byte MAC1;
	DWORD CRC_32B;

private:
	DWORD CRC_32Pro();//returns the CRC_32 of current section;


	//Functions:
public:
	Data_sec();
	~Data_sec();
	bool fill_from_databuffer(Data_buffer *);//makes (fills) the data section from databuffer content
	bool sec_is_read();//returns true if all the section has been written to ts packets
	byte * secB;//a buffer which keep the entire data section//starts from secB[0]
	int seclen;//the total length of section packet
	int sec_read_pointer;//the pointer which shows how much of the section has been written to ts packets
};


class PAT_SEC
{
	byte table_id;
	byte section_syntax_indicator;
	short section_length;
	short transport_stream_id;
	byte version_number;
	byte current_next_indicator;
	byte section_number;
	byte last_section_number;
	short program_number0;
	short network_PID0;
	
	short program_number1;
	short program_map_PID1;
	DWORD CRC_32B;
	DWORD CRC_32Pro();


public:
	PAT_SEC();//if makes the PAT table and fill the section buffer
	~PAT_SEC();
	bool sec_is_read();

	byte * secB;//starts from secB[0]
	int seclen;
	int sec_read_pointer;
};


class PMT_SEC
{
	byte table_id;
	byte section_syntax_indicator;
	short section_length;
	short program_number;
	byte version_number;
	byte current_next_indicator;
	byte section_number;
	byte last_section_number;
	short PCR_PID;
	short program_info_length;
	byte stream_type1;
	short elementary_PID1;
	short ES_info_length1;

	DWORD CRC_32B;

	DWORD CRC_32Pro();


public:
	PMT_SEC();//if makes the PAT table and fill the section buffer
	~PMT_SEC();
	bool sec_is_read();

	byte * secB;//starts from secB[0]
	int seclen;
	int sec_read_pointer;
};


class SDT_SEC
{
	byte table_id;
	byte section_syntax_indicator;
	short section_length;
	short transport_stream_id;
	byte version_number;
	byte current_next_indicator;
	byte section_number;
	byte last_section_number;
	short original_network_id;
	short service_id1;
	byte EIT_schedule_flag1;
	byte EIT_present_following_flag1;
	byte running_status1;
	byte free_CA_mode1;
	short descriptor_loop_length1;

	//data_broadcast_descriptor:
	byte descriptor_tag;
	byte descriptor_length;
	short data_broadcast_id;
	byte component_tag;
	byte selector_length;

	//structure:
	byte MAC_address_range;
	byte MAC_IP_mapping_flag;
	byte alignment_indicator;
	byte max_section_per_datagram;

	DWORD ISO_639_2_language_code;
	byte text_length;

	DWORD CRC_32B;

	DWORD CRC_32Pro();


public:
	SDT_SEC();//if makes the SDT table and fill the section buffer
	~SDT_SEC();
	bool sec_is_read();

	byte * secB;//starts from secB[0]
	int seclen;
	int sec_read_pointer;
};

class NIT_SEC
{
	byte table_id;
	byte section_syntax_indicator;
	short section_length;
	short network_id;
	byte version_number;
	byte current_next_indicator;
	byte section_number;
	byte last_section_number;
	short network_descriptor_length;
	short transport_stream_loop_length;
	short transport_stream_id1;
	short original_network_id1;
	short transport_descriptor_length1;
	DWORD CRC_32B;
	DWORD CRC_32Pro();


public:
	NIT_SEC();//if makes the NIT table and fill the section buffer
	~NIT_SEC();
	bool sec_is_read();

	byte * secB;//starts from secB[0]
	int seclen;
	int sec_read_pointer;

};


class TSpacket
{
public:
	//Parameters:	
	  byte transport_error_indicator ;
	  byte pusi ;
	  byte transport_priority ;
	  short PID;
	  byte transport_scrambling_control ;
	  byte adaptation_field_control;//for '11' ,the amount is 3
	  byte continuity_counter ;
	  byte pointer_field;

	  byte ts_data_pointer;//keeps the number of data bytes in payload(NOT included pointer field)
	  byte * tspayloadB;//the buffer wich keeps the data of payload (NOT included pointer field)//starts from _[0]


	  //Functions:
public:
	  bool set_pointer_field();
	  bool read_from_sec(Data_sec *);//reads a byte from data section and writes to ts;
	  bool read_from_secPAT(PAT_SEC *);//reads a byte from PAT section and writes to ts;
	  bool read_from_secPMT(PMT_SEC *);//reads a byte from PMT section and writes to ts;
	  bool read_from_secSDT(SDT_SEC *);//reads a byte from SDT section and writes to ts;
	  bool read_from_secNIT(NIT_SEC *);//reads a byte from NIT section and writes to ts;
	  bool TS_is_complete();//returns true if TS is completely filled
	  bool send_ts();//sends the TS packet 
	  bool have_enough_space();//returns true if there is enough space for another section
	  bool make_NULL();//converts the entire packet to a NULL packet
	  bool stuff_ts();//fills the rest of the packet with stuffing bytes
	  TSpacket();
	  ~TSpacket();
//private:
      
};


//this class is not used !
class CRC_32
{
public:
	CRC_32();
    ULONG Reflect(ULONG , char );
    void Calculate(const LPBYTE , UINT , ULONG &);
    ULONG * Table;
	~CRC_32();
};