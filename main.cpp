#include <iostream>
#include <ctime>
#include "dynamic_array.hpp"

int main(){
    lb::dynamic_array<int> a={1, 2, 3, 4, 5};

    a={2, 4, 6, 8, 10};
    
    a.prep_flags();
    for(int i = 0; i < a.size(); i++){
        std::cout << a[i] << '\n';
    }
}