#include <iostream>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>

#include "RpcCommunicator/MessageStructures.cpp"

class MessageStructuresTest : public CppUnit::TestFixture {
	TestMessage* t;
public:
	void setUp() {
		t = new TestMessage("test", "test");
	}

	void tearDown() {
		delete t;
	}

	void testAttributes() {
		CPPUNIT_ASSERT(t->Type == "test");
		CPPUNIT_ASSERT(t->MessageContent == "test");
	}

	static CppUnit::Test* suite() {
		CppUnit::TestSuite* suite = new CppUnit::TestSuite();

		suite->addTest(new CppUnit::TestCaller<MessageStructuresTest>("testAttributes", &MessageStructuresTest::testAttributes));

		return suite;
	}
};