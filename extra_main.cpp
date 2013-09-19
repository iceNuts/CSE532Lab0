#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        cout << "Invalid input" << endl;
        return 0;
    }
    
    stringstream ss1;
    stringstream ss2;
    stringstream ss3;
    
    string fragment_file;
    string configuration;
    string play_name;
    
    ss1 << argv[1];
    ss1 >> fragment_file;
    ss2 << argv[2];
    ss2 >> configuration;
    ss3 << argv[3];
    ss3 >> play_name;
    
    ifstream infile(fragment_file.c_str());
    string line;
    //create configuration file
    configuration = configuration + ".txt";
    ofstream outfile(configuration.c_str(), std::ofstream::out);
    outfile << play_name << endl;
    outfile.close();
    
    int cntLine = 0;
    bool nameLineFlag = true;
    string filename;
    
    map<string, int> nameList;
    
    while(getline(infile, line)){
        if(line.empty()){nameLineFlag = true; cntLine++;continue;}
        if(nameLineFlag == true || cntLine == 0)
        {
            //record name and write
            nameLineFlag = false;
            
            std::stringstream trimmer;
            trimmer << line;
            line.clear();
            trimmer >> line;
            
            filename = line + "txt";
            //write to configuration file and create that name file

            line = line.substr(0, line.size()-1);
            
            if(nameList.find(line.c_str()) == nameList.end()){
                ofstream outfile1(filename.c_str(), std::ofstream::out);
                outfile1.close();
                ofstream outfile2(configuration.c_str(), std::ios_base::app);
                nameList[line.c_str()] = 1;
                outfile2 << line << " " << filename << endl;
                outfile2.close();
            }
            cntLine++;
            continue;
        }
        ofstream outfile3(filename.c_str(), std::ios_base::app);
        outfile3 << cntLine++ << " " << line << endl;
        outfile3.close();
    }
    
    return 0;
}