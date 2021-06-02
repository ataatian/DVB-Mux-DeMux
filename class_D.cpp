// demux.cpp classes: Ali Taatian May 27, 2021
#include "stdAfx.h"
#include "class.h"


//TSpacket methods
TSpacket::TSpacket()
{
	TSbuffer=new byte[188];
	is_read=0;
}

TSpacket::~TSpacket()
{
	delete TSbuffer;
}

bool TSpacket::readTS_fromfile(FILE * infile)
{
	int sizeR;
	do{
	 sizeR=fread(TSbuffer,1,1,infile);
	}while ((TSbuffer[0]!=0x47)&&(sizeR>0));
	if (sizeR<1) 
		return false;
	sizeR=fseek(infile,-1,SEEK_CUR);
	sizeR=fread(TSbuffer,1,188,infile);
	if (sizeR<188)
		return false;

	PID=TSbuffer[2]+(TSbuffer[1]&31)*256;
	continuity_counter=TSbuffer[3]&15;
	pusi=TSbuffer[1]&64;
	pusi=pusi>>6;
	return true;
}


//PAT_SEC methods
PAT_SEC::PAT_SEC()
{
	secB=new byte[1600];
	sec_pointer=0;
	seclen=4;
}

PAT_SEC::~PAT_SEC()
{
	delete [] secB;
}

void PAT_SEC::fillfromTS(TSpacket* pack)
{
	int j=4;
	if (pack->pusi==1)
		j=5;
	//int i=sec_pointer;
	while((j<184)&&(sec_pointer<seclen))
	{
		secB[sec_pointer]=pack->TSbuffer[j];
		if (sec_pointer==2)
		{
			seclen=3+secB[2]+(secB[1]&15)*256;
		}
		sec_pointer++;
		j++;
	}

}

bool PAT_SEC::PAT_is_made()
{

	if (sec_pointer==seclen)
	{
		transport_stream_id=secB[4]+secB[3]*256;
		version_number=(secB[5]&62)>>1;
		program_number1=secB[9]+secB[8]*256;
		
		program_map_PID1=secB[11]+(secB[10]&31)*256;  
		return true;
	}
	else
		return false;
}

bool PAT_SEC::PAT_CRCcheck()
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

	for (int k=0;k<(seclen);k++)
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
	bool result=true;
	for (int z=0;z<32;z++)
	{	if (O[z])
		result=false;
	}

 	delete [] O;
	delete [] poly;
	delete [] I;
	delete [] T;
	delete [] R;

	return result;

}


//PMT_SEC methods

PMT_SEC::PMT_SEC()
{
	secB=new byte[1600];
	sec_pointer=0;
	seclen=4;
}

PMT_SEC::~PMT_SEC()
{
	delete [] secB;
}

void PMT_SEC::fillfromTS(TSpacket* pack)
{
	int j=4;
	if (pack->pusi==1)
		j=5;
	while((j<184)&&(sec_pointer<seclen))
	{
		secB[sec_pointer]=pack->TSbuffer[j];
		if (sec_pointer==2)
		{
			seclen=3+secB[2]+(secB[1]&15)*256;
		}
		sec_pointer++;
		j++;
	}

}

bool PMT_SEC::PMT_is_made()
{

	if (sec_pointer==seclen)
	{
		program_number=secB[4]+secB[3]*256;
		version_number=(secB[5]&62)>>1;
		stream_type1=secB[12];
		elementary_PID1=secB[14]+(secB[13]&31)*256;

		return true;
	}
	else
		return false;
}

bool PMT_SEC::PMT_CRCcheck()
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

	
	
	for (int k=0;k<(seclen);k++)
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
	bool result=true;
	for (int z=0;z<32;z++)
	{	if (O[z])
		result=false;
	}

 	delete [] O;
	delete [] poly;
	delete [] I;
	delete [] T;
	delete [] R;

	return result;

}

short PMT_SEC::getE_PID()
{
	return elementary_PID1;
}


//Data_sec methods

Data_sec::Data_sec()
{
	secB=new byte[1600];
	sec_pointer=0;
	seclen=4;
}

Data_sec::~Data_sec()
{
	delete [] secB;
}

bool Data_sec::fillfromTS(TSpacket* pack)
{
	int j=pack->is_read;
	

	if  (j==0)
	
		if (pack->pusi==1)
			j=5;
		else
			j=4;
	
	while((j<188)&&(sec_pointer<seclen))
	{
	
		secB[sec_pointer]=pack->TSbuffer[j];

		if (sec_pointer==2)
		
			seclen=3+secB[2]+(secB[1]&15)*256;
		
		sec_pointer++;
		j++;
	}
	pack->is_read=j;
	if (pack->is_read==184)
		return false;
	else
	{
		if (pack->TSbuffer[pack->is_read]==0xff)
			return false;
		else
			return true;
	}

}

bool Data_sec::SEC_is_made()
{

	if (sec_pointer==seclen)
	{
		MAC_6=secB[3];
		MAC_5=secB[4];
		MAC_4=secB[8];
		MAC_3=secB[9];
		MAC_2=secB[10];
		MAC_1=secB[11];
		return true;
	}
	else
		return false;
}

bool Data_sec::SEC_CRCcheck()
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
	
	for (int k=0;k<(seclen);k++)
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
	bool result=true;
	for (int z=0;z<32;z++)
	{	if (O[z])
		result=false;
	}

 	delete [] O;
	delete [] poly;
	delete [] I;
	delete [] T;
	delete [] R;

	return result;

}
void Data_sec::FillToFile(FILE * outfile)
{
	BYTE * buf=new BYTE[seclen-16];
	for (int j=0;j<(seclen-16);j++)
		buf[j]=secB[j+12];

	int sizeW=fwrite(buf,1,(seclen-16),outfile);
	delete [] buf;
}


//Program methods
Program::Program()
{
	number_of_elementary_streams=1;
}

Program::~Program()
{

}
      
void Program::set(PAT_SEC * pat)
{
	program_map_PID=pat->program_map_PID1;
}

short Program::getPMT_PID()
{
	return program_map_PID;
}