#include <iostream>
using namespace std;

int main()
{
    int alpha; 
    alpha = 2432535;
    int beta;
    beta = 356563;

    int *test[3] = {&alpha, &beta, &alpha};
    int *test1[3] = {&beta, &beta, &alpha};
    int *test2[3] = {&beta, &alpha, &alpha};

    int *test3[3][3] = {*test,*test1, *test2};

    cout << alpha << endl;
    cout << beta << endl;
    cout << &alpha << endl;
    cout << &beta << endl;
    cout << *test[0] << endl;
    cout << *test[1] << endl;
    cout << test[0] << endl;
    cout << test[1] << endl;
    cout << test[2] << endl;
    cout << test3[0][0] << endl;
    cout << sizeof(alpha) << endl;
    cout << sizeof(test) << endl;
    cout << sizeof(test3) << endl;
}