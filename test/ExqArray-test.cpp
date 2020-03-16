//
// Created by Omar Shahbaz Khan on 15/03/2020.
//

#include "gtest/gtest.h"
#include "ExqArray.h"

using exq::ExqArray;
using std::cout;
using std::endl;

TEST (SixteenBits, initializeAndSetValues) {
    auto exqArr = new ExqArray<uint16_t>(3);
    ASSERT_EQ(exqArr->getSize(), 3);

    uint16_t one = 33;
    uint16_t two = 45;
    uint16_t three = 68;
    exqArr->setItem(one, 0);
    exqArr->setItem(two, 1);
    exqArr->setItem(three, 2);

    ASSERT_EQ(one, exqArr->getItem(0));
    ASSERT_EQ(two, exqArr->getItem(1));
    ASSERT_EQ(three, exqArr->getItem(2));

    cout << "TEST initializeAndSetValues SUCCEEDED!" << endl;
}

TEST (SixteenBits, checkSizeOfArrayWith4Elements) {
    uint16_t arr[4] = {2,3,5,7};
    auto exqArr = new ExqArray<uint16_t>(arr, 4);
    ASSERT_EQ(exqArr->getSize(), 4);

    ASSERT_EQ(8, sizeof(exqArr->getArray()));

    cout << "TEST checkSizeOfArrayWith4Elements SUCCEEDED!" << endl;
}
