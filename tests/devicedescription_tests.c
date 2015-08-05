#include <config.h>
#include <check.h>

#include <string.h>
#include "../src/device-description.h"


// check if the support of 32 and 64 bit is set correctly
START_TEST(uint32_64_t_bit_size)
{
	if(USING_64BIT)
	{
		ck_assert_uint_eq(sizeof(uint32_64_t)*8, 64);
	}
	else
	{
		ck_assert_uint_eq(sizeof(uint32_64_t)*8, 32);
	}
}
END_TEST


// check the comparison routine for bit order arrays
START_TEST(bitOrderComparisonTest)
{
	int8_t bitOrder1[] = {
				0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 
				0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 
				0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, -1 };

	int8_t bitOrder2[] = {
				0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 
				0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 
				0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, -1 };
	int8_t bitOrder3[] = {
				0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 
				0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 
				0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 
				0x03, 0x13, 0x23, 0x33, 0x43, 0x53, 0x63, 0x73, -1 };

	int8_t bitOrder4[] = {
				0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 
				0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 
				0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 
				0x03, 0x13, 0x23, 0x33, 0x43, 0x53, 0x63, 0x73, 0x00 };

	int8_t bitOrder5[] = {-1};

	// compare bit order arrays
	ck_assert_int_eq(bitOrdersAreEqual(bitOrder1, bitOrder1), true);
	ck_assert_int_eq(bitOrdersAreEqual(bitOrder1, bitOrder2), true);
	ck_assert_int_eq(bitOrdersAreEqual(bitOrder1, bitOrder3), false);

	#if(USING_64BIT == 1)
	ck_assert_int_eq(bitOrdersAreEqual(bitOrder3, bitOrder4), false);
	#else
	ck_assert_int_eq(bitOrdersAreEqual(bitOrder3, bitOrder4), true);
	#endif

	ck_assert_int_eq(bitOrdersAreEqual(bitOrder5, bitOrder5), true);
	ck_assert_int_eq(bitOrdersAreEqual(bitOrder1, bitOrder5), false);

}
END_TEST


// check the initialization routine of the device data structure
START_TEST(deviceDataInitializationTest)
{
	int i;
	deviceData device;


	// initialize the device data structure
	initializeDeviceData(&device);

	// check initialization of the filename
	for(i=0; i<MAX_DEVICE_NAME_LENGTH; i++)
	{
		ck_assert_uint_eq(device.name[i], 0);
	}

	// check default values
	ck_assert_uint_eq(device.memorySize, 0);
	ck_assert_uint_eq(device.blockSize, 0);
	ck_assert_uint_eq(device.startAddress, DEFAULT_START_ADDRESS);
	ck_assert_uint_eq(device.addressStepPerWord, DEFAULT_ADDRESS_STEP_PER_WORD);
	ck_assert_uint_eq(device.wordLength, DEFAULT_WORD_LENGTH);
	ck_assert_uint_eq(device.addressLength, DEFAULT_ADDRESS_LENGTH);

	// check initialization of the bit order arrays
	for(i=0; i<MAX_BIT_ORDER_LENGTH; i++)
	{
		ck_assert_int_eq(device.wordBitOrder[PROGRAM][i], -1);
		ck_assert_int_eq(device.wordBitOrder[VERIFY][i], -1);
		ck_assert_int_eq(device.wordAddressBitOrder[PROGRAM][i], -1);
		ck_assert_int_eq(device.wordAddressBitOrder[VERIFY][i], -1);
		ck_assert_int_eq(device.preDataBlockAddrBitOrder[PROGRAM][i], -1);
		ck_assert_int_eq(device.preDataBlockAddrBitOrder[VERIFY][i], -1);
		ck_assert_int_eq(device.postDataBlockAddrBitOrder[PROGRAM][i], -1);
		ck_assert_int_eq(device.postDataBlockAddrBitOrder[VERIFY][i], -1);
	}
}
END_TEST


// try to load some invalid files
START_TEST(loadInvalidFileTest)
{
	deviceData device;


	// initialize the device data structure
	initializeDeviceData(&device);

	// try to load files
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/nonexistentfile"), EXIT_FAILURE);
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdevice0BytesSize"), EXIT_FAILURE);
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdeviceInvalidMaxWordLength"), EXIT_FAILURE);
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdevice1BytesSize"), EXIT_FAILURE);
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdeviceInvalidVersion"), EXIT_FAILURE);
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdevice10BytesSize"), EXIT_FAILURE);
}
END_TEST


// check the ability to load a 32bit file
START_TEST(load32BitFileTest)
{
	deviceData device;
	int8_t bitOrder[] = {
				0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 
				0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 
				0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 
				0x03, 0x13, 0x23, 0x33, 0x43, 0x53, 0x63, 0x73, -1 };


	// load file
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdevice32"), EXIT_SUCCESS);

	// name
	ck_assert_str_eq(device.name, "testdevice32");

	// memorySize
	ck_assert_uint_eq(device.memorySize, 0x40302010);

	// blockSize
	ck_assert_uint_eq(device.blockSize, 16);

	// startAddress
	ck_assert_uint_eq(device.startAddress, 0);

	// addressStepPerWord
	ck_assert_uint_eq(device.addressStepPerWord, 1);

	// wordLength
	ck_assert_uint_eq(device.wordLength, 16);

	// addressLength
	ck_assert_uint_eq(device.addressLength, 11);

	// wordBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.wordBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.wordBitOrder[VERIFY], bitOrder), true);

	// wordAddressBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.wordAddressBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.wordAddressBitOrder[VERIFY], bitOrder), true);

	// blockAddressBeforeDataBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.preDataBlockAddrBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.preDataBlockAddrBitOrder[VERIFY], bitOrder), true);

	// blockAddressAfterDataBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.postDataBlockAddrBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.postDataBlockAddrBitOrder[VERIFY], bitOrder), true);
}
END_TEST


// check the ability to load a 32bit compatible 64bit file
START_TEST(load32BitCompatible64BitFileTest)
{
	deviceData device;
	int8_t bitOrder[] = {
				0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 
				0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 
				0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 
				0x03, 0x13, 0x23, 0x33, 0x43, 0x53, 0x63, 0x73, -1 };


	// try to load file with too long bit orders
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdevice64notcomp32"), EXIT_FAILURE);

	// load file
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdevice64comp32"), EXIT_SUCCESS);

	// name
	ck_assert_str_eq(device.name, "testdevice64comp32");

	// memorySize
	ck_assert_uint_eq(device.memorySize, 0x40302010);

	// blockSize
	ck_assert_uint_eq(device.blockSize, 16);

	// startAddress
	ck_assert_uint_eq(device.startAddress, 0);

	// addressStepPerWord
	ck_assert_uint_eq(device.addressStepPerWord, 1);

	// wordLength
	ck_assert_uint_eq(device.wordLength, 16);

	// addressLength
	ck_assert_uint_eq(device.addressLength, 11);

	// wordBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.wordBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.wordBitOrder[VERIFY], bitOrder), true);

	// wordAddressBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.wordAddressBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.wordAddressBitOrder[VERIFY], bitOrder), true);

	// blockAddressBeforeDataBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.preDataBlockAddrBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.preDataBlockAddrBitOrder[VERIFY], bitOrder), true);

	// blockAddressAfterDataBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.postDataBlockAddrBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.postDataBlockAddrBitOrder[VERIFY], bitOrder), true);
}
END_TEST


// check the ability to load a 64bit file
START_TEST(load64BitFileTest)
{
	deviceData device;
	int8_t bitOrder[] = {
				0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 
				0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 
				0x02, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72, 
				0x03, 0x13, 0x23, 0x33, 0x43, 0x53, 0x63, 0x73,
				0x04, 0x14, 0x24, 0x34, 0x44, 0x54, 0x64, 0x74, 
				0x05, 0x15, 0x25, 0x35, 0x45, 0x55, 0x65, 0x75, -1 };


	#if(USING_64BIT == 0)
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdevice64"), EXIT_FAILURE);
	#else
	// load file
	ck_assert_int_eq(loadDeviceDescription(&device, "testfiles/testdevice64"), EXIT_SUCCESS);

	// name
	ck_assert_str_eq(device.name, "testdevice64");

	// memorySize
	ck_assert_uint_eq(device.memorySize, 0x8070605040302010);

	// blockSize
	ck_assert_uint_eq(device.blockSize, 0x100000000);

	// startAddress
	ck_assert_uint_eq(device.startAddress, 0x800000000);

	// addressStepPerWord
	ck_assert_uint_eq(device.addressStepPerWord, 4);

	// wordLength
	ck_assert_uint_eq(device.wordLength, 48);

	// addressLength
	ck_assert_uint_eq(device.addressLength, 48);

	// wordBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.wordBitOrder[PROGRAM], bitOrder), true);

	ck_assert_int_eq(bitOrdersAreEqual(device.wordBitOrder[VERIFY], bitOrder), true);

	// wordAddressBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.wordAddressBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.wordAddressBitOrder[VERIFY], bitOrder), true);

	// blockAddressBeforeDataBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.preDataBlockAddrBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.preDataBlockAddrBitOrder[VERIFY], bitOrder), true);

	// blockAddressAfterDataBitOrder
	ck_assert_int_eq(bitOrdersAreEqual(device.postDataBlockAddrBitOrder[PROGRAM], bitOrder), true);
	ck_assert_int_eq(bitOrdersAreEqual(device.postDataBlockAddrBitOrder[VERIFY], bitOrder), true);
	#endif
}
END_TEST


START_TEST(save32BitFileTest)
{
	int i;
	deviceData device;


	// fill datastructure with data
	initializeDeviceData(&device);
	strcpy(device.name, "testdevice");
	device.memorySize = 0x40302010;
	device.blockSize = 16;
	device.startAddress = 0;
	device.addressStepPerWord = 1;
	device.wordLength = 16;
	device.addressLength = 11;

	// set bit orders to defined values
	for(i=0; i<MAX_BIT_ORDER_LENGTH; i++)
	{
		device.wordBitOrder[0][i] = 0x51;
		device.wordBitOrder[1][i] = 0x52;
		device.wordAddressBitOrder[0][i] = 0x53;
		device.wordAddressBitOrder[1][i] = 0x54;
		device.preDataBlockAddrBitOrder[0][i] = 0x55;
		device.preDataBlockAddrBitOrder[1][i] = 0x56;
		device.postDataBlockAddrBitOrder[0][i] = 0x57;
		device.postDataBlockAddrBitOrder[1][i] = 0x58;
	}

	// save the device data structure
	ck_assert_int_eq(saveDeviceDescription(&device, "testdevice"), EXIT_SUCCESS);


	// reload the device data structure
	ck_assert_int_eq(loadDeviceDescription(&device, "testdevice"), EXIT_SUCCESS);

	// name
	ck_assert_str_eq(device.name, "testdevice");

	// memorySize
	ck_assert_uint_eq(device.memorySize, 0x40302010);

	// blockSize
	ck_assert_uint_eq(device.blockSize, 16);

	// startAddress
	ck_assert_uint_eq(device.startAddress, 0);

	// addressStepPerWord
	ck_assert_uint_eq(device.addressStepPerWord, 1);

	// wordLength
	ck_assert_uint_eq(device.wordLength, 16);

	// addressLength
	ck_assert_uint_eq(device.addressLength, 11);

	// check the bit order arrays
	for(i=0; i<MAX_BIT_ORDER_LENGTH; i++)
	{
		ck_assert_int_eq(device.wordBitOrder[PROGRAM][i], 0x51);
		ck_assert_int_eq(device.wordBitOrder[VERIFY][i], 0x52);
		ck_assert_int_eq(device.wordAddressBitOrder[PROGRAM][i], 0x53);
		ck_assert_int_eq(device.wordAddressBitOrder[VERIFY][i], 0x54);
		ck_assert_int_eq(device.preDataBlockAddrBitOrder[PROGRAM][i], 0x55);
		ck_assert_int_eq(device.preDataBlockAddrBitOrder[VERIFY][i], 0x56);
		ck_assert_int_eq(device.postDataBlockAddrBitOrder[PROGRAM][i], 0x57);
		ck_assert_int_eq(device.postDataBlockAddrBitOrder[VERIFY][i], 0x58);
	}
}
END_TEST


START_TEST(save64BitFileTest)
{
	int i;
	deviceData device;


	// fill datastructure with data
	initializeDeviceData(&device);
	strcpy(device.name, "testdevice");
	device.memorySize = 0x8070605040302010;
	device.blockSize = 0x100000000;
	device.startAddress = 0x800000000;
	device.addressStepPerWord = 4;
	device.wordLength = 48;
	device.addressLength = 48;

	// set bit orders to defined values
	for(i=0; i<MAX_BIT_ORDER_LENGTH; i++)
	{
		device.wordBitOrder[0][i] = 0x51;
		device.wordBitOrder[1][i] = 0x52;
		device.wordAddressBitOrder[0][i] = 0x53;
		device.wordAddressBitOrder[1][i] = 0x54;
		device.preDataBlockAddrBitOrder[0][i] = 0x55;
		device.preDataBlockAddrBitOrder[1][i] = 0x56;
		device.postDataBlockAddrBitOrder[0][i] = 0x57;
		device.postDataBlockAddrBitOrder[1][i] = 0x58;
	}

	// save the device data structure
	ck_assert_int_eq(saveDeviceDescription(&device, "testdevice"), EXIT_SUCCESS);


	// reload the device data structure
	ck_assert_int_eq(loadDeviceDescription(&device, "testdevice"), EXIT_SUCCESS);

	// name
	ck_assert_str_eq(device.name, "testdevice");

	// memorySize
	ck_assert_uint_eq(device.memorySize, 0x8070605040302010);

	// blockSize
	ck_assert_uint_eq(device.blockSize, 0x100000000);

	// startAddress
	ck_assert_uint_eq(device.startAddress, 0x800000000);

	// addressStepPerWord
	ck_assert_uint_eq(device.addressStepPerWord, 4);

	// wordLength
	ck_assert_uint_eq(device.wordLength, 48);

	// addressLength
	ck_assert_uint_eq(device.addressLength, 48);

	// check the bit order arrays
	for(i=0; i<MAX_BIT_ORDER_LENGTH; i++)
	{
		ck_assert_int_eq(device.wordBitOrder[PROGRAM][i], 0x51);
		ck_assert_int_eq(device.wordBitOrder[VERIFY][i], 0x52);
		ck_assert_int_eq(device.wordAddressBitOrder[PROGRAM][i], 0x53);
		ck_assert_int_eq(device.wordAddressBitOrder[VERIFY][i], 0x54);
		ck_assert_int_eq(device.preDataBlockAddrBitOrder[PROGRAM][i], 0x55);
		ck_assert_int_eq(device.preDataBlockAddrBitOrder[VERIFY][i], 0x56);
		ck_assert_int_eq(device.postDataBlockAddrBitOrder[PROGRAM][i], 0x57);
		ck_assert_int_eq(device.postDataBlockAddrBitOrder[VERIFY][i], 0x58);
	}
}
END_TEST


Suite	*testSuite()
{
	Suite *suite;
	TCase *testCase;


	suite = suite_create("Device Description");


	// test cases for 32 vs 64 bit support
	testCase = tcase_create("32_64_bit");
	tcase_add_test(testCase, uint32_64_t_bit_size);
	suite_add_tcase(suite, testCase);

	// test comparison routine for bit order arrays
	testCase = tcase_create("bitOrderComparison");
	tcase_add_test(testCase, bitOrderComparisonTest);
	suite_add_tcase(suite, testCase);

	// test initialization routine of device data structure
	testCase = tcase_create("deviceDataInitialization");
	tcase_add_test(testCase, deviceDataInitializationTest);
	suite_add_tcase(suite, testCase);

	// test cases for trying to load some invalid files
	testCase = tcase_create("loadInvalidFile");
	tcase_add_test(testCase, loadInvalidFileTest);
	suite_add_tcase(suite, testCase);

	// test cases for loading a 32bit device description file
	testCase = tcase_create("load32BitFile");
	tcase_add_test(testCase, load32BitFileTest);
	suite_add_tcase(suite, testCase);

	#if(USING_64BIT == 0)
	// test cases for loading a 64bit device description file
	// that can be loaded in a 32 bit data structure
	testCase = tcase_create("load32BitCompatible64BitFile");
	tcase_add_test(testCase, load32BitCompatible64BitFileTest);
	suite_add_tcase(suite, testCase);
	#endif

	// test cases for loading a 64bit device description file
	testCase = tcase_create("load64BitFile");
	tcase_add_test(testCase, load64BitFileTest);
	suite_add_tcase(suite, testCase);

	// test cases for saving a device description file
	#if(USING_64BIT == 0)
	testCase = tcase_create("save32BitFile");
	tcase_add_test(testCase, save32BitFileTest);
	suite_add_tcase(suite, testCase);
	#else
	testCase = tcase_create("save64BitFile");
	tcase_add_test(testCase, save64BitFileTest);
	suite_add_tcase(suite, testCase);
	#endif

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

