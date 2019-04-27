#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    messType(TEXT_TYPE)
{
    ui->setupUi(this);

    isID = true;

    recipientLine.setValidator(&intVal);

    connectionState.setText("Offline");
    recipient.setText("ID получателя: ");
    recipientLine.setPlaceholderText("ID");
    ui->lineEdit->setPlaceholderText("Ваше сообщение");
    openFileBtn.setText("Open");
    openFileBtn.setFixedSize(50,25);


    // Layouts
    vBox = new QVBoxLayout();
    hBox = new QHBoxLayout();
    hBox1 = new QHBoxLayout();

    hBox1->addWidget(&recipient);
    hBox1->addWidget(&recipientLine);


    vBox->addWidget(&connectionState);
    vBox->addLayout(hBox1);
    vBox->addWidget(ui->textEdit);
    vBox->addWidget(&image);

    hBox->addWidget(ui->lineEdit);
    hBox->addWidget(&openFileBtn);
    hBox->addWidget(ui->pushButton);

    vBox->addLayout(hBox);
    ui->textEdit->setReadOnly(true);
    auto central = new QWidget;
        central->setLayout(vBox);

        setCentralWidget(central);


    // Socket
    socket = new QTcpSocket(this);
    socket->connectToHost("localhost",1236);



        // GIGNALS - SLOTS
        connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(sendMessage()));

        connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));
        connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
        connect(&openFileBtn, SIGNAL(clicked()),this, SLOT(openFile()));
        connect(&recipientLine, SIGNAL(textChanged(const QString)), this, SLOT(clearTextEdit(const QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendMessage()
{
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    QByteArray tmp = sendingInformation();
    outputData.append(tmp);



    if(!recipientLine.text().isEmpty()){
        if(socket->waitForConnected(100)){

            if(!ui->lineEdit->text().isEmpty() && messType == TEXT_TYPE){
                ui->textEdit->append(ui->lineEdit->text());
                socket->write(outputData);
                ui->lineEdit->clear();
            }
            if(messType == FILE_TYPE){
                ui->textEdit->append(fileMessage.toBase64());
                socket->write(outputData);
                messType = TEXT_TYPE;
            }
            /*
            QImage image1(fileMessage);
            QPainter paint(this);
            QPicture pic;
            QPainter painter(&pic);
            painter.drawImage(0,0, image1);
            image.setPicture(pic);*/

            QTextCursor cursor = ui->textEdit->textCursor();
            QTextBlockFormat textBlockFormat = cursor.blockFormat();
            textBlockFormat.setAlignment(Qt::AlignRight);
            cursor.mergeBlockFormat(textBlockFormat);
            ui->textEdit->setTextCursor(cursor);
        }
    }
    else {
        QMessageBox::critical(this,"Ошибка","ID некорректен или не введен!");
    }
}

void MainWindow::readyRead()
{
    if(socket->waitForConnected(100)){
        socket->waitForReadyRead(100);
        if(isID){
            data = socket->readAll();;
            connectionState.setText("Ваш ID: " + data);
            isID = false;
        }
        else {
            /*
            QDataStream in(socket);
            in.setVersion(QDataStream::Qt_5_12);
            in >> data;*/
            QByteArray dataEncoding;
            data = socket->readAll();
            dataEncoding = QByteArray::fromBase64(data, QByteArray::Base64Encoding | QByteArray::OmitTrailingEquals);

            ui->textEdit->append(data);
            QTextCursor cursor = ui->textEdit->textCursor();
            QTextBlockFormat textBlockFormat = cursor.blockFormat();
            textBlockFormat.setAlignment(Qt::AlignLeft);
            cursor.mergeBlockFormat(textBlockFormat);
            ui->textEdit->setTextCursor(cursor);

            QImage image1;
            image1.fromData(dataEncoding, "PNG");
            QPainter paint(this);
            QPicture pic;
            QPainter painter(&pic);
            painter.drawImage(0,0, image1);
            image.setPicture(pic);
        }
    } else {
        QMessageBox::information(this, "Информация", "Вы не подключены к серверу!");
    }
}

void MainWindow::disconnected()
{
    socket->deleteLater();

}

QByteArray MainWindow::sendingInformation()
{
    QByteArray jsonData;
    switch(messType){
    case TEXT_TYPE:{
        jsonData.append("{\"type\":\"text\",");
        jsonData.append("\"recID\":\"" + recipientLine.text() + "\",");
        jsonData.append("\"message\":\"" + ui->lineEdit->text() + "\"}");
        break;
    }
    case FILE_TYPE:{
        QString tmp = sendingFile->readAll();
        fileMessage.append(tmp);
        jsonData.append("{\"type\":\"doc\",");
        jsonData.append("\"recID\":\"" + recipientLine.text() + "\",");
        jsonData.append("\"message\":\"" + fileMessage.toBase64() + "\"}");
        sendingFile->close();
        fileMessage.clear();
        break;
    }
    }
    return  jsonData;
}

void MainWindow::keyPressEvent(QKeyEvent *pe)
{
    if(pe->key() == Qt::Key_Enter || pe->key() == Qt::Key_Return){
        sendMessage();
    }
}
void MainWindow::openFile()
{
    filePath = QFileDialog::getOpenFileName(this, "Выберете файл","/home/aaaaaaaaa/Документы");
    sendingFile = new QFile(filePath);
    if (sendingFile->open(QIODevice::ReadOnly)){
        QMessageBox::information(this, "Удачно!","Вы удачно загрузили файл!");
        messType = FILE_TYPE;
    }
}

void MainWindow::clearTextEdit(const QString)
{
    ui->textEdit->clear();
}
