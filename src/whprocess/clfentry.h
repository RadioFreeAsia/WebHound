// clfentry.h
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

#ifndef CLFENTRY_H
#define CLFENTRY_H

#include <QDateTime>
#include <QList>
#include <QString>

class UserAgent
{
 public:
  UserAgent();
  QString name() const;
  void setName(const QString &str);
  QString version() const;
  void setVersion(const QString &str);
  QStringList options() const;
  void addOption(const QString &str);

 private:
  QString ua_name;
  QString ua_version;
  QStringList ua_options;
};




class ClfEntry
{
 public:
  ClfEntry();
  QString hostName() const;
  QString identUsername() const;
  QString httpUsername() const;
  QDateTime timestamp() const;
  QString requestMethod() const;
  QString requestResource() const;
  QString requestProtocolType() const;
  QString requestProtocolVersion() const;
  unsigned statusCode() const;
  unsigned size() const;
  QString referrer() const;
  QString userAgentString() const;
  int userAgentQuantity() const;
  UserAgent *userAgent(int n) const;
  QString dump() const;
  bool parse(const QString &line);

 private:
  bool ParseUserAgents(const QString &str);
  QString StringField(const QString &str) const;
  unsigned NumberField(const QString &str) const;
  QDate DateField(const QString &str) const;
  int MonthField(const QString &str) const;
  QString DelimitedField(QStringList *fields,int *ptr,const QString &delim1,
			 const QString &delim2,bool *ok);
  QString clf_host_name;
  QString clf_ident_username;
  QString clf_http_username;
  QDateTime clf_timestamp;
  QString clf_request_method;
  QString clf_request_resource;
  QString clf_request_protocol_type;
  QString clf_request_protocol_version;
  unsigned clf_status_code;
  unsigned clf_size;
  QString clf_referrer;
  QString clf_user_agent_string;
  QList<UserAgent *> clf_user_agents;
};


#endif  // CLFENTRY_H
