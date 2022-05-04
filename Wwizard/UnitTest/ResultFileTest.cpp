#include "pch.h"
#include "CppUnitTest.h"
#include "..\Wwizard\ResultFile.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ResultFileTest
{
	TEST_CLASS(ResultFileTest)
	{
	public:
		TEST_METHOD(Test_NamingResultFile)
		{
			std::string guid = "12";
			std::string name = "test";
			Issue issue = Issue::PREFIX;
			NamingResultFile nrf(guid, name, issue);

			Assert::AreEqual(name, nrf.name);
			Assert::AreEqual(guid, nrf.guid);
			Assert::AreEqual(static_cast<int>(issue), static_cast<int>(nrf.issue));
		}

		TEST_METHOD(Test_QueryResultFile)
		{
			std::string test_guid = "test_guid";
			std::string test_name = "test_name";
			std::string test_path = "test_path";
			std::string test_type = "test_type";
			int test_color = 1111;

			QueryResultFile qrf(test_name, test_guid, test_path, test_type, test_color);

			Assert::AreEqual(test_name, qrf.name);
			Assert::AreEqual(test_guid, qrf.guid);
			Assert::AreEqual(test_path, qrf.path);
			Assert::AreEqual(test_type, qrf.type);
			Assert::AreEqual(test_color, qrf.color);
		}
	};
}
