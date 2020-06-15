#include "tangledNatureMass.cpp"




int main(int argc, char *argv[]){
    pKill_Acti = 0.4;
    muActive = 0.10;

    for( int pkill_dorm = 0; pkill_dorm <= 20; pkill_dorm++)
    {
    //for( int mudormant = 0; mudormant <= 5; mudormant++)
    for( int mudormant = 1; mudormant <= 5; mudormant++)
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
        mass_tnm("./", 100);
    }
    }
    }
    }
    
    

}