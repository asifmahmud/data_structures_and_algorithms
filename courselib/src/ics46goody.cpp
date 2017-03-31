#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ics46goody.hpp"

namespace ics {

std::vector<std::string> split(const std::string& s, const std::string& pat){
  std::vector<std::string> answer;
  int pos2;
  for (unsigned int pos = 0; ; pos=pos2+pat.size()) {
    pos2 = s.find(pat,pos);
    if (pos2 == -1) {
      answer.push_back(s.substr(pos,s.size()+1-pos));
      break;
    }
    answer.push_back(s.substr(pos,pos2-pos));
  }
  return answer;
}


std::string join(const std::vector<std::string>& s, const std::string& pat){
  std::string answer;
  for (unsigned int i = 0; i<s.size(); ++i)
    answer.append(s[i] + (i != s.size()-1 ? pat : ""));
  return answer;
}


std::string prompt_string(std::string prompt,
                          std::string default_value,
                          std::string allowable[]) {
  std::string answer;
  for (;;) {
    std::cout << prompt;
    if (default_value != "")
      std::cout << "["+default_value+"]";
    std::cout << ": ";
    getline(std::cin,answer);
    if (answer == "" && default_value != "")
      answer = default_value;
    if (allowable == NULL)
      return answer;
    for (int i=0; allowable[i] != ""; ++i)
      if (answer == allowable[i])
        return answer;
    std::cout << answer << " not allowed" << std::endl;
  }

  return answer;  //For compiler
}


bool prompt_bool(std::string prompt,
                 int default_value) {
  std::string def = (default_value == 0 ? "[false]" : (default_value == 1 ? "[true]" : ""));
  for (;;) {
    std::string answer = ics::prompt_string(prompt+def+"?");
    if (answer == "" && (default_value == 0 || default_value == 1))
      return default_value;
    if (answer == "true")
      return true;
    if (answer == "false")
      return false;
  }
  return false;  //For compiler
}


int prompt_int(std::string prompt) {
  for (;;) {
    try {
      std::string answer = ics::prompt_string(prompt);
      int v;
      std::istringstream fake(answer);
      fake >> v;
      return v;
      //KLUDGE extract of non-int doesn't throw exception: fix this
    } catch (const std::exception& e) {
      std::cout << "illegal" << std::endl;
    }
  }
  return 0;  //For compiler
}


void safe_open(std::ifstream& f, const std::string& prompt, const std::string& default_name) {
  for (;;) {
    std::string file_name = prompt_string(prompt,default_name);
    f.open(file_name.c_str());
    if (!f.fail())
      break;
    std::cout << "Opening " << file_name << " failed." << std::endl;
  }
}

int rand_range(int low, int high) {
  return low + std::rand() % (high-low+1);
}

}
