#ifndef SYMULATOR_H
#define SYMULATOR_H

#include <QObject>
#include <QStringList>

#include <QTimer>

#include <math.h>

#include "serverthread.h"

class Symulator : public QObject
{
    Q_OBJECT
public:
    explicit Symulator(QObject *parent = 0);

    bool startPort(QString portName);
    void stopPort();
    bool portIsActive();

    QStringList availablePorts();

signals:
    void infoUpdate(QString info);

public slots:
    void write(QByteArray data);
    void toggleSinusoidTestSignal();

private:
    ServerThread symPort;

    QTimer samplingTimer, testSignalPeriodTimer;

    bool sinusoidTestSignal;
    float sinusoidAngle;

    int samplingInterval;
    unsigned char sampleNumber;

private slots:
    void sendSignalData();
};

#endif // SYMULATOR_H
