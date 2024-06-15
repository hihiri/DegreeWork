#include <iostream>
#include "MessageStructuresTest.cpp"

int main()
{
    CppUnit::TextUi::TestRunner run;

    run.addTest(MessageStructuresTest::suite());

    run.run();

    return 0;
}
