#include "myheader.h"
#include<thread>
#include<iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include<bits/stdc++.h>
#define PORT 8080 
#define BUFFER_SZE 20240
using namespace std;
vector<thread> processVector;
string serv_ip = "127.0.0.1";
string serv_port = "8080";
map<string,string> reg_user;
map<string,set<string>> grpIdAllMem;
map<string,set<string>> usrgrp;
set<string> usrlogedin;
map<string,string> grpIdOwn;
map<string,set<pair<string,string>>> ownpend;
map<string,multiset<string>> gidfile;
map<pair<string,string>,set<string>> cidgidfid;
map<string,string> fnamefpath;
map<string,int> fidfisze;
map<string,string> cidport;
map<pair<string,string>,set<string>> gidfidcid;
//map<string,map<string,set<string>>> grpIdFileUser;
int size;
struct client{

	string c_id;
	string c_pass;
	string c_port;

};
unordered_map<string,client> c_info;

FILE* openFile(char* fileName, char* type){
	FILE* fp = fopen(fileName, type);
	if (fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}
	return fp;
}

vector<string> parsestr(string st, char dmn){

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

void login(int client_sock,string u_id, string u_pass,string ports){
	auto itr = reg_user.find(u_id);
	if(itr==reg_user.end()){
		string msg = "No such user exist.;0";
		send(client_sock,msg.c_str(),sizeof(msg),0);
	}
	else{
		if(usrlogedin.find(u_id)==usrlogedin.end()){
			if(itr->second != u_pass){
			string msg = "Password Incorrect.;0";
			send(client_sock,msg.c_str(),sizeof(msg),0);			
			}
			else{
			usrlogedin.insert(u_id);
			cidport[u_id] = ports;
			string msg = "Successfully Logged In.;1";
			send(client_sock,msg.c_str(),sizeof(msg),0);
			}
		}
		else{
		string msg = "Already Logged In!!;0";
		send(client_sock,msg.c_str(),sizeof(msg),0);			
		}
	}


	close(client_sock);

}
void create_user(int client_sock,string u_id, string u_pass){
		//cout<<"ok";

		if(reg_user.find(u_id)==reg_user.end()){
			reg_user[u_id]=u_pass;
			string msg = "Registered Successfully";
			send(client_sock,msg.c_str(),sizeof(msg),0);
		}
		else{
			string msg = "User with same id already exist.";
			send(client_sock,msg.c_str(),sizeof(msg),0);
		}

	close(client_sock);
}

void create_group(int client_sock, string g_id, string u_id){

	auto itr = grpIdAllMem.find(g_id);
	if(itr==grpIdAllMem.end()){
		grpIdAllMem[g_id].insert(u_id);
		grpIdOwn[g_id]=u_id;
		usrgrp[u_id].insert(g_id);
		string msg = "Group Successfully Created.;1";
		send(client_sock,msg.c_str(),sizeof(msg),0);
	}
	else{
		string msg = "Group id already exist.;0";
		send(client_sock,msg.c_str(),sizeof(msg),0);	
	}

	close(client_sock);

}

void join_group(int client_sock, string g_id, string u_id){

	auto itr = grpIdAllMem.find(g_id);
	if(itr!=grpIdAllMem.end()){
		auto tt = grpIdAllMem[g_id].find(u_id);
		if(tt==grpIdAllMem[g_id].end()){
		//grpIdAllMem[g_id].insert(u_id);
		//usrgrp[u_id].insert(g_id);
		auto giko = grpIdOwn.find(g_id);
		if(ownpend[giko->second].find({u_id,g_id}) == ownpend[giko->second].end()){
		ownpend[giko->second].insert({u_id,g_id});
		string msg = "Request to join sent.;0";
		send(client_sock,msg.c_str(),sizeof(msg),0);
		}
		else{
		string msg = "Request Already Sent Earlier.;0";
		send(client_sock,msg.c_str(),sizeof(msg),0);
		}
	}
	else{
		string msg = "Already a Member.;0";
		send(client_sock,msg.c_str(),sizeof(msg),0);
	}
	}
	else{
		string msg = "No such group id exist.;0";
		send(client_sock,msg.c_str(),sizeof(msg),0);	
	}

	close(client_sock);

}

void logout(int client_sock, string u_id){

	usrlogedin.erase(u_id);
	cidport.erase(u_id);
	string msg = "User Logged Out!";
	send(client_sock,msg.c_str(),sizeof(msg),0);
	close(client_sock);
}

void list_requests(int client_sock, string g_id, string u_id){
	/*auto itr = grpIdOwn.find(g_id);
	string owner = itr->second;
	cout<<itr->first<<itr->second<<"ok";*/
	if(grpIdOwn[g_id]==u_id){

		auto tt = ownpend.find(u_id);
		if(tt!=ownpend.end()){
			string msg="";
			for(auto tr=ownpend[u_id].begin(); tr!=ownpend[u_id].end(); ++tr){
				if(tr->second == g_id)
					msg = msg + tr->first+" ";
			}
			if(msg==""){
				string msg = "No Requests Pending!";
				send(client_sock,msg.c_str(),sizeof(msg),0);
			}
			else{
			send(client_sock,msg.c_str(),sizeof(msg),0);
		}
		}
		else{
			string msg = "No Requests Pending!";
			send(client_sock,msg.c_str(),sizeof(msg),0);
		}


	}
	else{
	string msg = "You are not an owner of group!";
	send(client_sock,msg.c_str(),sizeof(msg),0);
	}

	close(client_sock);
}

void accept_request(int client_sock, string g_id, string u_id, string curu_id){
	/*auto itr = grpIdOwn.find(g_id);
	string owner = itr->second;
	cout<<itr->first<<itr->second<<"ok";*/
	/*cout<<curu_id<<g_id<<itr->second<<u_id<<itr->second<<itr->second;*/
	if(grpIdOwn[g_id]==curu_id){
		auto itr = ownpend[curu_id].find({u_id,g_id});
		if(itr!=ownpend[curu_id].end()){
			grpIdAllMem[g_id].insert(u_id);
			usrgrp[u_id].insert(g_id);
			ownpend[curu_id].erase({u_id,g_id});
			string msg = "Request Accepted Successfully!";
			send(client_sock,msg.c_str(),sizeof(msg),0);
		}
		else{
			string msg = "No pending Request!";
			send(client_sock,msg.c_str(),sizeof(msg),0);
		}
	}
	else{
		string msg = "You are not an owner of group!";
		send(client_sock,msg.c_str(),sizeof(msg),0);
	}

	close(client_sock);
}

void list_group(int client_sock){

	string msg="";
	for(auto itr=grpIdOwn.begin(); itr!=grpIdOwn.end(); ++itr)
		msg= msg + itr->first+" ";

	send(client_sock,msg.c_str(),sizeof(msg),0);
	close(client_sock);
	
}

void leave_group(int client_sock, string g_id, string u_id){

		auto itr = grpIdAllMem[g_id].find(u_id);
		if(itr!=grpIdAllMem[g_id].end()){
			grpIdAllMem[g_id].erase(u_id);
			usrgrp[u_id].erase(g_id);
			if(grpIdOwn[g_id]==u_id){
				auto tt = grpIdAllMem.find(g_id);
				if(grpIdAllMem.end()==tt){
					grpIdOwn.erase(g_id);
				}
				else{
					grpIdOwn[g_id]=*itr;
				}
			}
			//gidfile[g_id].erase();


		}
		else{
			string msg = "You are not a member of group!";
			send(client_sock,msg.c_str(),sizeof(msg),0);
		}
	close(client_sock);
}

void upload_file(int client_sock,string g_id, string f_id, string u_id){

			auto itr = grpIdAllMem[g_id].find(u_id);
			if(itr!=grpIdAllMem[g_id].end()){
				//cout<<"ok"<<f_id;
				gidfile[g_id].insert(f_id);
				cidgidfid[{u_id,g_id}].insert(f_id);
				gidfidcid[{g_id,f_id}].insert(u_id);
				string f_name = getfilename(f_id);
				//cout<<"ok"<<f_name<<"ok";
				fnamefpath[f_name]=f_id;
				//FILE *out = openFile(f_id, "r");
				fidfisze[f_id]=getfilesize(f_id.c_str());
				cout<<fidfisze[f_id];
				//for(auto fitr=fnamefpath.begin(); fitr!=fnamefpath.end(); fitr++)
						//cout<<fitr->first<<"ok"<<fitr->second<<"\n";
				string msg = "File Uploaded in group "+g_id;
				send(client_sock,msg.c_str(),sizeof(msg),0);

			}
			else{
				string msg = "You are not a member of group!";
				send(client_sock,msg.c_str(),sizeof(msg),0);
			}
	close(client_sock);
}

void dwnld_file(int client_sock, string g_id, string f_name, string u_id){

				auto itr = grpIdAllMem[g_id].find(u_id);
				//cout<<g_id<<"ok"<<u_id<<"ok"<<g_id<<"ok"<<u_id;
				/*for(auto itr = grpIdAllMem[g_id].begin(); itr!=grpIdAllMem[g_id].end(); ++itr)
					cout<<*itr<<"ok";*/
				if(itr!=grpIdAllMem[g_id].end()){
					string f_id;
					/*for(auto fitr=fnamefpath.begin(); fitr!=fnamefpath.end(); fitr++)
						cout<<fitr->first<<"ok"<<fitr->second<<"\n";*/
					auto fitr = fnamefpath.find(f_name);
					if(fitr==fnamefpath.end()){
						string msg = "No such file shared!";
						send(client_sock,msg.c_str(),sizeof(msg),0);
						return;
					}
					else{
						cout<<"filefound";
						f_id = fitr->second;
					}
					//cout<<f_id;
					auto tt = gidfile[g_id].find(f_id);
					if(tt != gidfile[g_id].end()){
						set<string> curusers = gidfidcid[{g_id,f_id}];
						string fsize = to_string(fidfisze[f_id]);
						string msg=fsize+";";
						bool flag=0;
						for(auto itt = curusers.begin(); itt!=curusers.end(); itt++){
							if(usrlogedin.find(*itt)!=usrlogedin.end()){
								flag=1;
								msg+=cidport[*itt]+";";
							}
						}
						if(flag){
							string st = msg.substr(0,msg.size()-1);
							send(client_sock,st.c_str(),sizeof(st),0);
						}
						else{
							string msg = "No user logged in having file!";
							send(client_sock,msg.c_str(),sizeof(msg),0);
						}

					}
					else{
						string msg = "No such file in this group!";
						send(client_sock,msg.c_str(),sizeof(msg),0);
					}						
						}
						else{
							string msg = "You are not a member of group!";
							send(client_sock,msg.c_str(),sizeof(msg),0);
						}
	close(client_sock);

}

void list_files(int client_sock, string g_id, string u_id){


		auto itr = grpIdAllMem[g_id].find(u_id);
		if(itr != grpIdAllMem[g_id].end()){
			string msg="";
			for(auto tt = gidfile[g_id].begin(); tt!=gidfile[g_id].end(); tt++){
				msg += getfilename(*tt);
				msg +="\n";
			}
			if(msg==""){
				string msg = "No file shared in group!";
				send(client_sock,msg.c_str(),sizeof(msg),0);
			}
			else{
					char *data_ptr = &msg[0];
					size_t data_size = msg.size();
					int bytes_sent;
					while(data_size>0){
						bytes_sent = send(client_sock,data_ptr,data_size,0);
						if(bytes_sent <=0)
							return;
						data_size-=bytes_sent;
						data_ptr+=bytes_sent;
				}
				
			}

		}
		else{
			string msg = "User does not belongs to group!";
			send(client_sock,msg.c_str(),sizeof(msg),0);
		}
		close(client_sock);

}

void stop_share(int client_sock, string g_id, string f_name, string u_id){

		auto itr = grpIdAllMem[g_id].find(u_id);
			if(itr!=grpIdAllMem[g_id].end()){
				//cout<<"ok"<<f_id;
				auto tt = fnamefpath.find(f_name);
				if(tt != fnamefpath.end()){
				string f_id = fnamefpath[f_name];
				auto ftr = cidgidfid.find({u_id,g_id});
				if(ftr != cidgidfid.end()){
				auto ftt = cidgidfid[{u_id,g_id}].find(f_id);
				if(ftt != cidgidfid[{u_id,g_id}].end()){
				gidfile[g_id].erase(f_id);
				cidgidfid[{u_id,g_id}].erase(f_id);
				gidfidcid[{g_id,f_id}].erase(u_id);
				string f_name = getfilename(f_id);
				//cout<<"ok"<<f_name<<"ok";
				fnamefpath[f_name]=f_id;
				//FILE *out = openFile(f_id, "r");
				//fidfisze[f_id]=getfilesize(f_id.c_str());
				//cout<<fidfisze[f_id];
				//for(auto fitr=fnamefpath.begin(); fitr!=fnamefpath.end(); fitr++)
						//cout<<fitr->first<<"ok"<<fitr->second<<"\n";
				string msg = "File stopped shared by user!"+g_id;
				send(client_sock,msg.c_str(),sizeof(msg),0);
			}
			else{
				string msg = "U have'nt shared no file!";
				send(client_sock,msg.c_str(),sizeof(msg),0);
			}
			}
			else{
				string msg = "U have'nt shared no file!";
				send(client_sock,msg.c_str(),sizeof(msg),0);
			}
			}
			else{
				string msg = "No such file shared!";
				send(client_sock,msg.c_str(),sizeof(msg),0);
			}
		}
			else{
				string msg = "You are not a member of group!";
				send(client_sock,msg.c_str(),sizeof(msg),0);
			}
	close(client_sock);


}


void clientRequest(int client_sock,string ip, int port){

	char buffer[BUFFER_SZE];
	bzero(buffer,BUFFER_SZE);
	int cli_msg = read(client_sock , buffer, BUFFER_SZE);

	string curmsg = buffer;
	cout<<curmsg;
	bzero(buffer,sizeof(buffer));
	vector<string> rmsg=parsestr(curmsg,';');
	//printf("%s",rmsg[0].c_str());   //rmsg[0];
	if(rmsg[0]=="create_user"){

		string u_id = rmsg[1];
		string u_pass = rmsg[2];
		create_user(client_sock,u_id,u_pass);

	}

	else if(rmsg[0]=="login"){

		string u_id = rmsg[1];
		string u_pass = rmsg[2];
		string ports = rmsg[3];
		login(client_sock,u_id,u_pass,ports);
	}
	else if(rmsg[0]=="create_group"){

		string g_id = rmsg[1];
		string u_id = rmsg[2];
		create_group(client_sock,g_id,u_id);

	}
	else if(rmsg[0]=="join_group"){

		string g_id = rmsg[1];
		string u_id = rmsg[2];
		join_group(client_sock, g_id, u_id);
	}
	else if(rmsg[0]=="logout"){

		string u_id = rmsg[1];
		logout(client_sock,u_id);
	}
	else if(rmsg[0]=="list_requests"){
		string g_id = rmsg[1];
		string u_id = rmsg[2];
		//string currown = grpIdOwn[g_id];
		//cout<<curmsg<<"ok"<<currown;
		list_requests(client_sock,g_id,u_id);
		}
	else if(rmsg[0]=="accept_request"){

		string g_id = rmsg[1];
		string u_id = rmsg[2];
		string curu_id = rmsg[3];
		//cout<<curmsg<<"ok"<<currown;
		accept_request(client_sock,g_id,u_id,curu_id);
	}
	else if(rmsg[0]=="list_group"){
		list_group(client_sock);
	}
	else if(rmsg[0]=="leave_group"){

		string g_id = rmsg[1];
		string u_id = rmsg[2];
		leave_group(client_sock,g_id,u_id);
	}
	else if(rmsg[0]=="upload_file"){
		string g_id = rmsg[1];
		string f_id = rmsg[2];
		string u_id = rmsg[3];
		upload_file(client_sock,g_id,f_id,u_id);
	}
	else if(rmsg[0]=="dwnld_file"){
		string g_id = rmsg[1];
		string f_name = rmsg[2];
		string u_id = rmsg[3];
		dwnld_file(client_sock,g_id,f_name,u_id);
	}
	else if(rmsg[0]=="get_file_id"){
		string f_name = rmsg[1];
		string msg = fnamefpath[f_name];
		cout<<"inserve"<<msg;
		char *data_ptr = &msg[0];
		size_t data_size = msg.size();
		int bytes_sent;
		while(data_size>0){
			bytes_sent = send(client_sock,data_ptr,data_size,0);
			if(bytes_sent <=0)
				return;
			data_size-=bytes_sent;
			data_ptr+=bytes_sent;
	}
		close(client_sock);
	}
	else if(rmsg[0]=="list_files"){
		string g_id= rmsg[1];
		string u_id= rmsg[2];
		list_files(client_sock,g_id,u_id);

	}
	printf("\n");

	/*while(1){
	int cli_msg;
	char buffer[BUFFER_SZE]={0};
	cli_msg = read(client_sock , buffer, 1024);
	printf("Message received from clients %s\n",buffer );
	
	char nw[BUFFER_SZE];
	fgets(nw,BUFFER_SZE, stdin);
	send(client_sock ,nw , strlen(nw) , 0 ); 
}*/
}
int main(int argc, char const *argv[]){

	if(argc!=3){
		cout<<"enter correct command line arguments.";
		perror("Error in command line argument list ");
		return -1;
	} 
	int main_sock, client_sock; 
	struct sockaddr_in address; 
	struct sockaddr_in client_addr;
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[BUFFER_SZE] = {0}; 	

	if ((main_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(main_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(main_sock, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(main_sock, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	while(1){ 
		if((client_sock = accept(main_sock, (struct sockaddr *)&address, (socklen_t*)&addrlen))!=-1){
		string ip=inet_ntoa(client_addr.sin_addr);
  		int port=(ntohs(client_addr.sin_port));
  		cout<<"ip="<<ip<<" port :"<<port<<" ";
  		processVector.push_back(thread(clientRequest,client_sock,ip,port));
  		//size=sizeof(struct sockaddr);
  	}
  	else{
		perror("accept"); 
		exit(EXIT_FAILURE); 
  	}
	} 


	for(auto itr=processVector.begin();itr!=processVector.end();itr++)
	{
		if(itr->joinable()) 
			itr->join();
	}
	/*send(client_sock , hello , strlen(hello) , 0 ); 
	printf("Hello message sent\n"); */
	return 0; 
} 
