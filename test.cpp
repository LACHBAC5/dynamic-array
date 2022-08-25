#include "dynamic_array.hpp"
int main(){
    lb::dynamic_array<int> a;

    std::vector<int> b;

    time_t start;

    std::cout << "\nPUSH_BACK TEST:\n";

    start = clock();
    a.set_alloc_size(1000);
    for(int i = 0; i < 1000000; i++){
        a.push_back(i);
    }

    std::cout << "lb::dynamic_array finished in " << difftime(clock(), start)/CLOCKS_PER_SEC << '\n';
    start=clock();


    for(int i = 0; i < 1000000; i++){
        b.push_back(i);
    }
    
    std::cout << "std::vector finished in " << difftime(clock(), start)/CLOCKS_PER_SEC << '\n';
    std::cout << "\nERASE TEST:\n";
    start = clock();

    a.prep_flags();
    a.erase(2500, 7500);

    std::cout << std::fixed << "lb::dynamic_array finished in " << difftime(clock(), start)/CLOCKS_PER_SEC << '\n';
    start=clock();

    b.erase(b.begin()+2500, b.begin()+7500);
    std::cout << std::fixed << "std::vector finished in " << difftime(clock(), start)/CLOCKS_PER_SEC << '\n';
    std::cout << "\nINDEXING TEST:\n";
    start = clock();

    a.set_flags(1000);
    a.prep_flags();
    for(int i = 0; i < a.size(); i++){
        a[i]=0;
    }

    std::cout << std::fixed << "lb::dynamic_array finished in " << difftime(clock(), start)/CLOCKS_PER_SEC << '\n';
    start=clock();

    for(int i = 0; i < b.size(); i++){
        b[i]=0;
    }
    std::cout << std::fixed << "std::vector finished in " << difftime(clock(), start)/CLOCKS_PER_SEC << '\n';
    start = clock();
}