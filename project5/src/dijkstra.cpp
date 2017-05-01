//Asif Mahmud 70556749


#include <string>
#include <iostream>
#include <fstream>
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "hash_graph.hpp"
#include "dijkstra.hpp"


std::string get_node_in_graph(const ics::HashGraph<int>& g, std::string prompt, bool allow_QUIT) {
 std::string node;
 for(;;) {
   node = ics::prompt_string(prompt + " (must be in graph" + (allow_QUIT ? " or QUIT" : "") + ")");
   if ((allow_QUIT && node == "QUIT") || g.has_node(node))
     break;
 }
 return node;
}

void handle_graph(){
	ics::HashGraph<int> g;
	std::string file_name = ics::prompt_string("Enter graph file name", "flightcost.txt");
	std::ifstream in_file;
	in_file.open(file_name);

	g.load(in_file, ";");
	std::cout << g << std::endl;
	std::string start_node = ics::prompt_string("Enter start node (must be in graph)");
	while (!g.has_node(start_node)){
		std::cout << start_node << " is not in Graph" << std::endl;
		start_node = ics::prompt_string("Enter start node (must be in graph)");
	}

	auto answer_map = extended_dijkstra(g, start_node);
	std::cout << answer_map << std::endl;
	std::string stop_node;
	while(true){
		stop_node = ics::prompt_string("Enter stop node (must be in graph or QUIT)");
		if (stop_node == "QUIT"){
			break;
		}
		while (!answer_map.has_key(stop_node)){
			if (stop_node == "QUIT")
				return;
			std::cout << stop_node << " is not in Graph" << std::endl;
			stop_node = ics::prompt_string("Enter stop node (must be in graph or QUIT)");
		}
		std::cout << recover_path(answer_map, stop_node) << std::endl;
	}
}


int main() {
 try {
	  handle_graph();
 } catch (ics::IcsError& e) {
   std::cout << e.what() << std::endl;
 }

 return 0;
}
