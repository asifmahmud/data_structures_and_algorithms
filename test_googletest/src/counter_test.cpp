/*
 * counter_test.cpp
 *
 *  Created on: Jan 7, 2014
 *      Author: Asif
 */


       #include <iostream>
       using namespace std;
       #include "gtest/gtest.h"
       #include "Counter.hpp"

       TEST(Counter, Increment) {
          Counter c;

          EXPECT_EQ(0, c.Increment());
          EXPECT_EQ(1, c.Increment());
          EXPECT_EQ(2, c.Increment());

          cout << "Congratulations, you have just run a test :-)" << endl;
        }

       int main(int argc, char **argv) {
           ::testing::InitGoogleTest(&argc, argv);
          return RUN_ALL_TESTS();
       }


