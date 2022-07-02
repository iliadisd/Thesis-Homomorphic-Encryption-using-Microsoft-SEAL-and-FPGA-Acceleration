#include <iomanip>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <array>
#include <cstddef>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <memory>
#include <sstream>
#include <cstring>
#include <functional>
#include <fstream>

#define times 393485

using namespace std;
using namespace std::chrono;

void div(uint64_t *num, uint64_t *den, uint64_t *quo);

int main(){

    //Read numerator
    uint64_t *num = (uint64_t *) malloc(sizeof(uint64_t) * times);
    // Read from the text file
    ifstream f1;
    f1.open("/PATH/TO/FILE/numerator.txt");
    int i = 0;
    if(f1.is_open()){
		while (!f1.eof() )
		{
			f1>>num[i];
			i++;
		}
    }
    f1.close();

    //Read denominator
    uint64_t *den = (uint64_t *) malloc(sizeof(uint64_t) * times);
    // Read from the text file
    ifstream f2;
    f2.open("/PATH/TO/FILE/denominator.txt");
    i = 0;
    if(f2.is_open()){
		while (!f2.eof() )
		{
			f2>>den[i];
			i++;
		}
    }
    f2.close();

    uint64_t *quo = (uint64_t *) malloc(sizeof(uint64_t) * times);

    auto start = high_resolution_clock::now();

    div(num, den, quo);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << endl;
    cout << duration.count() << endl;
    cout << endl;

    free(num);
    free(den);

    //Read quotient
    uint64_t *quo_check = (uint64_t *) malloc(sizeof(uint64_t) * times);
    // Read from the text file
    ifstream f3;
    f3.open("/PATH/TO/FILE/quotient.txt");
    i = 0;
    if(f3.is_open()){
		while (!f3.eof() )
		{
			f3>>quo_check[i];
			i++;
		}
    }
    f3.close();

    for (int i = 0; i < times; i++){
		if (quo_check[i] != quo[i]){
			std::cout<<"Wrong Quotient!"<<std::endl;
			break;
		}
    }

	std::cout<<"Correct Quotient!"<<std::endl;

    free(quo);
    free(quo_check);

    return 0;
}
