//
// HtFile.cc
//
// HtFile: Interface classes for retriving local documents
//
// Including:
// 	 -  Generic class
//
// Part of the ht://Dig package   <http://www.htdig.org/>
// Copyright (c) 1995-2000 The ht://Dig Group
// For copyright details, see the file COPYING in your distribution
// or the GNU Public License version 2 or later
// <http://www.gnu.org/copyleft/gpl.html>
//
// $Id: HtFile.cc,v 1.4 2003/01/03 13:26:19 lha Exp $ 
//

#ifdef HAVE_CONFIG_H
#include "htconfig.h"
#endif /* HAVE_CONFIG_H */

#include "lib.h"
#include "Transport.h"
#include "HtFile.h"
#include "Dictionary.h"
#include "StringList.h"
#include "defaults.h" // for config

#include <signal.h>
#include <sys/types.h>
#include <ctype.h>
#include <iostream.h>
#include <stdio.h> // for sscanf
#include <sys/stat.h>
#include <unistd.h>
#include <fstream.h>
#include <dirent.h> // for scandir


///////
   //    HtFile_Response class
///////


// Construction

HtFile_Response::HtFile_Response()
{
}


// Destruction

HtFile_Response::~HtFile_Response()
{
}

///////
   //    HtFile generic class
   //
   //    
///////


// Construction

HtFile::HtFile()
{
}

// Destruction

HtFile::~HtFile()
{
   // It's empty
}


// Return mime type indicated by extension  ext  (which is assumed not
// to contain the '.'), or  NULL  if  ext  is not a know mime type.
const String *HtFile::Ext2Mime (const char *ext)
{
   static Dictionary *mime_map = 0;

   if (!mime_map)
     {
       HtConfiguration* config= HtConfiguration::config();
       mime_map = new Dictionary();
       if (!mime_map)
	 return NULL;

       if (debug > 2)
 	    cout << "MIME types: " << config->Find("mime_types").get() << endl;
       ifstream in(config->Find("mime_types").get());
       if (in)
         {
           String line;
           while (in >> line)
             {
               line.chop("\n\r \t");
               int cmt;
               if ((cmt = line.indexOf('#')) >= 0)
                 line = line.sub(0, cmt);
               StringList split_line(line, "\t ");
               // Let's cache mime type to lesser the number of 
               // operator [] callings
               String mime_type = split_line[0];
               // Fill map with values.
               for (int i = 1; i < split_line.Count(); i++)
	       {
	         if (debug > 3)
		   cout << "MIME: " << split_line[i]
		        << "\t-> " << mime_type << endl;
                 mime_map->Add(split_line[i], new String(mime_type));
	       }
             }
         }
       else
	 {
	   if (debug > 2)
		cout << "MIME types file not found.  Using default types.\n";
	   mime_map->Add(String("html"), new String("text/html"));
	   mime_map->Add(String("htm"),  new String("text/html"));
	   mime_map->Add(String("txt"),  new String("text/plain"));
	   mime_map->Add(String("asc"),  new String("text/plain"));
	   mime_map->Add(String("pdf"),  new String("application/pdf"));
	   mime_map->Add(String("ps"),   new String("application/postscript"));
	   mime_map->Add(String("eps"),  new String("application/postscript"));
	 }
     }

   // return MIME type, or NULL if not found
   return (String *)mime_map->Find(ext);
}

// Return mime type of the file named 'fname'.
// If the type can't be determined, "application/x-unknown" is returned.
String HtFile::File2Mime (const char *fname)
{
     HtConfiguration* config= HtConfiguration::config();

    // default to "can't identify"
    char content_type [100] = "application/x-unknown\n";

    String cmd = config->Find ("content_classifier");
    if (cmd && *cmd)
    {
	cmd << " " << fname;
	FILE *fileptr;
	if ( (fileptr = popen (cmd.get(), "r")) != NULL )
	{
	    fgets (content_type, sizeof (content_type), fileptr);
	    pclose (fileptr);
	}
    }

    // Remove trailing newline, charset or language information
    int delim = strcspn (content_type, ",; \n\t");
    content_type [delim] = '\0';

    if (debug > 1)
	cout << "Mime type: " << fname << ' ' << content_type << endl;
    return (String (content_type));
}

///////
   //    Manages the requesting process
///////

HtFile::DocStatus HtFile::Request()
{
   // Reset the response
   _response.Reset();
   
   struct stat stat_buf;
   // Check that it exists, and is a regular file or directory
   // Don't allow symbolic links to directories; they mess up '../'.
   // Should we allow FIFO's?
   if ( stat(_url.path(), &stat_buf) != 0 || 
	!(S_ISREG(stat_buf.st_mode) || S_ISDIR(stat_buf.st_mode)) )
     return Transport::Document_not_found;

   // Now handle directories with a pseudo-HTML document (and appropriate noindex)
   if ( S_ISDIR(stat_buf.st_mode) )
     {
       _response._content_type = "text/html";
       _response._contents = "<html><head><meta name=\"robots\" content=\"noindex\">\n";

       struct dirent *namelist;
       DIR *dirList;
       String filename;

       if (( dirList = opendir(_url.path()) ))
	 {
	   while (( namelist = readdir(dirList) ))
	    {
	     filename = _url.path();
	     filename << namelist->d_name;
	     
	     if ( namelist->d_name[0] != '.' 
		  && lstat(filename.get(), &stat_buf) == 0 )
	       {
		 // Recursively resolve symbolic links.
		 // Could leave "absolute" links, or even all not
		 // containing '../'.  That would allow "aliasing" of
		 // directories without causing loops.

		 int i;		// avoid infinite loops
		 for (i=0; (stat_buf.st_mode & S_IFMT) == S_IFLNK && i<10; i++)
		 {
		     char link [100];
		     int count = readlink(filename.get(), link, sizeof(link)-1);

		     if (count < 0)
			 break;
		     link [count] = '\0';
		     URL newURL (link, _url);	// resolve relative paths
		     filename = newURL.path();
		     if (debug > 2)
			 cout << "Link to " << link << " gives "
			      << filename.get() << endl;
		     lstat(filename.get(), &stat_buf);
		 }
		 // filename now only sym-link if nested too deeply or I/O err.

		 if (S_ISDIR(stat_buf.st_mode))
		   _response._contents << "<link href=\"file://"
				       << filename.get() << "/\">\n";
		 else if (S_ISREG(stat_buf.st_mode))
		   _response._contents << "<link href=\"file://"
				       << filename.get() << "\">\n";  
	       }
	    }
	   closedir(dirList);
	 }

       _response._contents << "</head><body></body></html>\n";

       if (debug > 4)
	 cout << " Directory listing: " << endl << _response._contents << endl;

       _response._content_length = stat_buf.st_size;
       _response._document_length = _response._contents.length();
       _response._modification_time = new HtDateTime(stat_buf.st_mtime);
       _response._status_code = 0;
       return Transport::Document_ok;
     }

   if (_modification_time && *_modification_time >= HtDateTime(stat_buf.st_mtime))
     return Transport::Document_not_changed;

   bool unknown_ext = false;
   char *ext = strrchr(_url.path(), '.');
   if (ext == NULL)
      unknown_ext = true;
   else
   {
      const String *mime_type = Ext2Mime(ext + 1);
      if (mime_type)
        _response._content_type = *mime_type;
      else
        unknown_ext = true;
   }
   if (unknown_ext)
   {
       _response._content_type = File2Mime (_url.path());
       if (!strncmp (_response._content_type.get(), "application/x-", 14))
           return Transport::Document_not_local;
   }

   _response._modification_time = new HtDateTime(stat_buf.st_mtime);

   FILE *f = fopen((const char *)_url.path(), "r");
   if (f == NULL)
     return Document_not_found;

   char	docBuffer[8192];
   int		bytesRead;
   while ((bytesRead = fread(docBuffer, 1, sizeof(docBuffer), f)) > 0)
     {
	if (_response._contents.length() + bytesRead > _max_document_size)
	    bytesRead = _max_document_size - _response._contents.length();
	_response._contents.append(docBuffer, bytesRead);
	if (_response._contents.length() >= _max_document_size)
	    break;
     }
   fclose(f);

   _response._content_length = stat_buf.st_size;
   _response._document_length = _response._contents.length();
   _response._status_code = 0;

   if (debug > 2)
     cout << "Read a total of " << _response._document_length << " bytes\n";
   return Transport::Document_ok;
}

HtFile::DocStatus HtFile::GetDocumentStatus()
{ 
   // Let's give a look at the return status code
   if (_response._status_code == -1)
      return Transport::Document_not_found;
   return Transport::Document_ok;
}

