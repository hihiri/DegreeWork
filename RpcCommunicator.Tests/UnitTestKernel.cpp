#include <iostream>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>

#include "MessageStructuresTest.cpp"

int main()
{
    CppUnit::TextUi::TestRunner run;

    run.addTest(MessageStructuresTest::suite());

    run.run();

    return 0;
}
