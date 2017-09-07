// listunknownuas.h
//
// List unrecognized user agents
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

#ifndef LISTUNKNOWNUAS_H
#define LISTUNKNOWNUAS_H

#include <QDialog>
#include <QPushButton>

#include "config.h"
#include "listunknownhosts.h"
#include "sqltablemodel.h"
#include "tableview.h"

class ListUnknownUas : public QDialog
{
 Q_OBJECT;
 public:
  ListUnknownUas(QWidget *parent=0);
  QSize sizeHint() const;

 public slots:
  int exec();

 private slots:
  void listunknownhostsData();
  void deleteData();
  void closeData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  TableView *list_view;
  SqlTableModel *list_model;
  QPushButton *list_listunknownhosts_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
  ListUnknownHosts *list_listunknownhosts_dialog;
  Config *list_config;
};


#endif  // LISTUNKNOWNUAS_H
