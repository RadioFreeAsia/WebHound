// clfentry.cpp
//
// Abstract a Combined Log Format (CLF) entry.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdio.h>

#include <QStringList>

#include "clfentry.h"

UserAgent::UserAgent()
{
}


QString UserAgent::name() const
{
  return ua_name;
}


void UserAgent::setName(const QString &str)
{
  ua_name=str;
}


QString UserAgent::version() const
{
  return ua_version;
}


void UserAgent::setVersion(const QString &str)
{
  ua_version=str;
}


QStringList UserAgent::options() const
{
  return ua_options;
}


void UserAgent::addOption(const QString &str)
{
  ua_options.push_back(str);
}




ClfEntry::ClfEntry()
{
  clf_status_code=0;
  clf_size=0;
}


QString ClfEntry::hostName() const
{
  return clf_host_name;
}


QString ClfEntry::identUsername() const
{
  return clf_ident_username;
}


QString ClfEntry::httpUsername() const
{
  return clf_http_username;
}


QDateTime ClfEntry::timestamp() const
{
  return clf_timestamp;
}


QString ClfEntry::requestMethod() const
{
  return clf_request_method;
}


QString ClfEntry::requestResource() const
{
  return clf_request_resource;
}


QString ClfEntry::requestProtocolType() const
{
  return clf_request_protocol_type;
}


QString ClfEntry::requestProtocolVersion() const
{
  return clf_request_protocol_version;
}


unsigned ClfEntry::statusCode() const
{
  return clf_status_code;
}


unsigned ClfEntry::size() const
{
  return clf_size;
}


QString ClfEntry::referrer() const
{
  return clf_referrer;
}


QString ClfEntry::userAgentString() const
{
  return clf_user_agent_string;
}


int ClfEntry::userAgentQuantity() const
{
  return clf_user_agents.size();
}


UserAgent *ClfEntry::userAgent(int n) const
{
  return clf_user_agents.at(n);
}


QString ClfEntry::dump() const
{
  QString ret="";

  ret+="hostName: "+hostName()+"\n";
  ret+="identUsername: "+identUsername()+"\n";
  ret+="httpUsername: "+httpUsername()+"\n";
  ret+="timestamp: "+timestamp().toString("yyyy-MM-dd hh:mm:ss")+"\n";
  ret+="requestMethod: "+requestMethod()+"\n";
  ret+="requestResource: "+requestResource()+"\n";
  ret+="requestProtocolType: "+requestProtocolType()+"\n";
  ret+="requestProtocolVersion: "+requestProtocolVersion()+"\n";
  ret+="statusCode: "+QString().sprintf("%03u",statusCode())+"\n";
  ret+="size: "+QString().sprintf("%u",size())+"\n";
  ret+="referrer: "+referrer()+"\n";
  ret+="userAgentString: "+userAgentString()+"\n";
  for(int i=0;i<userAgentQuantity();i++) {
    ret+=QString().sprintf("  userAgent(%d) name: ",i)+
      userAgent(i)->name()+"\n";
    ret+=QString().sprintf("  userAgent(%d) version: ",i)+
      userAgent(i)->version()+"\n";
    QStringList options=userAgent(i)->options();
    for(int j=0;j<options.size();j++) {
      ret+=QString().sprintf("  userAgent(%d) option(%d): ",i,j)+
	options.at(j)+"\n";
    }
    ret+="\n";
  }

  return ret;
}


bool ClfEntry::parse(const QString &line)
{
  QStringList f0;
  QStringList f1;
  int field=0;
  bool ok=false;

  f0=line.split(" ");
  for(int i=0;i<f0.size();i++) {
    switch(field) {
    case 0:  // Hostname
      clf_host_name=StringField(f0.at(i));
      field++;
      break;

    case 1:  // Ident Username
      clf_ident_username=StringField(f0.at(i));
      field++;
      break;

    case 2:  // HTTP Username
      clf_http_username=StringField(f0.at(i));
      field++;
      break;

    case 3:  // Timestamp    FIXME: We ignore the GMT offset here!
      if(f0.size()<(i+2)) {
	return false;
      }
      if((f0.at(i).left(1)!="[")||(f0.at(i+1).right(1)!="]")) {
	return false;
      }
      f1=f0.at(i).split(":");
      if(f1.size()!=4) {
	return false;
      }
      clf_timestamp=QDateTime(DateField(f1.at(0).right(f1.at(0).length()-1)),
			      QTime(f1.at(1).toInt(),f1.at(2).toInt(),
				    f1.at(3).toInt()));
      if(!clf_timestamp.isValid()) {
	return false;
      }
      field++;
      i++;
      break;

    case 4:  // Request Method
      if(f0.at(i).left(1)!="\"") {
	return false;
      }
      clf_request_method=f0.at(i).right(f0.at(i).length()-1);
      field++;
      break;

    case 5:  // Request Resource
      clf_request_resource=StringField(f0.at(i));
      field++;
      break;

    case 6:  // Request Protocol
      if(f0.at(i).right(1)!="\"") {
	return false;
      }
      f1=f0.at(i).left(f0.at(i).length()-1).split("/");
      if(f1.size()!=2) {
	return false;
      }
      clf_request_protocol_type=f1.at(0);
      clf_request_protocol_version=f1.at(1);
      field++;
      break;

    case 7:  // Status Code
      clf_status_code=NumberField(f0.at(i));
      if((clf_status_code<100)||(clf_status_code>=600)) {
	return false;
      }
      field++;
      break;

    case 8:  // Size
      clf_size=NumberField(f0.at(i));
      field++;
      break;

    case 9:  // Referrer
      clf_referrer=DelimitedField(&f0,&i,"\"","\"",&ok);
      if(!ok) {
	return false;
      }
      field++;
      break;

    case 10:  // User Agent
      clf_user_agent_string=DelimitedField(&f0,&i,"\"","\"",&ok);
      if(!ok) {
	return false;
      }
      ParseUserAgents(clf_user_agent_string);
      field++;
      break;
    }
  }

  return true;
}


bool ClfEntry::ParseUserAgents(const QString &str)
{
  QStringList f0;
  QStringList f1;
  QString options;
  bool ok=false;

  f0=str.split(" ");
  for(int i=0;i<f0.size();i++) {
    if(f0.at(i).left(1)=="(") {  // Options
      options=DelimitedField(&f0,&i,"(",")",&ok);
      if(clf_user_agents.size()>0) {
	f1=options.split(";");
	for(int j=0;j<f1.size();j++) {
	  clf_user_agents.back()->addOption(f1.at(j).trimmed());
	}
      }
    }
    else {                       // Agent
      f1=f0[i].split("/");
      if(f1.size()!=2) {
	return false;
      }
      clf_user_agents.push_back(new UserAgent());
      clf_user_agents.back()->setName(f1.at(0));
      clf_user_agents.back()->setVersion(f1.at(1));
    }
  }
  return true;
}


QString ClfEntry::StringField(const QString &str) const
{
  if(str=="-") {
    return QString();
  }
  return str;
}


unsigned ClfEntry::NumberField(const QString &str) const
{
  if(str=="-") {
    return 0;
  }
  return str.toUInt();
}


QDate ClfEntry::DateField(const QString &str) const
{
  QStringList f0=str.split("/");
  if(f0.size()!=3) {
    return QDate();
  }
  return QDate(f0.at(2).toInt(),MonthField(f0.at(1)),f0.at(0).toInt());
}


int ClfEntry::MonthField(const QString &str) const
{
  for(int i=1;i<13;i++) {
    if(QDate::shortMonthName(i).toLower()==str.toLower()) {
      return i;
    }
  }
  return 0;
}


QString ClfEntry::DelimitedField(QStringList *fields,int *ptr,
				 const QString &delim1,const QString &delim2,
				 bool *ok)
{
  QString ret="";

  if(fields->at(*ptr).left(1)!=delim1) {
    *ok=false;
    return QString();
  }
  ret=fields->at(*ptr);
  if(fields->at(*ptr).right(1)!=delim2) {
    for(int j=*ptr+1;j<fields->size();j++) {
      ret+=" "+fields->at(j);
      if(fields->at(j).right(1)==delim2) {
	*ptr=j;
	j=fields->size();
      }
    }
  }
  ret.replace(delim1,"");
  ret.replace(delim2,"");
  ret=StringField(ret);

  *ok=true;
  return ret;
}
