// demux.cpp : Defines the entry point for the console application. Ali Taatian May 27, 2021


#include "stdAfx.h"
#include "class.h"

	const char * inFileName="c:\\mux.txt";
	const char * outFileName="c:\\demux.txt";

void main()
{
	bool Errorcheck=true;
	bool another_sec;
	FILE * pfile=fopen(inFileName,"rb");//the file which is going to be sent
	FILE * wfile=fopen(outFileName,"ab");//the file which TS packets are written in it

	TSpacket * packet;
	PAT_SEC * PAT_sec=new PAT_SEC;
	do
	{
		delete PAT_sec;
		PAT_sec=new PAT_SEC;	
		do
		{
			packet=new TSpacket ;
			Errorcheck=packet->readTS_fromfile(pfile);
			
			if (!Errorcheck)
			{
				cout<<"No PAT found in stream:Error"<<"\n";
				fclose(pfile);
				fclose(wfile);
				delete PAT_sec;
				delete packet;
				exit(0);
			}

			if (packet->PID==0)
				PAT_sec->fillfromTS(packet);
			delete packet;

		}while(!PAT_sec->PAT_is_made());
		
	}while(!PAT_sec->PAT_CRCcheck());
	
	Program * program1=new Program;
	program1->set(PAT_sec);

	PMT_SEC * PMT_sec=new PMT_SEC;
	short PMT_PID=program1->getPMT_PID();

	do
	{
		delete PMT_sec;
		PMT_sec=new PMT_SEC;

		do
		{
			packet=new TSpacket ;
			Errorcheck=packet->readTS_fromfile(pfile);
			if (!Errorcheck)
			{
				cout<<"No PMT found:Error";
				fclose(pfile);
				fclose(wfile);
				delete PAT_sec;
				delete packet;
				delete program1;
				delete PMT_sec;
				exit(0);
			}

			if (packet->PID==PMT_PID)
				PMT_sec->fillfromTS(packet);
			delete packet;
		}while(!PMT_sec->PMT_is_made());
		
	}while(!PMT_sec->PMT_CRCcheck());

	short E_PID=PMT_sec->getE_PID();

	//Now,all the parameters are known

	Data_sec * data_sec=new Data_sec;


	while (true)
	{
		packet=new TSpacket ;
		Errorcheck=packet->readTS_fromfile(pfile);



		if (!Errorcheck)
		{
			cout<<"finish"<<"\n";
			delete packet;
			break;
		}


		if (packet->PID==0x1fff)
		{
			//NULL
//+++++++++++++++++++			cout<<"OOOOOOOOOOOOOOOOOOO NULL"<<"\n";
		}
		if (packet->PID==0)
		{
//+++++++++++++++++++++			cout<<"OOOOOOOOOOOOOOOOOOO PAT"<<"\n";
			
		}
		if (packet->PID==PMT_PID)
		{
//++++++++++++++++++++++			cout<<"OOOOOOOOOOOOOOOOOOO PMT"<<"\n";

		}
		if (packet->PID==E_PID)
		{
			
			do{
				another_sec=data_sec->fillfromTS(packet);
				if (data_sec->SEC_is_made())
				{
					if (data_sec->SEC_CRCcheck())
						data_sec->FillToFile(wfile);
					delete data_sec;
					data_sec=new Data_sec;
				}
				else
					break;
			}while(another_sec);
		}
		delete packet;
			
	}


	delete data_sec;
	delete PAT_sec;
	delete PMT_sec;
	delete program1;

	fclose(pfile);
	fclose(wfile);	

}
