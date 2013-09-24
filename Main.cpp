/**
 * CSE532S Lab 0 FALL 2013
 * Authors --
 *		Amit Jha
 *		Di Yan
 *		Li Zeng
 */


#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <sstream>
#include <mutex> 


using namespace std;

/**
 *  Struct to hold the data.
 *	num -- line number.
 *  charname -- name of the character (Queen, King, etc).
 *  text -- actual text from the fragment file.
 */
struct struct_line{
	int num;
	string charname;
	string text;

};

mutex m;

class Play{

	private :
		string playname;
		vector<struct_line> inner_data;
		mutex mut;
		string curr_char ; //name of the current character whose text is being printed.
		
	public:
		Play(string playname_){
			playname = playname_;
		}
		/* Sorts and prints the output.
		 * lambda expression for compare is provided inline and compares the line number in the struct for ordering.
		 * before printing text, checks for current character's name with the one in text and if its not same, prints the new name with a '.'
		 * and new line.
		 */
		void print(){
			sort(inner_data.begin(), inner_data.end(), [](const struct_line &left, const struct_line &right ){ return left.num < right.num;});
			for(unsigned int i = 0 ; i <  inner_data.size(); i++){
				if(curr_char != inner_data[i].charname){
					curr_char = inner_data[i].charname;
					cout << endl << curr_char << "." <<  endl;
				}
				cout << inner_data[i].text << endl;
					
			}
		}

		/* pushes a new struct to the vector.
		 * locks the mutex before execution to make the access threadsafe
		 */
		Play& operator << (struct_line line){
			lock_guard<mutex> lk(mut);
			inner_data.push_back(line);
			return *this;
		}

};

class str_op{
public:
	template<typename T>
	T StringToNumber(const string &Text){
		stringstream ss(Text);
		T result;
		return ss >> result ? result : 0;
	}
	bool isblank(string &str){
		if(strlen(str.c_str())==0)
			return true;
		return false;
	}
	vector<string> split(string str, char c = ' '){
		vector<string> result;
		stringstream s(str);
		string token="";
		while(getline(s,token,c)){
			if(token.compare(" ")!=0)
				result.push_back(token);
		}

		return result;
	}	
};

class thread_guard
{
	thread& t;
public:
	explicit thread_guard(thread& t_):t(t_){}
	~thread_guard(){
		if(t.joinable()){
			t.join();
		}
	}
	thread_guard(thread_guard const &);
	thread_guard& operator=(thread_guard const&){}
};

/**
 *  function that's used to launch separate thread per valid line in configuration file.
 *  The actual validation is done in main itself.
 *  this function makes sure that each line in the fragment is well formed, if yes, prepares an instance of struct_line
 *  and pushes into play object. The play object itself hold mutex while doing the insert.
 */

void processChar(Play& play,string charname, string playfile ){
	str_op op;
	std::ifstream ifs ;
	ifs.open(playfile,std::ifstream::in);
	string str;
	while(!ifs.eof()){

		getline(ifs,str);
		if(op.isblank(str) || (str.find_first_of(" ") == string::npos) )
			continue;
		string linenum_s = str.substr(0,str.find_first_of(" "));
		string text = str.substr(str.find_first_of(" ")+1);
		int linenum  = op.StringToNumber<int>(linenum_s);
		if(linenum == 0 || op.isblank(text))
			continue;

		struct_line str_line ;
		str_line.num = linenum;
		str_line.charname = charname;
		str_line.text = text;

		play << str_line;
	}
	ifs.close();
	
}


int main(int argc, char* argv[]){

	std::ifstream ifs;
	
	if(argc == 1)
	{
			cout << "usage: " << argv[0] << "<configuration_file_name>" << endl;
			system("pause");
			return 1;
	}

	cout << "Configuration File is :: " << argv[1] << endl;

	ifs.open (argv[1], std::ifstream::in);
	string playname="";
				
	if(ifs.good()){
		str_op op;
		//read the first non blank line which will be assigned playname.
		while(!ifs.eof() && op.isblank(playname)){
				getline(ifs,playname);
		}
		
		if(playname.length() == 0){
			cout << "Configuration file is empty, exiting" << endl;
			system("pause");
			return 1;
		}
		
		//should have playname by now. ready to initialize the class 
		Play play(playname);
	

		string str="";
		vector<thread> threads;
		//read one line at a time, validate that its well formed and based on that prepare threads to be launched.
		while(!ifs.eof()){
			 getline(ifs, str);
			 
			 if(op.isblank(str))
				continue;
			 
			 vector<string> tokens = op.split(str);
			if(tokens.size() < 2)
			 {
				 cout << "ignoring line as its not correctly formatted -- " <<  str;
				 continue;
			 }
			 string character =tokens[0];
			 string filename = tokens [1];
			 
			 std::ifstream playfilefs ;
			 playfilefs.open(filename,std::ifstream::in);
			 if(!playfilefs.good())
			 {
				 cout << "ignoring line as file is in accessible " <<  str << endl;
				 continue;
			 }
			 playfilefs.close();
			 //create thread and join with it.
			 thread t(processChar, ref(play),character, filename);
			 threads.push_back(move(t));
		}
		for(unsigned int i = 0; i < threads.size(); i++){
			threads[i].join();
		}
		play.print();

	}else{
		cout << "Configuartion File not found. Please correct the path and try again" << endl;
	}

	ifs.close();
	system("pause");
	return 0;
}