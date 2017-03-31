// Asif Mahmud, 70556749
// Kim Le, 91394901
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming:
//   primarily that both partners worked on all parts together.



#ifndef LINKED_PRIORITY_QUEUE_HPP_
#define LINKED_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "priority_queue.hpp"
#include "array_stack.hpp"


namespace ics {

template<class T> class LinkedPriorityQueue : public PriorityQueue<T>  {
  public:
    LinkedPriorityQueue();
    explicit LinkedPriorityQueue(bool (*agt)(const T& a, const T& b));
    LinkedPriorityQueue(const LinkedPriorityQueue<T>& to_copy); //bool (*agt)(const T& a, const T& b));
    //KLUDGE: not usable yet. LinkedPriorityQueue(std::initializer_list<T> il,bool (*agt)(const T& a, const T& b));
    LinkedPriorityQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop,bool (*agt)(const T& a, const T& b));
    virtual ~LinkedPriorityQueue();

    virtual bool empty      () const;
    virtual int  size       () const;
    virtual T&   peek       () const;
    virtual std::string str () const;

    virtual int  enqueue (const T& element);
    virtual T    dequeue ();
    virtual void clear   ();

    virtual int enqueue (ics::Iterator<T>& start, const ics::Iterator<T>& stop);

    virtual LinkedPriorityQueue<T>& operator = (const LinkedPriorityQueue<T>& rhs);
    virtual bool operator == (const PriorityQueue<T>& rhs) const;
    virtual bool operator != (const PriorityQueue<T>& rhs) const;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T2>& s);

    virtual ics::Iterator<T>& abegin () const;
    virtual ics::Iterator<T>& aend   () const;

  private:
    class LN;

  public:
    class Iterator : public ics::Iterator<T> {
      public:
        //KLUDGE should be callable only in begin/end
        Iterator(LinkedPriorityQueue<T>* fof, LN* initial);
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
        LN*                     prev;     //if header, current at front of list
        LN*                     current;  //if can_erase is false, this value is unusable
        LinkedPriorityQueue<T>* ref_pq;
        int                     expected_mod_count;
        bool                    can_erase = true;
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

    bool (*gt)(const T& a, const T& b);  // gt(a,b) = true iff a has higher priority than b
    int used      =  0;
    LN* front     =  new LN();
    int mod_count =  0;                  //For sensing concurrent modification
    void delete_list(LN*& front);        //Recycle storage, set front's argument to nullptr;
  };




//Insert constructor/methods here: see array_queue.hpp and/or linked_queue.hpp
//constructor
template<class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(bool (*agt)(const T& a, const T& b)): gt(agt)  {
}


template<class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(const LinkedPriorityQueue<T>& to_copy) : gt(to_copy.gt)
{
	for (auto i = to_copy.front->next; i != nullptr; i = i->next) {
		this->enqueue(i->value);
	}
}


template<class T>
LinkedPriorityQueue<T>::LinkedPriorityQueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop, bool (*agt)(const T& a, const T& b))
  : gt(agt) {
  enqueue(start,stop);
}

template<class T>
bool LinkedPriorityQueue<T>::empty() const {
  return front->next == nullptr;
}

template<class T>
LinkedPriorityQueue<T>::~LinkedPriorityQueue() {
  delete front;
}

template<class T>
inline int LinkedPriorityQueue<T>::size() const {
	return used;

}

template<class T>
T& LinkedPriorityQueue<T>::peek () const {
  if (this->empty())
	throw EmptyError("LinkedPriorityQueue::peek");

  return front->next->value;
}

template<class T>
std::string LinkedPriorityQueue<T>::str() const {
	 std::ostringstream answer;
	 answer << *this << ",used=" << used << ",mod_count=" << mod_count << ")";
	 return answer.str();
}


template<class T>
int LinkedPriorityQueue<T>::enqueue(const T& element) {
	if (front->next == nullptr){
	 front ->next = new LN(element);
	}
	else {
		for(auto i = front->next ; i != nullptr ; i = i->next){
			if (gt(element, i->value)) {
				i->next = new LN(i->value, i->next);
				i->value = element;
				break;
			}
			else {
				if (i->next == nullptr) {
					i->next = new LN(element);
					break;
				}
			}
	    }
	}
	used++;
	mod_count++;
	return 1;
}

template<class T>
T LinkedPriorityQueue<T>::dequeue() {
  if (this->empty())
	throw EmptyError("LinkedPriorityQueue::dequeue");
  else {
	T to_dequeue;
	to_dequeue = front->next->value;
	front->next = front->next->next;
	used--;
	mod_count++;
	return to_dequeue;
  }


}

template<class T>
void LinkedPriorityQueue<T>::clear() {
	front = new LN();
	used = 0;
	mod_count++;
}

template<class T>
int LinkedPriorityQueue<T>::enqueue(ics::Iterator<T>& start, const ics::Iterator<T>& stop) {
  int count = 0;
  for (; start != stop; start++){
	 count += enqueue(*start);
  }
  return count;
}

template<class T>
LinkedPriorityQueue<T>& LinkedPriorityQueue<T>::operator = (const LinkedPriorityQueue<T>& rhs) {
	  if (this == &rhs) {
		++mod_count;
	    return *this;
	  }
	  else {
		 gt = rhs.gt;
		 this->clear();
		  for (auto i = rhs.front->next; i != nullptr; i = i->next) {
			  this->enqueue(i->value);
		  }
		  ++mod_count;
		  return *this;
	  }
	  std::cout << "addres check: " << (&(rhs.front) == &(this->front)) << std::endl;

}

template<class T>
bool LinkedPriorityQueue<T>::operator == (const PriorityQueue<T>& rhs) const {

  if (this == &rhs)
	return true;
  int used = this->size();
  if (used != rhs.size())
	return false;
  ics::Iterator<T>& itr = rhs.abegin();
  for (auto i = front->next; i != nullptr; i = i->next, itr++) {
	  if (i->value != *itr) {
		  return false;
	  }
  }
  return true;
}

template<class T>
bool LinkedPriorityQueue<T>::operator != (const PriorityQueue<T>& rhs) const {
  return !(*this == rhs);
}

template<class T>
std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T>& s) {
	 if (s.empty()) {
	    outs << "priority_queue[]";
	    return outs;
	  }else if (s.size() >= 2 && s.front->next->value > s.front->next->next->value){
	    outs << "priority_queue[";
	    for (auto i = s.front->next; i != nullptr; i = i->next) {
	    	outs << i->value;
	    	if(i->next != nullptr){
	    		outs << ",";
	    	}
	    }
	    outs <<"]:highest";
	  }
	  else {
		  ics::ArrayStack<T> stack;
		  outs << "priority_queue[";
		  for(auto i = s.front ; i != nullptr ; i = i->next){
			  stack.push(i->value);
		  }
		  while(stack.empty() == false){
			  outs << stack.peek();
			  stack.pop();
			  if (stack.size() > 1){
				  outs << ",";
			  }
		  }
		  outs <<"]:highest";
	  }

	  return outs;
}



//KLUDGE: memory-leak
template<class T>
auto LinkedPriorityQueue<T>::abegin () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinkedPriorityQueue<T>*>(this),front->next));
}

//KLUDGE: memory-leak
template<class T>
auto LinkedPriorityQueue<T>::aend () const -> ics::Iterator<T>& {
  return *(new Iterator(const_cast<LinkedPriorityQueue<T>*>(this),nullptr));
}

template<class T>
auto LinkedPriorityQueue<T>::begin () const -> LinkedPriorityQueue<T>::Iterator {
  return Iterator(const_cast<LinkedPriorityQueue<T>*>(this),front->next);
}

template<class T>
auto LinkedPriorityQueue<T>::end () const -> LinkedPriorityQueue<T>::Iterator {
  return Iterator(const_cast<LinkedPriorityQueue<T>*>(this),nullptr);
}


template<class T>
LinkedPriorityQueue<T>::Iterator::Iterator(LinkedPriorityQueue<T>* fof, LN* initial) : current(initial), ref_pq(fof) {
  prev = ref_pq->front;
  expected_mod_count = ref_pq->mod_count;
}

template<class T>
LinkedPriorityQueue<T>::Iterator::~Iterator() {}

template<class T>
T LinkedPriorityQueue<T>::Iterator::erase() {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::erase");
  if (!can_erase)
    throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor already erased");

  if (current == nullptr)
    throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor beyond data structure");

  can_erase = false;
  T to_return = current->value;

  //Add Code Here: Delete current value and update prev/curent as needed
  if (current->next == nullptr){ //current points to last
  	  prev->next = nullptr;
  	  current = nullptr;
    }

  else  if (current->next != nullptr){ // current points to middle
	   for(auto i = ref_pq->front; i->next != nullptr; i = i->next){
		   if (i == prev){
	   			  i->next = i->next->next;
	   		  }
	   	  }
	   	  current = current->next;
  }

  --ref_pq->used;
  expected_mod_count = ref_pq->mod_count;
  return to_return;
}

template<class T>
std::string LinkedPriorityQueue<T>::Iterator::str() const {
  std::ostringstream answer;
  answer << ref_pq->str() << "(current=" << current << ",expected_mod_count=" << expected_mod_count << ",can_erase=" << can_erase << ")";
  return answer.str();
}

template<class T>
const ics::Iterator<T>& LinkedPriorityQueue<T>::Iterator::operator ++ () {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ++");

  if (can_erase && current != nullptr) {
    prev = current;
    current = current->next;
  }
  can_erase = true;
  return *this;
}

//KLUDGE: creates garbage! (can return local value!)
template<class T>
const ics::Iterator<T>& LinkedPriorityQueue<T>::Iterator::operator ++ (int) {
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ++(int)");

  Iterator* to_return = new Iterator(this->ref_pq,prev);
  if (can_erase && current != nullptr) {
    to_return->current = prev = current;
    current = current->next;
  }
  can_erase = true;
  return *to_return;
}

template<class T>
bool LinkedPriorityQueue<T>::Iterator::operator == (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator ==");
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ==");
  if (ref_pq != rhsASI->ref_pq)
    throw ComparingDifferentIteratorsError("LinkedPriorityQueue::Iterator::operator ==");

  return current == rhsASI->current;
}


template<class T>
bool LinkedPriorityQueue<T>::Iterator::operator != (const ics::Iterator<T>& rhs) const {
  const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
  if (rhsASI == 0)
    throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator !=");
  if (expected_mod_count != ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator !=");
  if (ref_pq != rhsASI->ref_pq)
    throw ComparingDifferentIteratorsError("LinkedPriorityQueue::Iterator::operator !=");

  return current != rhsASI->current;
}

template<class T>
T& LinkedPriorityQueue<T>::Iterator::operator *() const {
  if (expected_mod_count !=
      ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator *");
  if (!can_erase || current == nullptr) {
    std::ostringstream where;
    where << current
          << " when front = " << ref_pq->front;
    throw IteratorPositionIllegal("LinkedPriorityQueue::Iterator::operator * Iterator illegal: "+where.str());
  }

  return current->value;
}

template<class T>
T* LinkedPriorityQueue<T>::Iterator::operator ->() const {
  if (expected_mod_count !=
      ref_pq->mod_count)
    throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator *");
  if (!can_erase || current == nullptr) {
    std::ostringstream where;
    where << current
          << " when front = " << ref_pq->front;
    throw IteratorPositionIllegal("LinkedPriorityQueue::Iterator::operator * Iterator illegal: "+where.str());
  }

  return &(current->value);
}

}

#endif /* LINKED_PRIORITY_QUEUE_HPP_ */
