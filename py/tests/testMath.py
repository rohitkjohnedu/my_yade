# -*- coding: utf-8 -*-
# This is the test of all C++ lib/high-precision/MathFunctions.hpp exported to python via py/high-precision/_math.cpp
# (C) 2015 Anton Gladky <gladk@debian.org>
# (C) 2019 Janek Kozicki

import unittest, math, sys
import yade
from yade import math as mth

import testMathHelper as mpmath
from   testMathHelper import mpc

class SimpleTests(unittest.TestCase):
	def setUp(self):
		self.printedAlready=set()
		self.extraName=""
		# the non-boost MPFR is not used in yade, that was just for more broader testing in minieigen-real, it used self.extraName="nb"
		#if(str("${LIBTOTEST}")[-2:] == "nb"):   extraName="nb" # non-boost MPFR
		self.nonBoostMPFR=False
		#if(str("${LIBTOTEST}")[-4:] == "BBFL"): extraName="_b" # boost cpp_bin_float
		if(yade.config.highPrecisionName.startswith("PrecisionBoost")):
			self.extraName="_b"
		# If failures appear and function is not broken then increase tolerance a little.
		self.defaultTolerances={
			#  function decimal places : tolerance factor. Each "10" corresponds to single wrong decimal place.
			#
			#                 float   double    long double float128        MPFR_100        MPFR_150     cpp_bin_float_100  cpp_bin_float_150
			#
			   "acos"      : {"6":100 , "15":100 , "18":100  , "33":1000   , "100":1000  , "150" :1000  , "100_b" :1000    , "150_b" :1000   }
			 , "atanh"     : {"6":100 , "15":100 , "18":100  , "33":1000   , "100":1000  , "150" :1000  , "100_b" :1000    , "150_b" :1000   }
			 , "acosh"     : {"6":100 , "15":100 , "18":100  , "33":1000   , "100":1000  , "150" :1000  , "100_b" :1000    , "150_b" :1000   }
			 , "asin"      : {"6":100 , "15":100 , "18":100  , "33":1000   , "100":1000  , "150" :1000  , "100_b" :1000    , "150_b" :1000   }

			# maybe the error lies in  mpmath, b ecause everything is compared with it.
			 , "sin"       : {"6":100 , "15":100 , "18":20000, "33":4000   , "100":80000 , "150" :80000 , "100_b" :800000  , "150_b" :800000 }
			 , "cos"       : {"6":100 , "15":100 , "18":20000, "33":4000   , "100":80000 , "150" :80000 , "100_b" :800000  , "150_b" :800000 }
			 , "tan"       : {"6":100 , "15":100 , "18":20000, "33":4000   , "100":80000 , "150" :80000 , "100_b" :800000  , "150_b" :800000 }

			 , "csin"      : {"6":100 , "15":100 , "18":20000, "33":4000   , "100":80000 , "150" :80000 , "100_b" :800000  , "150_b" :800000 }
			 , "ccos"      : {"6":100 , "15":100 , "18":20000, "33":4000   , "100":80000 , "150" :80000 , "100_b" :800000  , "150_b" :800000 }
			 , "ctan"      : {"6":100 , "15":100 , "18":20000, "33":4000   , "100":80000 , "150" :80000 , "100_b" :800000  , "150_b" :800000 }

			 , "cexp"      : {"6":10  , "15":10  , "18":10   , "33":10     , "100":10    , "150" :10    , "100_b" :100     , "150_b" :100    }
			 , "exp"       : {"6":10  , "15":10  , "18":10   , "33":10     , "100":10    , "150" :10    , "100_b" :100     , "150_b" :100    }
			 , "exp2"      : {"6":10  , "15":10  , "18":10   , "33":10     , "100":10    , "150" :10    , "100_b" :100     , "150_b" :100    }
			 , "expm1"     : {"6":10  , "15":10  , "18":10   , "33":10     , "100":10    , "150" :10    , "100_b" :100     , "150_b" :100    }
			 , "cosh"      : {"6":10  , "15":10  , "18":10   , "33":10     , "100":10    , "150" :10    , "100_b" :100     , "150_b" :100    }
			 , "sinh"      : {"6":10  , "15":10  , "18":10   , "33":10     , "100":10    , "150" :10    , "100_b" :100     , "150_b" :100    }

			 , "ccosh"     : {"6":100 , "15":100 , "18":20000, "33":4000   , "100":80000 , "150" :80000 , "100_b" :800000  , "150_b" :800000 }
			 , "csinh"     : {"6":100 , "15":100 , "18":20000, "33":4000   , "100":80000 , "150" :80000 , "100_b" :800000  , "150_b" :800000 }
			 , "ctanh"     : {"6":100 , "15":100 , "18":20000, "33":4000   , "100":80000 , "150" :80000 , "100_b" :800000  , "150_b" :800000 }

			 , "clog"      : {"6":1000, "15":1000, "18":100  , "33":100    , "100":100   , "150" :100   , "100_b" :100     , "150_b" :100    }
			 , "log"       : {"6":1000, "15":1000, "18":100  , "33":100    , "100":100   , "150" :100   , "100_b" :100     , "150_b" :100    }
			 , "log10"     : {"6":1000, "15":1000, "18":100  , "33":100    , "100":100   , "150" :100   , "100_b" :100     , "150_b" :100    }
			 , "log1p"     : {"6":1000, "15":1000, "18":100  , "33":100    , "100":100   , "150" :100   , "100_b" :100     , "150_b" :100    }
			 , "log2"      : {"6":1000, "15":1000, "18":100  , "33":100    , "100":100   , "150" :100   , "100_b" :100     , "150_b" :100    }

			 , "pow"       : {"6":5   , "15":10  , "18":50   , "33":50     , "100":50    , "150" :50    , "100_b" :50      , "150_b" :50     }
			 , "sqrt"      : {"6":5   , "15":10  , "18":50   , "33":50     , "100":50    , "150" :50    , "100_b" :50      , "150_b" :50     }

			 , "lgamma"    : {"6":100 , "15":100 , "18":1000 , "33":10000  , "100":100000, "150" :100000, "100_b" :1000000 , "150_b" :1000000}
			 , "tgamma"    : {"6":100 , "15":100 , "18":1000 , "33":10000  , "100":100000, "150" :100000, "100_b" :1000000 , "150_b" :1000000}
			 , "erfc"      : {"6":100 , "15":100 , "18":2000 , "33":20000  , "100":200000, "150" :200000, "100_b" :4000000 , "150_b" :8000000}
			 , "erf"       : {"6":100 , "15":100 , "18":5    , "33":5      , "100":100   , "150" :100   , "100_b" :100     , "150_b" :100    }

			 , "modf"      : {"6":10  , "15":100 , "18":5000 , "33":300000 , "100":10000 , "150" :100000, "100_b" :10000   , "150_b" :10000  }
			 , "fmod"      : {"6":10  , "15":100 , "18":5000 , "33":10000  , "100":10000 , "150" :100000, "100_b" :10000   , "150_b" :10000  }
			 , "remainder" : {"6":100 , "15":5000, "18":5000 , "33":10000  , "100":10000 , "150" :100000, "100_b" :10000   , "150_b" :10000  }
			 , "remquo"    : {"6":100 , "15":5000, "18":5000 , "33":10000  , "100":10000 , "150" :100000, "100_b" :10000   , "150_b" :10000  }
			 , "fma"       : {"6":10  , "15":100 , "18":10   , "33":10     , "100":100   , "150" :100   , "100_b" :100     , "150_b" :1000   }
			 }
		self.extraStrDigits = mth.extraStringDigits
		self.testLevelsHP   = mth.getRealHPSupportedByPython()
		self.baseDigits     = mth.getRealHPDigits10(1)
		self.builtinHP      = { 6 : [6,15,18,24,33] , 15 : [15,33] } # higher precisions are multiplies of baseDigits, see NthLevelRealHP in lib/high-precision/RealHP.hpp

	def getDigitsHP(self,N):
		ret = None
		if (self.baseDigits in self.builtinHP) and (N <= len(self.builtinHP[self.baseDigits])):
			ret = self.builtinHP[self.baseDigits][N-1]
		else:
			ret = self.baseDigits*N
		self.assertEqual(ret,mth.getRealHPDigits10(N))
		return ret

	def adjustDigs0(self,N,HPn):
		self.HPnHelper = HPn
		self.digs0 = self.getDigitsHP(N)
		mpmath.mp.dps  = self.digs0 + self.extraStrDigits
		# tolerance = 1.2×10⁻ᵈ⁺¹, where ᵈ==self.digs0
		# so basically we store one more decimal digit, and expect one less decimal digit. That amounts to ignoring one (two, if the extra one is counted) least significant digits.
		self.tolerance   = (mpmath.mpf(10)**(-self.digs0+1))*mpmath.mpf("1.2")
		if(self.extraName == "_b"):
			self.bits= mpmath.ceil(mpmath.mpf(self.digs0)/(mpmath.log(2)/mpmath.log(10)))+1 # TODO: file a bug report against MPFR, they don't use this formula for number of bits
			mpmathVsMpfrBits = 0
		else:
			self.bits= mpmath.ceil(mpmath.mpf(self.digs0)/(0.301))+1                        # it is reproducing MPFR's formula for number of bits. Discovered by experiments.
			mpmathVsMpfrBits = int(self.bits / 2992) # adjust discrepency between mpmath and MPFR due to incorrect log10/log2 value (above line). The 2992 was found empirically. Adjustments are possible.
		# mpmath has 5 more internal bits
		self.expectedEpsilon=(2**5)*mpmath.eps() / (2**mpmathVsMpfrBits)
		if(self.digs0 == 6): # float case
			self.bits=24
			self.expectedEpsilon=1.1920928955078125e-07
		if(self.digs0 == 15): # double case
			self.bits=53
			self.expectedEpsilon=2.220446049250313e-16
		if(self.digs0 == 18): # long double case
			self.bits=64
			self.expectedEpsilon=mpmath.mpf('1.084202172485504433993e-19')
		if(self.digs0 == 33): # float128 case
			self.bits=113
			self.expectedEpsilon=mpmath.mpf('1.925929944387235853055977942584926994e-34')
		if(yade.config.highPrecisionMpmath):
			self.maxval=(mpmath.mpf(1)-self.expectedEpsilon)*mpmath.power(2,HPn.max_exp2)
		else:
			import sys
			self.maxval=sys.float_info.max

	def runCheck(self,N,func):
		HPn = getattr(mth,"HP" + str(N)); # the same as the line 'std::string name = "HP" + boost::lexical_cast<std::string>(N)' in function registerInScope in _math.cpp
		if(N==1):
			self.adjustDigs0(N,mth)
			func(N,mth)               # test global scope functions with RealHP<1>
		self.adjustDigs0(N,HPn)
		func(N,HPn)                       # test scopes HP1, HP2, etc

	def getDefaultTolerance(self,name):
		dictForThisFunc = self.defaultTolerances[name]
		key = str(self.digs0)+self.extraName
		if(key in dictForThisFunc):
			return dictForThisFunc[key]*self.tolerance
		## lower than 33 digits are all hardware precision: 6, 15, 18, 33 digits. But 4*float is 24 digits, and it can be achieved by MPFR only so add exception for 24 also.
		self.assertTrue(self.digs0 >= 33 or self.digs0==24) ## 33 was here before
		low = dictForThisFunc["100"+self.extraName]
		high= dictForThisFunc["150"+self.extraName]
		import numpy
		return numpy.interp(self.digs0,[100,150],[low,high])

	def printOnce(self,functionName,a):
		if(functionName and (functionName not in self.printedAlready) and (not mpmath.isnan(abs(a)))):
			self.printedAlready.add(functionName)
			print(functionName.ljust(15)+" : "+a.__repr__())

	def checkRelativeError(self,a,b,tol=None,functionName=None,isComplex=False):
		if(abs(b) <= self.maxval and abs(b) >= self.HPnHelper.smallest_positive()):
			#print("a= ",a," b= ",b," smallest=",self.HPnHelper.smallest_positive(), " maxval=",self.maxval)
			self.printOnce(functionName,a)
			if(mpmath.isnan(a)):
				if(functionName != "lgamma"): # lgamma triggers this warning too often.
					print("\033[93m Warning: \033[0m got NaN, cannot verify if: ",a," == " ,b, " that was for function: \033[93m ",functionName, " \033[0m")
			else:
				if(tol != None):
					if isComplex:
						self.assertLessEqual(abs( (mpmath.mpc(a)-mpmath.mpc(b))/mpmath.mpc(b) ),tol)
					else:
						self.assertLessEqual(abs( (mpmath.mpf(a)-mpmath.mpf(b))/mpmath.mpf(b) ),tol)
				else:
					if(functionName in self.defaultTolerances):
						defaultToleranceForThisFunction = self.getDefaultTolerance(functionName)
						#print(defaultToleranceForThisFunction," ---- ",functionName)
						if isComplex:
							self.assertLessEqual(abs( (mpmath.mpc(a)-mpmath.mpc(b))/mpmath.mpc(b) ),defaultToleranceForThisFunction)
						else:
							self.assertLessEqual(abs( (mpmath.mpf(a)-mpmath.mpf(b))/mpmath.mpf(b) ),defaultToleranceForThisFunction)
					else:
						if isComplex:
							self.assertLessEqual(abs( (mpmath.mpc(a)-mpmath.mpc(b))/mpmath.mpc(b) ),self.tolerance)
						else:
							self.assertLessEqual(abs( (mpmath.mpf(a)-mpmath.mpf(b))/mpmath.mpf(b) ),self.tolerance)
		else:
			print("Skipping ",functionName," check, the builtin number: ", a, " cannot have value outside of its possible repesentation: " , b, ", because it has only ",self.digs0," digits.")
	
	def checkRelativeComplexError(self,a,b,tol=None,functionName=None):
		self.printOnce(functionName,a)
		self.checkRelativeError(abs(a),abs(b),tol,functionName,True)

	def oneArgMathCheck(self,HPn,r):
		self.checkRelativeError(HPn.sin(r),mpmath.sin(r),functionName="sin")
		self.checkRelativeError(HPn.sinh(r),mpmath.sinh(r),functionName="sinh")
		self.checkRelativeError(HPn.cos(r),mpmath.cos(r),functionName="cos")
		self.checkRelativeError(HPn.cosh(r),mpmath.cosh(r),functionName="cosh")
		self.checkRelativeError(HPn.tan(r),mpmath.tan(r),functionName="tan")
		self.checkRelativeError(HPn.tanh(r),mpmath.tanh(r),functionName="tanh")
		# check math functions, but ensure that input arguments produce real (not complex) results
		self.checkRelativeError(HPn.abs(r),abs(r),functionName="abs")
		self.checkRelativeError(HPn.acos(r%1),mpmath.acos(r%1),functionName="acos")
		self.checkRelativeError(HPn.acosh(abs(r)+1),mpmath.acosh(abs(r)+1),functionName="acosh")
		self.checkRelativeError(HPn.asin(r%1),mpmath.asin(r%1),functionName="asin")
		self.checkRelativeError(HPn.asinh(r),mpmath.asinh(r),functionName="asinh")
		self.checkRelativeError(HPn.atan(r),mpmath.atan(r),functionName="atan")
		self.checkRelativeError(HPn.atanh(r%1),mpmath.atanh(r%1),functionName="atanh")
		self.checkRelativeError(HPn.cbrt(abs(r)),mpmath.cbrt(abs(r)),functionName="cbrt")
		self.assertEqual(HPn.ceil(r),mpmath.ceil(r))
		self.checkRelativeError(HPn.erf(r),mpmath.erf(r),functionName="erf")
		self.checkRelativeError(HPn.erfc(r),mpmath.erfc(r),functionName="erfc")
		self.checkRelativeError(HPn.exp(r),mpmath.exp(r),functionName="exp")
		self.checkRelativeError(HPn.sqrt(abs(r)),mpmath.sqrt(abs(r)),functionName="sqrt")
		self.checkRelativeError(HPn.exp2(r),mpmath.power(2,r),functionName="exp2")
		self.checkRelativeError(HPn.expm1(r),mpmath.expm1(r),functionName="expm1")
		self.assertEqual(HPn.floor(r),mpmath.floor(r))
		#print(HPn.ilogb(r).__repr__()) # ilogb is not present in mpmath
		self.checkRelativeError(HPn.lgamma(r),mpmath.log(abs(mpmath.gamma(r))),functionName="lgamma")
		self.checkRelativeError(HPn.log(abs(r)+self.tolerance),mpmath.log(abs(r)+self.tolerance),functionName="log")
		self.checkRelativeError(HPn.log10(abs(r)+self.tolerance),mpmath.log10(abs(r)+self.tolerance),functionName="log10")
		self.checkRelativeError(HPn.log1p(abs(r)+self.tolerance),mpmath.log(1+abs(r)+self.tolerance),functionName="log1p")
		self.checkRelativeError(HPn.log2(abs(r)+self.tolerance),mpmath.log(abs(r)+self.tolerance)/mpmath.log(2),functionName="log2")
		#print(HPn.logb(r).__repr__()) # logb is not present in mpmath
		self.assertEqual(HPn.rint(r),round(r))
		self.assertTrue((HPn.round(r)==round(r)) or (r%1==0.5)) # ignore rounding 0.5 up or down.
		self.checkRelativeError(HPn.tgamma(r),mpmath.gamma(r),functionName="tgamma")
		self.assertEqual(HPn.trunc(abs(r)),int(abs(r)))

		self.checkRelativeError(HPn.fabs(r),abs(r),functionName="fabs")

		pair = HPn.frexp(abs(r))
		self.checkRelativeError(abs(r),pair[0]*mpmath.power(2,pair[1]),functionName="frexp")

		pair = HPn.modf(abs(r))
		self.checkRelativeError(pair[0],(abs(r))%1,functionName="modf")
		self.assertEqual(pair[1],int(abs(r)))

		#self.assertEqual(HPn.frexp(abs(r)),HPn.frexp_c_test(abs(r)))
		#self.assertEqual(HPn.modf(abs(r)),HPn.modf_c_test(abs(r)))

		if(r==0):
			self.assertEqual(HPn.sgn(r),0)
			self.assertEqual(HPn.sign(r),0)
		if(r>0):
			self.assertEqual(HPn.sgn(r),1)
			self.assertEqual(HPn.sign(r),1)
		if(r<0):
			self.assertEqual(HPn.sgn(r),-1)
			self.assertEqual(HPn.sign(r),-1)

		self.checkCgalNumTraits(HPn,r)

	def checkCgalNumTraits(self,HPn,r):
		if(HPn.testCgalNumTraits==False):
			print("Skipping test of CgalNumTraits")
			return
		self.assertEqual(HPn.CGAL_Is_valid(r),True)
		if(r != 0):
			self.checkRelativeError(HPn.CGAL_Square(r),mpmath.power(r,2),functionName="pow")
			self.checkRelativeError(HPn.CGAL_Sqrt(abs(r)),mpmath.sqrt(abs(r)),functionName="sqrt")
			for kk in range(5):
				k=kk+1
				self.checkRelativeError(HPn.CGAL_Kth_root(k,abs(r)),mpmath.power(abs(r),1/mpmath.mpf(k)),functionName="pow")
			# CGAL uses double for intervals
			interval = HPn.CGAL_To_interval(r)
			self.checkRelativeError(r,interval[0],1e-14)
			self.checkRelativeError(r,interval[1],1e-14)
		self.assertEqual(HPn.CGAL_Is_finite(r),True)
		if(r==0): self.assertEqual(HPn.CGAL_Sgn(r),0)
		if(r> 0): self.assertEqual(HPn.CGAL_Sgn(r),1)
		if(r< 0): self.assertEqual(HPn.CGAL_Sgn(r),-1)
		self.assertEqual(HPn.CGAL_Sgn(0),0)
		self.assertEqual(HPn.CGAL_Sgn(2.5),1)
		self.assertEqual(HPn.CGAL_Sgn(-2.3),-1)

	def testInfinityNaN(self):
		for N in self.testLevelsHP:
			self.runCheck(N , self.HPtestInfinityNaN)

	def HPtestInfinityNaN(self,N,HPn):
		if(HPn.hasInfinityNan==False):
			print("Skipping inf,nan regular test\n")
			print("\033[91m *** Warning: usually YADE needs Inf and NaN for most of the calculations. *** \033[0m")
			return
		self.assertEqual(HPn.isinf   (mpmath.mpf( 1   )),False)
		self.assertEqual(HPn.isinf   (mpmath.mpf('nan')),False)
		self.assertEqual(HPn.isinf   (mpmath.mpf('inf')),True )
		self.assertEqual(HPn.isnan   (mpmath.mpf( 1   )),False)
		self.assertEqual(HPn.isnan   (mpmath.mpf('nan')),True )
		self.assertEqual(HPn.isnan   (mpmath.mpf('inf')),False)
		self.assertEqual(HPn.isfinite(mpmath.mpf( 1   )),True )
		self.assertEqual(HPn.isfinite(mpmath.mpf('nan')),False)
		self.assertEqual(HPn.isfinite(mpmath.mpf('inf')),False)

	def testCgalNumTraits(self):
		for N in self.testLevelsHP:
			self.runCheck(N , self.HPtestCgalNumTraits)

	def HPtestCgalNumTraits(self,N,HPn):
		if(HPn.testCgalNumTraits==False):
			print("Skipping test of CgalNumTraits")
			return
		self.checkCgalNumTraits(HPn,0)
		self.checkCgalNumTraits(HPn,0.5)
		self.checkCgalNumTraits(HPn,-1.5)
		self.checkCgalNumTraits(HPn,55.5)
		self.assertEqual(HPn.CGAL_Is_valid (mpmath.mpf( 1   )),True )
		self.assertEqual(HPn.CGAL_Is_valid (mpmath.mpf('nan')),False)
		self.assertEqual(HPn.CGAL_Is_valid (mpmath.mpf('inf')),True )
		self.assertEqual(HPn.CGAL_Is_finite(mpmath.mpf( 1   )),True )
		self.assertEqual(HPn.CGAL_Is_finite(mpmath.mpf('nan')),False)
		self.assertEqual(HPn.CGAL_Is_finite(mpmath.mpf('inf')),False)
		self.assertEqual(HPn.CGAL_simpleTest(),mpmath.mpf("3.0"))

	def twoArgMathCheck(self,HPn,r1,r2):
		self.checkRelativeComplexError(HPn.sin (mpmath.mpc(r1,r2)),mpmath.sin (mpmath.mpc(r1,r2)),functionName="csin")
		self.checkRelativeComplexError(HPn.sinh(mpmath.mpc(r1,r2)),mpmath.sinh(mpmath.mpc(r1,r2)),functionName="csinh")
		self.checkRelativeComplexError(HPn.cos (mpmath.mpc(r1,r2)),mpmath.cos (mpmath.mpc(r1,r2)),functionName="ccos")
		self.checkRelativeComplexError(HPn.cosh(mpmath.mpc(r1,r2)),mpmath.cosh(mpmath.mpc(r1,r2)),functionName="ccosh")
		self.checkRelativeComplexError(HPn.tan (mpmath.mpc(r1,r2)),mpmath.tan (mpmath.mpc(r1,r2)),functionName="ctan")
		self.checkRelativeComplexError(HPn.tanh(mpmath.mpc(r1,r2)),mpmath.tanh(mpmath.mpc(r1,r2)),functionName="ctanh")

		self.checkRelativeComplexError(HPn.exp(mpmath.mpc(r1,r2)),mpmath.exp(mpmath.mpc(r1,r2)),functionName="cexp")
		self.checkRelativeComplexError(HPn.log(mpmath.mpc(r1,r2)),mpmath.log(mpmath.mpc(r1,r2)),functionName="clog")

		self.checkRelativeComplexError(HPn.abs  (mpmath.mpc(r1,r2)),abs(mpmath.mpc(r1,r2)),functionName="cabs")
		self.checkRelativeComplexError(HPn.conj (mpmath.mpc(r1,r2)),mpmath.conj(mpmath.mpc(r1,r2)),functionName="cconj")
		self.checkRelativeComplexError(HPn.real (mpmath.mpc(r1,r2)),r1,functionName="creal")
		self.checkRelativeComplexError(HPn.imag (mpmath.mpc(r1,r2)),r2,functionName="cimag")

		self.checkRelativeError(HPn.atan2(r1,r2),mpmath.atan2(r1,r2),functionName="atan2")
		self.checkRelativeError(HPn.fmod(abs(r1),abs(r2)),mpmath.fmod(abs(r1),abs(r2)),functionName="fmod")
		self.checkRelativeError(HPn.hypot(r1,r2),mpmath.hypot(r1,r2),functionName="hypot")
		self.checkRelativeError(HPn.max(r1,r2),max(r1,r2),functionName="max")
		self.checkRelativeError(HPn.min(r1,r2),min(r1,r2),functionName="min")
		self.checkRelativeError(HPn.pow(abs(r1),r2),mpmath.power(abs(r1),r2),functionName="pow")
		self.checkRelativeError(HPn.remainder(abs(r1),abs(r2)),abs(r1)-round(abs(r1)/abs(r2))*abs(r2),functionName="remainder")
		pair = HPn.remquo(abs(r1),abs(r2))
		self.checkRelativeError(pair[0],abs(r1)-round(abs(r1)/abs(r2))*abs(r2),functionName="remquo")
		self.assertEqual(pair[1]%8, round(abs(r1/r2))%8)

		self.checkRelativeError(HPn.ldexp(r1,int(r2)),mpmath.mpf(r1)*mpmath.power(2,int(r2)),functionName="ldexp")

	def threeArgMathCheck(self,HPn,r1,r2,r3):
		self.checkRelativeError(HPn.fma(r1,r2,r3),(mpmath.mpf(r1)*r2)+r3,functionName="fma")

	def testMathFunctions(self):
		for N in self.testLevelsHP:
			self.printedAlready=set()
			self.runCheck(N , self.HPtestMathFunctions)

	def HPtestMathFunctions(self,N,HPn):
		self.assertEqual(HPn.defprec , self.bits )
		zz=mpmath.acos(0)
		#print(zz.__repr__())
		#print("zz:",hex(id(zz)))
		#print("mpmath:",hex(id(mpmath)))
		a=HPn.Var()
		a.val=zz
		self.assertEqual(mpmath.mp.dps , self.digs0+1 )
		#print("---- a.val=",a.val.__repr__())
		#print("---- zz   =",zz   .__repr__())
		#print("---- DPS  =",mpmath.mp.dps)
		#print("---- abs  =",abs(mpmath.mpf(a.val-zz)))
		#print("---- 10** =",self.tolerance)
		self.checkRelativeError(a.val,zz)
		self.assertEqual(HPn.IsInteger, 0 )
		self.assertEqual(HPn.IsSigned, 1 )
		self.assertEqual(HPn.IsComplex,  0)
		if(self.bits >= 64):
			self.assertEqual(HPn.RequireInitialization, 1 )
		else:
			self.assertEqual(HPn.RequireInitialization, 0 )
		self.assertGreaterEqual(HPn.ReadCost, 1)
		self.assertGreaterEqual(HPn.AddCost, 1)
		self.assertGreaterEqual(HPn.MulCost, 1)
		self.checkRelativeError(HPn.highest(),self.maxval,2.1)
		self.checkRelativeError(-HPn.lowest(),self.maxval,2.1)
		self.checkRelativeError(HPn.Pi(),mpmath.pi)
		self.checkRelativeError(HPn.Euler(),mpmath.euler)
		self.checkRelativeError(HPn.Log2(),mpmath.log(2))
		self.checkRelativeError(HPn.Catalan(),mpmath.catalan)
		#print("HPn.epsilon() ",HPn.epsilon(),"  self.expectedEpsilon = ",self.expectedEpsilon)
		self.checkRelativeError(HPn.epsilon(),self.expectedEpsilon,10)
		if(self.digs0 == 6): # exception for float
			self.assertLessEqual(HPn.dummy_precision(),10e-6)
		else:
			self.checkRelativeError(mpmath.log(HPn.dummy_precision()/HPn.epsilon())/mpmath.log(10) , mpmath.mpf(self.digs0)/10 , 1.5 )
		for x in range(50):
			if(self.nonBoostMPFR): # this looks like a bug in /usr/include/eigen3/unsupported/Eigen/MPRealSupport !
				self.assertLessEqual(abs(HPn.random()-0.5),0.5)
			else:
				self.assertLessEqual(abs(HPn.random()    ),1.0)
		for aa in range(4):
			for bb in range(4):
				a = (aa-3)*5
				b = bb*10
				r = HPn.random(a,a+b+1)
				r2= HPn.random(a,a+b+1)
				r3= HPn.random(a,a+b+5)
				#print("random=",r)
				self.assertLessEqual(r,a+b+1)
				self.assertGreaterEqual(r,a)
				self.assertFalse(HPn.isMuchSmallerThan(r,1,HPn.epsilon()))
				# NOTE: Below is a very sensitive test. If it starts failing, then see in function adjustDigs0, how expectedEpsilon is calculated.
				# Maybe MPFR or cpp_bin_float changed the number of bits or changed their internal approximation of log10/log2.
				self.assertTrue(HPn.isMuchSmallerThan(self.expectedEpsilon,1+abs(r),HPn.epsilon()))
				self.assertTrue(HPn.isEqualFuzzy(r+self.expectedEpsilon*0.01,r,HPn.epsilon()))
				self.checkRelativeError(HPn.toLongDouble(r),float(r), 1e-14) # FIXME - should be 1e-17, but python does not support that
				self.checkRelativeError(HPn.toDouble(r),float(r), 1e-14)
				self.checkRelativeError(HPn.toDouble(r),float(r), 1e-14)
				self.assertEqual(HPn.toLong(r),int(r))
				self.assertEqual(HPn.toInt(r),int(r))
				#
				#print(r.__repr__(),r2.__repr__(),r3.__repr__())
				self.oneArgMathCheck(HPn,r)
				self.oneArgMathCheck(HPn,r2)
				self.oneArgMathCheck(HPn,r3)
				self.twoArgMathCheck(HPn,r,r2)
				self.twoArgMathCheck(HPn,r,r3)
				self.twoArgMathCheck(HPn,r2,r3)
				self.threeArgMathCheck(HPn,r,r2,r3)

	def testArray(self):
		for N in self.testLevelsHP:
			self.runCheck(N , self.HPtestArray)

	def HPtestArray(self,N,HPn):
		HPn.testArray()

	def testBasicVariable(self):
		for N in self.testLevelsHP:
			self.runCheck(N , self.HPtestBasicVariable)

	def HPtestBasicVariable(self,N,HPn):
		a=HPn.Var()
		self.checkRelativeError(a.val,-71.23,0.01)
		a.val=10
		self.checkRelativeError(a.val,10)
		self.checkRelativeComplexError(a.cpl,-71.23+33.23j,0.01)
		a.cpl=mpmath.mpc("1","-1")
		self.checkRelativeComplexError(a.cpl,1-1j,1e-15)
		self.checkRelativeComplexError(a.cpl,mpmath.mpc("1","-1"))

	def thisTestsExceptionReal(self):
		a=self.HPnHelper.Var()
		a.val="13123-123123*123"

	def thisTestsExceptionComplex(self):
		a=self.HPnHelper.Var()
		a.cpl="13123-123123*123-50j"

	def testWrongInput(self):
		for N in self.testLevelsHP:
			self.runCheck(N , self.HPtestWrongInput)

	def HPtestWrongInput(self,N,HPn):
		if(self.nonBoostMPFR): # this looks like another bug in /usr/include/mpreal.h
			print("skipping this test for non-boost /usr/include/mpreal.h")
			return
		# depending on backed Real use it throws TypeError or RuntimeError
		self.HPnHelper = HPn
		self.assertRaises(Exception,self.thisTestsExceptionReal)
		self.assertRaises(Exception,self.thisTestsExceptionComplex)

