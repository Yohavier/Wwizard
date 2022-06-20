#include "WaqlIntelliSense.h"
#include <iostream>
#include <algorithm>

//ID missing as property
//where doesnt support "and" yet

void WaqlIntelliSense::FindFittingCommands(std::string input)
{
	int vectorCommandIndex = 0;

	fittingCommands.clear();
	vectorCommand.clear();

	SplitStringInVector(input);
	
	if (vectorCommand.size() == 0)
		return;

	if (vectorCommand.size() == 1)
	{
		if (!IsInArray(sourceInit, sizeof(sourceInit) / sizeof(sourceInit[0]), vectorCommand.at(0)) && !IsInArray(transformInit, sizeof(transformInit) / sizeof(transformInit[0]), vectorCommand.at(0)))
		{
			SearchMatches(vectorCommand.at(0), sourceInit, sizeof(sourceInit) / sizeof(sourceInit[0]));
			SearchMatches(vectorCommand.at(0), transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
		}
	}
	isSourceFree = true;
	CheckSource(vectorCommandIndex);

	if (isSourceFree)
	{
		CheckTransform(vectorCommandIndex);
	}
}

bool WaqlIntelliSense::IsInArray(std::string* arrayBegin, int size, std::string searchWord)
{
	for (int ptr = 0; ptr <size; ptr++)
	{
		if (iequals(searchWord, arrayBegin[ptr]))
		{
			return true;
		}
	}
	return false;
}

bool WaqlIntelliSense::IsInSet(std::set<std::string>& mySet, std::string& searchWord)
{
	for (const auto& a : mySet)
	{
		if (iequals(a, searchWord))
			return true;
	}
	return false;
}

void WaqlIntelliSense::SearchMatches(std::string& inputCommand, const std::string* arrayBegin, int size)
{
	for (int i = 0; i < size; i++)
	{
		auto compare = arrayBegin[i].substr(0, inputCommand.length());
		if (iequals(compare, inputCommand))
		{
			fittingCommands.insert(arrayBegin[i]);
		}
	}
}

void WaqlIntelliSense::SearchMatchesInValueExpression(std::string& inputCommand)
{
	for (const auto& property : valueExpressions)
	{
		auto compare = property.substr(0, inputCommand.length());
		if (iequals(compare, inputCommand))
		{
			fittingCommands.insert(property);
		}
	}
}

bool WaqlIntelliSense::iequals(const std::string& a, const std::string& b)
{
	return std::equal(a.begin(), a.end(),
		b.begin(), b.end(),
		[](char a, char b) {
			return tolower(a) == tolower(b);
		});
}

void WaqlIntelliSense::CheckSource(int& vectorCommandIndex)
{
	if (vectorCommand.at(vectorCommandIndex) == "from")
	{
		isSourceFree = false;
 		vectorCommandIndex++;
 		if (vectorCommand.size() > vectorCommandIndex)
		{
			if (IsInArray(sourceNeedingString, sizeof(sourceNeedingString) / sizeof(sourceNeedingString[0]), vectorCommand.at(vectorCommandIndex)))
			{
				vectorCommandIndex++;
				if (vectorCommand.size() > vectorCommandIndex)
				{
					if (IsEnclosedString(vectorCommand.at(vectorCommandIndex)))
					{
						vectorCommandIndex++;
						if (vectorCommand.size() > vectorCommandIndex)
						{
							if (vectorCommand.at(vectorCommandIndex) == ",")
							{
								vectorCommandIndex++;
								isSourceFree = FindAllStringConcatenationsByComma(vectorCommandIndex);
							}
							else
							{
								isSourceFree = true;
							}
						}
						else
						{
							isSourceFree = true;
							fittingCommands.insert(",");
						}
					}
				}
			}
			else if (iequals(vectorCommand.at(vectorCommandIndex), "project"))
			{
				isSourceFree = true;
				vectorCommandIndex++;
				return;
			}
			else if (iequals(vectorCommand.at(vectorCommandIndex), "type"))
			{
				vectorCommandIndex++;
				if (vectorCommand.size() > vectorCommandIndex)
				{
					if (IsInArray(referenceObjects, sizeof(referenceObjects) / sizeof(referenceObjects[0]), vectorCommand.at(vectorCommandIndex)))
					{
						vectorCommandIndex++;
						if (vectorCommand.size() > vectorCommandIndex)
						{
							if (vectorCommand.at(vectorCommandIndex) == ",")
							{
								vectorCommandIndex++;
								isSourceFree = FindAllConcatenations(vectorCommandIndex, referenceObjects, sizeof(referenceObjects) / sizeof(referenceObjects[0]), ",");
							}
							else
							{
								isSourceFree = true;
							}
						}
						else
						{
							isSourceFree = true;
							fittingCommands.insert(",");
						}
					}
					else
					{
						SearchMatches(vectorCommand.at(vectorCommandIndex), referenceObjects, sizeof(referenceObjects) / sizeof(referenceObjects[0]));
						return;
					}
				}
				else
				{
					std::string empty = "";
					SearchMatches(empty, referenceObjects, sizeof(referenceObjects) / sizeof(referenceObjects[0]));
					return;
				}
			}
			else
			{
				SearchMatches(vectorCommand.at(vectorCommandIndex), sourceForCheck, sizeof(sourceForCheck) / sizeof(sourceForCheck[0]));
				return;
			}
		}
		else
		{
			std::string empty = "";
			SearchMatches(empty, sourceForCheck, sizeof(sourceForCheck) / sizeof(sourceForCheck[0]));
		}
	}
}

void WaqlIntelliSense::SplitStringInVector(std::string& input)
{
	std::string currentWord = "";
	for (const auto& c : input)
	{
		if (c == ' ')
		{
			if (currentWord != " " && currentWord != "")
			{
				vectorCommand.emplace_back(currentWord);
			}
			currentWord = "";
		}
		else if (c == ',')
		{
			if (currentWord != " " && currentWord != "")
			{
				vectorCommand.emplace_back(currentWord);
			}

			currentWord = c;
			vectorCommand.emplace_back(currentWord);
			currentWord = "";
		}
		else if (c == '.')
		{
			if (currentWord != " " && currentWord!= "")
			{
				vectorCommand.emplace_back(currentWord);
			}

			currentWord = c;
			vectorCommand.emplace_back(currentWord);
			currentWord = "";
		}
		else
		{
			currentWord += c;
		}
	}
	if (currentWord != " " && currentWord != "")
	{
		vectorCommand.emplace_back(currentWord);
	}
}

bool WaqlIntelliSense::IsEnclosedString(const std::string& part)
{
	if (part.length() > 2)
	{
		if (part.at(0) == '"' && part.at(part.length() - 1) == '"')
		{
			return true;
		}
	}
	return false;
}

void WaqlIntelliSense::CheckTransform(int& vectorCommandIndex)
{
	if (vectorCommandIndex >= vectorCommand.size())
	{
		std::string empty = "";
		SearchMatches(empty, transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
		return;
	}

	if (vectorCommand.at(vectorCommandIndex) == "where")
	{
		vectorCommandIndex++;
		if (vectorCommand.size() > vectorCommandIndex)
		{
			if(IsInSet(valueExpressions,vectorCommand.at(vectorCommandIndex)))
			{
				vectorCommandIndex++;
				if (vectorCommand.size() > vectorCommandIndex)
				{
					if (IsInArray(conditions, sizeof(conditions) / sizeof(conditions[0]), vectorCommand.at(vectorCommandIndex)))
					{
						vectorCommandIndex++;
						if (HandleConditionFollowUpValue(vectorCommandIndex))
						{
							vectorCommandIndex++;
							CheckTransform(vectorCommandIndex);
						}
					}
					else if (vectorCommand.size() - 1 == vectorCommandIndex)
					{
						SearchMatches(vectorCommand.at(vectorCommandIndex), conditions, sizeof(conditions) / sizeof(conditions[0]));
					}
				}
				else
				{
					std::string empty = "";
					SearchMatches(empty, conditions, sizeof(conditions) / sizeof(conditions[0]));
				}
			}
			else if(vectorCommand.size() - 1 == vectorCommandIndex)
			{
				SearchMatchesInValueExpression(vectorCommand.at(vectorCommandIndex));
			}
		}
		else
		{
			std::string empty = "";
			SearchMatchesInValueExpression(empty);
		}
	}
	else if (vectorCommand.at(vectorCommandIndex) == "select")
	{
		vectorCommandIndex++;
		if (vectorCommand.size() > vectorCommandIndex)
		{
			if (IsInArray(objectExpression, sizeof(objectExpression) / sizeof(objectExpression[0]), vectorCommand.at(vectorCommandIndex)))
			{
				vectorCommandIndex++;
				if (vectorCommand.size() > vectorCommandIndex)
				{
					if (vectorCommand.at(vectorCommandIndex) == "," || vectorCommand.at(vectorCommandIndex) == ".")
					{
						if (HandleConcatenations(vectorCommandIndex, objectExpression, sizeof(objectExpression) / sizeof(objectExpression[0])))
						{
							CheckTransform(vectorCommandIndex);
						}
					}
					else
					{
						CheckTransform(vectorCommandIndex);
					}
				}
				else
				{
					std::string empty = "";
					SearchMatches(empty, transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
					
 					fittingCommands.insert(",");
					if (IsInArray(concatenableObjectExpression, sizeof(concatenableObjectExpression) / sizeof(concatenableObjectExpression[0]), vectorCommand.at(vectorCommandIndex-1)))
					{
						fittingCommands.insert(".");
					}
				}
			}
			else if(vectorCommand.size() -1 == vectorCommandIndex)
			{
				SearchMatches(vectorCommand.at(vectorCommandIndex), objectExpression, sizeof(objectExpression) / sizeof(objectExpression[0]));
				return;
			}
		}
		else
		{
			std::string empty = "";
			SearchMatches(empty, objectExpression, sizeof(objectExpression) / sizeof(objectExpression[0]));
			return;
		}
	}
	else if (vectorCommand.at(vectorCommandIndex) == "skip")
	{
		vectorCommandIndex++;
		if (vectorCommand.size() > vectorCommandIndex)
		{
			if(!IsNumber(vectorCommand.at(vectorCommandIndex)))
				return;
		}
	}
	else if (vectorCommand.at(vectorCommandIndex) == "take")
	{
		vectorCommandIndex++;
		if (vectorCommand.size() > vectorCommandIndex)
		{
			if (!IsNumber(vectorCommand.at(vectorCommandIndex)))
				return;
		}
	}
	else if (vectorCommand.at(vectorCommandIndex) == "orderby")
	{
		vectorCommandIndex++;
		if (vectorCommand.size() > vectorCommandIndex)
		{
			if (IsInSet(valueExpressions, vectorCommand.at(vectorCommandIndex)))
			{
				vectorCommandIndex++;
				if (vectorCommand.size() > vectorCommandIndex)
				{
					if (iequals(vectorCommand.at(vectorCommandIndex), "Reverse"))
					{
						vectorCommandIndex++;
						CheckTransform(vectorCommandIndex);
					}
					else if(vectorCommand.size()-1 == vectorCommandIndex)
					{
						fittingCommands.insert("Reverse");
						return;
					}
					else
					{
						vectorCommandIndex++;
						CheckTransform(vectorCommandIndex);
					}
				}
				else
				{
					fittingCommands.insert("Reverse");
					std::string empty = "";
					SearchMatches(empty, transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
				}
			}
			else
			{
				SearchMatchesInValueExpression(vectorCommand.at(vectorCommandIndex));
				return;
			}
		}
		else
		{
			std::string empty = "";
			SearchMatchesInValueExpression(empty);
		}
	}
	else if (vectorCommand.at(vectorCommandIndex) == "distinct")
	{
		return;
	}
	else if (vectorCommand.size() - 1 == vectorCommandIndex)
	{
		SearchMatches(vectorCommand.at(vectorCommandIndex), transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
	}
}

void WaqlIntelliSense::OnConnected()	
{
	valueExpressions = wwizardClient->GetAllWwiseProperties();
	valueExpressions.insert("Name");
}

bool WaqlIntelliSense::IsNumber(const std::string& part)
{
	if (part.size() > 0)
	{
		for (int i = 0; i< part.size(); i++)
		{
			if (!std::isdigit(part[i]))
			{
				if (part[i] != '-' || i != 0 || part.size() < 2)
				{
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

bool WaqlIntelliSense::FindAllConcatenations(int& vectorCommandIndex, std::string* arrayToLook, int arraySize, std::string concatenator)
{
	bool comma = true;
	bool passingFlag = true;

	if (vectorCommandIndex == vectorCommand.size())
	{
		std::string empty = "";
		SearchMatches(empty, arrayToLook, arraySize);
		return false;
	}

	for (int i = vectorCommandIndex; i < vectorCommand.size(); i++)
	{
		if (comma && vectorCommand.at(i) != concatenator)
		{
			passingFlag = IsInArray(arrayToLook, arraySize, vectorCommand.at(i));
			if (!passingFlag && i == vectorCommand.size() - 1)
			{
				SearchMatches(vectorCommand.at(i), arrayToLook, arraySize);
				return false;
			}
			vectorCommandIndex++;
			comma = false;
		}
		else if(!comma && vectorCommand.at(i) == concatenator)
		{
			vectorCommandIndex++;
			comma = true;
		}
		else if (comma && vectorCommand.at(i) == concatenator)
		{
			return false;
		}
	}

	if (!comma)
	{
		if (vectorCommand.size() <= vectorCommandIndex)
			fittingCommands.insert(concatenator);
		return true;
	}
	else
	{
		std::string empty = "";
		SearchMatches(empty, arrayToLook, arraySize);
		return false;
	}
}

bool WaqlIntelliSense::FindAllStringConcatenationsByComma(int& vectorCommandIndex)
{
	bool passingFlag = true;
	bool comma = true;

	if (vectorCommandIndex == vectorCommand.size())
	{
		return false;
	}

	for (int i = vectorCommandIndex; i < vectorCommand.size(); i++) 
	{
		if (IsEnclosedString(vectorCommand.at(i)) && comma)
		{
			comma = false;
			vectorCommandIndex++;
		}
		else if (!comma && vectorCommand.at(i) == ",")
		{
			comma = true;
			vectorCommandIndex++;
		}
		else if (comma && vectorCommand.at(i) == ",")
		{
			return false;
		}
	}
	if (!comma)
	{
		if(vectorCommand.size() <= vectorCommandIndex)
			fittingCommands.insert(",");
		return true;
	}
	else
	{
		return false;
	}
}

bool WaqlIntelliSense::HandleConcatenations(int& vectorCommandIndex, std::string* arrayToLook, int arraySize)
{
	bool passing = false;

	if (vectorCommand.at(vectorCommandIndex) == ".")
	{
		vectorCommandIndex++;
		if(vectorCommand.size()> vectorCommandIndex)
		{
			if (IsInArray(concatenableObjectExpression, sizeof(concatenableObjectExpression) / sizeof(concatenableObjectExpression[0]), vectorCommand.at(vectorCommandIndex))) 
			{
				passing = true;
				vectorCommandIndex++;
				if (vectorCommand.size() > vectorCommandIndex)
				{
					if (vectorCommand.at(vectorCommandIndex) == "." || vectorCommand.at(vectorCommandIndex) == ",")
					{
						passing = HandleConcatenations(vectorCommandIndex, arrayToLook, arraySize);
					}
				}
				else
				{
					fittingCommands.insert(".");
					fittingCommands.insert(",");
				}
			}
			else if(vectorCommand.size() -1 == vectorCommandIndex)
			{
				SearchMatches(vectorCommand.at(vectorCommandIndex), concatenableObjectExpression, sizeof(concatenableObjectExpression) / sizeof(concatenableObjectExpression[0]));
			}
		}
		else
		{
			std::string empty = "";
			SearchMatches(empty, concatenableObjectExpression, sizeof(concatenableObjectExpression) / sizeof(concatenableObjectExpression[0]));
		}
	}
	else if (vectorCommand.at(vectorCommandIndex) == ",")
	{
		vectorCommandIndex++;
		if (vectorCommand.size() > vectorCommandIndex)
		{
			if (IsInArray(arrayToLook, arraySize, vectorCommand.at(vectorCommandIndex)))
			{
				passing = true;
				vectorCommandIndex++;
				if (vectorCommand.size() > vectorCommandIndex)
				{
					if (vectorCommand.at(vectorCommandIndex) == ",")
					{
						passing = HandleConcatenations(vectorCommandIndex, arrayToLook, arraySize);
					}
					else if (vectorCommand.at(vectorCommandIndex) == "." && IsInArray(concatenableObjectExpression, sizeof(concatenableObjectExpression) / sizeof(concatenableObjectExpression[0]), vectorCommand.at(vectorCommandIndex - 1)))
					{
						passing = HandleConcatenations(vectorCommandIndex, arrayToLook, arraySize);
					}
				}
				else
				{
					fittingCommands.insert(",");
					if (IsInArray(concatenableObjectExpression, sizeof(concatenableObjectExpression)/sizeof(concatenableObjectExpression[0]), vectorCommand.at(vectorCommandIndex-1)))
					{
						fittingCommands.insert(".");
					}
				}
			}
			else if (vectorCommand.size() - 1 == vectorCommandIndex)
			{
				SearchMatches(vectorCommand.at(vectorCommandIndex), arrayToLook, arraySize);
			}
		}
		else
		{
			std::string empty = "";
			SearchMatches(empty, arrayToLook, arraySize);
		}
	}
	return passing;
}

bool WaqlIntelliSense::HandleConditionFollowUpValue(int& vectorCommandIndex)
{
	if (vectorCommand.size() > vectorCommandIndex)
	{
		if (IsEnclosedString(vectorCommand.at(vectorCommandIndex)) || IsRegEx(vectorCommandIndex)|| IsNumber(vectorCommand.at(vectorCommandIndex)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool WaqlIntelliSense::IsRegEx(int& vectorCommandIndex)
{
	std::string regEx = vectorCommand.at(vectorCommandIndex);
	if (regEx.size() > 2)
	{
		if (regEx[0] == '/' && regEx[regEx.size() - 1] == '/')
		{
			return true;
		}
	}
	return false;
}