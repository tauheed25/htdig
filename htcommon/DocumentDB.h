//
// DocumentDB.h
//
// This class is the interface to the database of document references.
// This database is only used while digging.  An extract of this
// database is used for searching.  This is because digging requires a
// different index than searching.
//
// $Id: DocumentDB.h,v 1.5 1999/01/25 01:53:42 hp Exp $
//
// $Log: DocumentDB.h,v $
// Revision 1.5  1999/01/25 01:53:42  hp
// Provide a clean upgrade from old databses without "url_part_aliases" and
// "common_url_parts" through the new option "uncoded_db_compatible".
//
// Revision 1.4  1999/01/14 01:09:11  ghutchis
// Small speed improvements based on gprof.
//
// Revision 1.3  1999/01/14 00:30:10  ghutchis
// Added IncNextDocID to allow big changes in NextDocID, such as when merging
// databases.
//
// Revision 1.2  1998/01/05 00:47:27  turtle
// reformatting
//
// Revision 1.1.1.1  1997/02/03 17:11:07  turtle
// Initial CVS
//
//
#ifndef _DocumentDB_h_
#define _DocumentDB_h_

#include "DocumentRef.h"
#include <List.h>
#include <Database.h>


class DocumentDB
{
public:
    //
    // Construction/Destruction
    //
    DocumentDB();
    ~DocumentDB();

    //
    // The database used for searching is generated from our internal database:
    //
    int			CreateSearchDB(char *filename);

    //
    // Standard database operations
    //
    int			Open(char *filename);
    int			Read(char *filename);
    int			Close();

    int			Add(DocumentRef &);
    DocumentRef		*operator [] (char *url);
    int			Exists(char *url);
    int			Delete(char *url);

    //
    // The database keeps track of document ids.  Here is a way to get
    // the next document id.
    //
    int			NextDocID()		{return nextDocID++;}

    // And here's a way to increment NextDocID after adding lots of records
    // (for example when merging databases!)
    void                IncNextDocID (int next)     {nextDocID += next;}

    //
    // We will need to be able to iterate over the complete database.
    //
    List		*URLs();	// This returns a list of all the URLs

    //
    // Set compatibility mode (try to support when database
    // contains *unencoded* URLs as keys).
    //
    inline void                SetCompatibility(int on_flag = 1)
    { myTryUncoded = on_flag; }

private:
    Database		*dbf;
    int			isopen;
    int			isread;
    int			nextDocID;
    String                    temp;
    int			myTryUncoded;	// "Compatibility" mode.
};

#endif


