#ifndef SOLUTION_HPP_
#define SOLUTION_HPP_

#include <string>
#include <iostream>
#include <fstream>


template<class T>
class LN {
  public:
    LN ()                        : next(nullptr){}
    LN (const LN<T>& ln)         : value(ln->value), next(ln->next){}
    LN (T v, LN<T>* n = nullptr) : value(v), next(n){}
    T      value;
    LN<T>* next;
};


template<class T>
std::ostream& operator << (std::ostream& outs, LN<T>* l) {
  for (LN<T>* p = l; p!=nullptr; p=p->next)
    std::cout << p->value << "->";
  std::cout << "nullptr";
  return outs;
}


char relation (const std::string& s1, const std::string& s2) {
	char result;
	if      (s1 == s2) {result = '=';}
	else if (s1 < s2)  {result = '<';}
	else if (s1 > s2)  {result = '>';}
	else 			   {result = relation(s1.substr(1, s1.length()), s2.substr(1, s2.length()));}
	return result;
}


template<class T>
void add_ordered_i (LN<T>*& l, T value) {
	if (l == nullptr) {
		l = new LN<T>(value);
		return;
	}
	for (auto i = l; i != nullptr; i = i->next) {
		if (value < i->value) {
			i->next = new LN<T> (i->value, i->next);
			i->value = value;
			break;
		}
		if (i->next == nullptr && value > i->value) {
			i->next = new LN<T>(value);
			break;
		}
	}
}

template<class T>
void add_ordered_r (LN<T>*& l, T value) {
	if (l == nullptr) {
		l = new LN<T>(value);
	}
	else if (value < l->value) {
		l->next = new LN<T>(l->value, l->next);
		l->value = value;
	}
	else {
		add_ordered_r(l->next, value);
	}
}



#endif /* SOLUTION_HPP_ */