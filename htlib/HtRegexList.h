//
// HtRegexList.h
//
// HtRegexList: A list of HtRegex objects for handling large regex patterns
//
// Part of the ht://Dig package   <http://www.htdig.org/>
// Copyright (c) 1999-2001 The ht://Dig Group
// For copyright details, see the file COPYING in your distribution
// or the GNU General Public License version 2 or later 
// <http://www.gnu.org/copyleft/gpl.html>
//
// $Id: HtRegexList.h,v 1.1.2.1 2001/02/11 23:08:29 ghutchis Exp $
//
//

#ifndef	_HtRegexList_h_
#define	_HtRegexList_h_

#include "Object.h"
#include "List.h"
#include "StringList.h"
#include "HtRegex.h"

class HtRegexList : public List
{
public:
    //
    // Construction/Destruction
    //
    HtRegexList();
    virtual ~HtRegexList();

    //
    // Setting (construct from a list of patterns)
    // 
    int		setEscaped(StringList &list, int case_sensitive = 0);

    virtual const String &lastError();	// returns the last error message

    //
    // Methods for checking a match
    //
    int		match(const String& str, int nullmatch, int nullstr) 
      { return match(str.get(), nullmatch, nullstr); }
    int		match(const char *str, int nullmatch, int nullstr);

protected:
    int			compiled;

    String		lastErrorMessage;

private:
};

#endif


