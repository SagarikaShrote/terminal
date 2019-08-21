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

int main(){

	vector<string> result;
	
	do{
		string command;
		result.clear();
		printDir();
		getline(cin,command);

		boost::split(result,command,boost::is_any_of(" ")); 
		if(result.size()==0||result[0]=="exit"){
			//do nothing
		}
		else if(result[0]=="cd"){
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
		
	}while(result[0]!="exit");
	cout<<"\n\tBYE!"<<endl;
	return 0;
}
