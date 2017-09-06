// whprocess.cpp
//
// whprocess(1) WebHound log processor
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

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

  CmdSwitch *cmd=new CmdSwitch(qApp->argc(),qApp->argv(),"whprocess",VERSION,
			       WHPROCESS_USAGE);
  if(cmd->keys()==0) {
    fprintf(stderr,"whprocess: no log specified\n");
    exit(1);
  }
  filename=cmd->key(cmd->keys()-1);
  delete cmd;

  //
  // Open Database
  //
  QString err_msg;
  wh_config=new Config();
  if(!wh_config->openDb(&err_msg,true)) {
    fprintf(stderr,"unable to open database [%s]",
	    (const char *)err_msg.toUtf8());
    exit(1);
  }

  //
  // Process Logfile
  //
  if((f=fopen(filename.toUtf8(),"r"))==NULL) {
    fprintf(stderr,"whprocess: %s\n",strerror(errno));
    exit(1);
  }
  while(fgets(line,1024,f)!=NULL) {
    if(!ProcessLine(QString(line).trimmed())) {
      fprintf(stderr,"line %d: parser error\n",lineno+1);
    }
  }
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
    for(int i=0;i<clf->userAgentQuantity();i++) {
      sql=QString("select ID from PROGRAMS where ")+
	"USER_AGENT_STRING=\""+SqlQuery::escape(clf->userAgent(i)->name())+"\"";
      q=new SqlQuery(sql);
      while(q->next()) {
	pgm_quan++;
	sql=QString("select LAST_SEEN from HOSTS where ")+
	  "(NAME=\""+SqlQuery::escape(clf->hostName())+"\")&&"+
	  QString().sprintf("(PROGRAM_ID=%d)",q->value(0).toInt());
	q1=new SqlQuery(sql);
	if(q1->first()) {
	  if(q1->value(0).toDateTime()<clf->timestamp()) {
	    sql=QString("update HOSTS set ")+
	      "LAST_SEEN=\""+
	      clf->timestamp().toString("yyyy-MM-dd hh:mm:ss")+"\" where "+
	      "(NAME=\""+SqlQuery::escape(clf->hostName())+"\")&&"+
	      QString().sprintf("(PROGRAM_ID=%d)",q->value(0).toInt());
	  printf("SQL: %s\n",(const char *)sql.toUtf8());
	    SqlQuery::run(sql);
	  }
	}
	else {
	  sql=QString("insert into HOSTS set ")+
	    "NAME=\""+SqlQuery::escape(clf->hostName())+"\","+
	    QString().sprintf("PROGRAM_ID=%d,",q->value(0).toInt())+
	    "USER_AGENT_VERSION=\""+
	    SqlQuery::escape(clf->userAgent(i)->version())+"\","+
	    "USER_AGENT_OPTIONS=\""+
	    SqlQuery::escape(clf->userAgent(i)->options().join("; "))+"\","+
	    "LAST_SEEN=\""+
	    clf->timestamp().toString("yyyy-MM-dd hh:mm:ss")+"\"";
	  printf("SQL: %s\n",(const char *)sql.toUtf8());
	  SqlQuery::run(sql);
	}
	delete q1;
      }
      delete q;
    }
    if(pgm_quan==0) {
      sql=QString("select ID from UNKNOWN_EVENTS where ")+
	"(HOSTNAME=\""+SqlQuery::escape(clf->hostName())+"\")&&"+
	"(USER_AGENT=\""+SqlQuery::escape(clf->userAgentString())+"\")";
      q=new SqlQuery(sql);
      if(q->first()) {
	sql=QString("update UNKNOWN_EVENTS set ")+
	  "LOG_LINE=\""+SqlQuery::escape(line)+"\","+
	  "LAST_SEEN=\""+clf->timestamp().toString("yyyy-MM-dd hh:mm:ss")+"\" "+
	  "where "+
	  "(HOSTNAME=\""+SqlQuery::escape(clf->hostName())+"\")&&"+
	  "(USER_AGENT=\""+SqlQuery::escape(clf->userAgentString())+"\")";
	SqlQuery::run(sql);
      }
      else {
	sql=QString("insert into UNKNOWN_EVENTS set ")+
	  "HOSTNAME=\""+SqlQuery::escape(clf->hostName())+"\","+
	  "USER_AGENT=\""+SqlQuery::escape(clf->userAgentString())+"\","+
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


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
