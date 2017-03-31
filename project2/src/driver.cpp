//#include "driver_queue.hpp"
#include "linked_queue.hpp"
#include "linked_set.hpp"
#include "linked_priority_queue.hpp"
#include "driver_priority_queue.hpp"
#include "driver_set.hpp"

bool is_less(std::string a, std::string b) {
	if ( (a == "") & (b != "")) {
		return false;
	}
	else if (b == "" and a != "") {
		return false;
	}
	else {
		return a[0] < b[0] and is_less(a.substr(1), b.substr(1));
	}
}

int main() {
//  ics::DriverQueue d;
  //ics::DriverPriorityQueue d;
//  ics::DriverSet d;
	std::cout << is_less("ant", "anz") << std::endl;

  return 0;
}
