#include <iostream>
#include <ctime>
#include "dynamic_array.hpp"

int main(){
    lb::dynamic_array<int> a={0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    lb::dynamic_array<int> b={-1, -2, -3, -4, -5, -6, -1, -2, -3, -4, -5, -6};
    a.set_flags(2);
    a.prep_flags();
    b.prep_flags();

    a.insert(9, b, 0, b.size()-1);
    a.prep_flags();
    for(int i = 0; i < a.size(); i++){
        std::cout << a[i] << '\n';
    }
    a.debug();
}