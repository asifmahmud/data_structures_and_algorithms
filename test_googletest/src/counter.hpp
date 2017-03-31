/*
 * counter.hpp
 *
 *  Created on: Jan 7, 2014
 *      Author: Asif
 */

#ifndef COUNTER_HPP_
#define COUNTER_HPP_

        class Counter {
            private:
                int counter_;
            public:
                Counter() : counter_(0) {}
                int Increment();
        };



#endif /* COUNTER_HPP_ */
