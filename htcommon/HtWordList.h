//
// HtWordList.h
//
// HtWordList: Specialized WordList class that can hold a list 
//	       of words waiting to be inserted in the database.
//
// Part of the ht://Dig package   <http://www.htdig.org/>
// Copyright (c) 1999 The ht://Dig Group
// For copyright details, see the file COPYING in your distribution
// or the GNU Public License version 2 or later
// <http://www.gnu.org/copyleft/gpl.html>
//
// $Id: HtWordList.h,v 1.1 1999/10/01 12:53:51 loic Exp $
//

#ifndef _HtWordList_h_
#define _HtWordList_h_

#include <fcntl.h>

#include "WordList.h"

class HtWordList : public WordList
{
public:
    //
    // Construction/Destruction
    //
    HtWordList(const Configuration& config_arg);
    virtual ~HtWordList();
    
    //
    // Update/add a word, perform sanity checking and
    // fill information.
    //
    void		Replace(const WordReference& wordRef);

    //
    // Mark a document as already scanned for words or mark it as disappeared
    //
    void		MarkGone();

    //
    // Flush the words stored in the object to the database
    //
    void		Flush();


private:

    List			*words;
};

#endif


