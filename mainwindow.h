#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QWidget>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort>

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

    void updateUI();

    void on_actionZeige_Daten_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    void updateLabels();
};

#endif // MAINWINDOW_H
