#!/usr/bin/python
# encoding: utf-8
from __future__ import print_function
from __future__ import unicode_literals
try:
	import _bibtex as bib
	import _recode as bibRecode
except ImportError:
	raise ImportError("Unable to import _bibtex and/or _recode; install the python-bibtex package.")
import sys

def readBib(filename):
	## _bibtex has horrible interface
	bibfile=bib.open_file(filename,1) # 2nd argument: strict mode
	db={}
	#rq=bibRecode.request('latex..latin1')
	while True:
		entry=bib.next(bibfile)
		if entry is None: break
		key,type,dta=entry[0],entry[1],entry[4]
		item={'type':type}
		for field in dta.keys():
			expanded=bib.expand(bibfile,dta[field],-1)
			#conv=bibRecode.recode(rq,expanded[2])
			item[field]=expanded[2].strip()
		db[key]=item
	## now we don't need _bibtex anymore, everything is in our dicts
	return db

def dumpBib(db):
	for k in db.keys():
		print(k,db[k])

def formatRest(db):
	ret=[]
	keys=db.keys(); keys.sort()
	for key in keys:
		i=db[key]; type=i['type']
		line=r'.. [%s] \ '%key ## ← HACK: explicit space to prevent docutils from using abbreviated first name (e.g. "F.") as enumeration item; it works!!
		if 'author' in i: author=i['author'].replace(' and ',', ') # the module does not handle this..?

		# required fields from http://en.wikipedia.org/wiki/Bibtex
		# in some cases, they are not present, anyway
		if type=='article':
			if 'author' in i: line+='%s '%author
			if 'year' in i: line+='(%s), '%i['year']
			line+='**%s**. *%s*'%(i['title'],i['journal'])
			if 'issue' in i: line+=' %s'%i['issue']
			if 'volume' in i: line+=' (%s)'%i['volume']
			if 'pages' in i: line+=', pages %s'%i['pages']
			line+='.'
		elif type=='book':
			if 'author' in i: line+='%s '%author
			if 'year' in i: line+='(%s), '%i['year']
			line+='**%s**.'%i['title']
			if 'publisher' in i: line+=' %s.'%i['publisher']
		elif type=='inproceedings':
			line+='%s (%s), **%s**. In *%s*.'%(author,i['year'],i['title'],i['booktitle'] if 'booktitle' in i else i['journal'])
		elif type=='incollection':
			line+='%s (%s), **%s**. In *%s* '%(author,i['year'],i['title'],i['booktitle'] if 'booktitle' in i else i['journal'])
			if 'editor' in i: line+='( %s'%i['editor']+', ed.),'
			if 'publisher' in i: line+=' %s'%i['publisher']+' ,'
			if 'edition' in i: line+=' %s ed.'%i['edition']
		elif type=='phdthesis':
			line+='%s (%s), **%s**. PhD thesis at *%s*.'%(author,i['year'],i['title'],i['school'])
		elif type=='mastersthesis':
			typeThesis = 'Master thesis'
			if 'note' in i:
				typeThesis = i['note']
			line+='%s (%s), **%s**. %s at *%s*.'%(author,i['year'],i['title'],typeThesis,i['school'])
		elif type=='proceedings':
			if 'editor' in i: line+='%s (ed.), '%i['editor']
			line+='**%s** (%s).'%(i['title'],i['year'])
			if 'organization' in i: line+=' *%s*.'%i['organization']
			if 'publisher' in i: line+=' %s'%i['publisher']
		elif type=='misc':
			if 'author' in i: line+=author
			if 'year' in i: line+=' %s'%i['year']
			if 'title' in i: line+=' **%s**'%i['title']
		# add doi and url to everything, if present
		## ReST uses <..> to delimit URL, therefore < and > must be encoded in the URL (http://www.blooberry.com/indexdot/html/topics/urlencoding.htm)
		def escapeUrl(url): return url.replace('<','%3c').replace('>','%3e')
		if 'doi' in i: line+=' DOI `%s <http://dx.doi.org/%s>`_'%(i['doi'],escapeUrl(i['doi']))
		if 'url' in i: line+=' `(fulltext) <%s>`__'%escapeUrl(i['url'])
		if ('note' in i and type!='mastersthesis'): line+=' (%s)'%i['note']
		ret.append(line)
	return [l.replace('@tilde@','~') for l in ret]

def bib2rst(filename):
	"""Return string representing all items in given bibtex file, formatted as ReST."""
	import tempfile,shutil,os.path
	d=tempfile.mkdtemp()
	bib=d+'/'+os.path.basename(filename)
	open(bib,'w').write(open(filename).read().replace('~','@tilde@'))
	db=readBib(bib)
	shutil.rmtree(d)
	return '\n\n'.join(formatRest(db))
if __name__=='__main__':
	import sys
	print(bib2rst(sys.argv[1]))
