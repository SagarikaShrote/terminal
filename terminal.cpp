#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 
#include<fcntl.h>
#include <limits.h>
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp> 
#include<iostream>
using namespace std;


//printing username and current working directory
void printDir() 
{ 
	char cwd[1024]; 
	getcwd(cwd, sizeof(cwd)); 
	char* username = getenv("USER");
	printf("\n%s @ %s $",username, cwd); 
} 
int checkredir(string command){
	if(command.find(">")!=-1){
		return -1;
	}
	else if(command.find("<")!=-1){
		return -2;
	}
	else return 0;
}

int check(string command){
	int c,res=0;
	c=command.find("&&&");
	if(c==-1){
		c=command.find("&&");
		if(c!=-1){
			res=1;
		}
	}
	else{
		res=2;
	}
	if (res==0){
		res=checkredir(command);
	}
	return res;

}
//to handle standalone commands
void singlec(string command, vector<string> & result){
	boost::split(result,command,boost::is_any_of(" ")); 
			if(result[0]=="cd"){					//for cd only
				if(result.size()==1){
					cout<<"->";
					cin>>command;
					result.push_back(command);
				}
				int n = result[1].length();
				char dir[n + 1]; 
				strcpy(dir, result[1].c_str()); 
				int c=chdir(dir);
				if (c!= 0)  
	    		perror("cd failed");
	    		else
	    		cout<<"\tDirectory changed!"; 
			}
			else{
				int pid = fork();  
			  	
			  	//making a char array
			  	int n = result.size();
			  	char *carr[n+1];
			  	int i=0;
			  	for(i=0;i<n;i++){
			  		carr[i]=(char *)malloc(sizeof(char)*(result[i].length()+1));
			  		strcpy(carr[i], result[i].c_str()); 
			  	}
			  	carr[n]=NULL;
			    if (pid == -1) { 
			        printf("Process Creation failed\n");
			        
			    } else if (pid == 0) { 
			    	signal(SIGTSTP,SIG_IGN); 
					signal(SIGINT,SIG_DFL); 
			        if (execvp(carr[0], carr) < 0) { 
			            printf("\nShell: Incorrect command."); 
			        } 
			        exit(0); 
			    } else { 
			        // waiting for child to terminate 
			        //wait(NULL);  
			        int i=0;
			        for(i=0;i<n;i++){
			        	free(carr[i]);
			        }
			        //free(carr);
			    } 
			}
}
//it is assumed that at a time only one redirection is allowed and in which one side there will be command and other side will be file. It does not work on two files.
//input redirection 
void execipredir(string command,vector<string>& result){
	boost::trim_if(command, boost::is_any_of("<"));
	boost::split(result,command,boost::is_any_of("<"),boost::token_compress_on);
	int rc = fork();
	if(rc==0){
		signal(SIGTSTP,SIG_IGN); 
		signal(SIGINT,SIG_DFL); 
		int n = result[1].size();
		char carr[n+1];
		strcpy(carr, result[1].c_str());
		//file like f0<f1
		int f1; 	
		//for input redirection f1 must be a file so it must open in read only permissions as it must not be changed
		if((f1 = open(carr, O_RDONLY) )< 0){
			perror("can't open file:");
			exit(0);
		}
		dup2(f1, 0);
		close(f1);
		vector<string> v;
		singlec(result[0],v);
		wait(NULL);
		exit(0);
	}
	else if(rc<0){
		printf("Process Creation failed\n");
	}
}
//output redirection
void execopredir(string command,vector<string>& result){
	boost::trim_if(command, boost::is_any_of(">"));
	boost::split(result,command,boost::is_any_of(">"),boost::token_compress_on);
	int rc = fork();
	if(rc==0){
		signal(SIGTSTP,SIG_IGN); 
		signal(SIGINT,SIG_DFL); 
		int n = result[1].size();
		char carr[n+1];
		strcpy(carr, result[1].c_str());
		//file like f0>f1
		int f1; 	
		//for output redirection also f1 must be a file so it must open in write or append permissions as it must be changed

		if((f1 = open(carr, O_CREAT|O_WRONLY|O_APPEND, S_IRWXU)) < 0){
			perror("can't open file:");   
			exit(0);                 
		}
		dup2(f1,1);
		close(f1);
		vector<string> v;
		singlec(result[0],v);
		wait(NULL);
		exit(0);
	}
	else if(rc<0){
		printf("Process Creation failed\n");
	}
}

int main(){

	signal(SIGTSTP, SIG_IGN); 
	signal(SIGINT, SIG_IGN); 
	
	cout<<"\t WELCOME";

	vector<string> result;
	string command;
	do{
		
		result.clear();
		printDir();
		getline(cin,command);
		int type=check(command);
		
		if(command=="exit" || command==" " || command==""){
			//do nothing
		}
		else if(type==0){
			//call single command fn
			singlec(command,result);
			wait(NULL); 
		}
		//will not give space seperated commands for && and &&& this is the assumption
		else if(type==1){
			boost::trim_if(command, boost::is_any_of("&&"));
			boost::split(result,command,boost::is_any_of("&&"),boost::token_compress_on); 
			//cout<<"checkpt1 &&"<<endl;
			vector<string>temp;
			for(int i=0;i<result.size();i++){
				temp.clear();
				type=check(result[i]);
				switch(type){
					case 0:	singlec(result[i],temp);
						break;
					//case 2: it is assumed that all tasks can be in either parallel or serial there will not be miscleneous tasks
					case -1:execopredir(result[i],temp);
						break;
					case -2:execipredir(result[i],temp);
						break;
				}
				wait(NULL);
			}

		}
		else if(type==2){
			boost::trim_if(command, boost::is_any_of("&&&"));
			boost::split(result,command,boost::is_any_of("&&&"),boost::token_compress_on); 
			//cout<<"checkpt2 &&&"<<endl;
			vector<string>temp;
			for(int i=0;i<result.size();i++){
				temp.clear();
				type=check(result[i]);
				switch(type){
					case 0:	singlec(result[i],temp);
						break;
					//case 1: it is assumed that all tasks can be in either parallel or serial there will not be miscleneous tasks
					case -1:execopredir(result[i],temp);
						break;
					case -2:execipredir(result[i],temp);
						break;
				}
				
			}
			for(int i=0;i<result.size();i++){
				wait(NULL);
			}
			
		}
		//it is assumed that functionality like pipe etc is not used
		else if(type==-1){
			execopredir(command,result);
			wait(NULL);
		}
		else if(type==-2){
			execipredir(command,result);
			wait(NULL);
		}
		
	}while(command!="exit");
	cout<<"\n\tBYE!"<<endl;
	return 0;
}
