#pragma once

#include <Windows.h>
#include "MessageBox.h"

void CreateMessageBox(const std::string & message)
{
	CreateMessageBox(message, std::string());
}

void CreateMessageBox(const std::string & message, const std::string & caption)
{
	int result = ::MessageBox(nullptr, message.c_str(),
		caption.c_str(), MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON2);

	switch (result)
	{
	case IDABORT:
		::exit(EXIT_FAILURE);
		break;

	case IDRETRY:
		::DebugBreak();
		break;

	case IDIGNORE:
		break;
	}
}