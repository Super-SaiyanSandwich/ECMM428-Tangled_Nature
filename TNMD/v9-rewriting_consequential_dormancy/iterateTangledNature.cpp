#include "tangledNatureMass.cpp"




int main(int argc, char *argv[]){
    for( int pkill_acti = 1; pkill_acti <= 12; pkill_acti++)
    {
    for( int pkill_dorm = 0; pkill_dorm <= 20; pkill_dorm++)
    {
    for( int mudormant = 0; mudormant <= 5; mudormant++)
    {
    for( int muactive = 1; muactive <= 10; muactive++)
    {
    for( int pdormant = 0; pdormant <= 40; pdormant++)
    {
    for( int pactive = 1; pactive <= 15; pactive++)
    {
        pActive = pactive / 10.0;
        pDormant = pdormant / 50.0;
        muActive = muactive / 20.0;
        muDormant = mudormant / 100.0;
        pKill_Dorm = pkill_dorm / 100.0;
        pKill_Acti = pkill_acti / 20.0;
        mass_tnm("./", 100);
    }
    }
    }
    }
    }
    }
    
    

}