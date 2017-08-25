#include "symulator.h"

Symulator::Symulator(QObject *parent) : QObject(parent)
{
    sampleNumber = 0;
    samplingInterval = 1;

    sinusoidTestSignal = false;

    symPort.setBaudRate(115200);

    connect(&samplingTimer, SIGNAL(timeout()), this, SLOT(sendSignalData()));
}

bool Symulator::startPort(QString portName)
{
    samplingTimer.stop();

    bool success = symPort.startPort(portName);

    if (success)
        samplingTimer.start(samplingInterval);

    return success;
}

void Symulator::stopPort()
{
    samplingTimer.stop();
    symPort.stopPort();
}

bool Symulator::portIsActive()
{
    return symPort.portIsActive();
}

QStringList Symulator::availablePorts()
{
    return QStringList(symPort.availablePorts());
}

void Symulator::write(QByteArray data)
{
    symPort.write(data);
}

void Symulator::toggleSinusoidTestSignal()
{
    if (testSignalPeriodTimer.isActive())
        testSignalPeriodTimer.stop();

    sinusoidTestSignal = !sinusoidTestSignal;

    if (sinusoidTestSignal)
    {
        sinusoidAngle = 0.0f;

        samplingTimer.stop();
        samplingTimer.start(samplingInterval);

        emit infoUpdate("<b>Signal:</b> Sinusoid 1[Hz]<br><b>Sampling Rate:</b> 1 [KHz]<br><b>Gain:</b> 24<br><b>Amplitude:</b> 187.5 [mV]");
    }

    else
        emit infoUpdate("");
}

void Symulator::sendSignalData()
{
    QByteArray sentData;

    int data = 0;
    unsigned char byte1, byte2, byte3;

    if (sinusoidTestSignal)
    {
        data = (sin((double) sinusoidAngle) * ((double) 4.5) * 0x7FFFFF) / 5;

        sinusoidAngle += 0.0062832f;

        if (sinusoidAngle > 360.0f)
            sinusoidAngle = 0.0f;
    }

    sentData.append((unsigned char) 160);
    sentData.append(sampleNumber);

    byte1 = data >> 16;
    byte2 = (data - (byte1 << 16)) >> 8;
    byte3 = data - (byte1 << 16) - (byte2 << 8);

    for (int i = 0; i < 8; i++)
    {
        sentData.append(byte1);
        sentData.append(byte2);
        sentData.append(byte3);
    }

    sentData.append((char) 0);
    sentData.append((char) 1);

    sentData.append((char) 0);
    sentData.append((char) 1);

    sentData.append((char) 0);
    sentData.append((char) 1);

    sentData.append((char) 0);

    sampleNumber++;

    symPort.write(sentData);
}
