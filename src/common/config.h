// config.h
//
// Class for gnmc(1) and gnmd(8) configuration.
//
// (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

#define WEBHOUND_CONF_FILE QString("/etc/webhound.conf")
#define WEBHOUND_DEFAULT_MYSQL_HOSTNAME "localhost"
#define WEBHOUND_DEFAULT_MYSQL_DBNAME "WebHound"
#define WEBHOUND_DEFAULT_MYSQL_USERNAME "webhounduser"
#define WEBHOUND_DEFAULT_MYSQL_PASSWORD "letmein"
#define WEBHOUND_DEFAULT_MYSQL_ENGINE "InnoDB"
#define WEBHOUND_DEFAULT_MYSQL_CHARSET "utf8"
#define WEBHOUND_DEFAULT_MYSQL_COLLATION "utf8_general_ci"

class Config
{
 public:
  Config();
  QString mysqlHostname() const;
  QString mysqlDbname() const;
  QString mysqlUsername() const;
  QString mysqlPassword() const;
  QString mysqlEngine() const;
  QString mysqlCharset() const;
  QString mysqlCollation() const;
  QString createTablePostfix() const;
  bool openDb(QString *err_msg,bool schema_check);

 private:
  QString config_mysql_hostname;
  QString config_mysql_dbname;
  QString config_mysql_username;
  QString config_mysql_password;
  QString config_mysql_engine;
  QString config_mysql_charset;
  QString config_mysql_collation;
};


#endif  // CONFIG_H
