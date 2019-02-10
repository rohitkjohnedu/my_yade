#!/usr/bin/zsh
# This script automatically adds links to youtube videos of examples. First it greps list_of_examples for pattern "...│...│yes│"
# to identify examples which have a video uploaded.
# NOTE: it skips "...│...│YES│", because YES means that it is already in doc/sphinx/tutorial-more-examples.rst,
#       or it should not go there because the video is too bad for people to see.
# Then it runs a loop over all those examples and appends them to the file.

the_examples=("${(@f)$(grep -E "...│...│yes│" list_of_examples.txt | sed -e "s/.* \([A-Za-z_0-9-]\+\>\.py\>\).*/\1/")}")

for example in $the_examples; do
	file_location=("${(@f)$(find . -name "${example}" -type f)}")
	print "Working on ${example} in ${file_location}"
	if [[ -f ${file_location} ]]; then
		BASENAME=`basename ${example} .py`
		YOUTUBEURL=`grep -E " ${example}\>" list_of_examples.txt | sed -e "s/.* ${example}\>.*https:\/\/youtu.be\/\([^ ]\+\).*/\1/"`
		# make sure that .rst references do not contain illegal characters
		SANITIZE=`echo ${BASENAME} | sed -e "s/[^A-Za-z0-9]/_/g"`
		echo ".. _${SANITIZE}:\n"                                          >> ../doc/sphinx/tutorial-more-examples.rst
		echo "${BASENAME}"                                                >> ../doc/sphinx/tutorial-more-examples.rst
		echo '""""""""""""""""""""""""'                                   >> ../doc/sphinx/tutorial-more-examples.rst
		echo ""                                                           >> ../doc/sphinx/tutorial-more-examples.rst
		echo 'Following example is in file :ysrc:`'${file_location}'`'.   >> ../doc/sphinx/tutorial-more-examples.rst
		echo ""                                                           >> ../doc/sphinx/tutorial-more-examples.rst
		echo ".. youtube:: ${YOUTUBEURL}\n"                               >> ../doc/sphinx/tutorial-more-examples.rst
	else
		echo "Cannot find file ${example} in LOCATION: ${file_location}"
		sleep 1
	fi
done


