// whc.h
//
// whc(1) Management client for WebHound
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

#ifndef WHC_H
#define WHC_H

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>

#include "config.h"

#define WHC_USAGE "\n"

class MainWidget : public QMainWindow
{
 Q_OBJECT;
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  bool CheckDb(QString *err_msg);
  bool CreateDb();
  bool CheckSchema();
  QPushButton *whc_programs_button;
  QPushButton *whc_hosts_button;
  QPushButton *whc_close_button;
  Config *whc_config;
};


#endif  // WHC_H
