// createdb.cpp
//
// MySQL Database routines for webhound(1).
//
// (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
//     All Rights Reserved.
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

#include <QMessageBox>

#include "db.h"
#include "whc.h"

bool MainWidget::CheckDb(QString *err_msg)
{
  QString sql=QString("select DB from VERSION");
  SqlQuery *q=new SqlQuery(sql);
  if(!q->first()) {
    if(!CreateDb()) {
      *err_msg=tr("unable to create database");
      return false;
    }
  }
  delete q;
  if(!CheckSchema()) {
    *err_msg=tr("invalid/unrecognized database schema");
    return false;
  }

  return true;
}


bool MainWidget::CreateDb()
{
  QString sql;
  bool ok=false;

  sql=QString("create table VERSION(DB int not null)")+
    whc_config->createTablePostfix();
  SqlQuery::run(sql,&ok);
  if(!ok) {
    return false;
  }
  sql=QString("insert into VERSION values(0)");
  SqlQuery::run(sql,&ok);

  return ok;
}


bool MainWidget::CheckSchema()
{
  int schema;
  bool ok=false;
  QString sql;
  SqlQuery *q=NULL;

  sql=QString("select DB from VERSION");
  q=new SqlQuery(sql);
  if(!q->first()) {
    delete q;
    return false;
  }
  schema=q->value(0).toInt();
  delete  q;

  if(schema>WEBHOUND_SCHEMA_VERSION) {
    QMessageBox::warning(this,tr("WebHound - DB Schema Skew"),
	   tr("This version of WebHound is incompatible with schema version")+
			 QString().sprintf(" %d.",schema));
    exit(256);
  }

  if(schema<WEBHOUND_SCHEMA_VERSION) {
    if(QMessageBox::question(this,tr("WebHound - DB Update"),
			     tr("The DB schema needs to be updated.")+"\n"+
			     tr("Proceed?"),
			     QMessageBox::No,QMessageBox::Yes)!=
       QMessageBox::Yes) {
      exit(0);
    }
  }

  if(schema<1) {
    sql=QString("create table PROGRAMS (")+
      "ID integer primary key auto_increment,"+
      "NAME char(64) unique not null,"+
      "USER_AGENT_STRING char(64),"+
      "DESCRIPTION text,"+
      "index USER_AGENT_STRING_IDX(USER_AGENT_STRING))"+
      whc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }

    sql=QString("create table HOSTS (")+
      "ID integer primary key auto_increment,"+
      "NAME char(255) not null,"+
      "PROGRAM_ID int not null,"+
      "USER_AGENT_VERSION char(32),"+
      "USER_AGENT_OPTIONS text,"+
      "LAST_SEEN datetime not null,"+
      "index NAME_IDX(NAME),"+
      "index PROGRAM_ID_IDX(PROGRAM_ID,NAME))"+
      whc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }

  if(schema<2) {
    sql=QString("create table UNKNOWN_EVENTS (")+
      "ID integer primary key auto_increment,"+
      "HOSTNAME char(255) not null,"+
      "USER_AGENT char(255),"+
      "LAST_SEEN datetime not null,"+
      "LOG_LINE text not null,"+
      "index HOSTNAME_IDX(HOSTNAME,USER_AGENT))"+
      whc_config->createTablePostfix();
    SqlQuery::run(sql,&ok);
    if(!ok) {
      return false;
    }
  }



  //
  // *** End of schema updates ***
  //

  sql=QString("update VERSION set ")+
    QString().sprintf("DB=%d",WEBHOUND_SCHEMA_VERSION);
  SqlQuery::run(sql,&ok);

  return ok;
}
