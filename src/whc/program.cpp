// program.cpp
//
// Abstract a GlassNet program.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "program.h"

Program::Program(int id)
{
  program_id=id;
}


int Program::id() const
{
  return program_id;
}


bool Program::exists() const
{
  return Program::exists(program_id);
}


QString Program::name() const
{
  return getRow("NAME").toString();
}


void Program::setName(const QString &name)
{
  setRow("NAME",name);
}


QString Program::userAgentString() const
{
  return getRow("USER_AGENT_STRING").toString();
}


void Program::setUserAgentString(const QString &str)
{
  setRow("USER_AGENT_STRING",str);
}


QString Program::description() const
{
  return getRow("DESCRIPTION").toString();
}


void Program::setDescription(const QString &str)
{
  setRow("DESCRIPTION",str);
}


int Program::create()
{
  QString sql=QString("insert into PROGRAMS set ")+
    "NAME=\"[new program]\"";
  return SqlQuery::run(sql).toInt();
}


void Program::remove(int pgm_id)
{
  QString sql=QString("delete from PROGRAMS where ")+
    QString().sprintf("ID=%d",pgm_id);
  SqlQuery::run(sql);
}


bool Program::exists(int pgm_id)
{
  QString sql=QString("select ID from PROGRAMS where ")+
    QString().sprintf("ID=%d",pgm_id);
  return SqlQuery::rows(sql)>0;
}


QString Program::tableName() const
{
  return QString("PROGRAMS");
}


QString Program::whereClause() const
{
  return QString().sprintf("ID=%d",program_id);
}
