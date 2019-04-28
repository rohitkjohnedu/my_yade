import unittest
import yade.libVersions

class TestLibVersions(unittest.TestCase):
	def setUp(self):
		pass # no setup needed for tests here
	def testLibVersions(self):
		# calling them is enough, to make sure they do not throw any errors.
		print("cmake versions: ", yade.libVersions.allVersionsCmake())
		print("C++ versions: ", yade.libVersions.allVersionsCpp())
		print("\n")
		yade.libVersions.printAllVersions(True)
		yade.libVersions.printAllVersions(False)
