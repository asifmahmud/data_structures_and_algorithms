#include <string>
#include <iostream>
#include <fstream>
#include "array_set.hpp"
#include "array_map.hpp"
#include "solution.cpp"


int main() {
  try{
    //Testing extract
    std::cout << "Testing extract" << std::endl;
    ics::ArraySet<std::string> ss1,ss2,ss3;
    ss1.insert("a"); ss1.insert("b");
    ss2.insert("b"); ss2.insert("c");
    ss3.insert("b"); ss3.insert("x");ss3.insert("y");

    ics::ArraySet<ics::ArraySet<std::string>> s;
    s.insert(ss1);
    s.insert(ss2);
    s.insert(ss3);

    std::cout << "Original  Set = " << s << std::endl;
    std::cout << "Extracted Set = " << extract(s) << std::endl;


    //Testing swap
    std::cout << "\nTesting swap" << std::endl;
    ics::ArrayMap<std::string,ics::ArraySet<std::string>> names_set;
    ics::ArraySet<std::string> boy_names, girl_names;
    boy_names.insert("Mary");    boy_names.insert("Betty");    boy_names.insert("Mimsi");
    girl_names.insert("Peter");  girl_names.insert("Joey");    girl_names.insert("Joe"); girl_names.insert("Carl");
    names_set["Boy"]  = boy_names;
    names_set["Girl"] = girl_names;
    std::cout << "Original Map = " << names_set << std::endl;
    //Oops, got the names backwards; let's swap values mapped to/from "Boy" and "Girl"
    swap(names_set, std::string("Boy"), std::string("Girl"));
    std::cout << "Swapped  Map = " << names_set << std::endl;


    //Testing values_set_to_queue
    std::cout << "\nTesting values_set_to_queue" << std::endl;
    std::cout << "Original Map = " << names_set << std::endl;
    ics::ArrayMap<std::string,ics::ArrayQueue<std::string>> names_queue;
    values_set_to_queue(names_set, names_queue);
    std::cout << "New Map = " << names_queue << std::endl;


    //Testing reachable
    std::cout << "\nTesting reachable" << std::endl;
    ics::ArrayMap<std::string,std::string> f;
    f["a"] = "c";
    f["b"] = "b";
    f["c"] = "d";
    f["d"] = "c";
    std::cout << "For map f = " << f << std::endl;

    std::cout << "reachable(f,\"a\") = " << reachable<std::string,std::string>(f,"a") << std::endl;
    std::cout << "reachable(f,\"b\") = " << reachable<std::string,std::string>(f,"b") << std::endl;
    std::cout << "reachable(f,\"c\") = " << reachable<std::string,std::string>(f,"c") << std::endl;
    std::cout << "reachable(f,\"d\") = " << reachable<std::string,std::string>(f,"d") << std::endl;

  } catch (ics::IcsError& e) {
    std::cout << "  " << e.what() << std::endl;
  }

  return 0;
}



/*
Here is the output that the driver should produce:

Testing extract
Original  Set = set[set[a,b],set[b,c],set[b,x,y]]
Extracted Set = set[a,b,c,x,y]

Testing swap
Original Map = map[Boy->set[Mary,Betty,Mimsi],Girl->set[Peter,Joey,Joe,Carl]]
Swapped  Map = map[Boy->set[Peter,Joey,Joe,Carl],Girl->set[Mary,Betty,Mimsi]]

Testing values_set_to_queue
Original Map = map[Boy->set[Peter,Joey,Joe,Carl],Girl->set[Mary,Betty,Mimsi]]
New Map = map[Boy->queue[Peter,Joey,Joe,Carl]:rear,Girl->queue[Mary,Betty,Mimsi]:rear]

Testing reachable
For map f = map[a->c,b->b,c->d,d->c]
reachable(f,"a") = set[a,c,d]
reachable(f,"b") = set[b]
reachable(f,"c") = set[c,d]
reachable(f,"d") = set[d,c]
*/
