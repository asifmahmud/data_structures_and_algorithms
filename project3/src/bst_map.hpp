//# Asif Mahmud, 70556749
//# Kim Le, 91394901
//# We certify that we worked cooperatively on this programming
//#   assignment, according to the rules for pair programming


#ifndef BST_MAP_HPP_
#define BST_MAP_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "pair.hpp"
#include "map.hpp"
#include "array_queue.hpp"   //For traversal


namespace ics {

template<class KEY,class T> class BSTMap : public Map<KEY,T>	{
  public:
    typedef ics::pair<KEY,T> Entry;
	  BSTMap();
	  BSTMap(const BSTMap<KEY,T>& to_copy);
	  //BSTMap(std::initializer_list<Entry> il);
    BSTMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop);
	  virtual ~BSTMap();

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
    virtual BSTMap<KEY,T>& operator = (const BSTMap<KEY,T>& rhs);
    virtual bool operator == (const Map<KEY,T>& rhs) const;
    virtual bool operator != (const Map<KEY,T>& rhs) const;

    template<class KEY2,class T2>
    friend std::ostream& operator << (std::ostream& outs, const BSTMap<KEY2,T2>& m);

    virtual ics::Iterator<Entry>& abegin () const;
    virtual ics::Iterator<Entry>& aend   () const;

   private:
     class TN;

   public:
     class Iterator : public ics::Iterator<Entry> {
       public:
        //KLUDGE should be callable only in begin/end
        Iterator(BSTMap<KEY,T>* fof, bool begin);
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
        ics::ArrayQueue<Entry> it;          //Iterator (as Stack) for Map
        BSTMap<KEY,T>*         ref_map;
        int                    expected_mod_count;
        bool                   can_erase = true;
    };

    virtual Iterator begin () const;
    virtual Iterator end   () const;
    //KLUDGE: define
    //virtual ics::Iterator<KEY>&  begin_key   () const;
    //virtual ics::Iterator<KEY>&  end_key     () const;
    //virtual ics::Iterator<T>&    begin_value () const;
    //virtual ics::Iterator<T>&    end_value   () const;

    private:
      class TN {
        public:
          TN ()                     : left(nullptr), right(nullptr){}
          TN (const TN& tn)         : value(tn.value), left(tn.left), right(tn.right){}
          TN (Entry v, TN* l = nullptr,
                       TN* r = nullptr) : value(v), left(l), right(r){}

          Entry value;
          TN*   left;
          TN*   right;
      };

      TN* map       = nullptr;
      int used      = 0; //Amount of array used
      int mod_count = 0; //For sensing concurrent modification
      TN*  find_key      (TN*  root, const KEY& key) const;
      TN*  value_find    (TN*  root, const KEY& key) const;
      bool key_find      (TN*  root, const KEY& key) const;
      bool find_value    (TN*  root, const T& value) const;
      T&   insert        (TN*& root, const KEY& key, const T& value);
      T&   index_insert  (TN*& root, const Entry entry);
      ics::pair<KEY,T> remove_closest(TN*& root);
      T    remove        (TN*& root, const KEY& key);
      TN*  copy          (TN*  root)                 const;
      void copy_to_queue (TN* root, ArrayQueue<Entry>& q) const;
      void delete_BST    (TN*& root);
      bool equals        (TN*  root, const Map<KEY,T>& other) const;
      void change_value  (TN*&  root, const KEY& key, const T& value);
      std::string string_rotated(TN* root, std::string indent) const;
  };


template<class KEY,class T>
BSTMap<KEY,T>::BSTMap() {}

template<class KEY,class T>
BSTMap<KEY,T>::BSTMap(const BSTMap<KEY,T>& to_copy) : used(to_copy.used) {
	map = copy(to_copy.map);
}

template<class KEY,class T>
BSTMap<KEY,T>::BSTMap(ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop) {
  this->put(start, stop);
}

//KLUDGE: Not usable yet
//template<class KEY,class T>
//BSTMap<KEY,T>::BSTMap(std::initializer_list<Entry> i) {
//  map = new T[length];
//  put(il.abegin(),il.aend());
//}

template<class KEY,class T>
BSTMap<KEY,T>::~BSTMap() {
	delete_BST(map);
}


template<class KEY,class T>
inline bool BSTMap<KEY,T>::empty() const {
	return used == 0;
}

template<class KEY,class T>
int BSTMap<KEY,T>::size() const {
  return used;
}

template<class KEY,class T>
bool BSTMap<KEY,T>::has_key (const KEY& element) const {
	return key_find(map, element);

}

template<class KEY,class T>
bool BSTMap<KEY,T>::has_value (const T& element) const {
	return find_value(map, element);
}

template<class KEY,class T>
std::string BSTMap<KEY,T>::str() const {

}

template <class KEY, class T>
void BSTMap<KEY,T>::change_value(TN*& root, const KEY& key, const T& value) {
	if (root == nullptr) {
		return;
	}
	else if (root->value.first == key){
		root->value.second = value;
	}
	else {
		change_value(root->left, key, value);
		change_value(root->right, key, value);
	}
}


template<class KEY,class T>
T BSTMap<KEY,T>::put(const KEY& key, const T& value) {
	if (!this->has_key(key)){
		used++;
		mod_count++;
		return insert(this->map, key, value);
	}
	T to_return;
	auto node = value_find(map, key);
	if (node != nullptr) {
		to_return = node->value.second;
	}
	change_value(map, key, value);
	mod_count++;
	return to_return;
}

template<class KEY,class T>
T BSTMap<KEY,T>::erase(const KEY& key) {
  T to_return = remove(map, key);
  used--;
  mod_count--;
  return to_return;
}

template<class KEY,class T>
void BSTMap<KEY,T>::clear() {
  map = nullptr;
  used = 0;
}

template<class KEY,class T>
int BSTMap<KEY,T>::put (ics::Iterator<Entry>& start, const ics::Iterator<Entry>& stop) {
	  int count = 0;
	  for (; start != stop; ++start) {
	    ++count;
	    put(start->first,start->second);
	  }
	  return count;
}

template<class KEY,class T>
T& BSTMap<KEY,T>::operator [] (const KEY& key) {
	if (has_key(key)){
		auto node = value_find(map, key);
		return node->value.second;
	}
	used++;
	mod_count++;
	insert(map, key, T());
	auto node = value_find(map, key);
	return node->value.second;

}

template<class KEY,class T>
const T& BSTMap<KEY,T>::operator [] (const KEY& key) const {
	auto node = value_find(map, key);
	if (node != nullptr){
		return node->value.second;
	}
  std::ostringstream answer;
  answer << "ArrayMap::operator []: key(" << key << ") not in Map";
  throw KeyError(answer.str());
}

template<class KEY,class T>
bool BSTMap<KEY,T>::operator == (const Map<KEY,T>& rhs) const {
  if (this == &rhs){
	  return true;
  }
  else if (this->empty() and rhs.empty()){
	  return true;
  }
  else if (used != rhs.size()){
	  return false;
  }
  else {
	  return equals(this->map, rhs);
  }
}

template<class KEY,class T>
BSTMap<KEY,T>& BSTMap<KEY,T>::operator = (const BSTMap<KEY,T>& rhs) {
  if (this == &rhs)
	return *this;
  used = rhs.used;
  map = copy(rhs.map);
  mod_count++;
  return *this;
}

template<class KEY,class T>
bool BSTMap<KEY,T>::operator != (const Map<KEY,T>& rhs) const {
  return !(*this == rhs);
}

template<class KEY,class T>
std::string BSTMap<KEY,T>::string_rotated(TN* root, std::string indent) const {
	std::ostringstream os;
  if (root == nullptr) {
	  return "";
  }
  else {
	   os << root->value.first << "->"
		  << root->value.second << ","
		  << string_rotated(root->left, "")
		  << string_rotated(root->right, "");
  }
  return os.str();
}

template<class KEY,class T>
std::ostream& operator << (std::ostream& outs, const BSTMap<KEY,T>& m) {
  if (m.empty()) {
	outs << "map[]";
  }else{
	outs << "map[";
	std::string s = m.string_rotated(m.map, "");
	s[s.length()-1] = ']';
	outs << s;
  }
  return outs;
}

//KLUDGE: memory-leak
template<class KEY,class T>
auto BSTMap<KEY,T>::abegin () const -> ics::Iterator<Entry>& {
  return *(new Iterator(const_cast<BSTMap<KEY,T>*>(this),true));
}

//KLUDGE: memory-leak
template<class KEY,class T>
auto BSTMap<KEY,T>::aend () const -> ics::Iterator<Entry>& {
  return *(new Iterator(const_cast<BSTMap<KEY,T>*>(this),false));
}

template<class KEY,class T>
auto BSTMap<KEY,T>::begin () const -> BSTMap<KEY,T>::Iterator {
  return Iterator(const_cast<BSTMap<KEY,T>*>(this),true);
}

template<class KEY,class T>
auto BSTMap<KEY,T>::end () const -> BSTMap<KEY,T>::Iterator {
  return Iterator(const_cast<BSTMap<KEY,T>*>(this),false);
}

template <class KEY, class T>
bool BSTMap<KEY, T>::key_find (TN*  root, const KEY& key) const{
	if (root == nullptr)
		return false;
	else if (root->value.first == key)
		return true;
	else
		return key_find(root->left, key) or key_find(root->right, key);
}


template<class KEY,class T>
typename BSTMap<KEY,T>::TN* BSTMap<KEY,T>::find_key (TN* root, const KEY& key) const {

}

template <class KEY, class T>
typename BSTMap<KEY, T>::TN* BSTMap<KEY, T>::value_find(TN* root, const KEY& key) const{
	if (root == nullptr){
		return nullptr;
	}
	else if (root->value.first == key)
		return root;
	else if (value_find(root->left, key) == nullptr)
		return value_find(root->right, key);
	else
		return value_find(root->left, key);

}

template<class KEY,class T>
bool BSTMap<KEY,T>::find_value (TN* root, const T& value) const {
	if (root == nullptr) { return false;}
	else if (root->value.second == value)
		return true;
	else
		find_value(root->left, value) or find_value(root->right, value);
}


template<class KEY,class T>
T& BSTMap<KEY,T>::insert (TN*& root, const KEY& key, const T& value) {
	T val = value;
	if (root == nullptr) {
		root = new TN(ics::make_pair(key, value));
	}
	else {
		if (key > root->value.first) {
			insert(root->right, key, value);
		}
		else if (key < root->value.first){
			insert(root->left, key, value);
		}
	}
	return val;
}

template <class KEY, class T>
T& BSTMap<KEY, T>::index_insert (TN*& root, const Entry entry){
	T val = entry.second;
	if (root == nullptr) {
		root = new TN(entry);
	}
	else {
		if (entry.first > root->value.first) {
			index_insert(root->right, entry);
		}
		else if (entry.first < root->value.first){
			index_insert(root->left, entry);
		}
	}
	return val;
}

template<class KEY,class T>
ics::pair<KEY,T> BSTMap<KEY,T>::remove_closest(TN*& root) {
  if (root->right == nullptr) {
    ics::pair<KEY,T> to_return = root->value;
    TN* to_delete = root;
    root = root->left;
    delete to_delete;
    return to_return;
  }else
    return remove_closest(root->right);
}

template<class KEY,class T>
T BSTMap<KEY,T>::remove (TN*& root, const KEY& key) {
  if (root == nullptr) {
    std::ostringstream answer;
    answer << "BSTMap::erase: key(" << key << ") not in Map";
    throw KeyError(answer.str());
  }else
    if (key == root->value.first) {
      T to_return = root->value.second;
      if (root->left == nullptr) {
        TN* to_delete = root;
        root = root->right;
        delete to_delete;
      }else if (root->right == nullptr) {
        TN* to_delete = root;
        root = root->left;
        delete to_delete;
      }else
        root->value = remove_closest(root->left);
      return to_return;
    }else
      return remove( (key < root->value.first ? root->left : root->right), key);
}

template<class KEY,class T>
typename BSTMap<KEY,T>::TN* BSTMap<KEY,T>::copy (TN* root) const {
	TN* t;
	if (root == nullptr){
		return nullptr;
	}
	else {
		t = new TN(root->value);
		t->left = copy(root->left);
		t->right = copy(root->right);
	}
	return t;
}

template<class KEY,class T>
void BSTMap<KEY,T>::copy_to_queue (TN* root, ArrayQueue<Entry>& q) const {
  if (root == nullptr){
	  return;
  }
  else {
	  q.enqueue(root->value);
	  copy_to_queue(root->left, q);
	  copy_to_queue(root->right, q);
  }
}



template<class KEY,class T>
void BSTMap<KEY,T>::delete_BST (TN*& root) {
	if (root == nullptr) {
		return;
	}
	else {
		delete root;
		delete_BST(root->left);
		delete_BST(root->right);
	}
}


template<class KEY,class T>
bool BSTMap<KEY,T>::equals (TN*  root, const Map<KEY,T>& other) const {
	if (root == nullptr) {
		return (root == nullptr);
	}
	else {
		return 	(other[root->value.first] == root->value.second) &&
				(equals(root->left, other)) &&
				(equals(root->right, other));
	}
}

template<class KEY,class T>
BSTMap<KEY,T>::Iterator::Iterator(BSTMap<KEY,T>* fof, bool begin) : it(), ref_map(fof){
  expected_mod_count = ref_map->mod_count;
  ics::ArrayQueue<Entry> q;
  ref_map->copy_to_queue(ref_map->map, q);
  if (begin) {
	  it = q;
  }
  else {
	  while (!q.empty()){
		  q.dequeue();
	  }
	  it = q;
  }
}

template<class KEY,class T>
BSTMap<KEY,T>::Iterator::Iterator(const Iterator& i) :
    it(i.it), ref_map(i.ref_map), expected_mod_count(i.expected_mod_count), can_erase(i.can_erase)
{}

//KLUDGE: must define in .hpp
//template<class KEY,class T>
//BSTMap<KEY,T>::Iterator::~Iterator() {}

template<class KEY,class T>
auto BSTMap<KEY,T>::Iterator::erase() -> Entry {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("BSTMap::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("BSTMap::Iterator::erase Iterator cursor already erased");
  if (it.empty())
    throw CannotEraseError("BSTMap::Iterator::erase Iterator cursor beyond data structure");

  can_erase = false;
  auto to_return = it.dequeue();
  ref_map->erase(to_return.first);
  expected_mod_count = ref_map->mod_count;
  return to_return;
}

template<class KEY,class T>
std::string BSTMap<KEY,T>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_map->str() << "(expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}

//KLUDGE: cannot use Entry
template<class KEY,class T>
auto  BSTMap<KEY,T>::Iterator::operator ++ () -> const ics::Iterator<ics::pair<KEY,T>>& {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("BSTMap::Iterator::operator ++");
  if (can_erase && !it.empty()){
	  //ref_map->remove(ref_map->map, ref_map->map->value.first);
  	  it.dequeue();
  }
  can_erase = true;
  return *this;
}

//KLUDGE: creates garbage! (can return local value!)
template<class KEY,class T>
auto BSTMap<KEY,T>::Iterator::operator ++ (int) -> const ics::Iterator<ics::pair<KEY,T>>&{
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("BSTMap::Iterator::operator ++(int)");

  Iterator* to_return = new Iterator(this->ref_map,true);
  if (can_erase && !it.empty()) {
	  to_return->it.dequeue();
	  //ref_map->erase(ref_map->map->value.first);
//    to_return->it.dequeue();
    it.dequeue();
  }
  can_erase = true;
  return *to_return;
}

template<class KEY,class T>
bool BSTMap<KEY,T>::Iterator::operator == (const ics::Iterator<Entry>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("BSTMap::Iterator::operator ==");
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("BSTMap::Iterator::operator ==");
  if (ref_map != rhsASI->ref_map)
    throw ComparingDifferentIteratorsError("BSTMap::Iterator::operator ==");

  //return ref_map->map->value.first == rhsASI->ref_map->map->value.first;
  return it.size() == rhsASI->it.size();
}


template<class KEY,class T>
bool BSTMap<KEY,T>::Iterator::operator != (const ics::Iterator<Entry>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("BSTMap::Iterator::operator !=");
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("BSTMap::Iterator::operator !=");
  if (ref_map != rhsASI->ref_map)
    throw ComparingDifferentIteratorsError("BSTMap::Iterator::operator !=");

  return it.size() != rhsASI->it.size();
}

template<class KEY,class T>
ics::pair<KEY,T>& BSTMap<KEY,T>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_map->mod_count)
    throw ConcurrentModificationError("BSTMap::Iterator::operator *");
  if (!can_erase || it.empty())
    throw IteratorPositionIllegal("BSTMap::Iterator::operator * Iterator illegal: exhausted");

  return it.peek();
}

template<class KEY,class T>
ics::pair<KEY,T>* BSTMap<KEY,T>::Iterator::operator ->() const {
  if (expected_mod_count != ref_map->mod_count)
    throw ConcurrentModificationError("BSTMap::Iterator::operator *");
  if (!can_erase || it.empty())
    throw IteratorPositionIllegal("BSTMap::Iterator::operator -> Iterator illegal: exhausted");

  return &(it.peek());
}

}

#endif /* BST_MAP_HPP_ */
