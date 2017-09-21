// listhosts.cpp
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

#include <stdio.h>
#include <stdlib.h>

#include "db.h"
#include "listhosts.h"
#include "program.h"

ListHosts::ListHosts(QWidget *parent)
  : QDialog(parent)
{
  QFont label_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Hosts List
  //
  list_model=new SqlTableModel(this);
  list_model->setQuery(QString("select ")+
		      "ID,"+
		      "NAME,"+
		      "IP_ADDRESS,"+
		      "USER_AGENT_VERSION,"+
		      "USER_AGENT_OPTIONS,"+
		      "LAST_SEEN "+
		      "from HOSTS order by NAME,LAST_SEEN");
  list_model->setHeaderData(0,Qt::Horizontal,tr("ID"));
  list_model->setFieldType(0,SqlTableModel::NumericType);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Host Name"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Address"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("Version"));
  list_model->setHeaderData(4,Qt::Horizontal,tr("Options"));
  list_model->setHeaderData(5,Qt::Horizontal,tr("Last Seen"));
  list_view=new TableView(this);
  list_view->setModel(list_model);
  list_view->resizeColumnsToContents();

  //
  // Close Button
  //
  list_close_button=new QPushButton(tr("Close"),this);
  list_close_button->setFont(label_font);
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  setMinimumSize(sizeHint());
}


QSize ListHosts::sizeHint() const
{
  return QSize(640,480);
}


int ListHosts::exec(int pgm_id)
{
  Program *pgm=new Program(pgm_id);
  setWindowTitle(tr("Hosts for")+" \""+pgm->name()+"\"");
  list_model->setQuery(QString("select ")+
		       "ID,"+
		       "NAME,"+
		       "IP_ADDRESS,"+
		       "USER_AGENT_VERSION,"+
		       "USER_AGENT_OPTIONS,"+
		       "LAST_SEEN "+
		       "from HOSTS where "+
		       QString().sprintf("PROGRAM_ID=%d ",pgm_id)+
		       "order by NAME,LAST_SEEN");
  list_view->resizeColumnsToContents();
  delete pgm;
  return QDialog::exec();
}


void ListHosts::closeData()
{
  done(0);
}


void ListHosts::closeEvent(QCloseEvent *e)
{
  closeData();
}


void ListHosts::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,10,size().width()-20,size().height()-90);

  list_close_button->setGeometry(size().width()-90,size().height()-70,80,60);
}
