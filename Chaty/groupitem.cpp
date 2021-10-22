#include "groupitem.h"
#include "ui_groupitem.h"

groupitem::groupitem(QWidget* parent) : QWidget(parent), ui(new Ui::groupitem) {
  ui->setupUi(this);
}

void groupitem::setName(QString name) {
  ui->name->setText(name);
}

groupitem::~groupitem() {
  delete ui;
}
