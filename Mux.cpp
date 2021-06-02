// mux.cpp : Defines the entry point for the console application. Ali Taatian May 27, 2021


#include "stdAfx.h"
#include "Class.h"

const char * inFileName="c:\\in.txt";
const char * outFileName="c:\\mux.txt";

	int Number_of_PIDs;//keeps the number of different PIDs in the stream
	int Mytsnumber;//keeps the number of TS packets sent
	bool enough;
	
	short * PIDarray;
	BYTE * PIDcounter;
	FILE * pfile;
	FILE * wfile;

void mynewPID(short );

void main()
{
	enough=false;
	bool Errorcheck;
	bool process_command=true;
	int i=0;
	//number of TS packets to be produced
	Mytsnumber=0;

//alocation of PID counter
	Number_of_PIDs=0;//keeps the number of different PIDs in the stream
	PIDarray=new short[MAXPID];
	PIDcounter=new BYTE[MAXPID];
	mynewPID(dataPID);//PID=0x0101 for data packets
	mynewPID(PAT_PID);//PID=0x0000 for PAT 
	mynewPID(NIT_PID);
	mynewPID(PMT_PID);
	mynewPID(SDT_PID);

//alocation of source and destination
	pfile=fopen(inFileName,"rb");//the file which is going to be sent
	wfile=fopen(outFileName,"ab");//the file which TS packets are written in it

//alocation of objects
	Data_buffer *data_buff;
	data_buff=new Data_buffer;//creates a new data buffer
    //data_buff->initial();
	TSpacket *ts;
	Data_sec *data_sec;
	PAT_SEC * PAT_sec;
	PMT_SEC * PMT_sec;
	SDT_SEC * SDT_sec;
	NIT_SEC * NIT_sec;


	//test CRC generator^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/*
	unsigned char pBufferIn[] = {0x00,0xb0,0x39,0x04,0x37,0xc7,0x00,0x00,0x6d,0x66,0xe0,0x64,0x6d,0x6b,0xe2,0x58,
	0x6d,0x6e,0xe2,0x8a,0x6d,0x70,0xe4,0x4c,0x6d,0x67,0xe0,0xc8,0x6d,0x6d,0xe3,0x20,
	0x6d,0x6c,0xe2,0xbc,0x6d,0x68,0xe1,0x2c,0x6d,0x6f,0xe4,0x0a,0x6d,0x69,0xe1,0x90,
	0x6d,0x71,0xe3,0xec,0x6d,0x6a,0xe1,0xf4 };
*/
/*	unsigned char pBufferIn[]={
		0x02, 0xb0, 0xc1, 0x6d, 0x67, 0xc3,0x00, 0x00, 0xe0, 0xd2, 0xf0, 0x00, 0x02, 0xe0, 0xd2, 0xf0, 
		0x00, 0x03, 0xe0, 0xdc, 0xf0 ,0x06, 0x0a, 0x04, 0x64, 0x65, 0x75, 0x01, 0x06, 0xe0, 0xe6, 0xf0, 
		0x29, 0x56, 0x05, 0x64, 0x65, 0x75, 0x09, 0x00, 0x45, 0x20, 0x01, 0x18, 0xe7, 0xe8, 0xe9, 0xea, 
		0xeb, 0xec, 0xed ,0xee, 0xef, 0xf4, 0xf5, 0xf6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 
		0xcf, 0xd4, 0xd5, 0xd6, 0x04, 0x01, 0xf0, 0x05 ,0x01, 0xf7, 0x0b, 0xe3, 0xbc, 0xf0, 0x10, 0x66 ,
		0x04, 0x00, 0xf0, 0x00, 0x01, 0x52, 0x01, 0x0b, 0x13, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0b, 
		0xe3, 0xc3, 0xf0, 0x10, 0x66, 0x04, 0x00, 0xf0, 0x00, 0x01, 0x52, 0x01, 0x10, 0x13 ,0x05, 0x00, 
		0x00, 0x00, 0x03, 0x00, 0x0b, 0xe3, 0xc2, 0xf0, 0x10, 0x66, 0x04, 0x00, 0xf0, 0x00, 0x01, 0x52 ,
		0x01, 0x0f, 0x13, 0x05, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0b, 0xe3, 0xc1, 0xf0, 0x10, 0x66, 0x04, 
		0x00, 0xf0, 0x00, 0x01, 0x52, 0x01, 0x0e, 0x13, 0x05, 0x00, 0x00, 0x00 ,0x03, 0x00, 0x0b, 0xe3, 
		0xc0, 0xf0, 0x10, 0x66, 0x04, 0x00, 0xf0, 0x00, 0x01, 0x52, 0x01, 0x0d, 0x13, 0x05, 0x00, 0x00, 
		0x00, 0x03, 0x00, 0x05, 0xe3, 0xbb, 0xf0, 0x08, 0x52, 0x01, 0x70, 0x6f, 0x03, 0x00, 0x01 ,0xe0 };	
*/
	
	i=0;
	while ((process_command==true)&&(!enough))            
	{

// Here we send SI tables!

		if ((i%SI_sending_interval)==0)
		{
			//sending PAT table
			ts=new TSpacket;
			ts->pusi=1;
			Errorcheck=ts->set_pointer_field();
			ts->PID=PAT_PID;
			PAT_sec=new PAT_SEC;
			do
			{	
				Errorcheck=ts->read_from_secPAT(PAT_sec);
				
				if (ts->TS_is_complete())
				{
					Errorcheck=ts->send_ts();
					delete ts;
					ts=new TSpacket;
					ts->PID=PAT_PID;
				}
			}while(!PAT_sec->sec_is_read());
			delete PAT_sec;

			Errorcheck=ts->stuff_ts();
			Errorcheck=ts->send_ts();
			delete ts;

			//sending NIT table
			ts=new TSpacket;
			ts->pusi=1;
			Errorcheck=ts->set_pointer_field();
			ts->PID=NIT_PID;
			NIT_sec=new NIT_SEC;
			do
			{	
				Errorcheck=ts->read_from_secNIT(NIT_sec);
				
				if (ts->TS_is_complete())
				{
					Errorcheck=ts->send_ts();
					delete ts;
					ts=new TSpacket;
					ts->PID=NIT_PID;
				}
			}while(!NIT_sec->sec_is_read());
			delete NIT_sec;

			Errorcheck=ts->stuff_ts();
			Errorcheck=ts->send_ts();
			delete ts;

			//sending PMT table
			ts=new TSpacket;
			ts->pusi=1;
			Errorcheck=ts->set_pointer_field();
			ts->PID=PMT_PID;
			PMT_sec=new PMT_SEC;
			do
			{	
				Errorcheck=ts->read_from_secPMT(PMT_sec);
				
				if (ts->TS_is_complete())
				{
					Errorcheck=ts->send_ts();
					delete ts;
					ts=new TSpacket;
					ts->PID=PMT_PID;
				}
			}while(!PMT_sec->sec_is_read());
			delete PMT_sec;

			Errorcheck=ts->stuff_ts();
			Errorcheck=ts->send_ts();
			delete ts;

			//sending SDT table
			ts=new TSpacket;
			ts->pusi=1;
			Errorcheck=ts->set_pointer_field();
			ts->PID=SDT_PID;
			SDT_sec=new SDT_SEC;
			do
			{	
      				Errorcheck=ts->read_from_secSDT(SDT_sec);
				
				if (ts->TS_is_complete())
				{
					Errorcheck=ts->send_ts();
					delete ts;
					ts=new TSpacket;
					ts->PID=SDT_PID;
				}
			}while(!SDT_sec->sec_is_read());
			delete SDT_sec;

			Errorcheck=ts->stuff_ts();
			Errorcheck=ts->send_ts();
			delete ts;
		}
//end of sending SI tables

		ts=new TSpacket;//creates a new TS packet
		while (data_buff->check_for_data())
		{
			i++;           
		   data_sec=new Data_sec;//creates a new section packet
		   Errorcheck=data_sec->fill_from_databuffer(data_buff);
		   //now,the section (data_buff) is made
		   if (ts->pusi==0)
		   {
			   ts->pusi=1;
			   Errorcheck=ts->set_pointer_field();
		   }
		   do
		   {	
			   Errorcheck=ts->read_from_sec(data_sec);
			   ts->PID=dataPID;
			   if (ts->TS_is_complete())
			   {
				   Errorcheck=ts->send_ts();
				   delete ts;
				   ts=new TSpacket;
			   }
		   }while(!data_sec->sec_is_read());
		   delete data_sec;

		   if (!ts->have_enough_space())
			   break;
		   if ((i%SI_sending_interval)==0)
			   break;
		}

		if (ts->PID==0x1fff)
		{
			Errorcheck=ts->make_NULL();
			Errorcheck=ts->send_ts();
			delete ts;
		}
		else{
			Errorcheck=ts->stuff_ts();
			Errorcheck=ts->send_ts();
			delete ts;
		}
			
	}
	delete data_buff;
	fclose(pfile);
	fclose(wfile);
	delete [] PIDarray;
	delete [] PIDcounter;
	cout<<Mytsnumber<<" TS packet is sent!"<<"\n";
}

void mynewPID(short newPID)//it alocates counter to each PID
{
	Number_of_PIDs++;
	PIDarray[Number_of_PIDs]=newPID;
	PIDcounter[Number_of_PIDs]=0;
}

