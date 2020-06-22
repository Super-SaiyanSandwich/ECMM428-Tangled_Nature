#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

string average_path = "./Averages/";



int averageFolder(string path){

    bool more_Files = true;
    int file_Count = 0;
    fstream file_In; 

    int m_Gen = 10000;

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
            file_In.open("./" + path + "/" + to_string(file_Count) + ".csv", ios::in); 
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
    pop_file.open(filename);
    pop_file << "generation,Npop" << endl; 
    for (int i = 0; i < m_Gen; i++)
    {
        pop_file << to_string(i+1) << ", " << totals[i] << endl;
    }
    pop_file.close();
}



int main(int argc, char *argv[]){
    double pKill_Acti;
    double muActive;
    double pActive;
    double pDormant;
    int InitPop = 100;
    double muDormant;
    double pKill_Dorm;
    double theta = 0.25;
    double pMutate = 0.01;


    pKill_Acti = 0.4;
    muActive = 0.10;

    for( int pkill_dorm = 0; pkill_dorm <= 20; pkill_dorm++)
    {
    //for( int mudormant = 0; mudormant <= 5; mudormant++)
    for( int mudormant = 0; mudormant <= 5; mudormant++)
    {
    for( int pdormant = 0; pdormant <= 40; pdormant++)
    {
        std::cout << "Done pdormant" << endl;
    for( int pactive = 5; pactive <= 15; pactive++)
    {
        pActive = pactive / 10.0;
        pDormant = pdormant / 50.0;
        muDormant = mudormant / 100.0;
        pKill_Dorm = pkill_dorm / 100.0;
        
        string path = "massDorm";
        path += "_InitPop" + to_string(InitPop);
        path += "_killActive" + to_string(pKill_Acti);
        path += "_killDormant" + to_string(pKill_Dorm);
        path += "_pActive" + to_string(pActive);
        path += "_pDormant" + to_string(pDormant);
        path += "_muActive" + to_string(muActive);
        path += "_muDormant" + to_string(muDormant);
        path += "_pMutate" + to_string(pMutate);
        path += "_theta" + to_string(theta);
        path += "_dormancyTypeRIIS";
        averageFolder(path);
    }
    }
    }
    }
}
