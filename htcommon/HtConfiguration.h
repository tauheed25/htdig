#ifndef	_HtConfiguration_h_
#define	_HtConfiguration_h_

#include"Configuration.h"
#include "URL.h"

class HtConfiguration : public Configuration
{
 public:
    const String	Find(const String& name) const {return(Configuration::Find(name));}
    int		Value(const String& name, int default_value = 0) const     {return(Configuration::Value  (name,default_value));}
    double	Double(const String& name, double default_value = 0) const {return(Configuration::Double (name,default_value));}
    int		Boolean(const String& name, int default_value = 0) const   {return(Configuration::Boolean(name,default_value));}
    void		Add(const String& str){Configuration::Add(str);}
    void		Add(const String& name, const String& value){Configuration::Add(name,value);}

    void		Add(char *name, char *value, HtConfiguration *aList);
    const String	Find(URL *aUrl, const char *value) const;
    const String	Find(const char *blockName, const char *name, const char *value) const;
    int		Value(char *blockName,char *name,char *value,int default_value = 0);
    double	Double(char *blockName,char *name,char *value,double default_value = 0);
    int		Boolean(char *blockName,char *name,char *value,int default_value = 0);
    int		Value(URL *aUrl,char *value,int default_value = 0);
    double	Double(URL *aUrl,char *value,double default_value = 0);
    int		Boolean(URL *aUrl,char *value,int default_value = 0);

    //
    // We need some way of reading in the database from a configuration file
    // ... this uses the parser
    virtual int         Read(const String& filename);

 protected:
    Dictionary          dcServers;
    Dictionary          dcUrls;

 public:
    HtConfiguration():Configuration()
	{;}

    HtConfiguration(const HtConfiguration& config) :
	Configuration(config),
	dcServers(config.dcServers),
	dcUrls(config.dcUrls)
	{
	    ;
	}

};


#endif
