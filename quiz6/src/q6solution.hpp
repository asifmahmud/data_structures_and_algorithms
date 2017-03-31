//Asif Mahmud 70556749

#ifndef Q6SOLUTION_HPP_
#define Q6SOLUTION_HPP_


#include <iostream>
#include <exception>
#include <fstream>
#include <sstream>
#include <algorithm>                 // std::swap
#include "ics46goody.hpp"
#include "array_queue.hpp"


////////////////////////////////////////////////////////////////////////////////

//Problem 1

template<class T>
class LN {
  public:
    LN ()
      : value(), next()
    {}

    LN (const LN<T>& l)
      : value(l.value), next(l.next)
    {}

    LN (T value, LN* n = nullptr)
      : value(value), next(n)
    {}

  T   value;
  LN* next;
};


//Write this function
template<class T>
void selection_sort(LN<T>* l) {
	for (auto i = l; i != nullptr; i = i->next){
		auto index_of_min = i;
		for (auto j = i->next; j != nullptr; j = j->next) {
			if (j->value < index_of_min->value){
				index_of_min = j;
			}
		}
		std::swap(i->value, index_of_min->value);
	}
}


////////////////////////////////////////////////////////////////////////////////

//Problem 2


//Precondition : Array values with indexes left_low  to left_high  are sorted
//Precondition : Array values with indexes right_low to right_high are sorted
//Note that left_high+1 = right_low (I pass both to make the code a bit simpler)
//Postcondition: Array values with indexes left_low  to right_high are sorted
//Hint: Merge into a temporary array (declared to be just big enough to store
//  all the needed values) and then copy that temporary array back into
//  the parameter array (between left_low to right_high inclusive)
//See the quiz for pseudocode for this method

//Write this function
void merge(int a[], int left_low,  int left_high,
                    int right_low, int right_high) {

	int index = (right_high-left_low) + 1;
	int temp[index];
	int used =  0;
	int llc = left_low;
	int lhc = left_high;
	int rlc = right_low;
	int rhc = right_high;

	while (used < index){
		if (rlc > rhc){
			temp[used++] = a[llc++];
		}
		else if (llc > lhc){
			temp[used++] = a[rlc++];
		}
		else{
			if (a[llc] < a[rlc]){
				temp[used++] = a[llc++];
			}
			else if (a[llc] > a[rlc]){
				temp[used++] = a[rlc++];
			}
		}
	}
	int counter = 0;
	for (int i = left_low; i <= right_high; ++i, ++counter){
		a[i] = temp[counter];
	}
}


////////////////////////////////////////////////////////////////////////////////

//Problem 3

int select_digit (int number, int place)
{return number/place % 10;}


//Write this function
void radix_sort(int a[], int length) {
	int counter = 0;
	ics::ArrayQueue<int> temp[10];
	for (int i = 0; i < 10; ++i){
		ics::ArrayQueue<int> q;
		temp[i] = q;
	}
	for (int i = 1; i <= 100000; i *= 10) {
		for (int j = 0; j < length; ++j) {
			temp[select_digit(a[j], i)].enqueue(a[j]);
		}
		for (int k = 0; k < 10; ++k){
			while (!temp[k].empty()){
				a[counter++] = temp[k].dequeue();
			}
		}
		counter = 0;
	}
}

#endif /* Q6SOLUTION_HPP_ */
