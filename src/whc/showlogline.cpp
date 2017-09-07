// showlogline.cpp
//
// Show the Log Line entry for a given unknown host record
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

#include "db.h"
#include "showlogline.h"

ShowLogLine::ShowLogLine(QWidget *parent)
  : QDialog(parent)
{
  QFont show_font(font().family(),font().pointSize(),QFont::Bold);

  show_logline_text=new QTextEdit(this);
  show_logline_text->setReadOnly(true);
  show_logline_text->setAcceptRichText(false);

  show_close_button=new QPushButton(tr("Close"),this);
  show_close_button->setFont(show_font);
  connect(show_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  setMinimumSize(sizeHint());
}


QSize ShowLogLine::sizeHint() const
{
  return QSize(800,200);
}


int ShowLogLine::exec(int uhost_id)
{
  QString sql=QString("select LOG_LINE from UNKNOWN_HOSTS where ")+
    QString().sprintf("ID=%d",uhost_id);
  SqlQuery *q=new SqlQuery(sql);
  if(q->first()) {
    show_logline_text->setText(q->value(0).toString());
  }
  else {
    show_logline_text->clear();
  }

  return QDialog::exec();
}


void ShowLogLine::closeData()
{
  done(0);
}


void ShowLogLine::closeEvent(QCloseEvent *e)
{
  closeData();
}


void ShowLogLine::resizeEvent(QResizeEvent *e)
{
  show_logline_text->setGeometry(10,10,size().width()-20,size().height()-80);

  show_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
