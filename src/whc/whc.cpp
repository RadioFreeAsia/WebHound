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
  // Manage Programs Button
  //
  whc_programs_button=new QPushButton(tr("Manage")+"\n"+tr("Programs"),this);
  whc_programs_button->setFont(label_font);
  //  connect(whc_programs_button,SIGNAL(clicked()),
  //	  gnmc_listsites_dialog,SLOT(exec()));

  //
  // Manage Hosts Button
  //
  whc_hosts_button=new QPushButton(tr("Manage")+"\n"+tr("Hosts"),this);
  whc_hosts_button->setFont(label_font);
  //  connect(whc_hosts_button,SIGNAL(clicked()),
  //	  gnmc_listchassis_dialog,SLOT(exec()));

  //
  // Close Button
  //
  whc_close_button=new QPushButton(tr("Close"),this);
  whc_close_button->setFont(label_font);
  connect(whc_close_button,SIGNAL(clicked()),qApp,SLOT(quit()));
}


QSize MainWidget::sizeHint() const
{
  return QSize(370,261);
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  whc_programs_button->setGeometry(100,54,80,60);
  whc_hosts_button->setGeometry(190,54,80,60);

  whc_close_button->setGeometry(45,196,size().width()-90,60);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);

  MainWidget *w=new MainWidget();
  w->show();

  return a.exec();
}
