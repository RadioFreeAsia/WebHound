// whprocess.cpp
//
// whprocess(1) WebHound log processor
//
//   (C) Copyright 2017-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include <QCoreApplication>
#include <QStringList>

#include "clfentry.h"
#include "cmdswitch.h"
#include "db.h"
#include "whprocess.h"

MainObject::MainObject(QWidget *parent)
{
  QString filename;
  FILE *f=NULL;
  char line[1024];
  int lineno=0;
  wh_log_to_syslog=false;

  CmdSwitch *cmd=new CmdSwitch(qApp->argc(),qApp->argv(),"whprocess",VERSION,
			       WHPROCESS_USAGE);
  if(cmd->keys()==0) {
    fprintf(stderr,"whprocess: no log specified\n");
    exit(1);
  }
  for(unsigned i=0;i<cmd->keys();i++) {
    if(i==(cmd->keys()-1)) {
      if(cmd->key(i).left(2)=="--") {
	fprintf(stderr,"whprocess: no log specified\n");
	LogToSyslog(LOG_ERR,"no log specified");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--log-to-syslog") {
      wh_log_to_syslog=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"whprocess: unknown option \"%s\"\n",
	      (const char *)cmd->key(i).toUtf8());
      LogToSyslog(LOG_ERR,"unknown option \"%s\"",
		  (const char *)cmd->key(i).toUtf8());
      exit(1);
    }
  }
  filename=cmd->key(cmd->keys()-1);
  delete cmd;

  //
  // Open Database
  //
  QString err_msg;
  wh_config=new Config();
  if(!wh_config->openDb(&err_msg,true)) {
    fprintf(stderr,"unable to open database [%s]\n",
	    (const char *)err_msg.toUtf8());
    LogToSyslog(LOG_ERR,"unable to open database [%s]",
	    (const char *)err_msg.toUtf8());
    exit(1);
  }

  //
  // Process Logfile
  //
  if((f=fopen(filename.toUtf8(),"r"))==NULL) {
    fprintf(stderr,"whprocess: unable to open logfile [%s]\n",strerror(errno));
    LogToSyslog(LOG_ERR,"unable to open logfile \"%s\" [%s]\n",
		(const char *)filename.toUtf8(),
		strerror(errno));
    exit(1);
  }
  while(fgets(line,1024,f)!=NULL) {
    if(!ProcessLine(QString(line).trimmed())) {
      fprintf(stderr,"line %d: parser error [%s]\n",lineno+1,line);
      LogToSyslog(LOG_WARNING,"line %d: parser error [%s]\n",lineno+1,line);
    }
  }

  LogToSyslog(LOG_INFO,"processed \"%s\"",(const char *)filename.toUtf8());

  exit(0);
}


bool MainObject::ProcessLine(const QString &line)
{
  QString sql;
  SqlQuery *q;
  SqlQuery *q1;
  bool ret=false;
  ClfEntry *clf=new ClfEntry();
  int pgm_quan=0;

  if(clf->parse(line)) {
    //
    // Log known UA(s)
    //
    for(int i=0;i<clf->userAgentQuantity();i++) {
      sql=QString("select ID from PROGRAMS where ")+
	"USER_AGENT_STRING=\""+SqlQuery::escape(clf->userAgent(i)->name())+"\"";
      q=new SqlQuery(sql);
      while(q->next()) {
	pgm_quan++;
	sql=QString("select LAST_SEEN from HOSTS where ")+
	  "(IP_ADDRESS=\""+SqlQuery::escape(clf->hostAddress().toString())+
	  "\")&&"+
	  QString().sprintf("(PROGRAM_ID=%d)",q->value(0).toInt());
	q1=new SqlQuery(sql);
	if(q1->first()) {
	  if(q1->value(0).toDateTime()<clf->timestamp()) {
	    sql=QString("update HOSTS set ")+
	      "NAME=\""+SqlQuery::escape(clf->hostName())+"\","+
	      "USER_AGENT_VERSION=\""+
	      SqlQuery::escape(clf->userAgent(i)->version())+"\","+
	      "USER_AGENT_OPTIONS=\""+
	      SqlQuery::escape(clf->userAgent(i)->options().join("; "))+"\","+
	      "LAST_SEEN=\""+
	      clf->timestamp().toString("yyyy-MM-dd hh:mm:ss")+"\" where "+
	      "(IP_ADDRESS=\""+SqlQuery::escape(clf->hostAddress().toString())+
	      "\")&&"+
	      QString().sprintf("(PROGRAM_ID=%d)",q->value(0).toInt());
	    SqlQuery::run(sql);
	  }
	}
	else {
	  sql=QString("insert into HOSTS set ")+
	    "NAME=\""+SqlQuery::escape(clf->hostName())+"\","+
	    "IP_ADDRESS=\""+SqlQuery::escape(clf->hostAddress().toString())+
	    "\","+
	    QString().sprintf("PROGRAM_ID=%d,",q->value(0).toInt())+
	    "USER_AGENT_VERSION=\""+
	    SqlQuery::escape(clf->userAgent(i)->version())+"\","+
	    "USER_AGENT_OPTIONS=\""+
	    SqlQuery::escape(clf->userAgent(i)->options().join("; "))+"\","+
	    "LAST_SEEN=\""+
	    clf->timestamp().toString("yyyy-MM-dd hh:mm:ss")+"\"";
	  SqlQuery::run(sql);
	}
	delete q1;
      }
      delete q;
    }

    //
    // Log if no known UAs were found
    //
    if(pgm_quan==0) {
      sql=QString("select ID from UNKNOWN_USER_AGENTS where ")+
	"(USER_AGENT=\""+SqlQuery::escape(clf->userAgentString())+"\")";
      q=new SqlQuery(sql);
      if(q->first()) {
	sql=QString("select ID from UNKNOWN_HOSTS where ")+
	  QString().sprintf("(USER_AGENT_ID=%d)&&",q->value(0).toInt())+
	  "(HOSTNAME=\""+SqlQuery::escape(clf->hostName())+"\")";
	q1=new SqlQuery(sql);
	if(q1->first()) {
	  sql=QString("update UNKNOWN_HOSTS set ")+
	    "LOG_LINE=\""+SqlQuery::escape(line)+"\","+
	    "LAST_SEEN=\""+
	    clf->timestamp().toString("yyyy-MM-dd hh:mm:ss")+"\" "+
	    "where "+
	    "(IP_ADDRESS=\""+SqlQuery::escape(clf->hostAddress().toString())+
	    "\")&&"+
	    QString().sprintf("(USER_AGENT_ID=%d)",q1->value(0).toInt());
	  SqlQuery::run(sql);
	}
	else {
	  sql=QString("insert into UNKNOWN_HOSTS set ")+
	    "HOSTNAME=\""+SqlQuery::escape(clf->hostName())+"\","+
	    "IP_ADDRESS=\""+SqlQuery::escape(clf->hostAddress().toString())+
	    "\","+
	    QString().sprintf("USER_AGENT_ID=%d,",q->value(0).toInt())+
	    "LAST_SEEN=\""+clf->timestamp().toString("yyyy-MM-dd hh:mm:ss")+
	    "\","+
	    "LOG_LINE=\""+SqlQuery::escape(line)+"\"";
	  SqlQuery::run(sql);
	}
	delete q1;
      }
      else {
	sql=QString("insert into UNKNOWN_USER_AGENTS set ")+
	  "USER_AGENT=\""+SqlQuery::escape(clf->userAgentString())+"\"";
	int ua_id=SqlQuery::run(sql).toInt();
	sql=QString("insert into UNKNOWN_HOSTS set ")+
	  "HOSTNAME=\""+SqlQuery::escape(clf->hostName())+"\","+
	  "IP_ADDRESS=\""+SqlQuery::escape(clf->hostAddress().toString())+"\","+
	  QString().sprintf("USER_AGENT_ID=%d,",ua_id)+
	  "LAST_SEEN=\""+clf->timestamp().toString("yyyy-MM-dd hh:mm:ss")+"\","+
	  "LOG_LINE=\""+SqlQuery::escape(line)+"\"";
	SqlQuery::run(sql);
      }
      delete q;
    }
    ret=true;
  }
  delete clf;
  return ret;
}


void MainObject::LogToSyslog(int priority,const char *fmt,...) const
{
  va_list args;

  if(wh_log_to_syslog) {
    va_start(args,fmt);
    vsyslog(priority,fmt,args);
    va_end(args);
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
