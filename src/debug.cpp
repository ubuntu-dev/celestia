// debug.cpp
//
// Copyright (C) 2001, Chris Laurel <claurel@shatters.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#ifdef _MSC_VER
#include <windows.h>
#endif
#include <stdio.h>
#include <cstdarg>


void DebugPrint(char *format, ...)
{
    va_list args;
    va_start(args, format);

#ifdef _MSC_VER
    char buf[1024];
    vsprintf(buf, format, args);
    OutputDebugString(buf);
#else
    vfprintf(stderr, format, args);
#endif

    va_end(args);
}


void Log(char *format, ...)
{
    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);

    va_end(args);
}
