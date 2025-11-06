#include <iostream>
#include "bitarray.h"

int main()
{
    using namespace std;

    // int n, k;
    // char letter, letter1;
    // if (!scanf("%d %d %c %c", &n, &k, &letter, &letter1))
    // {
    //     return 0;
    // }
    
    BitArray A(17, 0b10110000101100010);
    //A.resize(20, 0);
    BitArray B(17, 0b00110111001100111);
    cout<<A.to_string(true, '_')<<endl;
    cout<<B.to_string(true, '_')<<endl;
    
    cout<<(A | B).to_string(true, '_')<<endl;
    cout<<(A & B).to_string(true, '_')<<endl;
    cout<<(A ^ B).to_string(true, '_')<<endl;

    // cout<<(A == B)<<endl;
    // A = B;
    // cout<<A.to_string(true, '_')<<endl;
    // cout<<(A == B)<<endl;
    
    // BitArray A(16, 0b0110111001100111);
    // cout<<A.to_string(true, '_')<<endl;
    // cout<<(~A).to_string(true, '_')<<endl;
    // cout<<(A>>4).to_string(true, '_')<<endl;
    // cout<<(A<<4).to_string(true, '_')<<endl;
    // cout<<(A>>8).to_string(true, '_')<<endl;
    // cout<<(A<<8).to_string(true, '_')<<endl;
    // cout<<(A>>16).to_string(true, '_')<<endl;
    // cout<<(A<<16).to_string(true, '_')<<endl;

    // A.resize(12);
    // cout<<A.to_string()<<endl;
    // A.push_back(1);
    // A.push_back(0);
    // cout<<A.to_string()<<endl;
    // A.resize(n + k, 1);
    // A.push_back(0);
    // A.set(n+k-1, 0);
    // A.set(n+k-5, 0);
    // A.set(n+k-8, 0);
    // A.set(n+k-13, 0);
    // A.set(n+k-17, 0);
    // A.PRINT();
    // cout<<A.size()<<endl;
    // (A << 4).PRINT();
    // (A >> 4).PRINT();
    // BitArray B(16, letter);
    // BitArray C(16, letter1);
    // B.PRINT();
    // C.PRINT();
    // (B & C).PRINT();
    // (B | C).PRINT();
    // (B ^ C).PRINT();
}
