#include "tangledNatureMass.cpp"




int main(int argc, char *argv[]){
    pKill_Acti = 0.4;
    muActive = 0.10;

    for( int pkill_dorm = 1; pkill_dorm <= 10; pkill_dorm++)
    {
    //for( int mudormant = 0; mudormant <= 5; mudormant++)
    for( int mudormant = 0; mudormant <= 2; mudormant++)
    {
    for( int pdormant = 4; pdormant <= 10; pdormant++)
    {
        std::cout << "Done pdormant" << endl;
    for( int pactive = 3; pactive <= 8; pactive++)
    {
        pActive = (pactive - 0.5) / 5.0;
        pDormant = pdormant / 25.0;
        muDormant = mudormant / 50.0;
        pKill_Dorm = pkill_dorm / 50.0;
        mass_tnm("MTNM_Results/", 25);
    }
    }
    }
    }
    
    

}
