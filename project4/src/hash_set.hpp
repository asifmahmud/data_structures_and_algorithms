//# Asif Mahmud, 70556749
//# Kim Le, 91394901
//# We certify that we worked cooperatively on this programming
//#   assignment, according to the rules for pair programming


#ifndef HASH_SET_HPP_
#define HASH_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include <complex>
#include "ics_exceptions.hpp"
#include "pair.hpp"
#include "iterator.hpp"
#include "set.hpp"


namespace ics {

template<class T> class HashSet : public Set<T>	{
  public:
    HashSet() = delete;
    HashSet(int (*ahash)(const T& element), double the_load_factor = 1.0);
    HashSet(int initial_bins, int (*ahash)(const T& element), double the_load_factor = 1.0);
    HashSet(const HashSet<T>& to_copy);
    //HashSet(std::initializer_list<Entry> il, int (*ahash)(const T& element), double the_load_factor = 1.0);
    HashSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop, int (*ahash)(const T& element), double the_load_factor = 1.0);
    virtual ~HashSet();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual bool contains   (const T& element) const;
    virtual std::string str () const;

    virtual bool contains (ics::Iterator<T>& start, const ics::Iterator<T>& stop) const;

    virtual int  insert (const T& element);
    virtual int  erase  (const T& element);
    virtual void clear  ();

    virtual int insert (ics::Iterator<T>& start, const ics::Iterator<T>& stop);
    virtual int erase  (ics::Iterator<T>& start, const ics::Iterator<T>& stop);
    virtual int retain (ics::Iterator<T>& start, const ics::Iterator<T>& stop);

    virtual HashSet<T>& operator = (const HashSet<T>& rhs);
    virtual bool operator == (const Set<T>& rhs) const;
    virtual bool operator != (const Set<T>& rhs) const;
    virtual bool operator <= (const Set<T>& rhs) const;
    virtual bool operator <  (const Set<T>& rhs) const;
    virtual bool operator >= (const Set<T>& rhs) const;
    virtual bool operator >  (const Set<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const HashSet<T2>& s);

    virtual ics::Iterator<T>& abegin () const;
    virtual ics::Iterator<T>& aend   () const;

  private:
    class LN;

  public:
    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(HashSet<T>* fof, bool begin);
        Iterator(const Iterator& i);
        virtual ~Iterator();
        virtual T           erase();
        virtual std::string str  () const;
        virtual const ics::Iterator<T>& operator ++ ();
        virtual const ics::Iterator<T>& operator ++ (int);
        virtual bool operator == (const ics::Iterator<T>& rhs) const;
        virtual bool operator != (const ics::Iterator<T>& rhs) const;
        virtual T& operator *  () const;
        virtual T* operator -> () const;
      private:
        ics::pair<int,LN*> current; //Bin Index and Cursor; stop: LN* == nullptr
        HashSet<T>*        ref_set;
        int                expected_mod_count;
        bool               can_erase = true;
        void advance_cursors();
    };

    virtual Iterator begin () const;
    virtual Iterator end   () const;

  private:
    class LN {
      public:
        LN ()                      : next(nullptr){}
        LN (const LN& ln)          : value(ln.value), next(ln.next){}
        LN (T v,  LN* n = nullptr) : value(v), next(n){}

        T   value;
        LN* next;
    };

    LN** set      = nullptr;
    int (*hash)(const T& element);
    double load_factor;//used/bins <= load_factor
    int bins      = 1; //# bins in array
    int used      = 0; //# of key->value pairs in the hash table
    int mod_count = 0; //For sensing concurrent modification
    int   hash_compress (const T& element) const;
    void  ensure_load_factor(int new_used);
    LN*   find_element (int bin, const T& element) const;
    LN*   copy_list(LN*   l) const;
    LN**  copy_hash_table(LN** ht, int bins) const;
    void  delete_hash_table(LN**& ht, int bins);
  };





template<class T>
HashSet<T>::HashSet(int (*ahash)(const T& element), double the_load_factor) : hash(ahash), load_factor(the_load_factor) {
	  set = new LN*[bins];
	  for (int i = 0; i < bins; i++){
		  set[i] = new LN(); // array of trailer nodes
	  }
}

template<class T>
HashSet<T>::HashSet(int initial_bins, int (*ahash)(const T& element), double the_load_factor) : bins(initial_bins), hash(ahash), load_factor(the_load_factor) {
	  set = new LN*[bins];
}

template<class T>
HashSet<T>::HashSet(const HashSet<T>& to_copy) : hash(to_copy.hash), load_factor(to_copy.load_factor), bins(to_copy.bins), used(to_copy.used) {
	  set = copy_hash_table(to_copy.set, bins);
}

template<class T>
HashSet<T>::HashSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop, int (*ahash)(const T& element), double the_load_factor) : hash(ahash), load_factor(the_load_factor) {
	  set = new LN*[bins];
	  for (int i = 0; i < bins; i++){
		  set[i] = new LN(); // array of trailer nodes
	  }
	this->insert(start, stop);
}

//KLUDGE: Not usable yet
//template<class KEY,class T>
//HashSet<T>::HashSet(std::initializer_list<Entry> i,int (*ahash)(const KEY& k), double the_load_factor = 1.0) {
//  set = new LN*[bins];
//  put(il.abegin(),il.aend());
//}

template<class T>
HashSet<T>::~HashSet() {
	 delete_hash_table(set, bins);
}


template<class T>
inline bool HashSet<T>::empty() const {
  return used == 0;
}

template<class T>
int HashSet<T>::size() const {
  return used;
}

template<class T>
bool HashSet<T>::contains (const T& element) const {
  int index = hash_compress(element);
  if (find_element(index, element) == nullptr){
	  return false;
  }
  return true;
}

template<class T>
std::string HashSet<T>::str() const {
	std::ostringstream answer;
	//print bins
//	for (int i = 0; i < bins ; i++ ){
//		answer << std::endl << "bin[" << i <<"]: ";
//		for(auto l_pointer = set[i] ; l_pointer != nullptr; l_pointer = l_pointer->next){
//			if (l_pointer->next == nullptr) // if it's a trailer node
//				answer << "#";
//			else{
//				answer << l_pointer->value <<  " -> ";
//			}
//		}
//
//	}
	//print map[]
	if (this->empty()){
			answer << "set[]";
		}
		else{
			answer << "set[";
			for (int i = 0; i < bins; ++i){
				for (auto j = set[i]; j->next != nullptr; j = j->next){
					answer << j->value;
					answer << ",";
				}
			}
			std::string string = answer.str();
			string[string.length()-1] = ']';
			return string;
		}
   return answer.str();
}

template<class T>
bool HashSet<T>::contains(ics::Iterator<T>& start, const ics::Iterator<T>& stop) const {
	  for (; start != stop; ++start)
	    if (!contains(*start))
	      return false;

	  return true;
}

template<class T>
int HashSet<T>::insert(const T& element) {
 if (contains(element)) {
	 return 0;
 }
  ensure_load_factor(used+1); // return expanded array
  int index_to_put = hash_compress(element);
  set[index_to_put] =  new LN(element, set[index_to_put]);
  used++;
  mod_count++;
  return 1;
}


template<class T>
int HashSet<T>::erase(const T& element) {
	for (int i = 0; i < bins; ++i) {
		for (LN* j = this->set[i]; j->next != nullptr; j = j->next){
			if (j->value == element) { // next is pointing to trailer node
				j->value = j->next->value;
				j->next = j->next->next;
				used--;
				mod_count++;
				return 1;
			}
		}
	}
	return 0;
}

template<class T>
void HashSet<T>::clear() {
  set = new LN*[bins];
  used = 0;
  mod_count++;
  for (int i = 0; i < bins; i++){
  	  set[i] = new LN(); // array of trailer nodes
    }

}

template<class T>
int HashSet<T>::insert(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  int count;
  for (;start != stop; ++start){
	  count += this->insert(*start);
  }
  return count;
}

template<class T>
int HashSet<T>::erase(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	  int count = 0;
	  for (; start != stop; start++)
	    count += erase(*start);
	  return count;
}

template<class T>
int HashSet<T>::retain(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	int count = 0;
	  HashSet<T> s(start,stop, hash);
	  for (int i=0; i<bins; i++)
		  for (auto j = set[i]; j->next != nullptr;)//j = j->next)
			  if (!s.contains(j->value)) {
				  erase(j->value);
			  }else{
				  j = j->next;
				  ++count;
			  }
	  return count;
}

template<class T>
HashSet<T>& HashSet<T>::operator = (const HashSet<T>& rhs) {
	  if (this == &rhs){
	    return *this;
	  }
	  this->clear();
	  this->insert(rhs.abegin(), rhs.aend());

	  ++mod_count;
	  return *this;
}

template<class T>
bool HashSet<T>::operator == (const Set<T>& rhs) const {
  if (this == &rhs)
	  return true;
  if (used != rhs.size())
	  return false;
  if (this->empty() and rhs.empty())
	  return true;

  for (int i = 0; i < bins; i++)
	  for (auto j = set[i]; j->next != nullptr; j = j->next)
		  if (!rhs.contains(j->value))
			  return false;
  return true;

}

template<class T>
bool HashSet<T>::operator != (const Set<T>& rhs) const {
  return (!(*this == rhs));
}

template<class T>
bool HashSet<T>::operator <= (const Set<T>& rhs) const {
  if (this == &rhs)
	return true;
  if (used > rhs.size())
	return false;
  for (int i = 0; i < bins; i++)
	  for (auto j = set[i]; j->next != nullptr; j = j->next)
		  if (!rhs.contains(j->value))
			  return false;

  return true;
}

template<class T>
bool HashSet<T>::operator < (const Set<T>& rhs) const {
	  if (this == &rhs)
	    return false;
	  if (used >= rhs.size())
	    return false;
	  for (int i = 0; i < bins; i++)
		  for (auto j = set[i]; j->next != nullptr; j = j->next)
			  if (!rhs.contains(j->value))
				  return false;

	  return true;
}

template<class T>
bool HashSet<T>::operator >= (const Set<T>& rhs) const {
	return rhs <= *this;
}

template<class T>
bool HashSet<T>::operator > (const Set<T>& rhs) const {
	return rhs < *this;
}

template<class T>
std::ostream& operator << (std::ostream& outs, const HashSet<T>& s) {

  outs << s.str();
  return outs;
}

//KLUDGE: memory-leak
template<class T>
auto HashSet<T>::abegin () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<HashSet<T>*>(this),true));
}

//KLUDGE: memory-leak
template<class T>
auto HashSet<T>::aend () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<HashSet<T>*>(this),false));
}

template<class T>
auto HashSet<T>::begin () const -> HashSet<T>::Iterator {
  return Iterator(const_cast<HashSet<T>*>(this),true);
}

template<class T>
auto HashSet<T>::end () const -> HashSet<T>::Iterator {
  return Iterator(const_cast<HashSet<T>*>(this),false);
}

template<class T>
int HashSet<T>::hash_compress (const T& element) const {
	  int index = hash(element);
	  int new_index = std::abs(index % bins);
	  return new_index;
}

template<class T>
void HashSet<T>::ensure_load_factor(int new_used) {
	   load_factor = (double)used/(double)bins;
	   double new_lf = (double)new_used/(double)bins;
		if ( new_lf >= 1){
			HashSet<T> old_set(*this); //save old map
			set = new LN*[bins*2];
			bins *= 2;
			for (int i = 0; i < bins; i++){set[i] = new LN();} // array of trailer nodes

			for (int i = 0; i < old_set.bins ; i++){ //look through bins and put them in their new indexes in the new array
				for(auto l_pointer = old_set.set[i] ; l_pointer->next != nullptr ; l_pointer = l_pointer->next){
					int index = hash_compress(l_pointer->value);
					this->set[index] = new LN(l_pointer->value, set[index]);
				}
			}
		}
}

template<class T>
typename HashSet<T>::LN* HashSet<T>::find_element (int bin, const T& element) const {
  for (LN* c = set[bin]; c->next!=nullptr; c=c->next){
    if (element == c->value)
      return c;
  }
  return nullptr;
}

template<class T>
typename HashSet<T>::LN* HashSet<T>::copy_list (LN* l) const {
		auto new_list = new LN(l->value, l->next); // trailer
		return new_list;
}

template<class T>
typename HashSet<T>::LN** HashSet<T>::copy_hash_table (LN** ht, int bins) const {
	LN** new_map = new LN*[bins];
	for (int i = 0; i < bins; i++){
	   new_map[i] = copy_list(ht[i]);
  }
	return new_map;
}

template<class T>
void HashSet<T>::delete_hash_table (LN**& ht, int bins) {
	  for (auto i = 0; i < bins; ++i) {
		  delete ht[i];
	  }
	  delete[] ht;
}


template<class T>
void HashSet<T>::Iterator::advance_cursors(){
	if (current.second->next == nullptr){
		int i = current.first;
		i++;
		while (i < ref_set->bins){
			if (ref_set->set[i]->next != nullptr){
				current = ics::make_pair(i, ref_set->set[i]);
				return;
			}
			else if (i == ref_set->bins -1 and ref_set->set[i]->next == nullptr){
				current.first = -1;
				current.second = nullptr;
				return;
			}
			i++;
		}
	}
	if (current.first == ref_set->bins-1 and current.second->next->next == nullptr){
		current.first = -1;
		current.second = nullptr;
		return;
	}
	else{
		for (int i = current.first; i < ref_set->bins; i++){
			for (auto j = ref_set->set[i]; j->next != nullptr; j = j->next){
				if (j->value == current.second->value and j->next->next != nullptr){
					current = ics::make_pair(i, j->next);
					return;
				}
				else if ((j->value == current.second->value and j->next->next == nullptr)){
					i++;
					while (i < ref_set->bins){
						if (ref_set->set[i]->next != nullptr){
							current = ics::make_pair(i, ref_set->set[i]);
							return;
						}
						else if (i == ref_set->bins -1 and ref_set->set[i]->next == nullptr){
							current.first = -1;
							current.second = nullptr;
							return;
						}
						i++;
					}
				}
			}
		}
	}
}

template<class T>
HashSet<T>::Iterator::Iterator(HashSet<T>* fof, bool begin) : ref_set(fof) {
  expected_mod_count = ref_set->mod_count;
  if (begin){
	  if (ref_set->bins == 1 and ref_set->set[0]->next == nullptr){
		  current.first = -1;
		  current.second = nullptr;
		  return;
	  }
	  for (int i = 0; i < ref_set->bins; i++){
		  for (auto j = ref_set->set[i]; j->next != nullptr; j = j->next){
			  current = ics::make_pair(i, j);
			  return;
		  }
	  }
  }
  else{
	  current.first = -1;
	  current.second = nullptr;
  }
}

template<class T>
HashSet<T>::Iterator::Iterator(const Iterator& i) :
    current(i.current), ref_set(i.ref_set), expected_mod_count(i.expected_mod_count), can_erase(i.can_erase) {}

template<class T>
HashSet<T>::Iterator::~Iterator() {}

template<class T>
T HashSet<T>::Iterator::erase() {
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("HashSet::Iterator::erase Iterator cursor already erased");
  if (current.second == nullptr or current.first == -1)
    throw CannotEraseError("HashSet::Iterator::erase Iterator cursor beyond data structure");


  can_erase = false;
  T to_return = current.second->value;
  if (current.second->next->next == nullptr){
	  ref_set->erase(current.second->value);
	  expected_mod_count = ref_set->mod_count;
	  advance_cursors();
  }
  else{
	  ref_set->erase(current.second->value);
	  expected_mod_count = ref_set->mod_count;
  }
  return to_return;
}

template<class T>
std::string HashSet<T>::Iterator::str() const {
  std::ostringstream s;
  s << "current.first = " << current.first << " current.second->value = ";
  if (current.second == nullptr or current.second->next == nullptr)
	  s << "nullptr";
  else
	  s << current.second->value;
  return s.str();
}

template<class T>
const ics::Iterator<T>& HashSet<T>::Iterator::operator ++ () {
	//std::cout << "in the first ++" << std::endl;
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator ++");

  if (can_erase and current.first != -1 and current.first < ref_set->bins){
	  //std::cout << "calling" << std::endl;
	  advance_cursors();
  }
  can_erase = true;
  return *this;
}

//KLUDGE: creates garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& HashSet<T>::Iterator::operator ++ (int) {
	//std::cout << "in the second ++" << std::endl;
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator ++(int)");

  Iterator* to_return = new Iterator(this->ref_set,true);
  if (can_erase and current.first != -1 and current.first < ref_set->bins) {
    to_return->advance_cursors();
    advance_cursors();
  }
  can_erase = true;
  return *to_return;
}

template<class T>
bool HashSet<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashSet::Iterator::operator ==");
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator ==");
  if (ref_set != rhsASI->ref_set)
    throw ComparingDifferentIteratorsError("HashSet::Iterator::operator ==");

  if (current.first == rhsASI->current.first)
	  return (current.second) == (rhsASI->current.second);
  return current.first == rhsASI->current.first;
}


template<class T>
bool HashSet<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashSet::Iterator::operator !=");
  if (expected_mod_count != ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator !=");
  if (ref_set != rhsASI->ref_set)
    throw ComparingDifferentIteratorsError("HashSet::Iterator::operator !=");

  if (current.first == rhsASI->current.first){
	  return (current.second) != (rhsASI->current.second);
  }
  return current.first != rhsASI->current.first;
}

template<class T>
T& HashSet<T>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashSet::Iterator::operator * Iterator illegal: exhausted");

  return current.second->value;
}

template<class T>
T* HashSet<T>::Iterator::operator ->() const {
  if (expected_mod_count !=
      ref_set->mod_count)
    throw ConcurrentModificationError("HashSet::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashSet::Iterator::operator * Iterator illegal: exhausted");

  return (&(current.second->value));
}

}

#endif /* HASH_SET_HPP_ */
