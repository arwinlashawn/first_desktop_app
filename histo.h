#ifndef HISTO_H
#define HISTO_H

#include <QDialog>

namespace Ui {
class Histo;
}

class Histo : public QDialog
{
    Q_OBJECT

public:
    explicit Histo(QWidget *parent = nullptr);
    ~Histo();


private slots:
    void on_histo_process_clicked();

private:
    Ui::Histo *ui;
};

#endif // HISTO_H
