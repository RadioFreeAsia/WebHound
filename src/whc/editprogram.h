// editprogram.h
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

#ifndef EDITPROGRAM_H
#define EDITPROGRAM_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>

class EditProgram : public QDialog
{
  Q_OBJECT
 public:
  EditProgram(QWidget *parent=0);
  ~EditProgram();
  QSize sizeHint() const;
  
 public slots:
  int exec(int *pgm_id);

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_name_label;
  QLineEdit *edit_name_edit;
  QLabel *edit_description_label;
  QLineEdit *edit_description_edit;
  QLabel *edit_user_agent_string_label;
  QLineEdit *edit_user_agent_string_edit;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  int *edit_program_id;
};


#endif  // EDITPROGRAM_H
