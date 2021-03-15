#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <vector>
#include <array>

#include "input_parameter.hpp"
#include "utils.hpp"

std::string info;

// Map of key = bitfield names and values = bitfield widths
std::map<std::string, unsigned> bitfieldSizes;

// Vector of pairs of bitfield name and bitfield width sorted by bitfield widths
std::vector<std::pair<std::string, unsigned>> bitfieldSizesVec;

// Map of key = bitfield variable name, value = string (containing all comments and appended things)
std::map<std::string, std::string> fullBitfieldNames;

// Vector of the global order of bitfields in globally processed struct
std::vector<std::pair<std::string, unsigned>> outputBitfield;

// The whole bitfield part of processed struct
std::string bitfieldOutput;

// Global output - for reposting - does not work right now...
std::string output;

// x-ambles
std::string preamble = "#ifdef debug\n";
std::string interamble = "#else\n";
std::string postamble = "#endif\n";

std::string padding = "u16";
std::string indent = "  ";


unsigned CountUsedGroups(std::map<std::string, unsigned> bitfieldSizes, unsigned alignment)
{
	unsigned sumOfBitfieldSizes = 0;

	for (const auto& elem: bitfieldSizes)
	{
		sumOfBitfieldSizes += elem.second;
	}

	unsigned modulo = sumOfBitfieldSizes % alignment;
	if (modulo)
	{
		//throw - or don't - there is no reason for throwing
		std::cout << "All bitfields sizes summed are NOT DIVISIBLE by the alignment!\n";
		std::cout << "Modulo is " << modulo << std::endl;
	}

	return sumOfBitfieldSizes / alignment;
}

void AddToGlobalBitfieldOutputString(const std::vector<std::pair<std::string, unsigned>>& groupedBitfieldSizesVec)
{
	//TODO: Adding to global bitfieldOutput

	bitfieldOutput += preamble; // Preamble contains \n currently...
	for (const auto& elem: groupedBitfieldSizesVec)
	{
		bitfieldOutput += elem.first + ":";
		bitfieldOutput += std::to_string(elem.second)+ "\n";
	}
	bitfieldOutput += interamble;
	for (std::vector<std::pair<std::string, unsigned>>::const_reverse_iterator iter = groupedBitfieldSizesVec.rbegin();
		iter != groupedBitfieldSizesVec.rend();
		iter++)
	{
		bitfieldOutput += iter->first + ":";
		bitfieldOutput += std::to_string(iter->second) + "\n";
	}
	bitfieldOutput += postamble;
	bitfieldOutput += "\n";
}

void AddToGlobalBitfieldOutputStringFull() // Not called currently
{
	// Needs to contain the unsigned also...
	std::vector<std::string> reverseNamesVec;

	unsigned alignmentCounter = 0;

	std:: cout << "\n\n\n in order FULL \n\n\n";

	for (auto& mapElem: outputBitfield)
	{
		// Print the preamble if we are at the begining of new group
		if (alignmentCounter == 0)
		{
			std::cout << preamble;
		}

		// Add the size of the bitfield to the counter
		alignmentCounter += mapElem.second;

		// This is printing with the comments...
		std::cout << (fullBitfieldNames.find(mapElem.first))->second;

		// Regex for removing comments...
		const std::regex bitFieldGrouped("^\040*\\w+\040*\\w+\040*:\040*[0-9]+");
		std::cmatch matchesGrouped;

		// If new padding was added it will not be found in the fullBitfieldNames...
		if (fullBitfieldNames.find(mapElem.first) == fullBitfieldNames.end())
		{
			// The entry wasn't found, let's do something else...
			// entry is pobably padding - construct it...
			std::string paddingEntry =
					indent + padding + " " + mapElem.first + ":" + std::to_string(mapElem.second) + ";" + "\n";

			reverseNamesVec.push_back(paddingEntry);

			// cout before the interamble...
			std::cout << paddingEntry;
		}
		else // the expected path
		{
			std::regex_search((fullBitfieldNames.find(mapElem.first))->second.c_str(), matchesGrouped, bitFieldGrouped);

			// Remove comments before pushing back...
			reverseNamesVec.push_back(std::string(matchesGrouped[0]) + ";" + "\n");
		}
		// Remove comments before pushing back...
		//reverseNamesVec.push_back(std::string(matchesGrouped[0]) + "\n");
		//reverseNamesVec.push_back((fullBitfieldNames.find(mapElem.first))->second);

		if (alignmentCounter == 16)
		{
			alignmentCounter = 0;

			std::cout << interamble;

			// Print elements in reverse order
			for (std::vector<std::string>::reverse_iterator iter = reverseNamesVec.rbegin();
				iter != reverseNamesVec.rend();
				iter++)
			{
				std::cout << *iter;
			}
			std::cout << postamble << "\n";
			reverseNamesVec.clear();
		}
	}
	std::cout << "\n\n\n";

	std::cout << "PRINTING THE bitfieldOutput: \n\n";
	std::cout << bitfieldOutput;
}

void AddToGlobalBitfieldOutputStringFull_printingCommentsInElse()
{
	// Needs to contain the unsigned also...
	std::vector<std::string> reverseNamesVec;

	unsigned alignmentCounter = 0;

	std::cout << "\nConstructing bitfield output...\n";

	for (auto& mapElem: outputBitfield)
	{
		// Print the preamble if we are at the begining of new group
		if (alignmentCounter == 0)
		{
			//std::cout << preamble;
			bitfieldOutput += preamble;
		}

		// Add the size of the bitfield to the counter
		alignmentCounter += mapElem.second;

		// This is printing with the comments...
		reverseNamesVec.push_back((fullBitfieldNames.find(mapElem.first))->second);
		//std::cout << (fullBitfieldNames.find(mapElem.first))->second;

		// Regex for removing comments...
		const std::regex bitFieldGrouped("^\040*\\w+\040*\\w+\040*:\040*[0-9]+");
		std::cmatch matchesGrouped;

		// If new padding was added it will not be found in the fullBitfieldNames...
		if (fullBitfieldNames.find(mapElem.first) == fullBitfieldNames.end())
		{
			// The entry wasn't found, let's do something else...
			// entry is pobably padding - construct it...
			std::string paddingEntry =
					indent + padding + " " + mapElem.first + ":" + std::to_string(mapElem.second) + ";" + "\n";

			reverseNamesVec.push_back(paddingEntry);

			// cout before the interamble...
			//std::cout << paddingEntry;
			bitfieldOutput += paddingEntry;
		}
		else // the expected path
		{
			std::regex_search((fullBitfieldNames.find(mapElem.first))->second.c_str(), matchesGrouped, bitFieldGrouped);

			// Remove comments before pushing back...
			//std::cout << (std::string(matchesGrouped[0]) + ";" + "\n");
			bitfieldOutput += (std::string(matchesGrouped[0]) + ";" + "\n");
			//reverseNamesVec.push_back(std::string(matchesGrouped[0]) + ";" + "\n");
		}
		// Remove comments before pushing back...
		//reverseNamesVec.push_back(std::string(matchesGrouped[0]) + "\n");
		//reverseNamesVec.push_back((fullBitfieldNames.find(mapElem.first))->second);

		if (alignmentCounter == 16)
		{
			alignmentCounter = 0;

			//std::cout << interamble;
			bitfieldOutput += interamble;

			// Print elements in reverse order
			for (std::vector<std::string>::reverse_iterator iter = reverseNamesVec.rbegin();
				iter != reverseNamesVec.rend();
				iter++)
			{
				//std::cout << *iter;
				bitfieldOutput += *iter;
			}
			//std::cout << postamble << "\n";
			bitfieldOutput += postamble +	 "\n";
			reverseNamesVec.clear();
		}
	}
}

void RemovePaddingEntries()
{
	// Map bitfieldSizes is ready at this point, create a vector of pairs and copy the pairs
	std::map<std::string, unsigned>::iterator iter;

	// During the insertion to vector padding and dummies varaibles will be removed
	for (iter = bitfieldSizes.begin(); iter != bitfieldSizes.end(); iter++)
	{
		// Only push if name isn't matching the padding regex...
		std::regex paddingRegex("padding|dummy|doNotUse");

		if (!std::regex_search(iter->first, paddingRegex))
		{
			bitfieldSizesVec.push_back(std::make_pair(iter->first, iter->second));
		}
	}
}

// Finding new layout for one group!!!!!! Only one group is processed here!!!
void FindNewLayoutForOneGroup(unsigned alignment, unsigned bitfieldGroup, unsigned& paddingInstance)
{
	// Pick the highest width bitfield from the bitfieldSizesVec vector
	unsigned counter = 0;
	std::vector<std::pair<std::string, unsigned>>::iterator iter = bitfieldSizesVec.begin();

	// Vector of bitfields that will be grouped together
	std::vector<std::pair<std::string, unsigned>> groupedBitfieldSizesVec;

	while (counter < alignment && iter != bitfieldSizesVec.end()) // or the vector is empty
	{
		if (iter->second + counter <= alignment)
		{
			// Add the chosen bitfield size to total size
			counter += iter->second;

			// Push the element to local vector
			groupedBitfieldSizesVec.push_back(*iter);

			// Erase the used element for available elements, make the iter point to next valid element
			iter = bitfieldSizesVec.erase(iter);
		}
		else
		{
			// Element does not fit - try the next one
			iter++;
		}
	}

	// Iteration has ended - check if the bitfields are perfectly filled to the alignment boundary or if
	// padding needs to be used
	if (counter != alignment)
	{
		// Add padding
		groupedBitfieldSizesVec.push_back(std::make_pair("padding" + std::to_string(paddingInstance), alignment - counter));
		paddingInstance++;
	}

	// Add to a global order vector...
	// Appending the elements of groupedBitfieldSizesVec to the end of outputBitfield
	outputBitfield.insert(outputBitfield.end(), groupedBitfieldSizesVec.begin(), groupedBitfieldSizesVec.end());
}

void ProcessExtractedLines(std::ifstream& logFile)
{
	std::cout << "Finding new layout...\n";

	std::string line;

	std::string cache;

	// Will be the pair.second for fullBitfieldNames map
	std::string fullBitfieldName;

	std::string fullEntry;

	std::cmatch matchesBitFieldGrouped;

	//const std::regex bitField(".*");
	//const std::regex bitField("^\\s*\\w+\\s*\\w+\\s*:\\s*[0-9]+");
	//const std::regex bitField("^\040*\\w+\040*\\w+\040*:\040*[0-9]+");
	const std::regex bitFieldGrouped("(^\040*\\w+)\040*(\\w+\040*)(:)(\040*)([0-9]+)");

	// TODO: Separate comments after bitfield regex?
	const std::regex bitFieldCommentGrouped("(^\040*\\w+\040*\\w+\040*:\040*[0-9]+\040*;\040*)(\\/\\/.*)");

	//const std::regex bitField("^\040*\\w+");
	const std::regex nonBitFieldVariable("non");


	const unsigned bitfieldNamePos = 2;
	const unsigned bitfieldWidthPos = 5;

	// Regex for bitfields variables
	// tested on https://regex101.com/
	//^\040*\w+\040*\w+\040*:\040*[0-9]+

	// Regex for NON-bitfield variables
	// tested on https://regex101.com/
	// has problem with int something[MACRO/2];
	//^\040*\w+\040*\w+\040*[^:]\040*[^0-9\s]+[;]

	// Regex for comments
	//^\040*\/\/|^\040*\*\/|^\040*\*
	// TODO: Use if the comments immediately follows the bitfield

	bool immediatelyFollowedComment = false;

	bool print = true;

	std::string lastBitfieldName;
	bool isBitfieldEntryActive = false;
	bool isHypotheticallyCommentAbove = false;

	bool isDuplicateProcessed = false;

	while (std::getline(logFile, line)) //TODO: Have two lines - oldLine and line
										// - if line matches regex - then start new processing
										// - if line does not match regex - then append the string to given entry
	{


		// If line starts with comments - add to cached
		const std::regex comment("^\040*\\/\\/|^\040*\\*\\/|^\040*\\*");

		if (std::regex_search(line.c_str(), comment))
		{
			cache += line + "\n";
			isHypotheticallyCommentAbove = true;
		}

		std::regex_search(line.c_str(), matchesBitFieldGrouped, bitFieldGrouped);

		// TODO: If has been found - do not repost
		// Add only if it has been found
		if (matchesBitFieldGrouped.size() > 2) // Why 2? How the submatches work? //Check this, but should be robust
		{
			if (print)
			{
				std::cmatch matchesComments;
				std::regex_search(line.c_str(), matchesComments, bitFieldCommentGrouped);

				print = false;
				info += std::to_string(matchesBitFieldGrouped.size()) + "\n";
				for (unsigned i = 0; i < matchesBitFieldGrouped.size(); ++i)
				{
					info += matchesBitFieldGrouped.str(i) + "\n";
				}

				for (unsigned i = 0; i < matchesComments.size(); ++i)
				{
					info += matchesComments.str(i) + "\n";
				}

			}
			// Construct the element to be inserted into the map
			std::pair<std::string, unsigned>
				bitfieldSizesElem(matchesBitFieldGrouped.str(bitfieldNamePos), std::stoul(matchesBitFieldGrouped.str(bitfieldWidthPos)));

			std::cmatch matchesComments;
			std::regex_search(line.c_str(), matchesComments, bitFieldCommentGrouped);

//			// TODO: Edit this to add spaces in between comments and bitfields
//			std::pair<std::string, std::string>
//				fullBitfielElem(matchesBitFieldGrouped.str(bitfieldNamePos), matchesBitFieldGrouped.str(1) + matchesBitFieldGrouped.str(2) + " " + matchesComments.str(2));
//


			//std::cout << "\n\n\n\n\n";
			//std::cout << fullBitfielElem.first;
			//std::cout << fullBitfielElem.second;

			std::map<std::string, unsigned>::iterator iter = bitfieldSizes.find(matchesBitFieldGrouped.str(bitfieldNamePos));

			// Check if it is not a duplicate of already seen bitfield
			if (iter == bitfieldSizes.end())
			{
				bitfieldSizes.insert(bitfieldSizesElem);
				isDuplicateProcessed = false;
//				bitfieldWholeEntryMap.insert(fullBitfielElem);
			}
			else // Bitfield already inserted, duplicate is found...
			{
				isDuplicateProcessed = true;

				// Need to check if bitfields have the same width...
				if (iter->second != bitfieldSizesElem.second)
				{
					// The bitfield duplicate has been found and has a different width!
					// throw
				}

//				//TODO: Need to check if there is a comment is the #else...
//				std::map<std::string, std::string>::iterator commentIter = bitfieldWholeEntryMap.find(iter->first);
//
//				// If the newly found line containing duplicate bitfield has a comment or the comment is more verbose
//				if (commentIter->second.length() < fullBitfielElem.second.length())
//				{
//
//					// Assign the longer line as the full entry
//					commentIter->second = fullBitfielElem.second;
//
//				}
			}

			// TODO: Throw away the unnecessary things - #defs


			std::string fullEntry;

			if (isHypotheticallyCommentAbove == true)
			{
				// Bitfield found - it has to be connected to cached comment that is above it
				fullEntry = cache;
			}
			// Full entry construction
			fullEntry +=
					matchesBitFieldGrouped.str(1) + " " +
					matchesBitFieldGrouped.str(2) + ":" + matchesBitFieldGrouped.str(bitfieldWidthPos) + ";" +
					" " + matchesComments.str(2) + "\n";

			// Bitfield has been found

			std::pair<std::string, std::string>
							fullBitfieldElem(matchesBitFieldGrouped.str(bitfieldNamePos), fullEntry);


			if (isDuplicateProcessed)
			{
				if ((fullBitfieldNames.find(fullBitfieldElem.first))->second.length() < fullBitfieldElem.second.length())
				{
					fullBitfieldNames.find(fullBitfieldElem.first)->second = fullBitfieldElem.second;
				}
			}
//
//			std::pair<std::string, std::string>
//				fullBitfieldElem(matchesBitFieldGrouped.str(bitfieldNamePos), matchesBitFieldGrouped.str(0) + " " + matchesComments.str(2) + "\n");

			// add the previous to the map
			//std::map<std::string, std::string>::iterator iter = fullBitfieldNames.find(lastBitfieldName);
			fullBitfieldNames.insert(fullBitfieldElem);

			isBitfieldEntryActive = true; // instead of that - just empty string (??)
			lastBitfieldName = fullBitfieldElem.first;

		} // BITFIELD FOUND...
		else
		{
			if (isBitfieldEntryActive && cache.empty() == false)
			{
				//std::cout << "here";
				// Immediately followed comment!
				// Apend to previously found bitfield and clear the cache

				// If it was processes then would be inserted two times...
				// Check if this found entry has more information
				/// Check only if the bitfield was seen already...!!!!!!!!!!!!!!

				// If processes duplicate has the comment information already
				if (isDuplicateProcessed && (fullBitfieldNames.find(lastBitfieldName)->second.find(cache) != std::string::npos))
				{
					// Just clear the cached, we have this info already
					cache.clear();
				}
				else
				{
					// Unique information, add to bitfield entry and clear the cache
					fullBitfieldNames.find(lastBitfieldName)->second += cache;
					cache.clear();
				}

				isBitfieldEntryActive = true;

			}
			else //if (!isHypotheticallyCommentAbove) //?????????
			{

				std::regex preprocessorDirective("#");

				if (!std::regex_search(line, preprocessorDirective))
				{
					output += cache;

					// To not have duplicates if we are processing the same line as it was cached...
					if (cache != (line + "\n"))
					{
						output += line + "\n";
					}
				}
				cache.clear();
				// repost and empty cache if no bitfield (the scope) AND no preprocessor directive
				isBitfieldEntryActive = false;

				// Hypothesy falsivied
				isHypotheticallyCommentAbove = false;
			}
		}



		// Find duplicates and throw if two the same bitfields have different sizes!

	}

    unsigned usedGroup = CountUsedGroups(bitfieldSizes, 16);

    std::cout << "We have " << usedGroup
			  << " groups of bitfields given the alignment of " << 16 << "\n";

	// Remove inserted padding entries
	RemovePaddingEntries();

	// Sort the vector by decreasing order of its pair's second value
	std::sort(bitfieldSizesVec.begin(), bitfieldSizesVec.end(), SortBySecondElem<std::string, unsigned>);

	// Only increase padding when padding is used...
	unsigned paddingInstance = 0;
	unsigned bitfieldGroup = 0;

	// bitfieldSizesVec is ready and without padding entries, find new layout and group bitfields
	while (!bitfieldSizesVec.empty())
	{
		FindNewLayoutForOneGroup(16, bitfieldGroup, paddingInstance);
		bitfieldGroup++;
	}

	std::cout << "\n\nNew number of bitfield group is : " << bitfieldGroup << "\n\n";

	//AddToGlobalBitfieldOutputStringFull();
	AddToGlobalBitfieldOutputStringFull_printingCommentsInElse();

	// Print the reposted part plus the bitfieldOutput...
	//std::cout << output + bitfieldOutput;

	std::cout << "\n\n\n\n\n";
	std::cout << bitfieldOutput;
}


void OverwriteInputParams()
{
	// Needs good interface for getting the values from InputParameter::parameters
	// Type erasure?
	// All of that are string parameters

	preamble = "#ifdef debug\n";
	interamble = "#else\n";
	postamble = "#endif\n";

	padding = "u16";
	indent = "  ";
}


int main(int argc, char* argv[])
{
	ReadParams();

	OverwriteInputParams();

	std::ifstream logFile;

	if (OpenFile(logFile, argv[1]))
	{
		ProcessExtractedLines(logFile);
	}
}
