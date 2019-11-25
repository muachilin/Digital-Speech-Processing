#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>


using namespace std;


int main(int argc , char*argv[])
{
	
	map<string,vector<string> >ZhuYin_Word_map;
	
	string input_map = argv[1];
	string output_map = argv[2];
	
	
	ifstream input_file;
	ofstream output_file;

	output_file.open(output_map.c_str());
	input_file.open(input_map.c_str());
	string buffer;
	
	int sum_line = 0;
	int buffer_size = 0;	
	int sum_repeat = 0;
	while(getline(input_file,buffer))
	{
		
		string character = buffer.substr(0,2);	
		buffer.erase(0,3);
		string first_head = buffer.substr(0,2);
		
		ZhuYin_Word_map[first_head].push_back(character);
		
		int a = 0;
		int tmp = buffer.find('/');
		map<string,bool>poly_map;
		
		if(tmp > 0)
		{
			a=1;
			poly_map[first_head] = true;
		}
		bool poly_repeat = true;
		while(tmp > 0)
		{
			buffer.erase(buffer.begin(),buffer.begin()+tmp+1);
                        first_head = buffer.substr(0,2);
			if(!poly_map.count(first_head))
			{
				poly_repeat = false;
				
				ZhuYin_Word_map[first_head].push_back(character);
				poly_map[first_head] = true;
			}
			
			tmp = buffer.find('/');
		}
			
		sum_line ++;
	}

	for(auto&i:ZhuYin_Word_map)
	{
		output_file<<i.first<<" ";
		for(auto&j:i.second)
		{
			output_file<<j<<" ";
		}
		output_file<<endl;
		for(auto&j:i.second)
                {
                        output_file<<j<<" "<<j<<endl;
                }
	}
	



}
