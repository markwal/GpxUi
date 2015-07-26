#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setAdvanced(bool isAdvanced);

private slots:
    void on_btnAdvancedToggle_clicked();
    void on_tbtnInputGcode_clicked();
    void on_btnTranslate_clicked();
    void on_about();

private:
    Ui::MainWindow *ui;
    int heightCollapsed;
};

#endif // MAINWINDOW_H
