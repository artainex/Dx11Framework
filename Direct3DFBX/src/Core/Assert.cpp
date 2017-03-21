// Precompiled Headers
#include "Core/Assert.h"

#include <MessageBox.h>
#include <sstream>
#include <string>


void Assert(char * const expression,
	const char * const message,
	const char * const filename,
	const size_t line)
{
	std::string file(filename);
	std::ostringstream msg;

	msg << "Expression: " << expression << std::endl
		<< "File: " << file.substr(file.find_last_of('\\') + 1) << ", Line: " << line
		<< std::endl << std::endl
		<< "Message: " << std::endl << message << std::endl;

	CreateMessageBox(msg.str().c_str(), "Assertion Error");
}