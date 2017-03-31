#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "ics46goody.hpp"
#include "ics_exceptions.hpp"
#include "iterator.hpp"
#include "array_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"

template<class T>
ics::ArraySet<T> extract (const ics::ArraySet<ics::ArraySet<T>>& s) {
  ics::ArraySet<T> answer;
  for (auto i : s){
	  for (auto j : i)
		  answer.insert(j);
  }
  return answer;
}


template<class KEY,class T>
void swap (ics::ArrayMap<KEY,T>& m, KEY key1, KEY key2) {
	m[key2] = m.put(key1, m[key2]);
}



template<class KEY,class T>
void values_set_to_queue (const ics::ArrayMap<KEY,ics::ArraySet<T>> m1,
                          ics::ArrayMap<KEY,ics::ArrayQueue<T>>& m2) {

	for (auto i : m1)
	{
		ics::ArrayQueue<T> a;
		for (auto j : i.second)
		{
			a.enqueue(j);
		}
		m2[i.first] = a;
	}
}

template<class KEY,class T>
ics::ArraySet<KEY> reachable (const ics::ArrayMap<KEY,KEY>& m, KEY k) {
  ics::ArraySet<KEY> seen;
  while(!seen.contains(k)){
	  seen.insert(k);
	  k = m[k];
  }
  return seen;
}







