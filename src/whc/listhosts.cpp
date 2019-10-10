// listhosts.cpp
//
// List host entries for a given program
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

#include <stdio.h>
#include <stdlib.h>

#include <QFile>

#include "db.h"
#include "listhosts.h"
#include "textfile.h"

ListHosts::ListHosts(QWidget *parent)
  : QDialog(parent)
{
  list_program=NULL;

  QFont label_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Filter
  //
  list_filter_spin=new QSpinBox(this);
  list_filter_spin->setRange(1,102);
  list_filter_spin->setSuffix(" "+tr("weeks"));
  list_filter_spin->setValue(4);
  connect(list_filter_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(filterChangedData(int)));
  list_filter_label=new QLabel(tr("Show No Records Older Than")+":",this);
  list_filter_label->setFont(label_font);
  list_filter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Hosts List
  //
  list_model=new SqlTableModel(this);
  list_model->setQuery(QString("select ")+
		       "ID,"+                  // 00
		       "NAME,"+                // 01
		       "IP_ADDRESS,"+          // 02
		       "USER_AGENT_VERSION,"+  // 03
		       "USER_AGENT_OPTIONS,"+  // 04
		       "LAST_SEEN "+           // 05
		       "from HOSTS");
  list_model->setHeaderData(0,Qt::Horizontal,tr("ID"));
  list_model->setFieldType(0,SqlTableModel::NumericType);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Host Name"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Address"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("Version"));
  list_model->setHeaderData(4,Qt::Horizontal,tr("Options"));
  list_model->setHeaderData(5,Qt::Horizontal,tr("Last Seen"));
  list_view=new TableView(this);
  list_view->setModel(list_model);
  list_view->setSortingEnabled(true);
  list_view->resizeColumnsToContents();

  //
  // Report Button
  //
  list_report_button=new QPushButton(tr("Generate")+"\n"+tr("Report"),this);
  list_report_button->setFont(label_font);
  connect(list_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  // Close Button
  //
  list_close_button=new QPushButton(tr("Close"),this);
  list_close_button->setFont(label_font);
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  setMinimumSize(sizeHint());
}


ListHosts::~ListHosts()
{
  for(int i=0;i<list_temp_files.size();i++) {
    QFile::remove(list_temp_files.at(i));
  }
}


QSize ListHosts::sizeHint() const
{
  return QSize(800,600);
}


int ListHosts::exec(int pgm_id)
{
  list_program=new Program(pgm_id);

  setWindowTitle(tr("Hosts for")+" \""+list_program->name()+"\"");
  filterChangedData(list_filter_spin->value());

  return QDialog::exec();
}


void ListHosts::filterChangedData(int days)
{
  QDate date=QDate::currentDate().addDays(-7*list_filter_spin->value());

  list_model->setQuery(QString("select ")+
		       "ID,"+
		       "NAME,"+
		       "IP_ADDRESS,"+
		       "USER_AGENT_VERSION,"+
		       "USER_AGENT_OPTIONS,"+
		       "LAST_SEEN "+
		       "from HOSTS where "+
		       QString().sprintf("PROGRAM_ID=%d && ",
					 list_program->id())+
		       "LAST_SEEN>=\""+
		       date.toString("yyyy-MM-dd")+" 00:00:00\"");
  list_view->resizeColumnsToContents();
}


void ListHosts::closeData()
{
  delete list_program;
  list_program=NULL;

  done(0);
}


void ListHosts::reportData()
{
  QString rpt="";
  QSqlQuery *q=NULL;
  QString tempfile;

  //
  // Title Line
  //
  rpt+=
    CenterLine(tr("Host List Report for")+" \""+list_program->name()+"\"")+"\n";

  //
  // Sub Title Line
  //
  rpt+=CenterLine(tr("Generated on")+" "+
		  QDate::currentDate().toString("MMMM d, yyyy"))+"\n";

  //
  // Column Headers
  //
  rpt+="------------------------------------------------------------------------------\n";
  rpt+="|         DNS Name         |   IP Address   | Version  |      Last Seen      |\n";
  rpt+="|--------------------------|----------------|----------|---------------------|\n";

  //
  // Report Body
  //
  q=new QSqlQuery(list_model->query()+" order by USER_AGENT_VERSION");
  while(q->next()) {
    rpt+="| ";
    rpt+=PadField(q->value(1).toString(),25)+"| ";  // Host Name
    rpt+=PadField(q->value(2).toString(),15)+"| ";  // IP Address
    rpt+=PadField(q->value(3).toString(),9)+"| ";  // Version
    rpt+=
      PadField(q->value(5).toDateTime().toString("MM/dd/yyyy hh:mm:ss"),20);
 
    rpt+="|\n";
  }
  delete q;

  //
  // Footer
  //
  rpt+="------------------------------------------------------------------------------\n";

  tempfile=TextFile(rpt);
  if(!tempfile.isEmpty()) {
    list_temp_files.push_back(tempfile);
  }
}


void ListHosts::closeEvent(QCloseEvent *e)
{
  closeData();
}


void ListHosts::resizeEvent(QResizeEvent *e)
{
  list_filter_label->setGeometry(10,2,250,20);
  list_filter_spin->setGeometry(265,2,100,20);

  list_view->setGeometry(10,32,size().width()-20,size().height()-122);

  list_report_button->setGeometry(10,size().height()-70,80,60);

  list_close_button->setGeometry(size().width()-90,size().height()-70,80,60);
}


QString ListHosts::CenterLine(const QString &line) const
{
  QString ret="";
  int margin=(WEBHOUND_REPORT_WIDTH-line.length())/2;

  for(int i=0;i<margin;i++) {
    ret+=" ";
  }

  ret+=line;
  return ret;
}


QString ListHosts::PadField(const QString &field,int width) const
{
  QString ret=field;

  for(int i=0;i<(width-field.length());i++) {
    ret+=" ";
  }

  return ret;
}
