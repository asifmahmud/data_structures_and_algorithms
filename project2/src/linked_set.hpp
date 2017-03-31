// Asif Mahmud, 70556749
// Kim Le, 91394901
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming:
//   primarily that both partners worked on all parts together.


#ifndef LINKED_SET_HPP_
#define LINKED_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "set.hpp"


namespace ics {

template<class T> class LinkedSet : public Set<T>	{
  public:
	  LinkedSet();
	  LinkedSet(const LinkedSet<T>& to_copy);
	  //KLUDGE: not usable yet. LinkedSet(std::initializer_list<T> il);
    LinkedSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop);
	  virtual ~LinkedSet();

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

    virtual LinkedSet<T>& operator = (const LinkedSet<T>& rhs);
    virtual bool operator == (const Set<T>& rhs) const;
    virtual bool operator != (const Set<T>& rhs) const;
    virtual bool operator <= (const Set<T>& rhs) const;
    virtual bool operator <  (const Set<T>& rhs) const;
    virtual bool operator >= (const Set<T>& rhs) const;
    virtual bool operator >  (const Set<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinkedSet<T2>& s);

    virtual ics::Iterator<T>& abegin () const;
    virtual ics::Iterator<T>& aend   () const;

  private:
    class LN;

  public:
    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(LinkedSet<T>* fof, LN* initial);
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
        LN*           current;  //if can_erase is false, this value is unusable
        LinkedSet<T>* ref_set;
        int           expected_mod_count;
        bool          can_erase = true;
    };

    virtual Iterator begin () const;
    virtual Iterator end   () const;

  private:
    class LN {
      public:
        LN ()                      : next(nullptr){}
        LN (const LN& ln)          : value(ln->value), next(ln->next){}
        LN (T v,  LN* n = nullptr) : value(v), next(n){}
        T   value;
        LN* next;
    };

    LN* front     = new LN();
    LN* trailer   = front;
    int used      = 0;             //Amount of array used
    int mod_count = 0;             //For sensing concurrent modification
    int erase_at(LN* p);
    void delete_list(LN*& front);  //Recycle storage, set front's argument to nullptr;
  };




//Insert constructor/methods here: see array_set.hpp
template<class T>
LinkedSet<T>::LinkedSet() {
}

template<class T>
LinkedSet<T>::LinkedSet(const LinkedSet<T>& to_copy) {
	for (auto i = to_copy.front; i != to_copy.trailer; i = i->next) {
		this->insert(i->value);
	}

}

template<class T>
LinkedSet<T>::LinkedSet(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  insert(start,stop);
}

template<class T>
LinkedSet<T>::~LinkedSet() {
  delete front;
  delete trailer;
}

template<class T>
inline bool LinkedSet<T>::empty() const {
  return used == 0;
}

template<class T>
int LinkedSet<T>::size() const {
  return used;
}

template<class T>
bool LinkedSet<T>::contains (const T& element) const {
  for (auto i = front; i != trailer; i = i->next )
    if (i->value == element)
      return true;

  return false;
}

template<class T>
std::string LinkedSet<T>::str() const {
  std::ostringstream answer;
  answer << *this << "used=" << used << ",mod_count=" << mod_count << ")";
  return answer.str();
}

template<class T>
bool LinkedSet<T>::contains(ics::Iterator<T>& start, const ics::Iterator<T>& stop) const {
  for (; start != stop; ++start)
    if (!contains(*start))
      return false;

  return true;
}

template<class T>
int LinkedSet<T>::insert(const T& element) {
  for (auto i = front; i != trailer; i = i->next)
    if (i->value == element)
      return 0;

  if (this->empty()) {
	  front = new LN(element, trailer);
  }
  else {
	  front->next = new LN(front->value, front->next);
	  front->value = element;
  }
  ++used;
  ++mod_count;
  return 1;
}

template<class T>
int LinkedSet<T>::erase(const T& element) {
	for (auto i = front; i != trailer; i = i->next) {
		if (i->value == element) {
			mod_count++;
			--used;
			return erase_at(i);
		}
	}
  return 0;
}

template<class T>
void LinkedSet<T>::clear() {
  front = trailer;
  used = 0;
  ++mod_count;
}

template<class T>
int LinkedSet<T>::insert(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  int count = 0;
  for (; start != stop; ++start)
    count += insert(*start);

  return count;
}

template<class T>
int LinkedSet<T>::erase(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  int count = 0;
  for (; start != stop; start++)
    count += erase(*start);
  return count;
}

template<class T>
int LinkedSet<T>::retain(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	LinkedSet<T> s(start, stop);
	int count = 0;
	for (auto i = front; i != trailer; i = i->next) {
		if (!(s.contains(i->value))){
			this->erase(i->value);
		}
		else {
			++count;
		}
	}
	return count;
}

template<class T>
LinkedSet<T>& LinkedSet<T>::operator = (const LinkedSet<T>& rhs) {
	if (this == &rhs) {
	  ++mod_count;
	return *this;
	}
	else {
	  this->clear();
	  for (auto i = rhs.front; i != rhs.trailer; i = i->next) {
		  this->insert(i->value);
	  }
	  ++mod_count;
	  return *this;
	}
}

template<class T>
bool LinkedSet<T>::operator == (const Set<T>& rhs) const {
	if (this == &rhs)
		return true;
	if (this->empty() && rhs.empty()){
		return true;
	}
	if (used != rhs.size())
		return false;
  for (auto i = front; i != trailer; i = i->next) {
	  if (!(rhs.contains(i->value))) {
		  return false;
	  }
  }
  return true;
}

template<class T>
bool LinkedSet<T>::operator != (const Set<T>& rhs) const {
  return !(*this == rhs);
}

template<class T>
bool LinkedSet<T>::operator <= (const Set<T>& rhs) const {
  if (this == &rhs)
    return true;
  if (used > rhs.size())
    return false;
  for (auto i = front; i != trailer; i = i->next) {
	  if (!(rhs.contains(i->value))) {
		  return false;
	  }
  }
  return true;
}

template<class T>
bool LinkedSet<T>::operator < (const Set<T>& rhs) const {
  if (this == &rhs)
    return false;
  if (used >= rhs.size())
    return false;
  for (auto i = front; i != trailer; i = i->next) {
	  if (!(rhs.contains(i->value))) {
		  return false;
	  }
  }
  return true;
}

template<class T>
bool LinkedSet<T>::operator >= (const Set<T>& rhs) const {
  return rhs <= *this;
}

template<class T>
bool LinkedSet<T>::operator > (const Set<T>& rhs) const {
  return rhs < *this;
}

template<class T>
std::ostream& operator << (std::ostream& outs, const LinkedSet<T>& s) {
  if (s.empty()) {
    outs << "set[]";
  }else{
    outs << "set[";
    for (auto i = s.front; i != s.trailer; i= i->next) {
    	outs << i->value;
		if(i->next != s.trailer){
			outs << ",";
		}
    }
    outs <<"]";
  }
  return outs;
}


//Include code for abegin/aend/begin/end here:
// (see linked_queue.hpp/linked_priority_queue.hpp)


template<class T>
auto LinkedSet<T>::abegin () const -> ics::Iterator<T>& {
	return *(new Iterator(const_cast<LinkedSet<T>*>(this),front));
}


template<class T>
auto LinkedSet<T>::aend () const -> ics::Iterator<T>& {
	return *(new Iterator(const_cast<LinkedSet<T>*>(this),trailer));
}
template<class T>
auto LinkedSet<T>::begin () const -> LinkedSet<T>::Iterator {
	return Iterator(const_cast<LinkedSet<T>*>(this),front);
}

template<class T>
auto LinkedSet<T>::end () const -> LinkedSet<T>::Iterator {
	return Iterator(const_cast<LinkedSet<T>*>(this), trailer);
}

template<class T>
int LinkedSet<T>::erase_at(LN* p) {
	for (auto i = front; i != trailer; i = i->next) {
		if (i->next->value == p->value) {
			i->next = p->next;
			break;
		}
		else if (i->value == p->value) {
			front = p->next;
			break;
		}
	}
	//std::cout << *this << std::endl;
	return 1;
}

//Insert Iterator constructor/methods here:
// (see linked_queue.hpp/linked_priority_queue.hpp)

//KLUDGE: memory-leak

template<class T>
LinkedSet<T>::Iterator::Iterator(LinkedSet<T>* fof, LN* initial) : current(initial), ref_set(fof) {
	expected_mod_count = ref_set->mod_count;
}

template<class T>
LinkedSet<T>::Iterator::~Iterator() {}

template<class T>
T LinkedSet<T>::Iterator::erase() {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::erase");
	if (!can_erase)
		throw CannotEraseError("LinkedSet::Iterator::erase Iterator cursor already erased");

	if (current == nullptr)
		throw CannotEraseError("LinkedSet::Iterator::erase Iterator cursor beyond data structure");

	can_erase = false;
	T to_return = current->value;

	//Add Code Here: Delete current value and update prev/curent as needed
	if(current == ref_set->front){ //want to delete first node
		ref_set->front = ref_set->front->next;
		current = current->next;
	}
	else if (current->next == nullptr){ //current is now the trailer
		throw CannotEraseError("LinkedSet::Iterator::erase Iterator cursor already erased");
	}
	else if (current->next != nullptr || current->next == ref_set->trailer){
		for (auto i = ref_set->front; i->next != nullptr; i = i->next){
			if(i->next == current) // i is the node before current
				i->next = current->next;
		}
		current = current->next;
	}
	--ref_set->used;
	expected_mod_count = ref_set->mod_count;
	return to_return;
}

template<class T>
std::string LinkedSet<T>::Iterator::str() const {
	std::ostringstream answer;
	answer << ref_set->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
	return answer.str();
}

template<class T>
const ics::Iterator<T>& LinkedSet<T>::Iterator::operator ++ () {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator ++");

	if (can_erase && current != nullptr) {
		current = current->next;
	}
	can_erase = true;
	return *this;
}

//KLUDGE: creates garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& LinkedSet<T>::Iterator::operator ++ (int) {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator ++(int)");

	Iterator* to_return = new Iterator(this->ref_set, current);
	if (can_erase && current != nullptr) {
		to_return->current = current;
		current = current->next;
	}
	can_erase = true;
	return *to_return;
}

template<class T>
bool LinkedSet<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("LinkedSet::Iterator::operator ==");
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator ==");
	if (ref_set != rhsASI->ref_set)
		throw ComparingDifferentIteratorsError("LinkedSet::Iterator::operator ==");

	return current == rhsASI->current;
}


template<class T>
bool LinkedSet<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
	const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
	if (rhsASI == 0)
		throw IteratorTypeError("LinkedSet::Iterator::operator !=");
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator !=");
	if (ref_set != rhsASI->ref_set)
		throw ComparingDifferentIteratorsError("LinkedSet::Iterator::operator !=");

	return current != rhsASI->current;
}

template<class T>
T& LinkedSet<T>::Iterator::operator *() const {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator *");
	if (!can_erase || current == nullptr) {
		std::ostringstream where;
		where << current
				<< " when front = " << ref_set->front << " and "
				<< " and rear = " << ref_set->trailer;
		throw IteratorPositionIllegal("LinkedSet::Iterator::operator * Iterator illegal: "+where.str());
	}

	return current->value;
}

template<class T>
T* LinkedSet<T>::Iterator::operator ->() const {
	if (expected_mod_count != ref_set->mod_count)
		throw ConcurrentModificationError("LinkedSet::Iterator::operator *");
	if (!can_erase || current == nullptr) {
		std::ostringstream where;
		where << current
		<< " when front = " << ref_set->front << " and "
		<< " and rear = " << ref_set->trailer;
		throw IteratorPositionIllegal("LinkedSet::Iterator::operator * Iterator illegal: "+where.str());
	}

	return &(current->value);
	}

}

#endif /* LINKED_SET_HPP_ */
