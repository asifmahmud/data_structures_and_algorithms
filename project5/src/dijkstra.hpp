//Asif Mahmud 70556749

#ifndef DIJKSTRA_HPP_
#define DIJKSTRA_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>                    //Biggest int: std::numeric_limits<int>::max()
#include "array_stack.hpp"
#include "hash_graph.hpp"

namespace ics {


int hash(const std::string& s) {std::hash<std::string> str_hash; return str_hash(s);}


class Info {
  public:
    Info(){}
    int          cost     = std::numeric_limits<int>::max();
    std::string from      = "?";
    bool operator == (const Info& rhs) const {
      return cost == rhs.cost && from == rhs.from;
    }
    bool operator != (const Info& rhs) const {
      return !(*this == rhs);
    }
    friend std::ostream& operator << (std::ostream& outs, const Info& i) {
      outs << "Info[" << i.cost << "," << i.from << "]";
      return outs;
    }

};


//Return the final_map as specified in the description of extended Dijkstra algorithm
ics::HashMap<std::string,Info> extended_dijkstra(const ics::HashGraph<int>& g, std::string start_node) {
	ics::HashMap<std::string, Info> info_map(hash);
	ics::HashMap<std::string, Info> answer_map(hash);
	auto nodes = g.all_nodes();
	for (auto i : nodes)
		info_map[i.first] = Info();
	info_map[start_node].cost = 0;
	while (!info_map.empty()){
		int limit = std::numeric_limits<int>::max();
		std::string min;
		for (auto i : info_map){
			if (info_map[i.first].cost < limit){
				min = i.first;
				limit = info_map[min].cost;
			}
		}
		answer_map[min] = info_map[min];
		info_map.erase(min);
		auto out_nodes = g.out_nodes(min);
		auto edges = g.all_edges();
		for (auto i : out_nodes){
			if (!answer_map.has_key(i)){
				int total_cost = limit + edges[ics::make_pair(min, i)];
				if (total_cost < info_map[i].cost){
					info_map[i].cost = total_cost;
					info_map[i].from = min;
				}
			}
		}
	}
	return answer_map;
}


//Return a string from the start node (implicit in answer_map) to the end node
//  (left to right, separated by a space)
std::string recover_path(ics::HashMap<std::string,Info> answer_map, std::string end_node) {
	ics::ArrayStack<std::string> stack;
	std::ostringstream iss;
	while (end_node != "?"){
		stack.push(end_node);
		end_node = answer_map[end_node].from;
	}
	while (!stack.empty())
		iss << stack.pop() << " ";
	return iss.str();
}


}

#endif /* DIJKSTRA_HPP_ */
