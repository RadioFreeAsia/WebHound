// listunknownuas.cpp
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

#include <stdio.h>
#include <stdlib.h>

#include <QMessageBox>

#include "db.h"
#include "listunknownuas.h"

ListUnknownUas::ListUnknownUas(QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle(tr("WebHound - Unknown User Agents"));
  QFont label_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Dialogs
  //
  list_listunknownhosts_dialog=new ListUnknownHosts(this);

  //
  // Hosts List
  //
  list_model=new SqlTableModel(this);
  list_model->setQuery(QString("select ")+
		      "ID,"+
		      "USER_AGENT "+
		      "from UNKNOWN_USER_AGENTS order by ID");
  list_model->setHeaderData(0,Qt::Horizontal,tr("ID"));
  list_model->setFieldType(0,SqlTableModel::NumericType);
  list_model->setHeaderData(1,Qt::Horizontal,tr("User Agent"));
  list_model->setNullText(1,tr("[empty]"));
  list_view=new TableView(this);
  list_view->setModel(list_model);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(clicked(const QModelIndex &)),
	  this,SLOT(clickedData(const QModelIndex &)));

  //
  // List Hosts Button
  //
  list_listunknownhosts_button=
    new QPushButton(tr("List")+"\n"+tr("Hosts"),this);
  list_listunknownhosts_button->setFont(label_font);
  connect(list_listunknownhosts_button,SIGNAL(clicked()),
	  this,SLOT(listunknownhostsData()));

  //
  // Delete Button
  //
  list_delete_button=new QPushButton(tr("Delete"),this);
  list_delete_button->setFont(label_font);
  list_delete_button->setDisabled(true);
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Close Button
  //
  list_close_button=new QPushButton(tr("Close"),this);
  list_close_button->setFont(label_font);
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  setMinimumSize(sizeHint());
}


QSize ListUnknownUas::sizeHint() const
{
  return QSize(800,600);
}


int ListUnknownUas::exec()
{
  list_model->setQuery(QString("select ")+
		       "ID,"+
		       "USER_AGENT "+
		       "from UNKNOWN_USER_AGENTS order by ID");
  list_view->resizeColumnsToContents();
  list_listunknownhosts_button->
    setEnabled(list_view->selectionModel()->hasSelection());
  return QDialog::exec();
}


void ListUnknownUas::deleteData()
{
  QString sql;

  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    if(QMessageBox::question(this,"WebHound - "+tr("Delete User Agent"),
	    tr("Are you sure you want to delete this user agent signature?"),
			 QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      return;
    }
    QSqlDatabase::database().transaction();
    sql=QString("delete from UNKNOWN_HOSTS where ")+
      QString().sprintf("USER_AGENT_ID=%d",s->selectedRows()[0].data().toInt());
    SqlQuery::run(sql);

    sql=QString("delete from UNKNOWN_USER_AGENTS where ")+
      QString().sprintf("ID=%d",s->selectedRows()[0].data().toInt());
    SqlQuery::run(sql);
    QSqlDatabase::database().commit();
    list_model->update();
  }
}


void ListUnknownUas::clickedData(const QModelIndex &index)
{
  list_listunknownhosts_button->setEnabled(true);
  list_delete_button->setEnabled(true);
}


void ListUnknownUas::closeData()
{
  done(0);
}


void ListUnknownUas::listunknownhostsData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    list_listunknownhosts_dialog->exec(s->selectedRows()[0].data().toInt());
  }
}


void ListUnknownUas::closeEvent(QCloseEvent *e)
{
  closeData();
}


void ListUnknownUas::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,10,size().width()-20,size().height()-90);

  list_listunknownhosts_button->setGeometry(10,size().height()-70,80,60);
  list_delete_button->setGeometry(100,size().height()-70,80,60);

  list_close_button->setGeometry(size().width()-90,size().height()-70,80,60);
}
