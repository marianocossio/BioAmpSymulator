#include "serverthread.h"

ServerThread::ServerThread(QObject *parent) : QThread(parent)
{
    abort = false;

    baudRate = 115200;
}

ServerThread::~ServerThread()
{
    mutex.lock();

    abort = true;
    condition.wakeOne();

    mutex.unlock();

    wait();
}

bool ServerThread::startPort(QString portName)
{
    bool result;

    QMutexLocker locker(&mutex);

    portBioAmp.setPortName(portName);
    portBioAmp.setBaudRate(baudRate);
    portBioAmp.open(QSerialPort::ReadWrite);

    result = portBioAmp.isOpen();

    if (!isRunning())
        start(LowPriority);
    else
        condition.wakeOne();

    if (result)
    {
        abort = false;

        emit portOpened();
    }

    return result;
}

void ServerThread::stopPort()
{
    mutex.lock();

    if (portBioAmp.isOpen())
        portBioAmp.close();

    abort = true;
    condition.wakeOne();

    mutex.unlock();

    wait();

    emit portClosed();
}

bool ServerThread::portIsActive()
{
    return portBioAmp.isOpen();
}

QList<QString> ServerThread::availablePorts()
{
    QList<QString> availablePortNames;

    for (int index = 0; index < QSerialPortInfo::availablePorts().size(); index++)
        availablePortNames.push_back(QSerialPortInfo::availablePorts()[index].portName());

    return availablePortNames;
}

void ServerThread::setBaudRate(long baudRate)
{
    this->baudRate = baudRate;

    if (portBioAmp.isOpen())
    {
        portBioAmp.close();

        startPort(portBioAmp.portName());
    }
}

void ServerThread::write(QByteArray data)
{
    mutex.lock();

    portBioAmp.write(data);

    mutex.unlock();
}

void ServerThread::run()
{
    forever
    {
        if (abort)
            return;

        bool bytesAvailable;

        mutex.lock();

        bytesAvailable = (portBioAmp.bytesAvailable() >= 1);

        mutex.unlock();

        if (bytesAvailable)
        {
            char receivedBytes;
            int bytesRead;

            mutex.lock();

            bytesRead = portBioAmp.read(&receivedBytes, 1);

            mutex.unlock();

            if (bytesRead != (-1))
                emit dataReceived((unsigned char) receivedBytes);
        }
    }
}
