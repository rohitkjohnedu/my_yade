import unittest
import yade.libVersions

class TestLibVersions(unittest.TestCase):
	def setUp(self):
		pass # no setup needed for tests here
	def testLibVersions(self):
		# calling them is enough, to make sure they do not throw any errors.
		cmakeVers = yade.libVersions.allVersionsCmake()
		cppVers   = yade.libVersions.allVersionsCpp()
		print("\n")
		yade.libVersions.printAllVersions(True)
		yade.libVersions.printAllVersions(False)
