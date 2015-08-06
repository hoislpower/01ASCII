#include <config.h>
#include <check.h>

#include "../src/scanner.h"


// scan an input file
START_TEST(scanFileTest)
{
	// initialize the scanner
	ck_assert_int_eq(initializeScanner("testfiles/scanner.dev"), EXIT_SUCCESS);

	// expect '='
	ck_assert_int_eq(getCurrentSymbol(), EQUAL_SYM);
	ck_assert_int_eq(getCurrentLine(), 1);
	ck_assert_int_eq(getCurrentColumn(), 2);

	// expect new line
	ck_assert_int_eq(scanNextSymbol(), NEWLINE_SYM);
	ck_assert_int_eq(getCurrentLine(), 1);
	ck_assert_int_eq(getCurrentColumn(), 4);

	// expect ident
	ck_assert_int_eq(scanNextSymbolSkipNewline(), IDENT_SYM);
	ck_assert_str_eq(getCurrentIdentName(), "wident");
	ck_assert_int_eq(getCurrentLine(), 2);
	ck_assert_int_eq(getCurrentColumn(), 3);

	// expect '='
	ck_assert_int_eq(scanNextSymbolSkipNewline(), EQUAL_SYM);
	ck_assert_int_eq(getCurrentLine(), 2);
	ck_assert_int_eq(getCurrentColumn(), 11);

	// expect decimal number
	ck_assert_int_eq(scanNextSymbolSkipNewline(), DEC_NUMBER_SYM);
	ck_assert_int_eq(getCurrentNumberValue(), 0);
	ck_assert_int_eq(getCurrentLine(), 2);
	ck_assert_int_eq(getCurrentColumn(), 13);

	// expect ident
	ck_assert_int_eq(scanNextSymbolSkipNewline(), IDENT_SYM);
	ck_assert_str_eq(getCurrentIdentName(), "xident");
	ck_assert_int_eq(getCurrentLine(), 4);
	ck_assert_int_eq(getCurrentColumn(), 1);

	// expect '='
	ck_assert_int_eq(scanNextSymbolSkipNewline(), EQUAL_SYM);
	ck_assert_int_eq(getCurrentLine(), 4);
	ck_assert_int_eq(getCurrentColumn(), 8);

	// expect hexadecimal number
	ck_assert_int_eq(scanNextSymbolSkipNewline(), HEX_NUMBER_SYM);
	ck_assert_int_eq(getCurrentNumberValue(), 0x400);
	ck_assert_int_eq(getCurrentLine(), 4);
	ck_assert_int_eq(getCurrentColumn(), 10);

	// expect ident
	ck_assert_int_eq(scanNextSymbolSkipNewline(), IDENT_SYM);
	ck_assert_str_eq(getCurrentIdentName(), "yident");
	ck_assert_int_eq(getCurrentLine(), 6);
	ck_assert_int_eq(getCurrentColumn(), 1);

	// expect new line
	ck_assert_int_eq(scanNextSymbol(), NEWLINE_SYM);
	ck_assert_int_eq(getCurrentLine(), 6);
	ck_assert_int_eq(getCurrentColumn(), 8);

	// expect new line
	ck_assert_int_eq(scanNextSymbol(), NEWLINE_SYM);
	ck_assert_int_eq(getCurrentLine(), 7);
	ck_assert_int_eq(getCurrentColumn(), 1);

	// expect new line
	ck_assert_int_eq(scanNextSymbol(), NEWLINE_SYM);
	ck_assert_int_eq(getCurrentLine(), 8);
	ck_assert_int_eq(getCurrentColumn(), 1);

	// expect ident
	ck_assert_int_eq(scanNextSymbol(), IDENT_SYM);
	ck_assert_str_eq(getCurrentIdentName(), "zident");
	ck_assert_int_eq(getCurrentLine(), 9);
	ck_assert_int_eq(getCurrentColumn(), 1);

	// expect '='
	ck_assert_int_eq(scanNextSymbolSkipNewline(), EQUAL_SYM);
	ck_assert_int_eq(getCurrentLine(), 9);
	ck_assert_int_eq(getCurrentColumn(), 8);

	// expect decimal number
	ck_assert_int_eq(scanNextSymbolSkipNewline(), DEC_NUMBER_SYM);
	ck_assert_int_eq(getCurrentNumberValue(), 8192);
	ck_assert_int_eq(getCurrentLine(), 9);
	ck_assert_int_eq(getCurrentColumn(), 10);

	// expect '"'
	ck_assert_int_eq(scanNextSymbolSkipNewline(), DOUBLE_QUOTE_SYM);
	ck_assert_int_eq(getCurrentLine(), 11);
	ck_assert_int_eq(getCurrentColumn(), 1);

	// expect ident
	ck_assert_int_eq(scanNextSymbolSkipNewline(), IDENT_SYM);
	ck_assert_str_eq(getCurrentIdentName(), "stringstring");
	ck_assert_int_eq(getCurrentLine(), 11);
	ck_assert_int_eq(getCurrentColumn(), 2);

	// expect '''
	ck_assert_int_eq(scanNextSymbolSkipNewline(), SINGLE_QUOTE_SYM);
	ck_assert_int_eq(getCurrentLine(), 11);
	ck_assert_int_eq(getCurrentColumn(), 14);

	// expect '{'
	ck_assert_int_eq(scanNextSymbolSkipNewline(), LEFT_CURLY_BRACKET_SYM);
	ck_assert_int_eq(getCurrentLine(), 12);
	ck_assert_int_eq(getCurrentColumn(), 3);

	// expect ')'
	ck_assert_int_eq(scanNextSymbolSkipNewline(), RIGHT_PARENTHESIS_SYM);
	ck_assert_int_eq(getCurrentLine(), 12);
	ck_assert_int_eq(getCurrentColumn(), 4);

	// expect ','
	ck_assert_int_eq(scanNextSymbolSkipNewline(), COMMA_SYM);
	ck_assert_int_eq(getCurrentLine(), 12);
	ck_assert_int_eq(getCurrentColumn(), 5);

	// expect '*'
	ck_assert_int_eq(scanNextSymbolSkipNewline(), ASTERISK_SYM);
	ck_assert_int_eq(getCurrentLine(), 12);
	ck_assert_int_eq(getCurrentColumn(), 6);

	// expect '-'
	ck_assert_int_eq(scanNextSymbolSkipNewline(), HYPHEN_SYM);
	ck_assert_int_eq(getCurrentLine(), 12);
	ck_assert_int_eq(getCurrentColumn(), 7);

	// expect '('
	ck_assert_int_eq(scanNextSymbolSkipNewline(), LEFT_PARENTHESIS_SYM);
	ck_assert_int_eq(getCurrentLine(), 12);
	ck_assert_int_eq(getCurrentColumn(), 8);

	// expect '}'
	ck_assert_int_eq(scanNextSymbolSkipNewline(), RIGHT_CURLY_BRACKET_SYM);
	ck_assert_int_eq(getCurrentLine(), 12);
	ck_assert_int_eq(getCurrentColumn(), 9);

	// expect unknown symbol
	ck_assert_int_eq(scanNextSymbolSkipNewline(), UNKNOWN_SYM);
	ck_assert_int_eq(getCurrentLine(), 13);
	ck_assert_int_eq(getCurrentColumn(), 5);

	// expect end of file
	ck_assert_int_eq(scanNextSymbolSkipNewline(), EOF_SYM);
	ck_assert_int_eq(getCurrentLine(), 14);
	ck_assert_int_eq(getCurrentColumn(), 1);

	// dispose the scanner
	disposeScanner();
}
END_TEST


Suite	*testSuite()
{
	Suite *suite;
	TCase *testCase;


	suite = suite_create("Scanner");


	// test cases for 32 vs 64 bit support
	testCase = tcase_create("scanFile");
	tcase_add_test(testCase, scanFileTest);
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
