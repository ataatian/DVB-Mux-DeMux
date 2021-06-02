// mux.cpp classes: Ali Taatian May 27, 2021

#include "stdAfx.h"
#include "Class.h"


//Data_buffer methods

bool Data_buffer::check_for_data()
{
	if (buff_pointer==0)
		buff_pointer=take_bytes_from_file(30);//30 is my number of reading data from source
	if (buff_pointer<30)
		enough=true;
	if (buff_pointer>0)
		return true;
	else
		return false;
	
}

Data_buffer::Data_buffer()
{
	buff_pointer=0;
	buffer=new byte[1600];
}

Data_buffer::~Data_buffer()
{
	delete [] buffer;

}

int Data_buffer::take_bytes_from_file(int number_of_bytes)
{

	int sizeR=fread(buffer,1,number_of_bytes,pfile);
	return sizeR;

}
 
//Data_sec methods
Data_sec::Data_sec()
{
	table_id=0x3e;
	section_syntax_indicator=1;
	private_indicator=1;

	MAC6=0;
	MAC5=0;
	payload_scrambling_control=0;
	address_scrambling_control=0;
	LLC_SNAP_flag=0;
	current_next_indicator=1;
	section_number=0;
	last_section_number=0;
	MAC4=0;
	MAC3=0;
	MAC2=0;
	MAC1=0;
	seclen=0;
	sec_read_pointer=0;

	secB=new byte[1600];
}

Data_sec::~Data_sec()
{
	delete [] secB;
}

bool Data_sec::fill_from_databuffer(Data_buffer * databuffer)
{
	section_length=(13 + (databuffer->buff_pointer));//without any 'checksum' byte!

	secB[0]=0x3e;
	secB[1]=(section_syntax_indicator*128)+(private_indicator*64)+48+(section_length/256);
	secB[2]=(section_length%256);
	secB[3]=MAC6;
	secB[4]=MAC5;
	secB[5]=192+(payload_scrambling_control*16)+(address_scrambling_control*4)+(LLC_SNAP_flag*2)+current_next_indicator;
	secB[6]=section_number;
	secB[7]=last_section_number;
	secB[8]=MAC4;
	secB[9]=MAC3;
	secB[10]=MAC2;
	secB[11]=MAC1;
	for (int j=1;j<=(databuffer->buff_pointer);j++)
		secB[j+11]=databuffer->buffer[j-1];//woooooooooooooooooops!
	
	seclen=(databuffer->buff_pointer)+16;

	CRC_32B=CRC_32Pro();
	secB[seclen-1]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-2]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-3]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-4]=(CRC_32B&255);
	//Now,all the section packet has been made

	databuffer->buff_pointer=0;

	return true;
}

DWORD Data_sec::CRC_32Pro()
{
	DWORD ulPolynomial2 = 0x04C11DB7;
	DWORD pp=ulPolynomial2 ;
	DWORD finalCRC=0;
	DWORD jj;
	byte i,g,l;
	bool s,p;
	bool * O=new bool[32];
	bool * I=new bool[32];
	bool * T=new bool[32];

	bool * R=new bool[32];
	bool * poly=new bool[32];
	pp=pp>>1;

	for (int j=0;j<32;j++)
	{
		if ((pp&1)==1)
			s=true;
		else
			s=false;

		pp=pp>>1;
		if (s)
			poly[j]=true;
		else
			poly[j]=false;
	}

	int count=0;

	for (i=0;i<32;i++)
		O[i]=true;

	for (int k=0;k<(seclen-4);k++)
	{
		g=secB[k];
		for(l=1;l<9;l++)
		{
			if ((g&128)==128)
				s=true;

			else
				s=false;

		
			g=g<<1;

			p=s^O[31];
			for (j=0;j<32;j++)
				T[j]=poly[j]&p;
			for(j=0;j<32;j++)
				R[j]=O[j]^T[j];
			for(j=0;j<31;j++)
				I[j+1]=R[j];
			I[0]=p;

			for(j=0;j<32;j++)
				O[j]=I[j];
			count++;

		}
		
	}
	jj=1;
	for (int z=0;z<32;z++)
	{	if (O[z])
	finalCRC=finalCRC+jj;
				jj=jj*2;
	}
	
 	delete [] O;
	delete [] poly;
	delete [] I;
	delete [] T;
	delete [] R;

	return finalCRC;
}

bool Data_sec::sec_is_read()
{
	if (sec_read_pointer==seclen)
		return true;
	else
		return false;
}


//PAT_SEC methods
PAT_SEC::PAT_SEC()
{
	table_id=0;
	section_syntax_indicator=1;
	section_length=17;//for one program
	transport_stream_id=1;
	version_number=0;
	current_next_indicator=1;
	section_number=0;
	last_section_number=0;

	program_number0=0;
	network_PID0=0x0010;
	
	program_number1=1;
	program_map_PID1=0x0102;

	secB=new byte[1600];
	
	seclen=20;
	sec_read_pointer=0;

	secB[0]=table_id;
	secB[1]=(128*section_syntax_indicator)+48+(section_length/256);
	secB[2]=section_length%256;
	secB[3]=transport_stream_id/256;
	secB[4]=transport_stream_id%256;
	secB[5]=192+(version_number*2)+current_next_indicator;
	secB[6]=section_number;
	secB[7]=last_section_number;
	secB[8]=program_number0/256;
	secB[9]=program_number0%256;
	secB[10]=224+(network_PID0/256);
	secB[11]=(network_PID0%256);


	secB[12]=program_number1/256;
	secB[13]=program_number1%256;
	secB[14]=224+(program_map_PID1/256);
	secB[15]=(program_map_PID1%256);

	CRC_32B=CRC_32Pro();
		
	secB[seclen-1]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-2]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-3]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-4]=(CRC_32B&255);
}

PAT_SEC::~PAT_SEC()
{

	delete [] secB;
}


bool PAT_SEC::sec_is_read()
{
	if (sec_read_pointer==seclen)
		return true;
	else
		return false;
}

DWORD PAT_SEC::CRC_32Pro()
{
	DWORD ulPolynomial2 = 0x04C11DB7;
	DWORD pp=ulPolynomial2 ;
	DWORD finalCRC=0;
	DWORD jj;
	byte i,g,l;
	bool s,p;
	bool * O=new bool[32];
	bool * I=new bool[32];
	bool * T=new bool[32];

	bool * R=new bool[32];
	bool * poly=new bool[32];
	pp=pp>>1;

	for (int j=0;j<32;j++)
	{
		if ((pp&1)==1)
			s=true;
		else
			s=false;

		pp=pp>>1;
		if (s)
			poly[j]=true;
		else
			poly[j]=false;
	}

	int count=0;

	for (i=0;i<32;i++)
		O[i]=true;

	for (int k=0;k<(seclen-4);k++)
	{
		g=secB[k];
		for(l=1;l<9;l++)
		{
			if ((g&128)==128)
				s=true;

			else
				s=false;

			g=g<<1;

			p=s^O[31];
			for (j=0;j<32;j++)
				T[j]=poly[j]&p;
			for(j=0;j<32;j++)
				R[j]=O[j]^T[j];
			for(j=0;j<31;j++)
				I[j+1]=R[j];
			I[0]=p;

			for(j=0;j<32;j++)
				O[j]=I[j];
			count++;
		}
		
	}
	jj=1;
	for (int z=0;z<32;z++)
	{	if (O[z])
	finalCRC=finalCRC+jj;
				jj=jj*2;
	}

 	delete [] O;
	delete [] poly;
	delete [] I;
	delete [] T;
	delete [] R;

	return finalCRC;
}

//NIT_SEC methods
NIT_SEC::NIT_SEC()
{
	table_id=0x40;
	section_syntax_indicator=1;
	section_length=19;
	network_id=1;
	version_number=0;
	current_next_indicator=1;
	section_number=0;
	last_section_number=0;
	network_descriptor_length=0;
	transport_stream_loop_length=6;
	transport_stream_id1=1;
	original_network_id1=1;
	transport_descriptor_length1=0;

	secB=new byte[1600];
	
	seclen=22;
	sec_read_pointer=0;

	secB[0]=table_id;
	secB[1]=(128*section_syntax_indicator)+48+(section_length/256);
	secB[2]=section_length%256;
	secB[3]=network_id/256;
	secB[4]=network_id%256;
	secB[5]=192+(version_number*2)+current_next_indicator;
	secB[6]=section_number;
	secB[7]=last_section_number;
	secB[8]=240+(network_descriptor_length/256);
	secB[9]=network_descriptor_length%256;
	secB[10]=240+(transport_stream_loop_length/256);
	secB[11]=transport_stream_loop_length%256;
	secB[12]=transport_stream_id1/256;
	secB[13]=transport_stream_id1%256;
	secB[14]=original_network_id1/256;
	secB[15]=original_network_id1%256;
	secB[16]=240+(transport_descriptor_length1/256);
	secB[17]=transport_descriptor_length1%256;

	CRC_32B=CRC_32Pro();
		
	secB[seclen-1]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-2]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-3]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-4]=(CRC_32B&255);
}

NIT_SEC::~NIT_SEC()
{

	delete [] secB;
}


bool NIT_SEC::sec_is_read()
{
	if (sec_read_pointer==seclen)
		return true;
	else
		return false;
}

DWORD NIT_SEC::CRC_32Pro()
{
	DWORD ulPolynomial2 = 0x04C11DB7;
	DWORD pp=ulPolynomial2 ;
	DWORD finalCRC=0;
	DWORD jj;
	byte i,g,l;
	bool s,p;
	bool * O=new bool[32];
	bool * I=new bool[32];
	bool * T=new bool[32];

	bool * R=new bool[32];
	bool * poly=new bool[32];
	pp=pp>>1;

	for (int j=0;j<32;j++)
	{
		if ((pp&1)==1)
			s=true;
		else
			s=false;

		pp=pp>>1;
		if (s)
			poly[j]=true;
		else
			poly[j]=false;
	}

	int count=0;

	for (i=0;i<32;i++)
		O[i]=true;

	for (int k=0;k<(seclen-4);k++)
	{
		g=secB[k];
		for(l=1;l<9;l++)
		{
			if ((g&128)==128)
				s=true;

			else
				s=false;

			g=g<<1;

			p=s^O[31];
			for (j=0;j<32;j++)
				T[j]=poly[j]&p;
			for(j=0;j<32;j++)
				R[j]=O[j]^T[j];
			for(j=0;j<31;j++)
				I[j+1]=R[j];
			I[0]=p;

			for(j=0;j<32;j++)
				O[j]=I[j];
			count++;
		}
		
	}
	jj=1;
	for (int z=0;z<32;z++)
	{	if (O[z])
	finalCRC=finalCRC+jj;
				jj=jj*2;
	}

 	delete [] O;
	delete [] poly;
	delete [] I;
	delete [] T;
	delete [] R;

	return finalCRC;
}


//PMT_SEC methods
PMT_SEC::PMT_SEC()
{
	table_id=0x02;
	section_syntax_indicator=1;
	section_length=18;//for one elementary component
	program_number=1;
	version_number=0;
	current_next_indicator=1;
	section_number=0;
	last_section_number=0;
	PCR_PID=0x1fff;
	program_info_length=0;
	stream_type1=0x0d;
	elementary_PID1=dataPID;
	ES_info_length1=0;

	secB=new byte[1600];
	
	seclen=21;
	sec_read_pointer=0;

	secB[0]=table_id;
	secB[1]=(128*section_syntax_indicator)+48+(section_length/256);
	secB[2]=section_length%256;
	secB[3]=program_number/256;
	secB[4]=program_number%256;
	secB[5]=192+(version_number*2)+current_next_indicator;
	secB[6]=section_number;
	secB[7]=last_section_number;
	secB[8]=224+(PCR_PID/256);
	secB[9]=PCR_PID%256;
	secB[10]=240+(program_info_length/256);
	secB[11]=program_info_length%256;
	secB[12]=stream_type1;
	secB[13]=224+(elementary_PID1/256);
	secB[14]=(elementary_PID1%256);
	secB[15]=240+(ES_info_length1/256);
	secB[16]=ES_info_length1%256;

	CRC_32B=CRC_32Pro();
		
	secB[seclen-1]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-2]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-3]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-4]=(CRC_32B&255);
}

PMT_SEC::~PMT_SEC()
{
	delete [] secB;
}


bool PMT_SEC::sec_is_read()
{
	if (sec_read_pointer==seclen)
		return true;
	else
		return false;
}

DWORD PMT_SEC::CRC_32Pro()
{
	DWORD ulPolynomial2 = 0x04C11DB7;
	DWORD pp=ulPolynomial2 ;
	DWORD finalCRC=0;
	DWORD jj;
	byte i,g,l;
	bool s,p;
	bool * O=new bool[32];
	bool * I=new bool[32];
	bool * T=new bool[32];

	bool * R=new bool[32];
	bool * poly=new bool[32];
	pp=pp>>1;

	for (int j=0;j<32;j++)
	{
		if ((pp&1)==1)
			s=true;
		else
			s=false;


		pp=pp>>1;
		if (s)
			poly[j]=true;
		else
			poly[j]=false;
	}

	int count=0;

	for (i=0;i<32;i++)
		O[i]=true;
	
	for (int k=0;k<(seclen-4);k++)
	{
		g=secB[k];
		for(l=1;l<9;l++)
		{
			if ((g&128)==128)
				s=true;

			else
				s=false;
		
			g=g<<1;

			p=s^O[31];
			for (j=0;j<32;j++)
				T[j]=poly[j]&p;
			for(j=0;j<32;j++)
				R[j]=O[j]^T[j];
			for(j=0;j<31;j++)
				I[j+1]=R[j];
			I[0]=p;

			for(j=0;j<32;j++)
				O[j]=I[j];
			count++;
		}
		
	}
	jj=1;
	for (int z=0;z<32;z++)
	{	if (O[z])
	finalCRC=finalCRC+jj;
				jj=jj*2;
	}

 	delete [] O;
	delete [] poly;
	delete [] I;
	delete [] T;
	delete [] R;

	return finalCRC;
}


//SDT_SEC methods
SDT_SEC::SDT_SEC()
{
	table_id=0x42;
	section_syntax_indicator=1;
	section_length=29;//for one service with one descriptor
	transport_stream_id=1;
	version_number=0;
	current_next_indicator=1;
	section_number=0;
	last_section_number=0;
	original_network_id=1;
	service_id1=1;
	EIT_schedule_flag1=0;
	EIT_present_following_flag1=0;
	running_status1=4;
	free_CA_mode1=0;
	descriptor_loop_length1=0x0c;

	//data_broadcast_descriptor:
	descriptor_tag=0x64;
	descriptor_length=0x0a;
	data_broadcast_id=0x0005;
	component_tag=0;
	selector_length=0x02;

	//structure:
	MAC_address_range=0x06;
	MAC_IP_mapping_flag=1;
	alignment_indicator=0;
	max_section_per_datagram=1;

	ISO_639_2_language_code=0x676e65;//eng
	text_length=0;

	secB=new byte[1600];
	
	seclen=32;
	sec_read_pointer=0;

	secB[0]=table_id;
	secB[1]=(128*section_syntax_indicator)+48+(section_length/256);
	secB[2]=section_length%256;
	secB[3]=transport_stream_id/256;
	secB[4]=transport_stream_id%256;
	secB[5]=192+(version_number*2)+current_next_indicator;
	secB[6]=section_number;
	secB[7]=last_section_number;
	secB[8]=original_network_id/256;
	secB[9]=original_network_id%256;
	secB[10]=255;
	secB[11]=service_id1/256;
	secB[12]=service_id1%256;
	secB[13]=252+EIT_schedule_flag1*2+EIT_present_following_flag1;
	secB[14]=running_status1*32+free_CA_mode1*16+(descriptor_loop_length1/256);
	secB[15]=descriptor_loop_length1%256;
	secB[16]=descriptor_tag;
	secB[17]=descriptor_length;
	secB[18]=data_broadcast_id/256;
	secB[19]=data_broadcast_id%256;
	secB[20]=component_tag;
	secB[21]=selector_length;
	secB[22]=MAC_address_range*32+MAC_IP_mapping_flag*16+alignment_indicator*8+7;
	secB[23]=max_section_per_datagram;
	
	DWORD iso=ISO_639_2_language_code;
	secB[26]=iso&255;
	iso=iso>>8;
	secB[25]=iso&255;
	iso=iso>>8;
	secB[24]=iso&255;

	secB[27]=text_length;

	CRC_32B=CRC_32Pro();
		
	secB[seclen-1]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-2]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-3]=(CRC_32B&255);
	CRC_32B=CRC_32B>>8;
	secB[seclen-4]=(CRC_32B&255);
}

SDT_SEC::~SDT_SEC()
{

	delete [] secB;
}


bool SDT_SEC::sec_is_read()
{
	if (sec_read_pointer==seclen)
		return true;
	else
		return false;
}

DWORD SDT_SEC::CRC_32Pro()
{
	DWORD ulPolynomial2 = 0x04C11DB7;
	DWORD pp=ulPolynomial2 ;
	DWORD finalCRC=0;
	DWORD jj;
	byte i,g,l;
	bool s,p;
	bool * O=new bool[32];
	bool * I=new bool[32];
	bool * T=new bool[32];

	bool * R=new bool[32];
	bool * poly=new bool[32];
	pp=pp>>1;

	for (int j=0;j<32;j++)
	{
		if ((pp&1)==1)
			s=true;
		else
			s=false;

		pp=pp>>1;
		if (s)
			poly[j]=true;
		else
			poly[j]=false;
	}

	int count=0;

	for (i=0;i<32;i++)
		O[i]=true;

	for (int k=0;k<(seclen-4);k++)
	{
		g=secB[k];
		for(l=1;l<9;l++)
		{
			if ((g&128)==128)
				s=true;

			else
				s=false;
		
			g=g<<1;

			p=s^O[31];
			for (j=0;j<32;j++)
				T[j]=poly[j]&p;
			for(j=0;j<32;j++)
				R[j]=O[j]^T[j];
			for(j=0;j<31;j++)
				I[j+1]=R[j];
			I[0]=p;

			for(j=0;j<32;j++)
				O[j]=I[j];
			count++;

		}
		
	}
	jj=1;
	for (int z=0;z<32;z++)
	{	if (O[z])
	finalCRC=finalCRC+jj;
				jj=jj*2;
	}

 	delete [] O;
	delete [] poly;
	delete [] I;
	delete [] T;
	delete [] R;

	return finalCRC;
}


//CRC_32 methods
CRC_32::CRC_32()
{
    // This is the official polynomial used by CRC-32 
    // in PKZip, WinZip and Ethernet. 
    ULONG ulPolynomial = 0x04C11DB7;//This is the polynomial used in MPEG2

    // 256 values representing ASCII character codes.
	Table=new ULONG[257];
    for(int i = 0; i <= 0xFF; i++)
    {
        Table[i] = Reflect(i, 8) << 24;
        for (int j = 0; j < 8; j++)
            Table[i] = (Table[i] << 1) ^ (Table[i] & (1 << 31) ? ulPolynomial : 0);
        Table[i] = Reflect(Table[i],  32);
    }
}

ULONG CRC_32::Reflect(ULONG ref, char ch)
{
    ULONG value = 0;
    // Swap bit 0 for bit 7
    // bit 1 for bit 6, etc.
    for (int i = 1; i < (ch + 1); i++)
    {
        if (ref & 1)
            value |= 1 << (ch - i);
        ref >>= 1;
    } 
	return value;
}

void CRC_32::Calculate(const LPBYTE buffer, UINT size, ULONG &CRC)
{   // calculate the CRC
    LPBYTE pbyte = buffer;

    while(size--)
	{
        CRC = (CRC >> 8) ^ Table[(CRC & 0xFF) ^ *pbyte++];

	}
}

CRC_32::~CRC_32()
{
	delete [] Table;
}


//TSpacket methods
TSpacket::TSpacket()
{

	  transport_error_indicator = 0;
	  pusi = 0;
	  transport_priority = 0;
	  PID=0x1fff;
	  transport_scrambling_control = 0;
	  adaptation_field_control=1;//for '11' ,the amount is 3
	  continuity_counter = 0;
	  pointer_field=0;

	  ts_data_pointer=0;
	tspayloadB=new byte[188];
}

TSpacket::~TSpacket()
{
	delete [] tspayloadB;
}

bool TSpacket::set_pointer_field()
{
	pointer_field=ts_data_pointer;
	return true;
}

bool TSpacket::read_from_sec(Data_sec * section)
{
	(ts_data_pointer)++; 
    (section->sec_read_pointer)++;
	tspayloadB[(ts_data_pointer)-1]=section->secB[(section->sec_read_pointer)-1];
	return true;
}

bool TSpacket::read_from_secPAT(PAT_SEC * section)
{
	(ts_data_pointer)++; 
    (section->sec_read_pointer)++;
	tspayloadB[(ts_data_pointer)-1]=section->secB[(section->sec_read_pointer)-1];
	return true;
}

bool TSpacket::read_from_secPMT(PMT_SEC * section)
{
	(ts_data_pointer)++; 
    (section->sec_read_pointer)++;
	tspayloadB[(ts_data_pointer)-1]=section->secB[(section->sec_read_pointer)-1];
	return true;
}

bool TSpacket::read_from_secSDT(SDT_SEC * section)
{
	(ts_data_pointer)++; 
    (section->sec_read_pointer)++;
	tspayloadB[(ts_data_pointer)-1]=section->secB[(section->sec_read_pointer)-1];
	return true;
}

bool TSpacket::read_from_secNIT(NIT_SEC * section)
{
	(ts_data_pointer)++; 
    (section->sec_read_pointer)++;
	tspayloadB[(ts_data_pointer)-1]=section->secB[(section->sec_read_pointer)-1];
	return true;
}

bool TSpacket::TS_is_complete()
{
	if (pusi==0)
	{
		if (ts_data_pointer>=184)
			return true;
		else 
			return false;
	}
	else
	{
		if (ts_data_pointer>=183)
			return true;
		else
			return false;
	}

}

bool TSpacket::send_ts()
{
	int sizeW;
	BYTE ch=0x47;
	sizeW=fwrite(&ch,1,1,wfile);

	ch=(transport_error_indicator*128)+pusi*64+(transport_priority*32)+(PID/256);
	sizeW=fwrite(&ch,1,1,wfile);

	ch=PID%256;
	sizeW=fwrite(&ch,1,1,wfile);

	if (PID!=0x1fff)
	{
		int j=0;
		bool find=false;
		while (!find)
		{
			j++;
			if (PIDarray[j]==PID)
				find=true;
		}
		continuity_counter=PIDcounter[j];
		PIDcounter[j]++;
		if (PIDcounter[j]==16)
			PIDcounter[j]=0;
	}
	
	ch=(transport_scrambling_control*64)+(adaptation_field_control*16)+continuity_counter;
	sizeW=fwrite(&ch,1,1,wfile);

	int rest=184;
	if (pusi==1)
	{ 
		ch=pointer_field;
		sizeW=fwrite(&ch,1,1,wfile);
		rest=183;
	}
	
	sizeW=fwrite(tspayloadB,1,rest,wfile);
	Mytsnumber++;

	char h='0'+continuity_counter;
	return true;

}

bool TSpacket::have_enough_space()
{
	if (ts_data_pointer>180)
		return false;
	else 
		return true;
}

bool TSpacket::make_NULL()
{
	PID=0x1fff;
	//it seems that just the PID=0x1fff if enough!
	for (int j=0;j<184;j++)
		tspayloadB[j]=0xff;
	return true;
}

bool TSpacket::stuff_ts()
{
	for (int j=ts_data_pointer;j<184;j++)
		tspayloadB[j]=0xff;
	return true;
}