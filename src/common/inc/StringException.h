/**
 * @author Thom Troy
 *
 * Copyright (C) 2015 Thom Troy
 */

#ifndef _VSID_COMMON_STRING_EXCEPTION_H__
#define _VSID_COMMON_STRING_EXCEPTION_H__

namespace VsidCommon
{

class StringException : public std::exception
{
public:
	StringException(std::string what) { _what = what; }
	const char* what() const noexcept { return _what.c_str(); }
private:
	std::string _what;
};

} // end namespace

#endif // END HEADER GUARD