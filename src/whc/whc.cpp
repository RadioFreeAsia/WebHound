// whc.cpp
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

#include <stdio.h>
#include <stdlib.h>

#include <QApplication>
#include <QMessageBox>

#include "cmdswitch.h"
#include "db.h"
#include "program.h"
#include "whc.h"

MainWidget::MainWidget(QWidget *parent)
  : QMainWindow(parent)
{
  QString err_msg;

  CmdSwitch *cmd=
    new CmdSwitch(qApp->argc(),qApp->argv(),"whc",VERSION,WHC_USAGE);
  for(unsigned i=0;i<(cmd->keys());i++) {
    if(!cmd->processed(i)) {
      fprintf(stderr,"whc: unknown option\n");
      exit(256);
    }
  }

  setWindowTitle(tr("WebHound Management Client")+" v"+VERSION);

  QFont label_font(font().family(),font().pointSize(),QFont::Bold);

  //
  // Open (and perhaps create) Database
  //
  whc_config=new Config();
  if(!whc_config->openDb(&err_msg,false)) {
    QMessageBox::warning(this,tr("WebHound - Error"),
			 tr("Unable to open database")+" ["+err_msg+"].");
    exit(256);
  }
  if(!CheckDb(&err_msg)) {
    QMessageBox::warning(this,tr("WebHound - Error"),
			 tr("Unable to open database")+" ["+err_msg+"].");
    exit(256);
  }
  DbHeartbeat(this);

  //
  // Dialogs
  //
  whc_editprogram_dialog=new EditProgram(this);
  whc_listhosts_dialog=new ListHosts(this);

  //
  // Programs List
  //
  whc_model=new SqlTableModel(this);
  whc_model->setQuery(QString("select ")+
		      "ID,"+
		      "NAME,"+
		      "DESCRIPTION,"+
		      "USER_AGENT_STRING "+
		      "from PROGRAMS order by NAME");
  whc_model->setHeaderData(0,Qt::Horizontal,tr("ID"));
  whc_model->setFieldType(0,SqlTableModel::NumericType);
  whc_model->setHeaderData(1,Qt::Horizontal,tr("Program"));
  whc_model->setHeaderData(2,Qt::Horizontal,tr("Description"));
  whc_model->setHeaderData(3,Qt::Horizontal,tr("User Agent String"));
  whc_view=new TableView(this);
  whc_view->setModel(whc_model);
  whc_view->resizeColumnsToContents();
  connect(whc_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  // Add Button
  //
  whc_add_button=new QPushButton(tr("Add"),this);
  whc_add_button->setFont(label_font);
  connect(whc_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Edit Button
  //
  whc_edit_button=new QPushButton(tr("Edit"),this);
  whc_edit_button->setFont(label_font);
  connect(whc_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  // Delete Button
  //
  whc_delete_button=new QPushButton(tr("Delete"),this);
  whc_delete_button->setFont(label_font);
  connect(whc_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // List Hosts Button
  //
  whc_listhosts_button=new QPushButton(tr("List")+"\n"+tr("Hosts"),this);
  whc_listhosts_button->setFont(label_font);
  connect(whc_listhosts_button,SIGNAL(clicked()),this,SLOT(listhostsData()));

  //
  // Close Button
  //
  whc_close_button=new QPushButton(tr("Close"),this);
  whc_close_button->setFont(label_font);
  connect(whc_close_button,SIGNAL(clicked()),qApp,SLOT(quit()));
}


QSize MainWidget::sizeHint() const
{
  return QSize(640,480);
}


void MainWidget::addData()
{
  int pgm_id=-1;

  if(whc_editprogram_dialog->exec(&pgm_id)) {
    whc_model->update();
    whc_view->select(0,pgm_id);
    whc_view->resizeColumnsToContents();
  }
  else {
    Program::remove(pgm_id);
  }
}


void MainWidget::editData()
{
  QItemSelectionModel *s=whc_view->selectionModel();
  if(s->hasSelection()) {
    int pgm_id=s->selectedRows()[0].data().toInt();
    if(whc_editprogram_dialog->exec(&pgm_id)) {
      whc_model->update();
    }
  }
}


void MainWidget::deleteData()
{
  QItemSelectionModel *s=whc_view->selectionModel();
  if(s->hasSelection()) {
    int pgm_id=s->selectedRows()[0].data().toInt();
    Program *pgm=new Program(pgm_id);
    if(QMessageBox::question(this,tr("WebHound - Delete Program Definition"),
			     tr("Are you sure you want to delete program definition")+
			     " \""+pgm->name()+"\"?",
			     QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete pgm;
      return;
    }
    Program::remove(pgm_id);
    whc_model->update();
    delete pgm;
  }
}


void MainWidget::listhostsData()
{
  QItemSelectionModel *s=whc_view->selectionModel();
  if(s->hasSelection()) {
    whc_listhosts_dialog->exec(s->selectedRows()[0].data().toInt());
  }
}


void MainWidget::doubleClickedData(const QModelIndex &index)
{
  listhostsData();
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  whc_view->setGeometry(10,10,size().width()-20,size().height()-90);

  whc_add_button->setGeometry(10,size().height()-70,80,60);
  whc_edit_button->setGeometry(100,size().height()-70,80,60);
  whc_delete_button->setGeometry(190,size().height()-70,80,60);

  whc_listhosts_button->setGeometry(370,size().height()-70,80,60);

  whc_close_button->setGeometry(size().width()-90,size().height()-70,80,60);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);

  MainWidget *w=new MainWidget();
  w->show();

  return a.exec();
}
