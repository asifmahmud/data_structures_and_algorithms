#include "driver_priority_queue.hpp"
#include "heap_priority_queue.hpp"
#include "bst_map.hpp"
//#include "driver_map.hpp"

int main() {
 //ics::DriverPriorityQueue d;
 //ics::DriverMap d;
	ics::BSTMap<std::string, int> m;
	ics::BSTMap<std::string, int> m1;
	m.put("d", 4);
	m.put("a", 1);
	m.put("c", 3);
	m.put("b", 2);
	m["f"] = 8;
	m1.put(m.abegin(), m.aend());
	std::cout << m << std::endl;
	std::cout << m1.size() << std::endl;
	std::cout << (m1 == m) << std::endl;


 return 0;
}
