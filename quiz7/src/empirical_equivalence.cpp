#include <string>
#include <iostream>
#include <fstream>
#include "set.hpp"
#include "ics46goody.hpp"
#include "stopwatch.hpp"
#include "ics_exceptions.hpp"
#include <string>                    // std::hash<std::string>
#include <random>
#include "array_set.hpp"
//#include "array_equivalence.hpp"
#include "hash_equivalence.hpp"

//For use with HashEquivalence
static int hash_int(const int& i) {std::hash<int> int_hash; return int_hash(i);}

int main() {
    int N            = ics::prompt_int("Enter N for test (creates N singletons)");
    int test_times   = 5;//ics::prompt_int("Enter number of times to test (averaged): 5 is good");
    int merge_factor = 5;//ics::prompt_int("Enter factor (for N) for merges: 5 is good");

    //Show as errors in Eclipse, but code will runn
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,N-1);   //0 - 999,999: up to 6 digits
    try {
      double total_time = 0;
      for (int count=0; count<test_times; ++count) {
        //ics::ArrayEquivalence<int> e;
        ics::HashEquivalence<int> e(hash_int);
         for (int i=0; i<N; ++i)
           e.add_singleton(i);

         ics::Stopwatch watch;
         watch.start();
         //while (e.numberOfClasses() != 1) {
         for (int i=0; i<merge_factor*N; ++i) {
           int m1 = distribution(generator);
           int m2 = distribution(generator);
           //std::cout << e.classes() << ": " << m1 << "/" << m2 << std::endl;
           if (!e.in_same_class(m1, m2))
             e.merge_classes_of(m1, m2);
         }
         watch.stop();
         total_time += watch.read();
         std::cout << "\n# of values (should be " << N << ") = " << e.size() << std::endl;
         std::cout << "# of classes = " << e.class_count() << std::endl;
         std::cout << "MaxHeight = " << e.max_height() << std::endl;
         std::cout << "Time = " << watch.read() << std::endl;
       }

       std::cout << "\nAverage time = " << total_time/test_times << std::endl;
    } catch (ics::IcsError& e) {
      std::cout << "  " << e.what() << std::endl;
    }
}

