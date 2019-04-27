#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QLayout>
#include <QTcpSocket>
#include <QMessageBox>
#include <QLabel>
#include <QDataStream>
#include <QLineEdit>
#include <QKeyEvent>
#include <QDebug>
#include <QIntValidator>
#include <QFile>
#include <QFileDialog>
#include <QPushButton>
#include <QImage>
#include <QPicture>
#include <QPainter>


#include "messagetype.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QLabel connectionState;
    QLabel recipient;
    QLineEdit recipientLine;
    QPushButton openFileBtn;

    QTcpSocket* socket;

    QByteArray data;
    QByteArray fileMessage;

    QIntValidator intVal;

    MessageType messType;

    QFile *sendingFile;
    QString filePath;

    QLabel image;

    QVBoxLayout* vBox;
    QHBoxLayout* hBox;
    QHBoxLayout* hBox1;

    bool isID;
private:
    QByteArray sendingInformation();
public slots:
    void sendMessage();
    void readyRead();
    void disconnected();
    void openFile();
    void clearTextEdit(const QString);

protected:
    void keyPressEvent(QKeyEvent *pe);
};

#endif // MAINWINDOW_H
