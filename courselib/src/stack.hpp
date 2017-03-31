#ifndef STACK_HPP_
#define STACK_HPP_

#include <iostream>
#include "iterator.hpp"

namespace ics {

template<class T> class Stack  {
  public:
    virtual ~Stack(){}//KLUDGE

    virtual bool empty      () const = 0;
    virtual int  size       () const = 0;
    virtual T&   peek       () const = 0;
    virtual std::string str () const = 0;

    virtual int  push (const T& element) = 0;
    virtual T    pop  () = 0;
    virtual void clear() = 0;

    virtual int push (Iterator<T>& start, const Iterator<T>& stop) = 0;

    virtual bool operator == (const Stack<T>& rhs) const = 0;
    virtual bool operator != (const Stack<T>& rhs) const = 0;

    template<class T2>
    friend std::ostream& operator << (std::ostream& outs, const Stack<T2>& s);

    virtual Iterator<T>& abegin () const = 0;
    virtual Iterator<T>& aend   () const = 0;
  };





//KLUDGE
//template<class T>
//Stack<T>::~Stack(){}

template<class T2>
std::ostream& operator << (std::ostream& outs, const Stack<T2>& s) {
  outs << s;
  return outs;
}

}

#endif /* STACK_HPP_ */
