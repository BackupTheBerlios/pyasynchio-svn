import unittest
suite = unittest.TestSuite()

import echo
suite.addTest(echo.suite)
import files
suite.addTest(files.suite)