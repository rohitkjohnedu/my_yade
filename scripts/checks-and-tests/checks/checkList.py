# encoding: utf-8
# 2011 © Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
from __future__ import print_function
from past.builtins import execfile
import yade,math,os,sys

class YadeCheckError(Exception):
	"""Use ``raise YadeCheckError("message") when check fails. Or throw any other python exception that seems suitable."""
	pass

def errprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

scriptsToRun=os.listdir(checksPath)
failedScripts=list()

skipScripts = ['checkList.py']
onlyOneScript = []

def mustCheck(sc):
	if(len(onlyOneScript)==1): return sc in onlyOneScript
	return sc not in skipScripts

for script in scriptsToRun:
	if (script[len(script)-3:]==".py" and mustCheck(script)):
		try:
			errprint("###################################")
			errprint("running: ",script)
			execfile(checksPath+"/"+script)
			errprint("Status: success")
			errprint("___________________________________")
		except Exception as e:
			failedScripts.append(script)
			errprint('\033[91m',script," failure, caught exception ",e.__class__.__name__,": ",e,'\033[0m')
		O.reset()
	elif (not mustCheck(script)):
		errprint("###################################")
		errprint("Skipping %s, because it is in SkipScripts"%script)

if (len(failedScripts)!=0):
	errprint('\033[91m', len(failedScripts) , " tests are failed"+'\033[0m')
	for s in failedScripts: errprint("  "+s)
	sys.exit(1)
else:
	# https://misc.flogisoft.com/bash/tip_colors_and_formatting
	errprint('\033[92m'+"*** ALL CHECKS PASSED ***"+'\033[0m')
	sys.exit(0)

