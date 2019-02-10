#!/usr/bin/zsh
# This script automatically adds links to youtube videos of examples. First it greps list_of_examples for pattern "...│...│YES│"
# to identify examples which have a video uploaded.
# Then it runs a loop over all those examples and appends them to the file.

# http://docutils.sourceforge.net/docs/user/rst/quickref.html
# http://openalea.gforge.inria.fr/doc/openalea/doc/_build/html/source/sphinx/rest_syntax.html
# http://docutils.sourceforge.net/docs/ref/rst/restructuredtext.html#anonymous-hyperlinks
# section levels: #, *, =, -, ^, ",
the_examples=("${(@f)$(grep -E "...│...│YES│" list_of_examples.txt | sed -e "s/.* \([A-Za-z_0-9-]\+\>\.py\>\).*/\1/")}")

PREV_SECTION=""
NEW_SECTION=""

for example in $the_examples; do
	file_location=("${(@f)$(find . -name "${example}" -type f -printf "%P\n")}")
	print "Working on ${example} in ${file_location}"
	if [[ -f ${file_location} ]]; then
		BASENAME=`basename ${example} .py`
		TMP_SECTION=`dirname ${file_location}`
		TMP2_SECTION=`echo "$TMP_SECTION" | cut -d "/" -f1`
		file_cutlocation=`echo $file_location | cut -d "/" -f2-`
		FILE_CUT_UPPER="$(tr '[:lower:]' '[:upper:]' <<< ${file_cutlocation:0:1})${file_cutlocation:1}"
		# capitalize first letter in section name
		NEW_SECTION="$(tr '[:lower:]' '[:upper:]' <<< ${TMP2_SECTION:0:1})${TMP2_SECTION:1}"
		YOUTUBEURL=`grep -E " ${example}\>" list_of_examples.txt | sed -e "s/.* ${example}\>.*https:\/\/youtu.be\/\([^ ]\+\).*/\1/"`
		# make sure that .rst references do not contain illegal characters
		SANITIZE=`echo ${BASENAME} | sed -e "s/[^A-Za-z0-9]/-/g"`
		Sanitize="$(tr '[:lower:]' '[:upper:]' <<< ${SANITIZE:0:1})${SANITIZE:1}"
		if [[ "${NEW_SECTION}" != "${PREV_SECTION}" ]]; then
			echo "${NEW_SECTION}"                                              >> ../doc/sphinx/tutorial-more-examples.rst
			echo '^^^^^^^^^^^^^^^^^^^^^'                                       >> ../doc/sphinx/tutorial-more-examples.rst
			echo ""                                                            >> ../doc/sphinx/tutorial-more-examples.rst
			PREV_SECTION=${NEW_SECTION}
		fi
#		echo ".. _Example_${SANITIZE}:\n"                                          >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ".. rst:directive:: .. "':ysrc:`'${FILE_CUT_UPPER}'<examples/'${file_location}'>`'"::\n\n" >> ../doc/sphinx/tutorial-more-examples.rst
#		.. :ysrc:`Chained-cylinder-spring.py<examples/chained-cylinders/chained-cylinder-spring.py>`:: ¶
#		echo ".. confval:: .. "':ysrc:`'${FILE_CUT_UPPER}'<examples/'${file_location}'>`'"\n\n" >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ':ysrc:`'${FILE_CUT_UPPER}'<examples/'${file_location}'>`'            >> ../doc/sphinx/tutorial-more-examples.rst
#		echo '""""""""""""""""""""""""""""""""""""""""""""""""""'"\n"              >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ".. rst:directive:: .. "':ysrc:`'${FILE_CUT_UPPER}'<examples/'${file_location}'>`'"\n\n" >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ".. rst:directive:: .. ${FILE_CUT_UPPER}\n" >> ../doc/sphinx/tutorial-more-examples.rst
#                     .. .. Capillar.py::¶
#		echo ".. rst:: ${FILE_CUT_UPPER}\n" >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ".. _example${SANITIZE}:\n"                                            >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ':ysrc:`'${file_cutlocation}'<examples/'${file_location}'>`' '`¶<'"example${SANITIZE}"'>`'_"\n"      >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ".. _example${SANITIZE}:\n"                                            >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ':ysrc:`'${file_cutlocation}'<examples/'${file_location}'>`'__"\n"     >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ".. __: example${SANITIZE}_\n"                                          >> ../doc/sphinx/tutorial-more-examples.rst
#
#		echo ".. _example${SANITIZE}:\n"                                            >> ../doc/sphinx/tutorial-more-examples.rst
#		echo "* example${SANITIZE}_ "':ysrc:`examples/'${file_location}'`'"\n"     >> ../doc/sphinx/tutorial-more-examples.rst

		echo ".. _ref${Sanitize}:\n"                                            >> ../doc/sphinx/tutorial-more-examples.rst
		echo "* ref${Sanitize}_ "':ysrc:`'${file_cutlocation}'<examples/'${file_location}'>`'"\n"     >> ../doc/sphinx/tutorial-more-examples.rst
		echo ".. youtube:: ${YOUTUBEURL}\n\n"                                       >> ../doc/sphinx/tutorial-more-examples.rst
	else
		echo "Cannot find file ${example} in LOCATION: ${file_location}"
		sleep 1
	fi
done


