#include <config.h>
#include <check.h>

#include "../src/converter.h"


// test findUsedBlocks function
START_TEST(findUsedBlocksTest)
{
	int i;
	uint8_t programData[10];
	int usedBlocks[5];
	deviceData device;


	// initialize the device data structure
	initializeDeviceData(&device);

	// initialize the programData
	memset(programData, 0xFF, 10);

	// set memory and block size
	device.memorySize = 10;
	device.blockSize = 2;

	// write some data
	programData[3] = 0xFE;
	programData[6] = 1;
	programData[9] = 0;


	// find used blocks
	findUsedBlocks(&device, programData, usedBlocks);

	ck_assert_int_eq(usedBlocks[0], false);
	ck_assert_int_eq(usedBlocks[1], true);
	ck_assert_int_eq(usedBlocks[2], false);
	ck_assert_int_eq(usedBlocks[3], true);
	ck_assert_int_eq(usedBlocks[4], true);
}
END_TEST


// test wordToOutputString function
START_TEST(wordToOutputStringTest)
{
	char outputString[MAX_BIT_ORDER_LENGTH*2 + 3];
	uint32_64_t word;
	int8_t bitOrder[] = {
				0x0F, 0x0E, 0x0D, 0x0C, 0x08, 0x09, 0x0A, 0x0B, 
				0x04, 0x05, 0x06, 0x07, 0x03, 0x02, 0x01, 0xA0, 
				0x1F, 0x1E, 0x1D, 0x1C, 0x18, 0x19, 0x1A, 0x1B, 
				0x14, 0x15, 0x16, 0x17, 0x13, 0x12, 0x11,   -1,
				  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
				  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
				  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
				  -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1 };

	word = 0xFF55AA11;

	// generate ascii string
	ck_assert_int_eq(wordToOutputString(word, bitOrder, true, outputString), 64);
	ck_assert_str_eq(outputString, "1 0 1 0 0 1 0 1 1 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 0 1 0 0 1 0 \r\n");

	// generate binary string
	ck_assert_int_eq(wordToOutputString(word, bitOrder, false, outputString), 31);
	ck_assert_uint_eq(outputString[0], 1);
	ck_assert_uint_eq(outputString[1], 0);
	ck_assert_uint_eq(outputString[2], 1);
	ck_assert_uint_eq(outputString[3], 0);
	ck_assert_uint_eq(outputString[4], 0);
	ck_assert_uint_eq(outputString[5], 1);
	ck_assert_uint_eq(outputString[6], 0);
	ck_assert_uint_eq(outputString[7], 1);
	ck_assert_uint_eq(outputString[8], 1);
	ck_assert_uint_eq(outputString[9], 0);
	ck_assert_uint_eq(outputString[10], 0);
	ck_assert_uint_eq(outputString[11], 0);
	ck_assert_uint_eq(outputString[12], 0);
	ck_assert_uint_eq(outputString[13], 0);
	ck_assert_uint_eq(outputString[14], 0);
	ck_assert_uint_eq(outputString[15], 1);
	ck_assert_uint_eq(outputString[16], 1);
	ck_assert_uint_eq(outputString[17], 1);
	ck_assert_uint_eq(outputString[18], 1);
	ck_assert_uint_eq(outputString[19], 1);
	ck_assert_uint_eq(outputString[20], 1);
	ck_assert_uint_eq(outputString[21], 1);
	ck_assert_uint_eq(outputString[22], 1);
	ck_assert_uint_eq(outputString[23], 1);
	ck_assert_uint_eq(outputString[24], 1);
	ck_assert_uint_eq(outputString[25], 0);
	ck_assert_uint_eq(outputString[26], 1);
	ck_assert_uint_eq(outputString[27], 0);
	ck_assert_uint_eq(outputString[28], 0);
	ck_assert_uint_eq(outputString[29], 1);
	ck_assert_uint_eq(outputString[30], 0);
}
END_TEST


Suite	*testSuite()
{
	Suite *suite;
	TCase *testCase;


	suite = suite_create("Converter");


	// test cases for finding used blocks
	testCase = tcase_create("findUsedBlocks");
	tcase_add_test(testCase, findUsedBlocksTest);
	suite_add_tcase(suite, testCase);

	// test cases for converting a word into an output string
	testCase = tcase_create("wordToOutputString");
	tcase_add_test(testCase, wordToOutputStringTest);
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
