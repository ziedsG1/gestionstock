#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableView>
#include <QHeaderView>
#include <QPageSize>
#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ajouterB_clicked();
    void on_supprimerB_clicked();
    void on_rechercheB_clicked();
    void on_ModifierB_clicked();
    void on_afffstat_2_clicked();
    void on_trib_clicked();
    void on_pdfB_clicked();
    void on_pdfB_R_clicked();
   // void on_pushButton_sendServoCommand_clicked();
    void checkStockQuantity();
    void startStockQuantityMonitoring();
    void moveServoBasedOnQuantity(int quantity);





private:
    Ui::MainWindow *ui;
    QSerialPort *serial; // The serial port for communication with Arduino

    int currentQuantity = -1;

};
#endif // MAINWINDOW_H

