// program.h
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

#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdint.h>

#include <QString>

#include "accessor.h"
#include "db.h"

class Program : public Accessor
{
 public:
  Program(int id);
  int id() const;
  bool exists() const;
  QString name() const;
  void setName(const QString &name);
  QString userAgentString() const;
  void setUserAgentString(const QString &str);
  QString description() const;
  void setDescription(const QString &str);
  static int create();
  static void remove(int pgm_id);
  static bool exists(int pgm_id);

 private:
  QString tableName() const;
  QString whereClause() const;
  int program_id;
};


#endif  // PROGRAM_H
