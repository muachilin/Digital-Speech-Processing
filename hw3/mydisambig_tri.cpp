#include <limits.h>
#include <float.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <stdio.h>

#include "Ngram.h"
using namespace std;

Vocab voc;
Ngram lm( voc, 3);


// Get P(W2 | W1) -- bigram
double getBigramProb(const char *w1, const char *w2)
{
	VocabIndex wid1 = voc.getIndex(w1);
	VocabIndex wid2 = voc.getIndex(w2);

	if(wid1 == Vocab_None)  //OOV
        {
		wid1 = voc.getIndex(Vocab_Unknown);
	}
    	if(wid2 == Vocab_None)  //OOV
        {
		wid2 = voc.getIndex(Vocab_Unknown);
	}
	
	VocabIndex context[] = { wid1, Vocab_None };
	return lm.wordProb( wid2, context);
}

// Get P(W3 | W1, W2) -- trigram
double getTrigramProb(const char *w1, const char *w2, const char *w3) 
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);
    VocabIndex wid3 = voc.getIndex(w3);

    if(wid1 == Vocab_None)  //OOV
        {wid1 = voc.getIndex(Vocab_Unknown);}
    if(wid2 == Vocab_None)  //OOV
        {wid2 = voc.getIndex(Vocab_Unknown);}
    if(wid3 == Vocab_None)  //OOV
        {wid3 = voc.getIndex(Vocab_Unknown);}

    VocabIndex context[] = { wid2, wid1, Vocab_None };
    return lm.wordProb( wid3, context );
}

double getOnegramProb(const char *w)
{

	
	VocabIndex wid = voc.getIndex(w);
	VocabIndex context[] = { Vocab_None };
	double one_gram;
	if(wid != Vocab_None)
	{
	 	one_gram = lm.wordProb( wid,context);
		return one_gram;
	}
	else{ return -10;}
	
}

int main(int argc , char*argv[])
{
	// construct the map

	double top_sampling_rate = 1;

                string lm_filename = argv[3];
                File lmFile( lm_filename.c_str(), "r" );
                lm.read(lmFile);
                lmFile.close();
	
	

	map<string,vector<string> > ZhuYin_map;	
	map<string,vector<string> >word_map;
	
	string map_filename = argv[2];
	ifstream map_file;
	map_file.open(map_filename.c_str());
	string mbuffer;
	while(getline(map_file,mbuffer))
	{
		string character = mbuffer.substr(0,2);
		mbuffer.erase(0,3);
		
		stringstream ss;
		string tmp="";
		ss << mbuffer;
                mbuffer = "";
		ss >> tmp;
		
		bool only_ZhuYin = false;
		
		if(!tmp.empty()&&ss.eof())
		{
		
			mbuffer = mbuffer+tmp;
		}	
                while(!ss.eof())
                {
			only_ZhuYin = true;
                        mbuffer = mbuffer+tmp;
                	ss >> tmp;
		}
		
		//map<double,string>top_can_word;
		
		vector<string>word_list;
		
		for(int i=0;i<mbuffer.size()/2;++i)
		{
			string sub_str = mbuffer.substr(2*i,2);
			word_map[character].push_back(sub_str);
					
			if(only_ZhuYin == true)
			{
				VocabIndex wid = voc.getIndex(sub_str.c_str());
                        	double wid_prob \
				= getOnegramProb(sub_str.c_str());
				
				if(wid!=Vocab_None && wid_prob>-6)
                       	 	{
					word_list.push_back(sub_str);
					
				}
					
			}
		}
		
		if(only_ZhuYin == true)
		{
			for(auto&i:word_list)
			{
				ZhuYin_map[character].push_back(i);
			}
		}
	}
	
	map_file.close();	


	string input_filename = argv[1];
	ifstream input_file;
	input_file.open(input_filename.c_str()); 

		
	string buffer;
	int sum = 0;

	ofstream output_file_txt;
        output_file_txt.open(argv[4]);
	
	
	while(getline(input_file,buffer))
	{
		string tmp;
		stringstream ss;
		ss << buffer;
		
		
		buffer = "";
		ss >> tmp;
		while(!ss.eof())
		{
			buffer = buffer + tmp;
			ss >> tmp;
		}
		
		
		int half_buffer_size = buffer.size()/2;
		vector<string> word_string;
		bool no_ZhuYin = true;
		
		
		for(int i=0;i<half_buffer_size;++i)
		{
			string sub_str = buffer.substr(2*i,2);
			if(ZhuYin_map.count(sub_str))
			{
				no_ZhuYin = false;
			}
			word_string.push_back(sub_str);
		}
	
		word_string.push_back("</s>");
			
		if(no_ZhuYin==false)
		{
		
				
		vector<vector<double> > delta;
		vector<vector<string> > tree;
		vector<vector<int>> backtrack; // size should be tree.size()-1
		
		for(int i=0;i<word_string.size();++i)
		{
			if(!ZhuYin_map.count(word_string[i]))
			{
				
				vector<double> delta_arr;
				vector<string> tree_arr;
				vector<int> backtrack_arr;
			
				if(i==0)
				{
					double bi_gram = getBigramProb\
					("<s>",\
					word_string[i].c_str());
					
					delta_arr.push_back(bi_gram);
					tree_arr.push_back(word_string[i]);
					
				}
				else if(i==1)
				{
					double delta_max = DBL_MAX*(-1);
                                        int backtrack_index = 0;
                                        int last_pos = 0;
                                        for(int j=0;\
                                        j<tree[last_pos].size();++j)
                                        {
                                           double score = getTrigramProb\
                                           ("<s>",\
                                           tree[last_pos][j].c_str()
                                           ,word_string[i].c_str()\
                                           );
                                           double delta_can;
                                           delta_can = score+delta[last_pos][j];
                                                
                                           if(delta_can > delta_max)
                                           {
                                                delta_max = delta_can;
                                                backtrack_index = j;
                                            }
                                        }
					
					
					delta_arr.push_back(delta_max);
                                        tree_arr.push_back(word_string[i]);
                                        backtrack_arr.push_back(backtrack_index);
                                        backtrack.push_back(backtrack_arr);		
				}		
				else{
 					double delta_max = DBL_MAX*(-1);
					int backtrack_index = 0;
					int last_pos = tree.size()-1;
                                        for(int j=0;\
                                        j<tree[last_pos].size();++j)
					{
					   for(int k=0;\
					   k<tree[last_pos-1].size();++k)
					   {
						
					   double score = getTrigramProb\
                                           (tree[last_pos-1][k].c_str(),
                                           tree[last_pos][j].c_str()
					   ,word_string[i].c_str()\
					   );
                                           double delta_can;
                                           delta_can = score+delta[last_pos][j];
					   
					   //if(i==word_string.size()-1)
					  // {
					//	delta_can +=  (getBigramProb\
                                        	(word_string[i].c_str(),"</s>")\
					//	+getTrigramProb\
						( tree[last_pos][j].c_str(),\
						word_string[i].c_str(),\
						"</s>")
					//	);
						
							
					  // }
   					  	
 					   if(delta_can > delta_max)
					   {
						delta_max = delta_can;
						backtrack_index = j;
					    }
					
					   }//k  for "last_pos-1"
						
				        }// for j
					
					delta_arr.push_back(delta_max);
					tree_arr.push_back(word_string[i]);
					backtrack_arr.push_back(backtrack_index);
					backtrack.push_back(backtrack_arr);
				}
					
				delta.push_back(delta_arr);
				tree.push_back(tree_arr);
				
			}
			
		
			else	
			{
				vector<double> delta_arr;
				vector<string> tree_arr;
				vector<int> backtrack_arr;
				if(i==0)
				{
				 
				  for(int j=0;j<ZhuYin_map[word_string[i]].size();++j)
				  { 
					string candidate = ZhuYin_map[word_string[i]][j];
                        		double bi_gram = 0.0;
                              		bi_gram = \
					getBigramProb("<s>"\
					,candidate.c_str());
					
					delta_arr.push_back(bi_gram);	
					tree_arr.push_back(candidate);
				   }
				
				   delta.push_back(delta_arr);
				   tree.push_back(tree_arr);
				   
				}
				else if(i==1)
				{
				
				  for(int j=0;j<ZhuYin_map[word_string[i]].size();++j)
                                    {
                                        double delta_max = DBL_MAX*(-1);
                                        int backtrack_index = 0;

                                        string candidate = \
                                        ZhuYin_map[word_string[i]][j];
                                        int last_pos = 0;

                                        for(int k=0;\
                                        k<tree[last_pos].size();++k)
                                        {
                                           double score = getTrigramProb\
                                           ("<s>",\
                                           tree[last_pos][k].c_str(),\
                                                candidate.c_str());

                                           double delta_can;
                                           delta_can = score+delta[last_pos][k];

                                           if(delta_can > delta_max)
                                           {
                                                delta_max = delta_can;
                                                backtrack_index = k;
                                           }

                                        }
                                        delta_arr.push_back(delta_max);
                                        tree_arr.push_back(candidate);
                                        backtrack_arr.push_back(backtrack_index);
                                    }

                                    delta.push_back(delta_arr);
                                    tree.push_back(tree_arr);
                                    backtrack.push_back(backtrack_arr);
                                }
					
				
				else{


				    for(int j=0;j<ZhuYin_map[word_string[i]].size();++j)
				    {
					double delta_max = DBL_MAX*(-1);
					int backtrack_index = 0;
						
					string candidate = \
					ZhuYin_map[word_string[i]][j];
					int last_pos = tree.size()-1;
					for(int k=0;\
					k<tree[last_pos].size();++k)
					{
					   for(int g=0;\
					   g<tree[last_pos-1].size();++g)
				  	   {
					
					   double score = getTrigramProb\
					   (tree[last_pos-1][g].c_str(),\
					   tree[last_pos][k].c_str(),\
						candidate.c_str());
					
					   double delta_can;
					   delta_can = score+delta[last_pos][k];
					    
					  //if(i==word_string.size()-1)
                                           //{
                                             // delta_can +=  (getBigramProb\
                                              (word_string[i].c_str(),"</s>")\
						//+getTrigramProb(\
					      tree[last_pos][k].c_str(),\
					      candidate.c_str(),"</s>"\
						)
					//	);
                                          // }
					  
					   if(delta_can > delta_max)
					   {
						delta_max = delta_can;
						backtrack_index = k;
				   	   }
					  }// g
					}//k
					delta_arr.push_back(delta_max);
					tree_arr.push_back(candidate);
					backtrack_arr.push_back(backtrack_index);
			     	    }	
				    
   				    delta.push_back(delta_arr);
     				    tree.push_back(tree_arr);	
				    backtrack.push_back(backtrack_arr);
				}
				
				
			}//if ZhuYin
			
		}
	
		/**   start to backtrack result  **/
		
		int last_big_index;
		double delta_last_max = DBL_MAX*(-1);
			
		for(int i=0;i<delta[delta.size()-1].size();++i)
		{
			if(delta[delta.size()-1][i]> delta_last_max)
			{
				delta_last_max = delta[delta.size()-1][i]; 
				last_big_index = i;
			}
			
		}
		
		vector<int> backtrack_result;
		backtrack_result.push_back(last_big_index);
		
		for(int i=backtrack.size()-1;i>=0;--i)
		{
			int last_index\
			 = backtrack[i]\
			 [backtrack_result[backtrack_result.size()-1]];	

			backtrack_result.push_back(last_index);
			
		}
		
		reverse(backtrack_result.begin(),\
		backtrack_result.end());
		
		
		output_file_txt << "<s>";	
		for(int i=0;i<backtrack_result.size();++i)
		{
			output_file_txt <<" " <<tree[i][backtrack_result[i]];
				
		}
		 output_file_txt << endl;
		
			
		} // if ZhuYin exists
		else
		{
			
			output_file_txt << "<s>" ;
			for(int i=0;i<word_string.size();++i)
			{
				output_file_txt <<" "<<word_string[i];
			}
			output_file_txt <<endl;	
			
			
		}
	}

	
}


