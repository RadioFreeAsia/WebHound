// whprocess.h
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

#ifndef WHPROCESS_H
#define WHPROCESS_H

#include <QObject>

#include "config.h"

#define WHPROCESS_USAGE "[--log-to-syslog] <logfile> \n"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QWidget *parent=0);

 private:
  bool ProcessLine(const QString &line);
  void LogToSyslog(int priority,const char *fmt,...) const;
  bool wh_log_to_syslog;
  Config *wh_config;
};


#endif  // WHPROCESS_H
