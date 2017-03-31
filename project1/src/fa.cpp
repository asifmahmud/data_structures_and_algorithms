#include <string>
#include <iostream>
#include <fstream>
#include <vector>                    //For use with split function in ics46goody
#include "ics46goody.hpp"
#include "iterator.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"

typedef ics::ArrayQueue<std::string>             InputsQueue;
typedef ics::ArrayMap<std::string,std::string>   InputStateMap;

typedef ics::ArrayMap<std::string,InputStateMap> FA;
typedef ics::pair<std::string,InputStateMap>     FAEntry;
typedef ics::ArrayPriorityQueue<FAEntry>         FAPQ;

typedef ics::pair<std::string,std::string>       Transition;
typedef ics::ArrayQueue<Transition>              TransitionQueue;

const FA read_fa(std::ifstream &file) {
	FA fa;
	file.open(ics::prompt_string("Enter File Name"));
	std::string s;
	while (std::getline(file, s)) {
		std::vector<std::string> vs = ics::split(s, ";");
		InputStateMap inputs;
		std::string key = vs[0];
		vs.erase(vs.begin());
		for (unsigned int i = 0; i < vs.size(); i+=2) {
			inputs[vs[i]] = vs[i+1];
		}
		fa[key] = inputs;
	}
	return fa;
}


void print_fa(const FA& fa) {
	std::cout << "Finite Automaton Description\n";
	for (auto i : fa) {
		std::cout << " " << i.first << " transitions: " << i.second << std::endl;
	}
}


TransitionQueue process(const FA& fa, std::string state, const InputsQueue& inputs) {
	TransitionQueue tq;
	Transition tp("", state);
	tq.enqueue(tp);
	InputStateMap input_map = fa[state];
	InputsQueue inputs_queue = inputs;
	std::string input_node;
	while (! inputs_queue.empty()) {
		input_node = inputs_queue.dequeue();
		if (!input_map.has_key(input_node)) {
			tp.first = input_node;
			tp.second = "illegal input";
		}
		else {
			state = input_map[input_node];
			input_map = fa[state];
			tp.first = input_node;
			tp.second = state;
		}
		tq.enqueue(tp);
	}
	return tq;
}

void interpret(const TransitionQueue& tq) {  //or TransitionQueue
	std::cout << "\nStarting New Simulation" << std::endl;
	TransitionQueue transition_queue = tq;
	Transition fa = transition_queue.dequeue();
	std::cout << "Starting State: " << fa.second << std::endl;

	while(true) {
		fa = transition_queue.dequeue();
		if (fa.second == "illegal input") {
			std::cout << " Inputs = " << fa.first << "; " << "illegeal input: terminated"<< std::endl;
			std::cout << "Stop State = None" << std::endl;
			break;
		}
		std::cout << " Inputs = " << fa.first << "; " << "new state = " << fa.second << std::endl;
		if (transition_queue.empty()) {
			std::cout << "Stop State = " << fa.second << std::endl;
			break;
		}
	}
}


int main() {
  try {
	  FA fa;
	  std::ifstream fa_file;
	  std::ifstream input_file;
	  fa = read_fa(fa_file);
	  std::cout << std::endl;
	  print_fa(fa);

	  std::cout << std::endl;
	  input_file.open(ics::prompt_string("Enter file with start-state and input"));
	  std::string s;
	  while (std::getline(input_file, s)) {
		  std::vector<std::string> vs = ics::split(s, ";");
		  std::string state = vs[0];
		  InputsQueue input_queue;
		  for (unsigned int i = 1; i < vs.size(); ++i) {
			  input_queue.enqueue(vs[i]);
		  }
		  TransitionQueue tq = process(fa, state, input_queue);
		  interpret(tq);
	  }
  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
