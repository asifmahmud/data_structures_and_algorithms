//# Asif Mahmud, 70556749
//# Kim Le, 91394901
//# We certify that we worked cooperatively on this programming
//#   assignment, according to the rules for pair programming

#ifndef HASH_MAP_HPP_
#define HASH_MAP_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <complex>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "pair.hpp"
#include "map.hpp"
#include "array_queue.hpp"   //For traversal


namespace ics {

template<class KEY,class T> class HashMap : public Map<KEY,T>	{
  public:
    typedef ics::pair<KEY,T> Entry;
    HashMap() = delete;
    HashMap(int (*ahash)(const KEY& k), double the_load_factor = 1.0);
    HashMap(int initial_bins, int (*ahash)(const KEY& k), double the_load_factor = 1.0);
	  HashMap(const HashMap<KEY,T>& to_copy);
	  //HashMap(std::initializer_list<Entry> il, int (*ahash)(const KEY& k), double the_load_factor = 1.0);
    HashMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop, int (*ahash)(const KEY& k), double the_load_factor = 1.0);
	  virtual ~HashMap();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual bool has_key    (const KEY& key) const;
    virtual bool has_value  (const T& value) const;
    virtual std::string str () const;

    virtual T    put   (const KEY& key, const T& value);
    virtual T    erase (const KEY& key);
    virtual void clear ();

    virtual int put   (ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop);

    virtual T&       operator [] (const KEY&);
    virtual const T& operator [] (const KEY&) const;
    virtual HashMap<KEY,T>& operator = (const HashMap<KEY,T>& rhs);
    virtual bool operator == (const Map<KEY,T>& rhs) const;
    virtual bool operator != (const Map<KEY,T>& rhs) const;

    template<class KEY2,class T2>
    friend std::ostream& operator << (std::ostream& outs, const HashMap<KEY2,T2>& m);

    virtual ics::Iterator<Entry>& abegin () const;
    virtual ics::Iterator<Entry>& aend   () const;

   private:
     class LN;

   public:
     class Iterator : public ics::Iterator<Entry> {
       public:
        //KLUDGE should be callable only in begin/end
        Iterator(HashMap<KEY,T>* fof, bool begin);
        Iterator(const Iterator& i);
        virtual ~Iterator() {}
        virtual Entry       erase();
        virtual std::string str  () const;
        virtual const ics::Iterator<Entry>& operator ++ ();
        virtual const ics::Iterator<Entry>& operator ++ (int);
        virtual bool operator == (const ics::Iterator<Entry>& rhs) const;
        virtual bool operator != (const ics::Iterator<Entry>& rhs) const;
        virtual Entry& operator *  () const;
        virtual Entry* operator -> () const;
      private:
        ics::pair<int,LN*> current; //Bin Index and Cursor; stop: LN* == nullptr
        HashMap<KEY,T>*    ref_map;
        int                expected_mod_count;
        bool               can_erase = true;
        void advance_cursors();
    };

    virtual Iterator begin () const;
    virtual Iterator end   () const;
    //KLUDGE: define
    //virtual ics::Iterator<KEY>&  begin_key   () const;
    //virtual ics::Iterator<KEY>&  end_key     () const;
    //virtual ics::Iterator<T>&    begin_value () const;
    //virtual ics::Iterator<T>&    end_value   () const;

    private:
      class LN {
        public:
          LN ()                         : next(nullptr){}
          LN (const LN& ln)             : value(ln.value), next(ln.next){}
          LN (Entry v, LN* n = nullptr) : value(v), next(n){}

          Entry value;
          LN*   next;
      };

      LN** map      = nullptr;
      int (*hash)(const KEY& k);
      double load_factor;//used/bins <= load_factor
      int bins      = 1; //# bins in array
      int used      = 0; //# of key->value pairs in the hash table
      int mod_count = 0; //For sensing concurrent modification
      int   hash_compress (const KEY& key) const;
      void  ensure_load_factor(int new_used);
      LN*   find_key (int bin, const KEY& key) const;
      bool  find_value (const T& value) const;
      LN*   copy_list(LN*   l) const;
      LN**  copy_hash_table(LN** ht, int bins) const;
      void  delete_hash_table(LN**& ht, int bins);
  };





template<class KEY,class T>
HashMap<KEY,T>::HashMap(int (*ahash)(const KEY& k), double the_load_factor) : hash(ahash), load_factor(the_load_factor) {
  map = new LN*[bins];
  for (int i = 0; i < bins; i++){
	  map[i] = new LN(); // array of trailer nodes
  }

}

template<class KEY,class T>
HashMap<KEY,T>::HashMap(int initial_bins, int (*ahash)(const KEY& k), double the_load_factor) : bins(initial_bins), hash(ahash), load_factor(the_load_factor) {
	  map = new LN*[bins];
	  for (int i = 0; i < bins; i++){
		  map[i] = new LN(); // array of trailer nodes
	  }
}

template<class KEY,class T>
HashMap<KEY,T>::HashMap(const HashMap<KEY,T>& to_copy) : hash(to_copy.hash), load_factor(to_copy.load_factor), bins(to_copy.bins), used(to_copy.used) {
  map = copy_hash_table(to_copy.map, bins);
}

template<class KEY,class T>
HashMap<KEY,T>::HashMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop, int (*ahash)(const KEY& k), double the_load_factor) : hash(ahash), load_factor(the_load_factor) {
	  map = new LN*[bins];
	  for (int i = 0; i < bins; i++){
		  map[i] = new LN(); // array of trailer nodes
	  }
	this->put(start, stop);
}

//KLUDGE: Not usable yet
//template<class KEY,class T>
//HashMap<KEY,T>::HashMap(std::initializer_list<Entry> i,int (*ahash)(const KEY& k), double the_load_factor = 1.0) {
//  map = new LN*[bins];
//  put(il.abegin(),il.aend());
//}

template<class KEY,class T>
HashMap<KEY,T>::~HashMap() {
	delete_hash_table (map, bins);
}


template<class KEY,class T>
inline bool HashMap<KEY,T>::empty() const {
  return used == 0;
}

template<class KEY,class T>
int HashMap<KEY,T>::size() const {
  return used;
}

template<class KEY,class T>
bool HashMap<KEY,T>::has_key (const KEY& key) const {
   for(int i = 0; i < bins ; i++){
	   for(auto l_pointer = map[i]; l_pointer->next != nullptr ; l_pointer = l_pointer->next){
		   if(key == l_pointer->value.first)
			   return true;
	   }
   }
   return false;
}

template<class KEY,class T>
bool HashMap<KEY,T>::has_value (const T& value) const {
	for(int i = 0; i < bins ; i++){
		   for(auto l_pointer = map[i]; l_pointer->next != nullptr ; l_pointer = l_pointer->next){
			   if(value == l_pointer->value.second)
				   return true;
		   }
	   }
	   return false;
}

template<class KEY,class T>
std::string HashMap<KEY,T>::str() const {
	std::ostringstream answer;
	//print bins
//	for (int i = 0; i < bins ; i++ ){
//		answer << std::endl << "bin[" << i <<"]: ";
//		for(auto l_pointer = map[i] ; l_pointer != nullptr; l_pointer = l_pointer->next){
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
			answer << "map[]";
		}
		else{
			answer << "map[";
			for (int i = 0; i < bins; ++i){
				for (auto j = map[i]; j->next != nullptr; j = j->next){
					answer << j->value.first << "->" << j->value.second;
					answer << ",";
				}
			}
			std::string string = answer.str();
			string[string.length()-1] = ']';
			return string;
		}
   return answer.str();
}

template<class KEY,class T>
T HashMap<KEY,T>::put(const KEY& key, const T& value) {

  if(!has_key(key)){
	  ensure_load_factor(used+1); // return expanded array
	  int index_to_put = hash_compress(key);
	  map[index_to_put] =  new LN(ics::make_pair(key, value), map[index_to_put]);
	  used++;
	  mod_count++;
	  return value;
  }
  else {
	  int index_to_put = hash_compress(key);
	  T old_value = find_key(index_to_put, key)->value.second;
	  find_key(index_to_put, key)->value.second = value;
	  mod_count++;
	  return old_value;
  }
}

template<class KEY,class T>
T HashMap<KEY,T>::erase(const KEY& key) {
	for (int i = 0; i < bins; ++i) {
		for (LN* j = this->map[i]; j->next != nullptr; j = j->next){
			if (j->value.first == key) { // next is pointing to trailer node
				T to_return = j->value.second;
				j->value = j->next->value;
				j->next = j->next->next;
				used--;
				mod_count++;
				return to_return;
			}
		}
	}
	  std::ostringstream answer;
	  answer << "HashMap::operator []: key(" << key << ") not in Map";
	  throw KeyError(answer.str());
}

template<class KEY,class T>
void HashMap<KEY,T>::clear() {
	//delete_hash_table(map, bins);
  map = new LN*[bins];
  used = 0;
  mod_count++;
  for (int i = 0; i < bins; i++){
  	  map[i] = new LN(); // array of trailer nodes
    }
}

template<class KEY,class T>
int HashMap<KEY,T>::put (ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop) {
	  int count = 0;
	  for (;start != stop; ++start){
		  this->put(start->first, start->second);
		  count++;
	  }
	  return count;
}

template<class KEY,class T>
const T& HashMap<KEY,T>::operator [] (const KEY& key) const { // retrieve the value
	if(has_key(key)){
		int index_to_put = hash_compress(key);
		const T& value = find_key(index_to_put, key)->value.second;
		return value;
	}
	  std::ostringstream answer;
	  answer << "HashMap::operator []: key(" << key << ") not in Map";
	  throw KeyError(answer.str());
}

template<class KEY,class T>
T& HashMap<KEY,T>::operator [] (const KEY& key) { // put value in it
	//std::cout << "putting []" << std::endl;
	if(has_key(key)){
		int index_to_put = hash_compress(key);
		return find_key(index_to_put, key)->value.second;
	}
	else{
		//std::cout << "here" << std::endl;
		put(key, T());
		int index_to_put = hash_compress(key);
		return find_key(index_to_put,key)->value.second;
	}
}



template<class KEY,class T>
bool HashMap<KEY,T>::operator == (const Map<KEY,T>& rhs) const {
	 if (this == &rhs)
	    return true;
	 if (used != rhs.size())
	    return false;
	  for (int i = 0; i < this->bins; i++){
		  for (LN* l_ptr = this->map[i]; l_ptr->next != nullptr; l_ptr = l_ptr->next){ // skip trailer nodes
			  if (rhs.has_key(l_ptr->value.first) and l_ptr->value.second != rhs[l_ptr->value.first])
				  return false;
		  }
	  }
	  return true;
}

template<class KEY,class T>
HashMap<KEY,T>& HashMap<KEY,T>::operator = (const HashMap<KEY,T>& rhs) {
	 if (this == &rhs)
	    return *this;

	 this->clear();
	 this->put(rhs.abegin(), rhs.aend());
	 ++mod_count;
	 return *this;
}

template<class KEY,class T>
bool HashMap<KEY,T>::operator != (const Map<KEY,T>& rhs) const {
	return !(*this == rhs);
}


template<class KEY,class T>
std::ostream& operator << (std::ostream& outs, const HashMap<KEY,T>& m) {

	outs << m.str();
	return outs;
}

//KLUDGE: memory-leak
template<class KEY,class T>
auto HashMap<KEY,T>::abegin () const -> ics::Iterator<Entry>& {
  return *(new Iterator(const_cast<HashMap<KEY,T>*>(this),true));
}

//KLUDGE: memory-leak
template<class KEY,class T>
auto HashMap<KEY,T>::aend () const -> ics::Iterator<Entry>& {
  return *(new Iterator(const_cast<HashMap<KEY,T>*>(this),false));
}

template<class KEY,class T>
auto HashMap<KEY,T>::begin () const -> HashMap<KEY,T>::Iterator {
  return Iterator(const_cast<HashMap<KEY,T>*>(this),true);
}

template<class KEY,class T>
auto HashMap<KEY,T>::end () const -> HashMap<KEY,T>::Iterator {
  return Iterator(const_cast<HashMap<KEY,T>*>(this),false);
}

template<class KEY,class T>
int HashMap<KEY,T>::hash_compress (const KEY& key) const {
  int index = hash(key);
  int new_index = std::abs(index % bins);
  return new_index;
}

template<class KEY,class T>
void HashMap<KEY,T>::ensure_load_factor(int new_used) {
   load_factor = (double)used/(double)bins;
   double new_lf = (double)new_used/(double)bins;
	if ( new_lf >= 1){
		HashMap<KEY,T> old_map(*this); //save old map
		map = new LN*[bins*2];
		bins *= 2;
		for (int i = 0; i < bins; i++){map[i] = new LN();} // array of trailer nodes

		for (int i = 0; i < old_map.bins ; i++){ //look through bins and put them in their new indexes in the new array
			for(auto l_pointer = old_map.map[i] ; l_pointer->next != nullptr ; l_pointer = l_pointer->next){
				int index = hash_compress(l_pointer->value.first);

				this->map[index] = new LN(ics::make_pair(l_pointer->value.first, l_pointer->value.second), map[index]);

			}
		}
	}
}

template<class KEY,class T>
typename HashMap<KEY,T>::LN* HashMap<KEY,T>::find_key (int bin, const KEY& key) const {
  for (LN* i = this->map[bin]; i->next != nullptr; i = i->next){
	  if (key == i->value.first){
		  return i;
	  }
  }
  return nullptr;
}

template<class KEY,class T>
bool HashMap<KEY,T>::find_value (const T& value) const {
	for(int i = 0; i < bins ; i++){
	  for (auto l = map[i]; l != nullptr; l = l->next){
		  if (value == l->value.second){
			  return true;
		  }
	  }
	}
	  return false;
}

template<class KEY,class T>
typename HashMap<KEY,T>::LN* HashMap<KEY,T>::copy_list (LN* l) const {
	auto new_list = new LN(l->value, l->next); // trailer
	return new_list;
}

template<class KEY,class T>
typename HashMap<KEY,T>::LN** HashMap<KEY,T>::copy_hash_table (LN** ht, int bins) const {
	LN** new_map = new LN*[bins];
	for (int i = 0; i < bins; i++){
	   new_map[i] = copy_list(ht[i]);
  }
	return new_map;
}

template<class KEY,class T>
void HashMap<KEY,T>::delete_hash_table (LN**& ht, int bins) {
  for (auto i = 0; i < bins; ++i) {
	  delete ht[i];
  }
  delete[] ht;
}


template<class KEY,class T>
void HashMap<KEY,T>::Iterator::advance_cursors(){
	if (current.second->next == nullptr){
		int i = current.first;
		i++;
		while (i < ref_map->bins){
			if (ref_map->map[i]->next != nullptr){
				current = ics::make_pair(i, ref_map->map[i]);
				return;
			}
			else if (i == ref_map->bins -1 and ref_map->map[i]->next == nullptr){
				current.first = -1;
				current.second = nullptr;
				return;
			}
			i++;
		}
	}
	if (current.first == ref_map->bins-1 and current.second->next->next == nullptr){
		current.first = -1;
		current.second = nullptr;
		return;
	}
	else{
		for (int i = current.first; i < ref_map->bins; i++){
			for (auto j = ref_map->map[i]; j->next != nullptr; j = j->next){
				if (j->value == current.second->value and j->next->next != nullptr){
					current = ics::make_pair(i, j->next);
					return;
				}
				else if ((j->value == current.second->value and j->next->next == nullptr)){
					i++;
					while (i < ref_map->bins){
						if (ref_map->map[i]->next != nullptr){
							current = ics::make_pair(i, ref_map->map[i]);
							return;
						}
						else if (i == ref_map->bins -1 and ref_map->map[i]->next == nullptr){
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

template<class KEY,class T>
HashMap<KEY,T>::Iterator::Iterator(HashMap<KEY,T>* fof, bool begin) : ref_map(fof) {
	expected_mod_count = ref_map->mod_count;
	  if (begin){
		  if (ref_map->bins == 1 and ref_map->map[0]->next == nullptr){
			  current.first = -1;
			  current.second = nullptr;
			  return;
		  }
		  for (int i = 0; i < ref_map->bins; i++){
			  for (auto j = ref_map->map[i]; j->next != nullptr; j = j->next){
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

template<class KEY,class T>
HashMap<KEY,T>::Iterator::Iterator(const Iterator& i) :
    current(i.current), ref_map(i.ref_map), expected_mod_count(i.expected_mod_count), can_erase(i.can_erase) {}

//KLUDGE: must define in .hpp
//template<class KEY,class T>
//HashMap<KEY,T>::Iterator::~Iterator() {}

template<class KEY,class T>
auto HashMap<KEY,T>::Iterator::erase() -> Entry {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("HashMap::Iterator::erase Iterator cursor already erased");
  if (current.second == nullptr)
    throw CannotEraseError("HashMap::Iterator::erase Iterator cursor beyond data structure");

  can_erase = false;
  Entry to_return = current.second->value;
  if (current.second->next->next == nullptr){
	  ref_map->erase(current.second->value.first);
	  expected_mod_count = ref_map->mod_count;
	  advance_cursors();
  }
  else{
	  ref_map->erase(current.second->value.first);
	  expected_mod_count = ref_map->mod_count;
  }
  return to_return;
}

template<class KEY,class T>
std::string HashMap<KEY,T>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_map->str() << "(current=" << current.first << "/" << current.second->value.first << ":" << current.second->value.second << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}

//KLUDGE: cannot use Entry
template<class KEY,class T>
auto  HashMap<KEY,T>::Iterator::operator ++ () -> const ics::Iterator<ics::pair<KEY,T>>& {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator ++");

  if (can_erase and current.first != -1 and current.first < ref_map->bins){
	  //std::cout << "calling" << std::endl;
	  advance_cursors();
  }
  can_erase = true;
  return *this;
}

//KLUDGE: creates garbage! (can return local value!)
template<class KEY,class T>
auto HashMap<KEY,T>::Iterator::operator ++ (int) -> const ics::Iterator<ics::pair<KEY,T>>&{
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator ++(int)");

  Iterator* to_return = new Iterator(this->ref_map,true);
  if (can_erase and current.first != -1 and current.first < ref_map->bins) {
    to_return->advance_cursors();
    advance_cursors();
  }
  can_erase = true;
  return *to_return;
}

template<class KEY,class T>
bool HashMap<KEY,T>::Iterator::operator == (const ics::Iterator<Entry>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashMap::Iterator::operator ==");
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator ==");
  if (ref_map != rhsASI->ref_map)
    throw ComparingDifferentIteratorsError("HashMap::Iterator::operator ==");

  if (current.first == rhsASI->current.first)
	  return (current.second) == (rhsASI->current.second);
  return current.first == rhsASI->current.first;

}


template<class KEY,class T>
bool HashMap<KEY,T>::Iterator::operator != (const ics::Iterator<Entry>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("HashMap::Iterator::operator !=");
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator !=");
  if (ref_map != rhsASI->ref_map)
    throw ComparingDifferentIteratorsError("HashMap::Iterator::operator !=");

  if (current.first == rhsASI->current.first){
	  return (current.second) != (rhsASI->current.second);
  }
  return current.first != rhsASI->current.first;
}

template<class KEY,class T>
ics::pair<KEY,T>& HashMap<KEY,T>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashMap::Iterator::operator * Iterator illegal: exhausted");

  return current.second->value;
}

template<class KEY,class T>
ics::pair<KEY,T>* HashMap<KEY,T>::Iterator::operator ->() const {
  if (expected_mod_count !=
      ref_map->mod_count)
    throw ConcurrentModificationError("HashMap::Iterator::operator *");
  if (!can_erase || current.second == nullptr)
    throw IteratorPositionIllegal("HashMap::Iterator::operator -> Iterator illegal: exhausted");

  return (&(current.second->value));

}

}
#endif /* HASH_MAP_HPP_ */
