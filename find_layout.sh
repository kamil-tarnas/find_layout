#!/bin/bash

# array for holding bit field sizes
declare -A bitfields
declare file_path
declare start_line
declare end_line
declare sed_start_line

structure_prefix="typedef"
#structure_prefix="class" #default will be typedef, one could provide class (for example)

#make the searching possible in other files than only .h

structure_name=$1; shift;


# read here the settings file and pass the necessary things to C++ engine??

prepareTempFiles()
{
	echo "Preparing temporary files"
	mkdir find_layout.tmp; touch find_layout.tmp/extracted_lines.tmp
}

deleteTempFiles()
{
	echo "Deleting temporary files"
	rm -rf find_layout.tmp
}

buildAndInvokeCppEngine()
{
	make
}

buildAndInvokeCppEngine_manual()
{
	echo "Building C++ regex engine"
	g++ regex_engine.cpp input_parameter.cpp utils.cpp input_parameter_value.cpp -std=c++11 -o regex_engine.out
	echo "Running C++ regex engine"
	./regex_engine.out find_layout.tmp/extracted_lines.tmp
}

putResultIntoOriginalFile()
{
	# read the results from C++ engine and swap them with original content of file
	echo "Putting the new layout of $structure_name to original file $file_path"
}

findStructure()
{
	echo "Searching for file containing definition of $structure_name"

	#declare grep_result=($(grep --color=never --include=*.h -Hrn $structure_prefix.*$structure_name))
	# should be one, unique file path
	file_path=($(grep --color=never --include=*.h -Hrn $structure_prefix.*$structure_name | awk -F : '{print $1}'))
	
	# TODO: should be done in one pass 
	sed_start_line=($(grep --color=never --include=*.h -Hrn $structure_prefix.*$structure_name | awk -F : '{print $2}'))
	
}

extractLines()
{
	echo "Extracting the definition of $structure_name"
	extracted_lines=$(sed -n "/$structure_prefix.*$structure_name/,/};/p" $file_path)
	echo "$extracted_lines" > find_layout.tmp/extracted_lines.tmp
}

deleteLines()
{
	echo "deleted!"
}

findEntry()
{
	
	#find one entry? Or do it till end of file?
	temp=$(echo "$extracted_lines" | sed -n -e /^\s*[^\/]*:.*/p)
	echo "$temp"
}

#extract lines
#make the C++ engine
#pass it to C++ engine for processing
#post processing - put the new processed file into the file
#remove the temp_files

# then find the bitfields with ':' characters and read it to the array 
# need to preserve the comments!
# needs to not start with "//", needs a ':' and after the ':' find digits and read
# before ':' find letters not containing spces
#ifdefs need to be ignored or the ones contatinng spaces need to be chosen

# how to extract one variable? 
#read one enty staring from line that not start with "//", needs a ':' 
#until a line that do not start with // (bitfield or normal varaible) (or #endif? #ifdef?)

#For every entry fild duplicates with the same name and chose that are longer (possibly contain comments)

#put the bitfields at the end! 

#paddingX, dummyX, padding, dummy, notUsed remove those


#is such algo optimal?

prepareTempFiles
findStructure
extractLines
buildAndInvokeCppEngine_manual
deleteTempFiles

#echo $sed_start_line
echo $file_path
#echo "$extracted_lines"
echo "$extracted_lines"	 > dupa
deleteLines
#findEntry


