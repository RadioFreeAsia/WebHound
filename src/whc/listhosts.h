// listhosts.h
//
// List host entries for a given program
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

#ifndef LISTHOSTS_H
#define LISTHOSTS_H

#include <QDialog>
#include <QPushButton>

#include "config.h"
#include "sqltablemodel.h"
#include "tableview.h"

class ListHosts : public QDialog
{
 Q_OBJECT;
 public:
  ListHosts(QWidget *parent=0);
  QSize sizeHint() const;

 public slots:
  int exec(int pgm_id);

 private slots:
  void closeData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  TableView *list_view;
  SqlTableModel *list_model;
  QPushButton *list_close_button;
  Config *list_config;
};


#endif  // LISTHOSTS_H
