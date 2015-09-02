#include <config.h>
#include <check.h>

#include "../src/bit-array.h"


#define	ARRAY_SIZE	12


// test all bit array procedures and functions
START_TEST(bitArrayTest)
{
	int i;
	int8_t *array;


	// initialize the bit array with size ARRAY_SIZE
	ck_assert_int_eq(initializeBitArray(ARRAY_SIZE), EXIT_SUCCESS);

	// check if a valid array will be returned
	array = NULL;
	array = getBitArray();
	ck_assert_int_ne(array, NULL);

	// check the current index
	ck_assert_int_eq(bitArrayGetCurrentIndex(), 0);

	// check if the array has been initialized
	for(i=0; i<ARRAY_SIZE; i++)
		ck_assert_int_eq(array[i], UNUSED_BIT);

	// add values to the bit array
	ck_assert_int_eq(bitArrayAdd(LITERAL1_BIT, LITERAL1_BIT), EXIT_SUCCESS);
	ck_assert_int_eq(bitArrayAdd(5, 5), EXIT_SUCCESS);
	ck_assert_int_eq(bitArrayAdd(LITERAL0_BIT, LITERAL0_BIT), EXIT_SUCCESS);
	ck_assert_int_eq(bitArrayAdd(6, 3), EXIT_SUCCESS);

	// try to add too much values to the array
	ck_assert_int_eq(bitArrayAdd(1, 6), EXIT_FAILURE);

	// check the current index
	ck_assert_int_eq(bitArrayGetCurrentIndex(), 7);

	// check the content of the array
	ck_assert_int_eq(array[0], LITERAL1_BIT);
	ck_assert_int_eq(array[1], 5);
	ck_assert_int_eq(array[2], LITERAL0_BIT);
	ck_assert_int_eq(array[3], 6);
	ck_assert_int_eq(array[4], 5);
	ck_assert_int_eq(array[5], 4);
	ck_assert_int_eq(array[6], 3);
	ck_assert_int_eq(array[7], UNUSED_BIT);
	ck_assert_int_eq(array[8], UNUSED_BIT);
	ck_assert_int_eq(array[9], UNUSED_BIT);
	ck_assert_int_eq(array[10], UNUSED_BIT);
	ck_assert_int_eq(array[11], UNUSED_BIT);


	// try to repeat a part of the array
	ck_assert_int_eq(bitArrayRepeat(3, 4, 3), EXIT_FAILURE);

	// repeat a part of the array
	ck_assert_int_eq(bitArrayRepeat(3, 4, 2), EXIT_SUCCESS);

	// try an invalid repeat
	ck_assert_int_eq(bitArrayRepeat(5, 3, 1), EXIT_SUCCESS);

	// check the current index
	ck_assert_int_eq(bitArrayGetCurrentIndex(), 11);

	// check the content of the array
	ck_assert_int_eq(array[0], LITERAL1_BIT);
	ck_assert_int_eq(array[1], 5);
	ck_assert_int_eq(array[2], LITERAL0_BIT);
	ck_assert_int_eq(array[3], 6);
	ck_assert_int_eq(array[4], 5);
	ck_assert_int_eq(array[5], 6);
	ck_assert_int_eq(array[6], 5);
	ck_assert_int_eq(array[7], 6);
	ck_assert_int_eq(array[8], 5);
	ck_assert_int_eq(array[9], 4);
	ck_assert_int_eq(array[10], 3);
	ck_assert_int_eq(array[11], UNUSED_BIT);

	// add one more value to the array
	ck_assert_int_eq(bitArrayAdd(9, 9), EXIT_SUCCESS);

	// check the current index
	ck_assert_int_eq(bitArrayGetCurrentIndex(), 12);

	// try to add one more value to the array
	ck_assert_int_eq(bitArrayAdd(8, 8), EXIT_FAILURE);

	// dispose the array
	disposeBitArray();
}
END_TEST


Suite	*testSuite()
{
	Suite *suite;
	TCase *testCase;


	suite = suite_create("Bit Array");


	// test cases for bit array procedures an functions
	testCase = tcase_create("bitArray");
	tcase_add_test(testCase, bitArrayTest);
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
