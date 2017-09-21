// listunknownhosts.cpp
//
// List hosts associated with unrecognized user agents
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

#include "db.h"
#include "listunknownhosts.h"

ListUnknownHosts::ListUnknownHosts(QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle(tr("WebHound - Unknown User Agent Hosts"));
  QFont label_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Dialogs
  //
  list_showlogline_dialog=new ShowLogLine(this);

  //
  // Hosts List
  //
  list_model=new SqlTableModel(this);
  list_model->setQuery(QString("select ")+
		       "ID,"+
		       "HOSTNAME,"+
		       "IP_ADDRESS,"+
		       "LAST_SEEN "+
		       "from UNKNOWN_HOSTS order by HOSTNAME");
  list_model->setHeaderData(0,Qt::Horizontal,tr("ID"));
  list_model->setFieldType(0,SqlTableModel::NumericType);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Host Name"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Address"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("Last Seen"));
  list_view=new TableView(this);
  list_view->setModel(list_model);
  list_view->resizeColumnsToContents();

  //
  // Close Button
  //
  list_showlogline_button=new QPushButton(tr("Show Log")+"\n"+tr("Entry"),this);
  list_showlogline_button->setFont(label_font);
  connect(list_showlogline_button,SIGNAL(clicked()),this,SLOT(showloglineData()));

  //
  // Close Button
  //
  list_close_button=new QPushButton(tr("Close"),this);
  list_close_button->setFont(label_font);
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  setMinimumSize(sizeHint());
}


QSize ListUnknownHosts::sizeHint() const
{
  return QSize(300,400);
}


int ListUnknownHosts::exec(int ua_id)
{
  list_model->setQuery(QString("select ")+
		       "ID,"+
		       "HOSTNAME,"+
		       "IP_ADDRESS,"+
		       "LAST_SEEN "+
		       "from UNKNOWN_HOSTS where "+
		       QString().sprintf("USER_AGENT_ID=%d ",ua_id)+
		       "order by HOSTNAME");
  list_view->resizeColumnsToContents();
  return QDialog::exec();
}


void ListUnknownHosts::showloglineData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    list_showlogline_dialog->exec(s->selectedRows()[0].data().toInt());
  }
}


void ListUnknownHosts::closeData()
{
  done(0);
}


void ListUnknownHosts::closeEvent(QCloseEvent *e)
{
  closeData();
}


void ListUnknownHosts::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,10,size().width()-20,size().height()-90);

  list_showlogline_button->setGeometry(10,size().height()-70,80,60);
  list_close_button->setGeometry(size().width()-90,size().height()-70,80,60);
}
