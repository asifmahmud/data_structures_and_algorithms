//Asif Mahmud 70556749


#ifndef ARRAY_EQUIVALENCE_HPP_
#define ARRAY_EQUIVALENCE_HPP_
#include "ics46goody.hpp"
#include <sstream>
#include "ics_exceptions.hpp"
#include "array_map.hpp"
#include "array_set.hpp"
#include "hash_map.hpp"

namespace ics {

//KLUDGE: Should be in ics_exceptions.hpp
class EquivalenceClassError : public IcsError {
  public:
    EquivalenceClassError(const std::string& message);
    virtual ~EquivalenceClassError();
    virtual const std::string what () const;
};

EquivalenceClassError::EquivalenceClassError(const std::string& message) : IcsError(message) {};
EquivalenceClassError::~EquivalenceClassError() {};
const std::string EquivalenceClassError::what () const {
  return "ArrayEquivalenceClassError " + message;
};



template<class T>
class ArrayEquivalence {
  public:
    //Fundamental constructor/methods
    ArrayEquivalence();
    void add_singleton    (T a);
    bool in_same_class    (T a, T b);
    void merge_classes_of (T a, T b);

    //Other methods
    int size        () const;
    int class_count () const;
    ics::ArraySet<ics::ArraySet<T>> classes ();

    //Useful for debugging (bassed on the implementation)
    int max_height  () const;
    ics::ArrayMap<T,int> heights () const;
    void show_equivalence () const;
  private:
    ics::ArrayMap<T,T>   parent;
    ics::ArrayMap<T,int> root_size;
    T compress_to_root (T a);
};



template<class T>
ArrayEquivalence<T>::ArrayEquivalence () : parent(), root_size() {
}


//Raise a EquivalenceClassError (with a descriptive message) if the parameter a
//  already a value in the Equivalence (was previously added as a singleton)
template<class T>
void ArrayEquivalence<T>::add_singleton (T a) {
  if (parent.has_key(a)) {
	std::ostringstream exc;
	exc << "ArrayEquivalence.add_singleton: a(" << a << ") already in an equivalence class";
	throw EquivalenceClassError(exc.str());
  }
  parent   [a] = a; //acts as its own parent
  root_size[a] = 1; //its equivalence class has 1 value in it
}


//Call compress_to_root in in_same_class and merge_classes_of
//When finished, a and all its ancestors should be associated
//  (in the parent map) with the root of a's equivalence class
//Raise a EquivalenceClassError (with a descriptive message) if the parameter a
//  is not already a value in the Equivalence (was never added as a singleton)
template<class T>
T ArrayEquivalence<T>::compress_to_root (T a) {
	ics::ArraySet<T> set;
	  if (!parent.has_key(a)) {
		std::ostringstream exc;
		exc << "ArrayEquivalence.compress_to_root: a(" << a << ") not in an equivalence class";
		throw EquivalenceClassError(exc.str());
	  }
	  else if (parent[parent[a]] == parent[a]){
		return parent[a];
	  }
	  else {
		set.insert(a);
		T val = a;
		T root;
		while (true){
			if (parent[parent[val]] == parent[val]){
				root = parent[val];
				break;
			}
			set.insert(parent[val]);
			val = parent[val];
		}
		for (auto i : set){
			parent[i] = root;
		}
		return root;
	  }
}


//Two values are in the same class if their equivalence trees have the
//  same roots
//In the process of finding the roots, compress all the values on the
//  path to the root: make the parent of a and all its ancestors directly
//  refer to the root of a's equivalence class (same for b).
//Raise a EquivalenceClassError (with a descriptive message) if the parameter a or b
//  is not already a value in the Equivalence (were never added as singletons)
template<class T>
bool ArrayEquivalence<T>::in_same_class (T a, T b) {
  if (!parent.has_key(a) or !parent.has_key(b)) {
	std::ostringstream exc;
	exc << "ArrayEquivalence.compress_to_root: a(" << (!parent.has_key(a) ? a : b) << ") not in an equivalence class";
	throw EquivalenceClassError(exc.str());
  }
  if (compress_to_root(a) == compress_to_root(b))
	  return true;

  return false;

}


//Compress a and b to their roots.
//If they are in different equivalence classes, make the parent of the
//  root of the smaller equivalence class refer to the root of the larger
//  equivalence class; update the size of the root of the larger equivalence
//  class and remove the root of the smaller equivalence class from the root_size.
//Raise a EquivalenceClassError (with a descriptive message) if the parameter a or b
//  is not already a value in the Equivalence (were never added as singletons)
template<class T>
void ArrayEquivalence<T>::merge_classes_of (T a, T b) {
  if (!parent.has_key(a) or !parent.has_key(b)) {
	std::ostringstream exc;
	exc << "ArrayEquivalence.compress_to_root: a(" << (!parent.has_key(a) ? a : b) << ") not in an equivalence class";
	throw EquivalenceClassError(exc.str());
  }
  T root_a = compress_to_root(a);
  T root_b = compress_to_root(b);
  if (root_a == root_b){
	  return;
  }
  else{
	  if (root_size[root_a] >= root_size[root_b]){
		  parent[root_b] = root_a;
		  root_size[root_a] += root_size[root_b];
		  root_size.erase(root_b);
	  }
	  else{
		  parent[root_a] = root_b;
		  root_size[root_b] += root_size[root_a];
		  root_size.erase(root_a);
	  }
  }
}


template<class T>
int ArrayEquivalence<T>::size () const{
  return parent.size();
}

template<class T>
int ArrayEquivalence<T>::class_count () const{
  return root_size.size();
}

template<class T>
int ArrayEquivalence<T>::max_height () const{
  int mh = 0;
  for (auto h : heights())
    if (h.second > mh)
      mh = h.second;
  return mh;
}


//Return the appropriate set of sets that represents the Equivalence relation.
//Hint: For every value, compress it to its root and put it in a set associated
//  with its root (the root is the key); then put each of these resulting sets
//  into a set the function returns.
template<class T>
ics::ArraySet<ics::ArraySet<T>> ArrayEquivalence<T>::classes () {
	ics::ArraySet<ics::ArraySet<T>> outer_set;
	//for (auto i : parent) { compress_to_root(i.first); }
	for (auto j : root_size){
		ics::ArraySet<T> inner_set;
		T root = compress_to_root(j.first);
		for (auto k : parent){
			if (parent[k.first] == root)
				inner_set.insert(k.first);
		}
		outer_set.insert(inner_set);
	}
	return outer_set;
}


template<class T>
ics::ArrayMap<T,int> ArrayEquivalence<T>::heights () const {
  ics::ArrayMap<T,int> answer;
  for (auto np : parent) {
    T e = np.first;
    int depth = 0;
    while (parent[e] != e) {
      e = parent[e];
      depth++;
    }
    int so_far = answer[e];
    if (so_far < depth)
      answer[e] = depth;
  }
  return answer;
}


template<class T>
void ArrayEquivalence<T>::show_equivalence () const {
  std::cout << "  parent map:    " << parent       << std::endl;
  std::cout << "  root_size map: " << root_size    << std::endl;
  std::cout << "  heights map:   " << heights()    << std::endl;
  std::cout << "  max height:    " << max_height() << std::endl;
}


}

#endif /* ARRAY_EQUIVALENCE_HPP_ */
