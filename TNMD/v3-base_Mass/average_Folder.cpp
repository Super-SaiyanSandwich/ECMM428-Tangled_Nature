#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;





int main(int argc, char *argv[]){
    string path;

    if(argc < 2)
    {
        return -1;
    }

    path = argv[1];

    bool more_Files = true;
    int file_Count = 0;
    fstream file_In; 

    size_t found_Start = path.find("_mGen");
    size_t found_Len = path.find("_", found_Start + 5);

    found_Len -= found_Start + 5;

    int m_Gen = stoi(path.substr(found_Start + 5 , found_Len));

    float* totals = new float[m_Gen];
    //totals = {0};
    for (int i = 0; i < m_Gen; i++)
    {
        totals[i] = 0.0;
    }

    while(more_Files){
        try
        {
            cout << "Opening File " << file_Count << endl; 
            file_In.open(path + to_string(file_Count) + ".csv", ios::in); 
        }
        catch (int e)
        {
            more_Files = false;
            break;
        }

        if(!file_In.is_open())
        {
            cout << "Failed to open " << file_Count << endl;
            more_Files = false;
            break;
        }

        int i = 0;
        vector<string> row;
        string line, word; 
        file_In >> line;

        while (file_In >> line)
        {
            row.clear();

            stringstream s(line); 
  
            while (getline(s, word, ',')) { 
               row.push_back(word); 
            } 

            totals[stoi(row[1]) - 1] += stoi(row[2]);            
        }         


        file_Count++;
        file_In.close();
    }

    for (int i = 0; i < m_Gen; i++)
    {
        totals[i] /= float(file_Count);
    }
    cout << "Saving average file\n"; 

    string filename = path + "__AVERAGE.csv";
    ofstream pop_file;
    pop_file << "generation,Npop"; 
    pop_file.open(filename);
    for (int i = 0; i < m_Gen; i++)
    {
        pop_file << to_string(i+1) << ", " << totals[i] << endl;
    }
    pop_file.close();
}