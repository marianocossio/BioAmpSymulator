#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QByteArray>
#include <QList>

class ServerThread : public QThread
{
    Q_OBJECT

public:
    ServerThread(QObject *parent = 0);
    ~ServerThread();

    bool startPort(QString portName);
    void stopPort();
    bool portIsActive();

    QList<QString> availablePorts();

signals:
    void dataReceived(unsigned char data);
    void portOpened();
    void portClosed();

public slots:
    void setBaudRate(long baudRate);
    void write(QByteArray data);

protected:
    void run() override;

private:
    QMutex mutex;
    QWaitCondition condition;

    QSerialPort portBioAmp;

    bool abort;
    long baudRate;
};

#endif // SERVERTHREAD_H
