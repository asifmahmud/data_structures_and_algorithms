//#include <string>
//#include <iostream>
//#include <fstream>
//#include <vector>                    //For use with split function in ics46goody
//#include <limits>                    //Biggest int: std::numeric_limits<int>::max()
//#include "ics46goody.hpp"
//#include "iterator.hpp"
//#include "array_queue.hpp"
//#include "array_priority_queue.hpp"
//#include "array_set.hpp"
//#include "array_map.hpp"
//
//typedef ics::ArrayQueue<std::string>              CandidateQueue;
//typedef ics::ArraySet<std::string>                CandidateSet;
//typedef ics::ArrayMap<std::string,int>            CandidateTally;
//
//typedef ics::ArrayMap<std::string,CandidateQueue> Preferences;
//typedef ics::pair<std::string,CandidateQueue>     PreferencesEntry;
//
//typedef ics::pair<std::string,int>                TallyEntry;
//typedef ics::ArrayPriorityQueue<TallyEntry>       TallyEntryPQ;
//
//
//Preferences read_voter_preferences(std::ifstream &file) {
//	Preferences p;
//	std::string s;
//	std::string file_name = ics::prompt_string("Enter File Name");
//	file.open(file_name);
//	while (std::getline(file, s)) {
//		CandidateQueue c;
//		std::vector<std::string> votes = ics::split(s, ";");
//		c.enqueue(votes[1]); c.enqueue(votes[2]); c.enqueue(votes[3]);
//		p[votes[0]] = c;
//	}
//	file.close();
//	return p;
//}
//
//
//void print_voter_preferences(const Preferences& preferences, bool (*sort_by)(const std::string& i,const std::string& j)) {
//	ics::ArrayPriorityQueue<std::string> p(sort_by);
//
//	std::cout << std::endl << "Voter Preferences" << std::endl;
//	for (auto i : preferences){
//		p.enqueue(i.first);
//	}
//
//	while (! p.empty()){
//		std::string node = p.dequeue();
//		std::cout << node << "-> " << preferences[node] << std::endl;
//	}
//}
//
//
//void print_tally(std::string message, const CandidateTally& tally, bool (*sort_by)(const TallyEntry& i,const TallyEntry& j)) {
//	ics::ArrayPriorityQueue<TallyEntry> p(sort_by);
//
//	for (auto i : tally) {
//		p.enqueue(i);
//	}
//
//	while (!p.empty()) {
//		TallyEntry t = p.dequeue();
//		std::cout << t.first << " -> " << t.second << std::endl;
//	}
//
//}
//
//CandidateTally evaluate_ballot(const Preferences& preferences, const CandidateSet& candidates) {
//	CandidateTally ct;
//	std::string c;
//	for(auto q: preferences){ //pairs "A" -> [x,y,z]
//		c = q.second.peek();
//		CandidateQueue votes = q.second;
//		if (!candidates.contains(c)){
//			votes.dequeue();
//			c = votes.peek();
//		}
//		if (candidates.contains(c) and ct.has_key(c)){
//			ct[c] += 1;
//		}
//		else if (candidates.contains(c) and !ct.has_key(c)){
//			ct[c] = 1;
//		}
//	}
//	return ct;
//}
//
//CandidateSet remaining_candidates(const CandidateTally& tally) {
//	CandidateSet remaining;
//	int min;
//	for (auto c : tally){
//		if(min > c.second){
//			min = c.second;
//		}
//	}
//	for (auto c2 : tally){
//	//	std::cout << "tally[c2.first] " << tally[c2.first] << std::endl;
//		if (!(tally[c2.first] == min)){
//			remaining.insert(c2.first);
//		}
//	}
//	//std::cout << "remaining " << remaining << std::endl; //map: str -> int
//	return remaining;
//}
//
//
//int main() {
//  try {
//
//	  Preferences p;
//	  std::ifstream file;
//	  p = read_voter_preferences(file);
//	  print_voter_preferences(p, [] (const std::string& i, const std::string& j) { return i < j;});
//	  CandidateSet list_candidates;
//
//	  for (auto i : p) {
//		 list_candidates.insert(i.second.abegin(), i.second.aend());
//	  }
//
//	  int counter = 1;
//	  while (true){
//		  CandidateTally ct = evaluate_ballot(p, list_candidates);
//		  std::string message = "";
//		  auto alphabetical = [] (const TallyEntry& i, const TallyEntry& j){return i.first < j.first;};
//		  auto numerical =    [] (const TallyEntry& i, const TallyEntry& j){return i.second > j.second;};
//		  std::cout << "Vote count on ballot #" << counter << " with candidates alphabetically: still in election = " << list_candidates << std::endl;
//		  print_tally(message, ct , alphabetical);
//		  std::cout << "Vote count on ballot #" << counter << " with candidates numerically: still in election = " << list_candidates << std::endl;
//		  print_tally(message, ct , numerical);
//		  list_candidates = remaining_candidates(ct);
//		  if (list_candidates.size() == 0) {
//			  CandidateTally::Iterator first = ct.begin();
//			  if (ct.size() > 1) {
//				  std::cout << "It is a Tie" << std::endl;
//			  }
//			  else {
//				  std::cout << "The Winner is " << first->first << std::endl;
//			  }
//			  break;
//		  }
//		  ++counter;
//	  }
//
//
//  } catch (ics::IcsError& e) {
//    std::cout << e.what() << std::endl;
//  }
//  return 0;
//}
