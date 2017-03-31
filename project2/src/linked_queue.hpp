// Asif Mahmud, 70556749
// Kim Le, 91394901
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming:
//   primarily that both partners worked on all parts together.



#ifndef LINKED_QUEUE_HPP_
#define LINKED_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "queue.hpp"


namespace ics {

template<class T> class LinkedQueue : public Queue<T>  {
  public:
    LinkedQueue();
    LinkedQueue(const LinkedQueue<T>& to_copy);
    //KLUDGE: not usable yet. LinkedQueue(std::initializer_list<T> il);
    LinkedQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop);
    virtual ~LinkedQueue();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual T&   peek       () const;
    virtual std::string str () const;

    virtual int  enqueue (const T& element);
    virtual T    dequeue ();
    virtual void clear   ();

    virtual int enqueue (ics::Iterator<T>& start, const ics::Iterator<T>& stop);

    virtual LinkedQueue<T>& operator = (const LinkedQueue<T>& rhs);
    virtual bool operator == (const Queue<T>& rhs) const;
    virtual bool operator != (const Queue<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinkedQueue<T2>& s);

    virtual ics::Iterator<T>& abegin () const;
    virtual ics::Iterator<T>& aend   () const;

  private:
    class LN;

  public:
    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(LinkedQueue<T>* fof, LN* initial);
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
        LN*             prev = nullptr;  //if nullptr, current at front of list
        LN*             current;         //if can_erase is false, this value is unusable
        LinkedQueue<T>* ref_queue;
        int             expected_mod_count;
        bool            can_erase = true;
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

    int used      =  0;
    LN* front     =  nullptr;
    LN* rear      =  nullptr;
    int mod_count =  0;            //For sensing concurrent modification
    void delete_list(LN*& front);  //Recycle storage, set front's argument to nullptr;
  };




//Insert constructor/methods here: see array_queue.hpp

//constructor
template<class T>
LinkedQueue<T>::LinkedQueue() {
}

template<class T>
LinkedQueue<T>::LinkedQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
	enqueue(start,stop);
}

template<class T>
LinkedQueue<T>::LinkedQueue(const LinkedQueue<T>& to_copy)  {
	this->clear();
	this->enqueue(to_copy.abegin(), to_copy.aend());
	rear = front;
}


template<class T>
LinkedQueue<T>::~LinkedQueue() {
	delete front;
	delete rear;
}

template<class T>
bool LinkedQueue<T>::empty() const {
  return front == nullptr;
}

template<class T>
inline int LinkedQueue<T>::size() const {
	return used;
}

template<class T>
T& LinkedQueue<T>::peek () const {
  if (this->empty())
	throw EmptyError("LinkedQueue::peek");
  else{
	  return front->value;
  }
}

template<class T>
std::string LinkedQueue<T>::str() const {
	 std::ostringstream answer;

	 if (this->empty()) {
	 	    answer << "queue[]";
	 	   answer << "(length=" << this->used << ",front=" << "none" << ",rear=" << "none" <<  ")";
	 	    return answer.str();
	 	  }else{
	 	    answer << "queue[";
	 	    for (auto i = this->front; i != nullptr; i = i->next) {
	 	    	answer << i->value;
	 	    	if(i->next != nullptr){
	 	    		answer << ",";
	 	    	}
	 	    }
	 	    answer << "]" << std::endl;
	 	  }
	  answer << "(length=" << this->used << ",front=" << this->peek() << ",rear=" << rear->value <<  ")";
	  return answer.str();
}


template<class T>
int LinkedQueue<T>::enqueue(const T& element) {
	if (front == nullptr){
		front = new LN(element);
		rear = front;
	}
	else{
		rear->next = new LN(element);
		rear = rear->next;
	}
	used++;
	mod_count++;
	return 1;
}

template<class T>
T LinkedQueue<T>::dequeue() {
  if (this->empty())
	throw EmptyError("LinkedQueue::dequeue");
  else {
	T to_dequeue;
	to_dequeue = front->value;
	front = front->next;
	used--;
	mod_count++;
	return to_dequeue;
  }


}

template<class T>
void LinkedQueue<T>::clear() {
	front = nullptr;
	rear = nullptr;
	used = 0;
	mod_count++;
}

template<class T>
int LinkedQueue<T>::enqueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  int count = 0;
  for (; start != stop; start++){
	 count += enqueue(*start);
  }

  return count;
}

template<class T>
LinkedQueue<T>& LinkedQueue<T>::operator = (const LinkedQueue<T>& rhs) {
	  if (this == &rhs) {
		  ++mod_count;
	    return *this;
	  }

	  else {
		  this->clear();
		  this->enqueue(rhs.abegin(), rhs.aend());
		  ++mod_count;
		  return *this;
	  }

}

template<class T>
bool LinkedQueue<T>::operator == (const Queue<T>& rhs) const {

  if (this == &rhs)
	return true;
  int used = this->size();
  if (used != rhs.size())
	return false;
  ics::Iterator<T>& itr = rhs.abegin();
  for (auto i = front; i != nullptr; i = i->next, itr++) {
	  if (i->value != *itr) {
		  return false;
	  }
  }
  return true;
}

template<class T>
bool LinkedQueue<T>::operator != (const Queue<T>& rhs) const {
  return !(*this == rhs);
}

template<class T>
std::ostream& operator << (std::ostream& outs, const LinkedQueue<T>& s) {
	 if (s.empty()) {
	    outs << "queue[]";
	    return outs;
	  }else{
	    outs << "queue[";
	    for (auto i = s.front; i != nullptr; i = i->next) {
	    	outs << i->value;
	    	if(i->next != nullptr){
	    		outs << ",";
	    	}
	    }
	    outs <<"]:rear";
	  }
	  return outs;
}

//KLUDGE: memory-leak
template<class T>
auto LinkedQueue<T>::abegin () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinkedQueue<T>*>(this),front));
}

//KLUDGE: memory-leak
template<class T>
auto LinkedQueue<T>::aend () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinkedQueue<T>*>(this),nullptr));
}

template<class T>
auto LinkedQueue<T>::begin () const -> LinkedQueue<T>::Iterator {
  return Iterator(const_cast<LinkedQueue<T>*>(this),front);
}

template<class T>
auto LinkedQueue<T>::end () const -> LinkedQueue<T>::Iterator {
  return Iterator(const_cast<LinkedQueue<T>*>(this),nullptr);
}


template<class T>
LinkedQueue<T>::Iterator::Iterator(LinkedQueue<T>* fof, LN* initial) : current(initial), ref_queue(fof) {
  expected_mod_count = ref_queue->mod_count;
}

template<class T>
LinkedQueue<T>::Iterator::~Iterator() {}

template<class T>
T LinkedQueue<T>::Iterator::erase() {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("LinkedQueue::Iterator::erase Iterator cursor already erased");

  if (current == nullptr)
    throw CannotEraseError("LinkedQueue::Iterator::erase Iterator cursor beyond data structure");

  can_erase = false;
  T to_return = current->value;

  //Add Code Here: Delete current value and update prev/curent as needed
  if (prev == nullptr){ // current is in the front
	  ref_queue->front = ref_queue->front->next;
	  current = ref_queue->front;
  }
  else if (current->next == nullptr){ //current points to last
  	  prev->next = nullptr;
  	  ref_queue->rear = prev;
  	  current = nullptr;
    }

  else  if (current->next != nullptr){ // current points to middle
	  for(auto i = ref_queue->front; i->next != nullptr; ++i){
		  if (i == prev){
			  i->next = i->next->next;
		  }
	  }
	  current = current->next;
  }

  --ref_queue->used;
  expected_mod_count = ref_queue->mod_count;
  return to_return;
}

template<class T>
std::string LinkedQueue<T>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_queue->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}

template<class T>
const ics::Iterator<T>& LinkedQueue<T>::Iterator::operator ++ () {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator ++");

  if (can_erase && current != nullptr) {
    prev = current;
    current = current->next;
  }
  can_erase = true;
  return *this;
}

//KLUDGE: creates garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& LinkedQueue<T>::Iterator::operator ++ (int) {
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator ++(int)");

  Iterator* to_return = new Iterator(this->ref_queue,prev);
  if (can_erase && current != nullptr) {
    to_return->current = prev = current;
    current = current->next;
  }
  can_erase = true;
  return *to_return;
}

template<class T>
bool LinkedQueue<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("LinkedQueue::Iterator::operator ==");
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator ==");
  if (ref_queue != rhsASI->ref_queue)
    throw ComparingDifferentIteratorsError("LinkedQueue::Iterator::operator ==");

  return current == rhsASI->current;
}


template<class T>
bool LinkedQueue<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("LinkedQueue::Iterator::operator !=");
  if (expected_mod_count != ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator !=");
  if (ref_queue != rhsASI->ref_queue)
    throw ComparingDifferentIteratorsError("LinkedQueue::Iterator::operator !=");

  return current != rhsASI->current;
}

template<class T>
T& LinkedQueue<T>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator *");
  if (!can_erase || current == nullptr) {
    std::ostringstream where;
    where << current
          << " when front = " << ref_queue->front << " and "
          << " and rear = " << ref_queue->rear;
    throw IteratorPositionIllegal("LinkedQueue::Iterator::operator * Iterator illegal: "+where.str());
  }

  return current->value;
}

template<class T>
T* LinkedQueue<T>::Iterator::operator ->() const {
  if (expected_mod_count !=
      ref_queue->mod_count)
    throw ConcurrentModificationError("LinkedQueue::Iterator::operator *");
  if (!can_erase || current == nullptr) {
    std::ostringstream where;
    where << current
          << " when front = " << ref_queue->front << " and "
          << " and rear = " << ref_queue->rear;
    throw IteratorPositionIllegal("LinkedQueue::Iterator::operator * Iterator illegal: "+where.str());
  }

  return &(current->value);
}

}

#endif /* LINKED_QUEUE_HPP_ */
