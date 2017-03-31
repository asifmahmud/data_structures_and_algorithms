//#include <string>
//#include <iostream>
//#include <fstream>
//#include <vector>                    //For use with split function in ics46goody
//#include "ics46goody.hpp"
//#include "iterator.hpp"
//#include "array_queue.hpp"
//#include "array_priority_queue.hpp"
//#include "array_set.hpp"
//#include "array_map.hpp"
//
//typedef ics::ArraySet<std::string>           NodeSet;
//typedef ics::ArrayPriorityQueue<std::string> NodePQ;
//typedef ics::ArrayMap<std::string,NodeSet>   Graph;
//typedef ics::pair<std::string,NodeSet>       GraphEntry;
//
//
//
//Graph read_graph(std::ifstream &file) {
//	Graph g;
//	std::string s;
//
//	std::string file_name = ics::prompt_string("Enter File name");
//	file.open(file_name);
//	while (std::getline(file, s))
//	{
//		std::vector<std::string> vs;
//		vs = ics::split(s, ";");
//		if (g.has_key(vs[0])){
//			g[vs[0]].insert(vs[1]);
//		}
//		else{
//			NodeSet n;
//			n.insert(vs[1]);
//			g.put(vs[0], n);
//		}
//	}
//	file.close();
//	return g;
//}
//
//
//void print_graph(const Graph& graph, bool (*sort_by)(const std::string& i,const std::string& j)) {
//	std::cout << "Graph: source -> {destination} edges" << std::endl;
//	ics::ArrayPriorityQueue<std::string> p(sort_by);
//
//	for (auto i : graph){
//		p.enqueue(i.first);
//	}
//
//	while (! p.empty()){
//		std::string node = p.dequeue();
//		std::cout << node << "-> " << graph[node] << std::endl;
//	}
//}
//
//ics::ArraySet<std::string> reachable(const Graph& graph, std::string start) {
//
//	if (!graph.has_key(start)){
//		throw std::string("KeyError");
//	}
//
//	NodeSet reached;
//	ics::ArrayQueue<std::string> search;
//	search.enqueue(start);
//	reached.insert(start);
//
//	while (!search.empty()){
//		std::string node = search.dequeue();
//		if (graph.has_key(node)){
//			for (auto i : graph[node]){
//				reached.insert(i);
//				search.enqueue(i);
//			}
//		}
//	}
//	return reached;
//}
//
//
//int main() {
//  try {
//	  Graph g;
//	  std::ifstream file;
//	  g = read_graph(file);
//
//	  print_graph(g, [] (const std::string& i, const std::string& j){return i < j;});
//	  std::cout << std::endl;
//
//	  std::string input;
//	  while(true){
//		  input = ics::prompt_string("Enter Starting Node");
//		  if (input == "quit"){
//			  break;
//		  }
//		  else {
//			  try {
//				  std::cout << "Reachable from " << input << " = " << reachable(g, input) << "\n" << std::endl;
//
//			  } catch (std::string std){
//				  std::cout << input << " is not a key in the graph\n" << std::endl;
//			  }
//		  }
//	  }
//
//  } catch (ics::IcsError& e) {
//    std::cout << e.what() << std::endl;
//  }
//
//  return 0;
//}
