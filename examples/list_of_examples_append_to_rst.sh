#!/usr/bin/zsh
# This script automatically adds links to youtube videos of examples. First it greps list_of_examples for pattern "...│...│YES│"
# to identify examples which have a video uploaded.
# Then it runs a loop over all those examples and appends them to the file.

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
		# capitalize first letter in section name
		NEW_SECTION="$(tr '[:lower:]' '[:upper:]' <<< ${TMP2_SECTION:0:1})${TMP2_SECTION:1}"
		YOUTUBEURL=`grep -E " ${example}\>" list_of_examples.txt | sed -e "s/.* ${example}\>.*https:\/\/youtu.be\/\([^ ]\+\).*/\1/"`
		# make sure that .rst references do not contain illegal characters
		SANITIZE=`echo ${BASENAME} | sed -e "s/[^A-Za-z0-9]/_/g"`
		if [[ "${NEW_SECTION}" != "${PREV_SECTION}" ]]; then
			echo "${NEW_SECTION}"                                              >> ../doc/sphinx/tutorial-more-examples.rst
			echo '"""""""""""""""""""""'                                       >> ../doc/sphinx/tutorial-more-examples.rst
			echo ""                                                            >> ../doc/sphinx/tutorial-more-examples.rst
			PREV_SECTION=${NEW_SECTION}
		fi
		echo ".. _${SANITIZE}:\n"                                                  >> ../doc/sphinx/tutorial-more-examples.rst
# that would be a section for each file, but let's skip this.
#		echo "${file_location}"                                                    >> ../doc/sphinx/tutorial-more-examples.rst
#		echo '""""""""""""""""""""""""'                                            >> ../doc/sphinx/tutorial-more-examples.rst
#		echo ""                                                                    >> ../doc/sphinx/tutorial-more-examples.rst
		echo 'Following example is in file :ysrc:`examples/'${file_location}'`'.   >> ../doc/sphinx/tutorial-more-examples.rst
		echo ""                                                                    >> ../doc/sphinx/tutorial-more-examples.rst
		echo ".. youtube:: ${YOUTUBEURL}\n"                                        >> ../doc/sphinx/tutorial-more-examples.rst
		echo ""                                                                    >> ../doc/sphinx/tutorial-more-examples.rst
	else
		echo "Cannot find file ${example} in LOCATION: ${file_location}"
		sleep 1
	fi
done


