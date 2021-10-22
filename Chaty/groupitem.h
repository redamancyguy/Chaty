#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <QWidget>

namespace Ui {
class groupitem;
}

class groupitem : public QWidget {
  Q_OBJECT

 public:
  explicit groupitem(QWidget* parent = nullptr);
  void setName(QString name);
  ~groupitem();

 private:
  Ui::groupitem* ui;
};

#endif  // GROUPITEM_H
