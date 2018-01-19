#ifndef DIALOGREPO_H
#define DIALOGREPO_H

#include <QDialog>

namespace Ui {
class dialogrepo;
}

class dialogrepo : public QDialog
{
    Q_OBJECT

public:
    explicit dialogrepo(QWidget *parent = 0);
    ~dialogrepo();

    QString getText();

private:
    Ui::dialogrepo *ui;
};

#endif // DIALOGREPO_H
