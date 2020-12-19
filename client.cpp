#include "myheader.h" 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <openssl/md5.h> 
#include<bits/stdc++.h>
#define PORT 8080
#define BUFFER_SZE 20240
#define FILE_SIZE 524288
#define BUFFSIZE 16384
using namespace std;
string serv_ip = "127.0.0.1";
string serv_port = "8080";
string cport;
string ci;
string currid;
string currgrp;
bool islogged=0;
vector<thread> threads;
vector<thread> clientprocessVector;


vector<string> parsestr(string st, char dmn){
		//if(st=="logout")
		stringstream ss(st);
		string temp;
		vector<string> res;
		while(getline(ss,temp,dmn)){

			res.push_back(temp);
		}
		return res;
}

string getfilename(string st){
	int ln = st.size()-1;
	while(st[ln]!='/'){
		ln--;
	}
	string filename = st.substr(ln+1,st.size()-ln-1);
	return filename;
}

std::ifstream::pos_type getfilesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}


string get_sha1hash( const std::string& fname) 
{ 
 
  char buffer[BUFFSIZE]; 
  unsigned char digest[MD5_DIGEST_LENGTH]; 
 
  std::stringstream ss; 
  std::string md5string; 
   
  std::ifstream ifs(fname, std::ifstream::binary); 
 
  MD5_CTX md5Context; 
 
  MD5_Init(&md5Context); 
 
 
   while (ifs.good())  
    { 
 
       ifs.read(buffer, BUFFSIZE); 
 
       MD5_Update(&md5Context, buffer, ifs.gcount());  
    } 
 
   ifs.close(); 
 
   int res = MD5_Final(digest, &md5Context); 
    
    if( res == 0 ) // hash failed 
      return {};   // or raise an exception 
 
  // set up stringstream format 
  ss << std::hex << std::uppercase << std::setfill('0'); 
 
 
 for(unsigned char uc: digest) 
    ss << std::setw(2) << (int)uc; 
 
 
  md5string = ss.str(); 
  
 return md5string; 
}


int login(string u_id, string u_pass){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 

	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="login";
	msg+=";"+u_id+";"+u_pass+";"+cport;

	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE); 
	msg=buffer;
	vector<string> retmsg = parsestr(msg,';');
	printf("Tracker--> %s\n",retmsg[0].c_str());
	if(retmsg[1]=="1"){
		islogged=1;
		currid=u_id;
	}
	bzero(buffer,sizeof(buffer));
	return 0;
}

int create_user(string u_id, string u_pass){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 

	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="create_user";
	msg+=";"+u_id+";"+u_pass;

	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE); 
	printf("Tracker--> %s\n",buffer );
	bzero(buffer,sizeof(buffer));
	return 0;
}

int create_group(string g_id){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 

	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="create_group";
	msg+=";"+g_id+";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	msg=buffer;
	vector<string> retmsg = parsestr(msg,';');
	printf("Tracker--> %s\n",retmsg[0].c_str());
	if(retmsg[1]=="1"){
		currgrp=g_id;
	}
	bzero(buffer,sizeof(buffer));
	return 0;

}

int join_group(string g_id){

	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="join_group";
	msg+=";"+g_id+";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	msg=buffer;
	vector<string> retmsg = parsestr(msg,';');
	printf("Tracker--> %s\n",retmsg[0].c_str());
	if(retmsg[1]=="1"){
		currgrp=g_id;
	}
	bzero(buffer,sizeof(buffer));
	return 0;

}

int logout(){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="logout";
	msg+=";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	printf("Tracker--> %s\n",buffer);
	islogged=0;
	bzero(buffer,sizeof(buffer));
	return 0;
}

int list_requests(string g_id){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="list_requests";
	msg+=";"+g_id+";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	printf("Tracker--> %s\n",buffer);
	bzero(buffer,sizeof(buffer));
	return 0;

}

int accept_request(string g_id, string u_id){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="accept_request";
	msg+=";"+g_id+";"+u_id+";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	printf("Tracker--> %s\n",buffer);
	bzero(buffer,sizeof(buffer));
	return 0;

}

int list_group(){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="list_group";
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	printf("Tracker--> %s\n",buffer);
	bzero(buffer,sizeof(buffer));
	return 0;
}

int leave_group(string g_id){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="leave_group";
	msg+=";"+g_id+";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	printf("Tracker--> %s\n",buffer);
	bzero(buffer,sizeof(buffer));
	return 0;

}
int stop_share(string g_id, string f_name){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+=";"+g_id+";"+f_name+";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	printf("Tracker--> %s\n",buffer);
	bzero(buffer,sizeof(buffer));
	return 0;


}


int list_files(string g_id){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="";
	msg+="list_files";
	msg+=";"+g_id+";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	printf("Tracker--> %s\n",buffer);
	bzero(buffer,sizeof(buffer));
	return 0;	
}


int upload_file(string f_id, string g_id){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="upload_file";
	msg+=";"+g_id+";"+f_id+";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	printf("Tracker--> %s\n",buffer);
	bzero(buffer,sizeof(buffer));
	return 0;

}

int connecttoclinet(string clntport,string fileid, int chnkno, string dest_path,int noofchnk){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(stoi(clntport));
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);
	string msg="download_chunk";
	msg+=";"+fileid+";"+to_string(chnkno);
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
   	bzero(buffer,sizeof(buffer));

    int count=0;
    int sum=0;
    fstream fs;
    cout<<"okkkkkk";
      fs.open(dest_path,ios::out|ios::in|ios::binary);
        if (fs.fail() ==0)
  {
    //in << "lorem ipsum";
    std::cout << "Operation successfully performed\n";
  }
  else
  {
    std::cout << "Error opening file";
    cerr << "Error: " << strerror(errno);
    //return;
  }
     cout<<chnkno<<" "<<"chunk number"<<"\n";
      fs.seekp(chnkno*(FILE_SIZE),ios::beg);
      cout<<"download file in piece in progress "<<"for chunk "<<endl;
      int val=1;
      int sz = getfilesize(fileid.c_str());
      int size = min(FILE_SIZE, sz - chnkno*(FILE_SIZE));
   while(val>0){
   		int BUFFER_SZES = min(20240,size);
	   	val=recv(c_sock,buffer,BUFFER_SZES,0);
	   	cout<<"ok"<<" "<<val<<" "<<size<<endl;
	   	if(val>0){
	      fs.write(buffer,val);   //
	      bzero(buffer,sizeof(buffer));
	      sum=sum+val;
	      size=size-val;
      }
   }

      fs.close();
      //sem_post(&m);
    if(noofchnk-1==chnkno){
    string destsha = get_sha1hash(dest_path);
 	string cursha = get_sha1hash(fileid);
 	cout<<"SHA1 OF PRESENT FILE : "<<cursha<<"\n";
	cout<<"SHA1 OF DOWNLOADED FILE : "<<destsha<<"\n";
	if(cursha==destsha){
		//upload_file(dest_path,g_id);
	}
	else{
		cout<<"Corrupted file receieved."<<"\n";
	}
}
   	  close(c_sock);

	return 0;
}

string getfilepath(string f_name){
	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 

	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		exit(1);
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		exit(1);
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		exit(1);
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg = "get_file_id";
	msg +=";"+f_name;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	read(c_sock , buffer, BUFFER_SZE);
	string fileid=buffer;
	bzero(buffer,sizeof(buffer));

	close(c_sock);

	return fileid;

}

int download_file(string g_id, string f_name, string dest_path){

	int c_sock=0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[BUFFER_SZE] = {0}; 
	vector<thread> filedownloadthread;
	if ((c_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Unable to connect to tracker.. \n"); 
		return -1;
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	if(inet_pton(AF_INET, serv_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1;
	} 
	if (connect(c_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{ 
		printf("\nConnection Failed from Tracker.. \n"); 
		return -1;
	} 
	bzero(&(serv_addr.sin_zero),8);

	string msg="dwnld_file";
	msg+=";"+g_id+";"+f_name+";"+currid;
	send(c_sock,msg.c_str(),strlen(msg.c_str()),0);
	valread = read(c_sock , buffer, BUFFER_SZE);
	printf("Tracker--> %s\n",buffer);
	string Pinfo = buffer;
	bzero(buffer,sizeof(buffer));

	vector<string> infoPort = parsestr(Pinfo,';');
	//cout<<infoPort.size()<<" ";

	if(infoPort.size()>1){

	
	string fileid = getfilepath(f_name);
	cout<<fileid<<"ok";
	bzero(buffer,sizeof(buffer));
	//string cursha = get_sha1hash(fileid);
	//cout<<"SHA1 OF PRESENT FILE : "<<cursha<<"\n";
	dest_path+="/"+f_name;
	ofstream output(dest_path);
	
	int szfile = stoi(infoPort[0]);
	int noofchunks = ceil(szfile/FILE_SIZE)+1;
	int noofuserhavfile = infoPort.size()-1;
	int chnkno = 0;
	cout<<noofchunks<<"no of chunks"<<"\n";
	if(noofuserhavfile >= 1){
		while(ceil((noofchunks-chnkno)/noofuserhavfile)>1){
	for(int i=0; i<noofuserhavfile; ++i){
		filedownloadthread.push_back(thread(connecttoclinet,infoPort[i+1],fileid,chnkno, dest_path,noofchunks));
		chnkno++;
		cout<<chnkno<<"chnk noumber"<<"\n";
		}
	}
	while(chnkno!=noofchunks){
		int scno = rand()%noofuserhavfile+1;
		filedownloadthread.push_back(thread(connecttoclinet,infoPort[scno],fileid,chnkno, dest_path,noofchunks));
		chnkno++;
	}
		
	for(auto itr=filedownloadthread.begin();itr!=filedownloadthread.end();itr++){
		if(itr->joinable()) 
			itr->join();
	
	}
	

	}
	else{
		cout<<"No user have files"<<"\n";
	}
}
else{
	cout<<"No user have file"<<"\n";
	
}
return 0;
}

void download_chunk(int request_sock, string fil_id, string chnk_no){

	FILE *fp=fopen(fil_id.c_str(),"rb");
    cout<<"in line 365"<<endl;
   int val;
   char buffer[BUFFER_SZE];
   bzero(buffer,BUFFER_SZE);

      fseek(fp,stoi(chnk_no)*(FILE_SIZE),SEEK_SET);
      int sz = getfilesize(fil_id.c_str());
      int size = min(FILE_SIZE, sz - stoi(chnk_no)*(FILE_SIZE));
    
      while(size>0){
      	int BUFFER_SZES = min(20240,size);
      	val=fread(buffer,sizeof(char),BUFFER_SZES,fp);
      	if(val>0){
          cout<<"Sent chunk of current size "<<val<<" "<<size<<endl;
         send(request_sock,buffer,val,0);
         memset ( buffer , '\0', sizeof(buffer));
         size=size-val;
     }
      }
      bzero(buffer,BUFFER_SZE);
   
   fclose(fp);
   //close(request_sock);
}



void downloadRequest(int request_sock){
	char buffer[BUFFER_SZE];
	bzero(buffer,BUFFER_SZE);
	int cli_msg = read(request_sock , buffer, BUFFER_SZE);

	string curmsg = buffer;
	cout<<curmsg;
	bzero(buffer,sizeof(buffer));
	vector<string> rmsg=parsestr(curmsg,';');

	if(rmsg[0]=="download_chunk"){
		string fil_id = rmsg[1];
		string chnk_no = rmsg[2];
		download_chunk(request_sock, fil_id, chnk_no);
	}
}

void clientserver(){

	int peer_sock, request_sock; 
	struct sockaddr_in peer_address; 
	struct sockaddr_in request_addr;
	int opt = 1; 
	int addrlen = sizeof(peer_address); 
	char buffer[BUFFER_SZE] = {0}; 	

	if ((peer_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(peer_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	peer_address.sin_family = AF_INET; 
	peer_address.sin_addr.s_addr = INADDR_ANY; 
	peer_address.sin_port = htons(stoi(cport)); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(peer_sock, (struct sockaddr *)&peer_address, sizeof(peer_address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(peer_sock, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

	while(1){
		if((request_sock = accept(peer_sock, (struct sockaddr *)&peer_address, (socklen_t*)&addrlen))!=-1){
		string ips=inet_ntoa(request_addr.sin_addr);
  		int ports=(ntohs(request_addr.sin_port));
  		//cout<<"ip="<<ip<<" port :"<<port<<" ";
  		clientprocessVector.push_back(thread(downloadRequest,request_sock));
	}
}
	for(auto itr=clientprocessVector.begin();itr!=clientprocessVector.end();itr++)
	{
		if(itr->joinable()) 
			itr->join();
	}

}

int main(int argc, char const *argv[]){ 

	if(argc!=3){
		cout<<"enter correct command line arguments.";
		perror("Error in command line argument list ");
		return -1;
	}
	vector<string> cip = parsestr(argv[1],':');
	cport=cip[1];
	ci=cip[0];
	thread serverthread(clientserver);
	//serverthread.detach();
	while(1){
	char buf[BUFFER_SZE];
	fgets(buf,BUFFER_SZE, stdin);
	string str = buf;
	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
	vector<string> cmnd = parsestr(str,' ');
	//cout<<cmnd[0]<<"\n";
	if(cmnd[0]=="create_user"){

		if(!islogged){
		if(cmnd.size()!=3)
			cout<<"Enter correct command"<<endl;
		else{
			threads.push_back(thread(create_user,cmnd[1],cmnd[2]));
		}
	}
	else{
		printf("Someone is already logged in!!\n");
	}

	}
	else if(cmnd[0]=="login"){
		if(!islogged){
		if(cmnd.size()!=3)
			cout<<"Enter correct command"<<endl;
		else{
			threads.push_back(thread(login,cmnd[1],cmnd[2]));
		}
	}
	else{
		printf("Someone is already logged in!!\n");
	}
	}
	else if(cmnd[0]=="create_group"){
		if(islogged){
		if(cmnd.size()!=2)
			cout<<"Enter correct command"<<endl;
		else{
			threads.push_back(thread(create_group,cmnd[1]));
		}
	}
	else{
		printf("No user is currently logged in to make the group!!\n");
	}
	}
	else if(cmnd[0]=="join_group"){
		if(islogged){
		if(cmnd.size()!=2)
			cout<<"Enter correct command"<<endl;
		else{
			threads.push_back(thread(join_group,cmnd[1]));
		}
	}
	else{
		printf("No user is currently logged in to join the group!!\n");
	}
	}
	else if(cmnd[0]=="logout"){
		if(islogged){
		if(cmnd.size()!=1)
			cout<<"Enter correct command"<<endl;
		else{
			threads.push_back(thread(logout));
		}
	}
	else{
		printf("No user is logged in currently!!\n");
	}
	}
	else if(cmnd[0]=="list_requests"){
		if(islogged){
			if(cmnd.size()!=2)
				cout<<"Enter correct command"<<endl;
			else{
				threads.push_back(thread(list_requests,cmnd[1]));
			}
		}
		else{
		printf("No user is logged in currently!!\n");
	}

	}
	else if(cmnd[0]=="accept_request"){
		if(islogged){
			if(cmnd.size()!=3)
				cout<<"Enter correct command"<<endl;
			else{
				threads.push_back(thread(accept_request,cmnd[1],cmnd[2]));
			}
		}
		else{
		printf("No user is logged in currently!!\n");
	}
	}
	else if(cmnd[0]=="list_group"){
		if(cmnd.size()!=1)
			cout<<"Enter correct command"<<endl;
		else{
				threads.push_back(thread(list_group));
			}
	}
	else if(cmnd[0]=="leave_group"){
		if(islogged){
			if(cmnd.size()!=2)
				cout<<"Enter correct command"<<endl;
			else{
				threads.push_back(thread(leave_group,cmnd[1]));
			}
		}
		else{
		printf("No user is logged in currently!!\n");
	}
	}
	else if(cmnd[0]=="upload_file"){
		if(islogged){
			if(cmnd.size()!=3)
				cout<<"Enter correct command"<<endl;
			else{
				threads.push_back(thread(upload_file,cmnd[1],cmnd[2]));
			}
		}
		else{
		printf("No user is logged in currently!!\n");
	}	
	}
	else if(cmnd[0]=="download_file"){
				if(islogged){
			if(cmnd.size()!=4)
				cout<<"Enter correct command"<<endl;
			else{
				threads.push_back(thread(download_file,cmnd[1],cmnd[2],cmnd[3]));
			}
		}
		else{
		printf("No user is logged in currently!!\n");
	}
	}
	else if(cmnd[0]=="list_files"){
				if(islogged){
			if(cmnd.size()!=2)
				cout<<"Enter correct command"<<endl;
			else{
				threads.push_back(thread(list_files,cmnd[1]));
			}
		}
		else{
		printf("No user is logged in currently!!\n");
	}
	}
	else if(cmnd[0]=="stop_share"){
				if(islogged){
			if(cmnd.size()!=3)
				cout<<"Enter correct command"<<endl;
			else{
				threads.push_back(thread(stop_share,cmnd[1],cmnd[2]));
			}
		}
		else{
		printf("No user is logged in currently!!\n");
	}
	}
	else if(cmnd[0]=="show_downloads"){
		cout<<"Not Implemented due to lack of time!!."<<"\n";
	}
	else{
		printf("Enter valid command\n");
	}
	/*send(sock , buf, strlen(buf), 0 );
	printf("sent to server %s\n",buf); 
	valread = read( sock , buffer, BUFFER_SZE); 
	printf("received from server %s\n",buffer );
	bzero(buffer,sizeof(buffer));*/

	}
	
	
	return 0; 
} 
