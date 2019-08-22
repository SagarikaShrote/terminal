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

//signal handlers ctr+c/z
void sigintHandler(int sig_num)
{
	
	signal(SIGINT, sigintHandler); 
}

void sighandler(int sig_num) 
{
	
    signal(SIGTSTP, sighandler); 
} 

//printing username and current working directory
void printDir() 
{ 
	char cwd[1024]; 
	getcwd(cwd, sizeof(cwd)); 
	char* username = getenv("USER");
	printf("\n%s @ %s $",username, cwd); 
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
	return res;

}

int main(){

	signal(SIGTSTP, sighandler); 
	signal(SIGINT, sigintHandler); 
	
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
			        printf("\nFailed forking child.."); 
			        
			    } else if (pid == 0) { 
			    	//signal(SIGTSTP,  SIG_DFL); 
					//signal(SIGINT,  SIG_DFL); 
			        if (execvp(carr[0], carr) < 0) { 
			            printf("\nCould not execute command.."); 
			        } 
			        exit(0); 
			    } else { 
			        // waiting for child to terminate 
			        wait(NULL);  
			        int i=0;
			        for(i=0;i<n;i++){
			        	free(carr[i]);
			        }
			        //free(carr);
			    } 
			}
		}
		else if(type==1){
			boost::trim_if(command, boost::is_any_of("&&"));
			boost::split(result,command,boost::is_any_of("&&"),boost::token_compress_on); 
			if(result.size()==1){
				
			}
			else{
			
			}

		}
		else{
			boost::trim_if(command, boost::is_any_of("&&&"));
			boost::split(result,command,boost::is_any_of("&&&"),boost::token_compress_on); 
			if(result.size()==1){
				
			}
			else{
				
				
			}
		}
		
	}while(command!="exit");
	cout<<"\n\tBYE!"<<endl;
	return 0;
}
