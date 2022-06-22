#pragma once
#include "WaqlIntelliSense.h"
#include <iostream>
#include <algorithm>

//@ sign for properties

void WaqlIntelliSense::FindFittingCommands(std::string input)
{
	readIndex = 0;
	fittingCommands.clear();
	suggestions.clear();
	vectorCommand.clear();

	SplitStringInVector(input);

	bool hasSource = true;
	if (IsBufferReadable())
	{
		hasSource = HandleSource();
		if (readIndex >= vectorCommand.size() && hasSource)
		{
			SearchMatches("", transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
		}

		while (readIndex < vectorCommand.size() && hasSource || (readIndex == 0 && !hasSource))
		{
			hasSource = HandleTransform();
		}
		if (hasSource)
		{
			SearchMatches("", transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
		}
	}
	else
	{
		fittingCommands.insert("from");
		SearchMatches("", transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
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

void WaqlIntelliSense::SearchMatches(std::string inputCommand, const std::string* arrayBegin, int size)
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

void WaqlIntelliSense::SearchMatchesInValueExpression(std::string inputCommand)
{
	for (const auto& property : valueProperties)
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

void WaqlIntelliSense::SplitStringInVector(std::string& input)
{
	bool openEnclosedString = false;

	std::string currentWord = "";
	for (const auto& c : input)
	{
		if (c == '"')
			openEnclosedString = !openEnclosedString;

		if (c == ' ')
		{
			if (openEnclosedString)
			{
				currentWord += c;
			}
			else
			{
				if (currentWord != " " && currentWord != "")
				{
					vectorCommand.emplace_back(currentWord);
				}
				currentWord = "";
			}
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
			if (IsWordInValueExpressions(currentWord))
			{
				currentWord += c;
			}
			else
			{
				if (currentWord != " " && currentWord != "")
				{
					vectorCommand.emplace_back(currentWord);
				}

				currentWord = c;
				vectorCommand.emplace_back(currentWord);
				currentWord = "";
			}
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

void WaqlIntelliSense::OnConnected()	
{
	valueProperties.clear();
	auto returnValues = wwizardClient->GetAllWwiseProperties();
	
	for (const auto& value : returnValues)
	{
		if (!IsInArray(concatenableReferences, sizeof(concatenableReferences) / sizeof(concatenableReferences[0]), value))
		{
			valueProperties.insert(value);
		}
	}
}

bool WaqlIntelliSense::HandleSource()
{
	if (IsBufferReadable())
	{
		if (vectorCommand.at(readIndex) == "from")
		{
			readIndex++;
			if (IsBufferReadable())
			{
				int tempRead = readIndex;
				readIndex++;

				if (iequals(vectorCommand.at(tempRead), "object"))
				{
					return HandleSourceObject();
				}
				else if (iequals(vectorCommand.at(tempRead), "type"))
				{
					return HandleSourceType();
				}
				else if (iequals(vectorCommand.at(tempRead), "query"))
				{
					return HandleSourceQuery();
				}	
				else if (iequals(vectorCommand.at(tempRead), "search"))
				{
					return HandleSourceSearch();
				}
				else if (iequals(vectorCommand.at(tempRead), "project"))
				{
					return HandleSourceProject();
				}
				else
				{
					SearchMatches(vectorCommand.at(tempRead), sourceForCheck, sizeof(sourceForCheck) / sizeof(sourceForCheck[0]));
				}
			}
			else
			{
				SearchMatches("", sourceForCheck, sizeof(sourceForCheck) / sizeof(sourceForCheck[0]));
			}
		}
		else
		{
			SearchMatches(vectorCommand.at(readIndex), sourceInit, sizeof(sourceInit) / sizeof(sourceInit[0]));
		}
	}
	else
	{
		return true;
	}

	return false;
}

bool WaqlIntelliSense::HandleSourceObject()
{
	if (IsBufferReadable())
	{
		if (IsEnclosedString(vectorCommand.at(readIndex)))
		{
			readIndex++;
			if (IsBufferReadable())
			{
				if (vectorCommand.at(readIndex) == ",")
				{
					readIndex++;
					return HandleSourceObject();
				}
			}
			else
			{
				fittingCommands.insert(",");
			}
			return true;
		}
		else
		{
			suggestions.insert("\"my path\"");
		}
	}
	else
	{
		suggestions.insert("\"my path\"");
	}
	return false;
}

bool WaqlIntelliSense::HandleSourceType()
{
	if (IsBufferReadable())
	{
		if (IsInArray(referenceObjects, sizeof(referenceObjects) / sizeof(referenceObjects[0]), vectorCommand.at(readIndex)))
		{
			readIndex++;
			if (IsBufferReadable())
			{
				if (vectorCommand.at(readIndex) == ",")
				{
					readIndex++;
					return HandleSourceType();
				}
			}
			else
			{
				fittingCommands.insert(",");
			}
			return true;
		}
		else
		{
			SearchMatches(vectorCommand.at(readIndex), referenceObjects, sizeof(referenceObjects) / sizeof(referenceObjects[0]));
		}
	}
	else
	{
		SearchMatches("", referenceObjects, sizeof(referenceObjects) / sizeof(referenceObjects[0]));
	}
	return false;
}

bool WaqlIntelliSense::HandleSourceQuery()
{
	if (IsBufferReadable())
	{
		if (IsEnclosedString(vectorCommand.at(readIndex)))
		{
			readIndex++;
			return true;
		}
	}
	suggestions.insert("\"Query Path\"");
	suggestions.insert("\"{GUID}\"");
	return false;
}

bool WaqlIntelliSense::HandleSourceSearch()
{
	if (IsBufferReadable())
	{
		if (IsEnclosedString(vectorCommand.at(readIndex)))
		{
			readIndex++;
			return true;
		}
	}
	suggestions.insert("\"search word\"");
	return false;
}

bool WaqlIntelliSense::HandleSourceProject()
{
	readIndex++;
	return true;
}


bool WaqlIntelliSense::HandleTransform()
{
	if (IsBufferReadable())
	{
		int tempRead = readIndex;
		readIndex++;

		if (iequals(vectorCommand.at(tempRead), "where"))
		{
			return HandleTransformWhere();
		}
		else if (iequals(vectorCommand.at(tempRead), "skip"))
		{
			return HandleTransformSkip();
		}
		else if (iequals(vectorCommand.at(tempRead), "take"))
		{
			return HandleTransformTake();
		}
		else if (iequals(vectorCommand.at(tempRead), "select"))
		{
			return HandleTransformSelect();
		}
		else if (iequals(vectorCommand.at(tempRead), "orderby"))
		{
			return HandleTransformOrderBy();
		}
		else if (iequals(vectorCommand.at(tempRead), "distinct"))
		{
			return HandleTransformDistinct();
		}
		SearchMatches(vectorCommand.at(tempRead), transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
	}
	else
	{
		SearchMatches("", transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
	}
	return false;
}

bool WaqlIntelliSense::HandleTransformWhere()
{
	bool passingFlag = false;
	passingFlag = HandleBooleanExpression();

	if (IsBufferReadable())
	{
		if (passingFlag)
		{
			if (IsInArray(logicalOperator, sizeof(logicalOperator) / sizeof(logicalOperator[0]), vectorCommand.at(readIndex)))
			{
				readIndex++;
				passingFlag = HandleTransformWhere();
			}
			else
			{
				SearchMatches(vectorCommand.at(readIndex), logicalOperator, sizeof(logicalOperator) / sizeof(logicalOperator[0]));
			}
		}
	}
	else if (passingFlag)
	{
		SearchMatches("", logicalOperator, sizeof(logicalOperator) / sizeof(logicalOperator[0]));
		SearchMatches("", transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
	}

	return passingFlag;
}

bool WaqlIntelliSense::HandleTransformSkip()
{
	if (IsBufferReadable())
	{
		if (IsNumber(vectorCommand.at(readIndex), false))
		{
			readIndex++;
			return true;
		}
	}
	suggestions.insert("int");
	return false;
}

bool WaqlIntelliSense::HandleTransformTake()
{
 	if (IsBufferReadable())
	{
		if (IsNumber(vectorCommand.at(readIndex), false))
		{
			readIndex++;
			return true;
		}
	}
	suggestions.insert("int");
	return false;
}

bool WaqlIntelliSense::HandleTransformSelect()
{
	if (HandleObjectExpression(true, true))
	{
		if (!IsBufferReadable())
		{
			SearchMatches("", transformInit, sizeof(transformInit) / sizeof(transformInit[0]));
		}
		return true;
	}
	return false;
}

bool WaqlIntelliSense::HandleTransformOrderBy()
{
	if (IsBufferReadable())
	{
		if (HandleValueExpression())
		{
 			if (IsBufferReadable())
			{
				if (iequals(vectorCommand.at(readIndex), "reverse"))
				{
					readIndex++;
				}
				else if(MatchingWordA("reverse", vectorCommand.at(readIndex)))
				{
					fittingCommands.insert("reverse");
				}
			}
			else
			{
				fittingCommands.insert("reverse");
			}
			return true;
		}
	}
	else
	{
		SearchMatches("", valueExpressions, sizeof(valueExpressions) / sizeof(valueExpressions[0]));
		SearchMatches("", concatenableReferences, sizeof(concatenableReferences) / sizeof(concatenableReferences[0]));
		SearchMatches("", nonConcatenableReferences, sizeof(nonConcatenableReferences) / sizeof(nonConcatenableReferences[0]));
		SearchMatchesInValueExpression("");
	}
	return false;
}

bool WaqlIntelliSense::HandleTransformDistinct()
{
	return true;
}


bool WaqlIntelliSense::HandleBooleanExpression()
{
	if (HandleValueExpression())
	{
		if (IsBufferReadable())
		{
			if (IsInArray(conditions, sizeof(conditions)/sizeof(conditions[0]), vectorCommand.at(readIndex)))
			{
				readIndex++;
				if (IsBufferReadable())
				{	
					if (IsEnclosedString(vectorCommand.at(readIndex)) || IsNumber(vectorCommand.at(readIndex), true))
					{
						readIndex++;
						return true;
					}
				}
				suggestions.insert("\"string\"");
				suggestions.insert("int");
			}
			else
			{
				SearchMatches(vectorCommand.at(readIndex), conditions, sizeof(conditions) / sizeof(conditions[0]));
			}
		}	
		else
		{
			SearchMatches("", conditions, sizeof(conditions) / sizeof(conditions[0]));
		}
	}
	return false;
}

bool WaqlIntelliSense::HandleObjectExpression(const bool allowConcatenation, const bool allowComma)
{
	if (IsBufferReadable()) 
	{
		bool found = false;
		bool concat = false;

		if (IsInArray(valueExpressions, sizeof(valueExpressions) / sizeof(valueExpressions[0]), vectorCommand.at(readIndex)) && allowConcatenation)
		{
			readIndex++;
			found = true;
			concat = true;
		}
		else if (IsInArray(concatenableReferences, sizeof(concatenableReferences) / sizeof(concatenableReferences[0]), vectorCommand.at(readIndex)) && allowConcatenation)
		{
			readIndex++;
			found = true;
			concat = true;
		}
		else if (IsInArray(nonConcatenableReferences, sizeof(nonConcatenableReferences) / sizeof(nonConcatenableReferences[0]), vectorCommand.at(readIndex)))
		{
			readIndex++;
			found = true;
			concat = false;
		}
		else if (IsInSet(valueProperties, vectorCommand.at(readIndex)))
		{
			readIndex++;
			found = true;
			concat = true;
		}
		
	
		if (found)
		{
			if (IsBufferReadable())
			{
				if (vectorCommand.at(readIndex) == "." && allowConcatenation)
				{
					readIndex++;
					return HandleObjectExpression(concat, allowComma);
				}
				else if (vectorCommand.at(readIndex) == ",")
				{
					if (allowComma)
					{
						readIndex++;
						return HandleObjectExpression(true, allowComma);
					}
					else
					{
						return false;
					}

				}
			}
			else
			{
				if (allowComma)
					fittingCommands.insert(",");
				if (allowConcatenation && concat)
					fittingCommands.insert(".");
			}
 			return true;
		}
		SearchMatches(vectorCommand.at(readIndex), valueExpressions, sizeof(valueExpressions) / sizeof(valueExpressions[0]));
		SearchMatches(vectorCommand.at(readIndex), concatenableReferences, sizeof(concatenableReferences) / sizeof(concatenableReferences[0]));
		SearchMatches(vectorCommand.at(readIndex), nonConcatenableReferences, sizeof(nonConcatenableReferences) / sizeof(nonConcatenableReferences[0]));
		SearchMatchesInValueExpression(vectorCommand.at(readIndex));
		return false;
	}
	SearchMatches("", valueExpressions, sizeof(valueExpressions) / sizeof(valueExpressions[0]));
	SearchMatches("", concatenableReferences, sizeof(concatenableReferences) / sizeof(concatenableReferences[0]));
	SearchMatches("", nonConcatenableReferences, sizeof(nonConcatenableReferences) / sizeof(nonConcatenableReferences[0]));
	SearchMatchesInValueExpression("");

	return false;
}

bool WaqlIntelliSense::HandleValueExpression()
{
	if(HandleObjectExpression(true, false))
	{
		return true;
	}
	return false;
}


bool WaqlIntelliSense::IsBufferReadable()
{
	return vectorCommand.size() > readIndex;
}

bool WaqlIntelliSense::IsWordInValueExpressions(const std::string& firstWordPart)
{
	for (const auto& word : valueExpressions)
	{
		auto part = word.find(firstWordPart + ".", 0);
		if (part != word.npos)
		{
			return true;
		}
	}
	return false;
}

bool WaqlIntelliSense::MatchingWordA(const std::string a, const std::string b)
{
	if (iequals(a.substr(0, b.size()), b))
	{
		return true;
	}
	return false;
}

bool WaqlIntelliSense::IsNumber(const std::string& part, bool allowNegative)
{
	if (part.size() > 0)
	{
		for (int i = 0; i < part.size(); i++)
		{
			if (!std::isdigit(part[i]))
			{
				if (part[i] != '-' || i != 0 || part.size() < 2 || !allowNegative)
				{
					return false;
				}
			}
		}
		return true;
	}
	return false;
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
