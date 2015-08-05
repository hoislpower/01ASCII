#include <config.h>
#include <check.h>

#include "../src/input.h"


// try to load some bin files
START_TEST(readBinFileTest)
{
	int i;
	uint8_t data[10];
	deviceData device;


	// initialize the device data structure
	initializeDeviceData(&device);

	// set memory size
	device.memorySize = 10;

	// try to load files
	ck_assert_int_eq(readBinFile("testfiles/nonexistentfile.bin", &device, data), EXIT_FAILURE);
	ck_assert_int_eq(readBinFile("testfiles/empty.bin", &device, data), EXIT_FAILURE);
	ck_assert_int_eq(readBinFile("testfiles/testfile.bin", &device, data), EXIT_SUCCESS);

	for(i=0; i<device.memorySize; i++)
	{
		if(i<5)
		{
			ck_assert_uint_eq(data[i], i+1);
		}
		else
		{
			ck_assert_uint_eq(data[i], 0xFF);
		}
	}
}
END_TEST


Suite	*testSuite()
{
	Suite *suite;
	TCase *testCase;


	suite = suite_create("Bin Input");


	// test cases for 32 vs 64 bit support
	testCase = tcase_create("readBinFile");
	tcase_add_test(testCase, readBinFileTest);
	suite_add_tcase(suite, testCase);

	return suite;
}


int main(void)
{
	int testResult;
	Suite *suite;
	SRunner *suiteRunner;


	// create the test suite and the runner
	suite = testSuite();
	suiteRunner = srunner_create(suite);

	// execute the tests
	srunner_run_all(suiteRunner, CK_NORMAL);
	testResult = srunner_ntests_failed(suiteRunner);
	srunner_free(suiteRunner);

	return (testResult == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
