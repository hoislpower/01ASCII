#include <config.h>
#include <check.h>

#include "../src/input.h"
#include "../src/hex-input.h"


// test calculateChecksum function
START_TEST(calculateChecksumTest)
{
	hexFileLine line;


	// calculate some checksums

	// :10002800 FBCF05F0E9CF06F0EACF07F0E1CF08F0 03
	line.byteCount = 0x10;
	line.address = 0x0028;
	line.recordType = 0x00;
	line.data[0] = 0xFB;
	line.data[1] = 0xCF;
	line.data[2] = 0x05;
	line.data[3] = 0xF0;
	line.data[4] = 0xE9;
	line.data[5] = 0xCF;
	line.data[6] = 0x06;
	line.data[7] = 0xF0;
	line.data[8] = 0xEA;
	line.data[9] = 0xCF;
	line.data[10] = 0x07;
	line.data[11] = 0xF0;
	line.data[12] = 0xE1;
	line.data[13] = 0xCF;
	line.data[14] = 0x08;
	line.data[15] = 0xF0;
	ck_assert_uint_eq(calculateChecksum(line), 0x03);

	// :10005800 F5CF11F011C0F5FF10C0F8FF0FC0F7FF 82
	line.byteCount = 0x10;
	line.address = 0x0058;
	line.recordType = 0x00;
	line.data[0] = 0xF5;
	line.data[1] = 0xCF;
	line.data[2] = 0x11;
	line.data[3] = 0xF0;
	line.data[4] = 0x11;
	line.data[5] = 0xC0;
	line.data[6] = 0xF5;
	line.data[7] = 0xFF;
	line.data[8] = 0x10;
	line.data[9] = 0xC0;
	line.data[10] = 0xF8;
	line.data[11] = 0xFF;
	line.data[12] = 0x0F;
	line.data[13] = 0xC0;
	line.data[14] = 0xF7;
	line.data[15] = 0xFF;
	ck_assert_uint_eq(calculateChecksum(line), 0x82);

	// :10FBD600 444A5140FFFFFFFFFF7E1010081E3845 C4
	line.byteCount = 0x10;
	line.address = 0xFBD6;
	line.recordType = 0x00;
	line.data[0] = 0x44;
	line.data[1] = 0x4A;
	line.data[2] = 0x51;
	line.data[3] = 0x40;
	line.data[4] = 0xFF;
	line.data[5] = 0xFF;
	line.data[6] = 0xFF;
	line.data[7] = 0xFF;
	line.data[8] = 0xFF;
	line.data[9] = 0x7E;
	line.data[10] = 0x10;
	line.data[11] = 0x10;
	line.data[12] = 0x08;
	line.data[13] = 0x1E;
	line.data[14] = 0x38;
	line.data[15] = 0x45;
	ck_assert_uint_eq(calculateChecksum(line), 0xC4);

	// :020000040001F9
	line.byteCount = 0x02;
	line.address = 0x0000;
	line.recordType = 0x04;
	line.data[0] = 0x00;
	line.data[1] = 0x01;
	ck_assert_uint_eq(calculateChecksum(line), 0xF9);
}
END_TEST


// test calculateChecksum function
START_TEST(saveHexLineToProgramDataTest)
{
	uint8_t programData[1024];
	deviceData device;
	hexFileLine line;


	// initialize the programData
	memset(programData, 0xFF, 1024);

	// initialize a device
	initializeDeviceData(&device);
	device.memorySize = 1024;
	device.startAddress = 0x08000000;


	// setup a line to save
	line.address = 0x03E0;
	line.recordType = 0x00;
	line.data[0] = 0xFB;
	line.data[1] = 0xCF;
	line.data[2] = 0x05;
	line.data[3] = 0xF0;
	line.data[4] = 0xE9;

	// try to save some lines that are invalid for the given device
	line.byteCount = 0x05;
	line.extendedAddress = 0x00000000;
	ck_assert_int_eq(saveHexLineToProgramData(line, &device, programData), EXIT_FAILURE);

	line.byteCount = 32;
	line.extendedAddress = 0x08000000;
	ck_assert_int_eq(saveHexLineToProgramData(line, &device, programData), EXIT_FAILURE);

	// write 0 bytes to programData
	line.byteCount = 0;
	line.extendedAddress = 0x08000000;
	ck_assert_int_eq(saveHexLineToProgramData(line, &device, programData), EXIT_SUCCESS);
	ck_assert_uint_eq(programData[line.address], 0xFF);

	// write 5 bytes to programData
	line.byteCount = 5;
	line.extendedAddress = 0x08000000;
	ck_assert_int_eq(saveHexLineToProgramData(line, &device, programData), EXIT_SUCCESS);
	ck_assert_uint_eq(programData[line.address-1], 0xFF);
	ck_assert_uint_eq(programData[line.address+0], 0xFB);
	ck_assert_uint_eq(programData[line.address+1], 0xCF);
	ck_assert_uint_eq(programData[line.address+2], 0x05);
	ck_assert_uint_eq(programData[line.address+3], 0xF0);
	ck_assert_uint_eq(programData[line.address+4], 0xE9);
	ck_assert_uint_eq(programData[line.address+5], 0xFF);
}
END_TEST


// test convertHexToDecBuffer function
START_TEST(convertHexToDecBufferTest)
{
	char hexBuffer[20];
	char decBuffer[10];


	hexBuffer[0] = '0';
	hexBuffer[1] = '1';
	hexBuffer[2] = '2';
	hexBuffer[3] = '3';
	hexBuffer[4] = '4';
	hexBuffer[5] = '5';
	hexBuffer[6] = '6';
	hexBuffer[7] = '7';
	hexBuffer[8] = '8';
	hexBuffer[9] = '9';
	hexBuffer[10] = 'a';
	hexBuffer[11] = 'b';
	hexBuffer[12] = 'c';
	hexBuffer[13] = 'd';
	hexBuffer[14] = 'e';
	hexBuffer[15] = 'f';
	hexBuffer[16] = '0';
	hexBuffer[17] = 'x';

	// try to convert a hex buffer containing an invalid character
	ck_assert_int_eq(convertHexToDecBuffer(hexBuffer, decBuffer, 18), EXIT_FAILURE);

	// convert a hex buffer
	ck_assert_int_eq(convertHexToDecBuffer(hexBuffer, decBuffer, 16), EXIT_SUCCESS);

	ck_assert_uint_eq((uint8_t) decBuffer[0], 1);
	ck_assert_uint_eq((uint8_t) decBuffer[1], 35);
	ck_assert_uint_eq((uint8_t) decBuffer[2], 69);
	ck_assert_uint_eq((uint8_t) decBuffer[3], 103);
	ck_assert_uint_eq((uint8_t) decBuffer[4], 137);
	ck_assert_uint_eq((uint8_t) decBuffer[5], 171);
	ck_assert_uint_eq((uint8_t) decBuffer[6], 205);
	ck_assert_uint_eq((uint8_t) decBuffer[7], 239);
}
END_TEST


// test readHexFileTest function
START_TEST(readHexFileTest)
{
	uint8_t programData[1024];
	deviceData device;


	// initialize the programData
	memset(programData, 0xFF, 1024);

	// initialize the device
	initializeDeviceData(&device);
	device.memorySize = 1024;
	device.startAddress = 40;

	// try to read a file that does not exist
	ck_assert_int_eq(readHexFile("testfiles/nonexistentfile.hex", &device, programData), EXIT_FAILURE);

	// try to open a file with a missing start code
	ck_assert_int_eq(readHexFile("testfiles/missingstartcode.hex", &device, programData), EXIT_FAILURE);

	// try to open a file containing an invalid character
	ck_assert_int_eq(readHexFile("testfiles/invalidchar.hex", &device, programData), EXIT_FAILURE);

	// try to open a file containing an invalid checksum
	ck_assert_int_eq(readHexFile("testfiles/invalidchecksum.hex", &device, programData), EXIT_FAILURE);

	// try to open a file containing an invalid record type
	ck_assert_int_eq(readHexFile("testfiles/invalidrecord.hex", &device, programData), EXIT_FAILURE);

	// try to open a file containing data not within the address boundaries of the device
	ck_assert_int_eq(readHexFile("testfiles/wrongaddress.hex", &device, programData), EXIT_FAILURE);

	// try to open a file with a missing end of file record
	ck_assert_int_eq(readHexFile("testfiles/missingendoffile.hex", &device, programData), EXIT_FAILURE);

	// try to open a file containing an invalid end of file record
	ck_assert_int_eq(readHexFile("testfiles/invalidendoffile.hex", &device, programData), EXIT_FAILURE);

	// try to open a file containing an invalid start segment address record
	ck_assert_int_eq(readHexFile("testfiles/invalidstartsegmentaddress.hex", &device, programData), EXIT_FAILURE);

	// try to open a file containing an invalid extended segment address record
	ck_assert_int_eq(readHexFile("testfiles/invalidextendedsegmentaddress.hex", &device, programData), EXIT_FAILURE);

	// try to open a file containing an invalid start linear address record
	ck_assert_int_eq(readHexFile("testfiles/invalidstartlinearaddress.hex", &device, programData), EXIT_FAILURE);

	// try to open a file containing an invalid extended linear address record
	ck_assert_int_eq(readHexFile("testfiles/invalidextendedlinearaddress.hex", &device, programData), EXIT_FAILURE);

	// read two files containing all record types
	device.startAddress = 0x00080000;
	memset(programData, 0xFF, 1024);
	ck_assert_int_eq(readHexFile("testfiles/testfilelinearaddress.hex", &device, programData), EXIT_SUCCESS);
	ck_assert_uint_eq(programData[0x27], 0xFF);
	ck_assert_uint_eq(programData[0x28], 0xFB);
	ck_assert_uint_eq(programData[0x29], 0xCF);
	ck_assert_uint_eq(programData[0x2A], 0x05);
	ck_assert_uint_eq(programData[0x36], 0x08);
	ck_assert_uint_eq(programData[0x37], 0xF0);
	ck_assert_uint_eq(programData[0x38], 0xFF);

	device.startAddress = 0x00080000;
	memset(programData, 0xFF, 1024);
	ck_assert_int_eq(readHexFile("testfiles/testfilesegmentaddress.hex", &device, programData), EXIT_SUCCESS);
	ck_assert_uint_eq(programData[0x27], 0xFF);
	ck_assert_uint_eq(programData[0x28], 0xFB);
	ck_assert_uint_eq(programData[0x29], 0xCF);
	ck_assert_uint_eq(programData[0x2A], 0x05);
	ck_assert_uint_eq(programData[0x36], 0x08);
	ck_assert_uint_eq(programData[0x37], 0xF0);
	ck_assert_uint_eq(programData[0x38], 0xFF);
}
END_TEST


Suite	*testSuite()
{
	Suite *suite;
	TCase *testCase;


	suite = suite_create("Hex Input");


	// test cases for calculating a checksum
	testCase = tcase_create("calculateChecksum");
	tcase_add_test(testCase, calculateChecksumTest);
	suite_add_tcase(suite, testCase);

	// test cases for saving a hex line to the program data
	testCase = tcase_create("saveHexLineToProgramData");
	tcase_add_test(testCase, saveHexLineToProgramDataTest);
	suite_add_tcase(suite, testCase);

	// test cases for converting a buffer with hex digits to decimal digits
	testCase = tcase_create("convertHexToDecBuffer");
	tcase_add_test(testCase, convertHexToDecBufferTest);
	suite_add_tcase(suite, testCase);

	// test cases for reading complete hex files
	testCase = tcase_create("readHexFile");
	tcase_add_test(testCase, readHexFileTest);
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

