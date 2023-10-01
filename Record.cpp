#include <iostream>
#include "Record.h"

Record::Record (int value, const std::string& text ) : _value (value),_text(text)
{
	// TRACE (true);
} // ScanPlan::ScanPlan

Record::~Record ()
{
	TRACE (true);
} // ScanPlan::~ScanPlan

void Record::display() const
{
    std::cout << "Value: " << _value << ", Text: " << _text << std::endl;
}

bool Record::isEqual(const Record& other) const
{
    return _value == other._value && _text == other._text;
}