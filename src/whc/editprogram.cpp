// editprogram.cpp
//
// Edit a WebHound program definition
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

#include <QMessageBox>

#include "db.h"
#include "editprogram.h"
#include "program.h"

EditProgram::EditProgram(QWidget *parent)
  : QDialog(parent)
{
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);
  setMinimumSize(sizeHint());

  //
  // Name
  //
  edit_name_label=new QLabel(tr("Name")+":",this);
  edit_name_label->setFont(bold_font);
  edit_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_name_edit=new QLineEdit(this);
  edit_name_edit->setMaxLength(64);

  //
  // Description
  //
  edit_description_label=new QLabel(tr("Description")+":",this);
  edit_description_label->setFont(bold_font);
  edit_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setMaxLength(255);

  //
  // User Agent String
  //
  edit_user_agent_string_label=new QLabel(tr("User Agent String")+":",this);
  edit_user_agent_string_label->setFont(bold_font);
  edit_user_agent_string_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_user_agent_string_edit=new QLineEdit(this);
  edit_user_agent_string_edit->setMaxLength(255);

  //
  // Ok Button
  //
  edit_ok_button=new QPushButton(tr("OK"),this);
  edit_ok_button->setFont(bold_font);
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  edit_cancel_button=new QPushButton(tr("Cancel"),this);
  edit_cancel_button->setFont(bold_font);
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


EditProgram::~EditProgram()
{
}


QSize EditProgram::sizeHint() const
{
  return QSize(600,147);
}
  

int EditProgram::exec(int *pgm_id)
{
  edit_program_id=pgm_id;
  if(*pgm_id>0) {
    setWindowTitle(tr("WebHound - Edit Program")+
		   QString().sprintf(" %d",*pgm_id));
    Program *pgm=new Program(*pgm_id);
    edit_name_edit->setText(pgm->name());
    edit_user_agent_string_edit->setText(pgm->userAgentString());
    edit_description_edit->setText(pgm->description());
    delete pgm;
  }
  else {
    setWindowTitle(tr("WebHound - Edit Program [new]"));
    edit_name_edit->setText("[new program]");
    edit_user_agent_string_edit->setText("");
    edit_description_edit->setText("");
  }
  return QDialog::exec();
}


void EditProgram::okData()
{
  if(edit_user_agent_string_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("WebHound - Invalid Program Definition"),
			 tr("The User Agent String cannot be empty!"));
    return;
  }
  if(*edit_program_id<0) {
    *edit_program_id=Program::create();
  }
  Program *pgm=new Program(*edit_program_id);
  pgm->setName(edit_name_edit->text());
  pgm->setUserAgentString(edit_user_agent_string_edit->text());
  pgm->setDescription(edit_description_edit->text());
  delete pgm;

  done(true);
}


void EditProgram::cancelData()
{
  done(false);
}


void EditProgram::resizeEvent(QResizeEvent *e)
{
  edit_name_label->setGeometry(10,10,150,20);
  edit_name_edit->setGeometry(165,10,200,20);

  edit_description_label->setGeometry(10,32,150,20);
  edit_description_edit->setGeometry(165,32,size().width()-190,20);

  edit_user_agent_string_label->setGeometry(10,54,150,20);
  edit_user_agent_string_edit->setGeometry(165,54,size().width()-190,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
