#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QWidget>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void sendData(unsigned char addresse, int data);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void readData();

    void on_actionZeige_Konsole_toggled(bool arg1);

    void on_actionZeige_USB_Optionen_toggled(bool arg1);

    void on_actionRefresh_triggered();

    void on_pushConnect_clicked();

    void on_pushButtonSerialSend_clicked();

private:
    Ui::MainWindow *ui;
    void updateLabels();
};

#endif // MAINWINDOW_H
