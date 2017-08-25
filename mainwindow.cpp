#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->BioAmp8->setChecked(true);

    ui->availablePorts->addItems(symulator.availablePorts());

    ui->sinusoidTestButton->setEnabled(false);

    connect(ui->availablePorts, SIGNAL(currentIndexChanged(int)), this, SLOT(portModified(int)));
    connect(ui->sinusoidTestButton, SIGNAL(clicked(bool)), &symulator, SLOT(toggleSinusoidTestSignal()));
    connect(&symulator, SIGNAL(infoUpdate(QString)), ui->infoLabel, SLOT(setText(QString)));
}

MainWindow::~MainWindow()
{
    symulator.stopPort();

    delete ui;
}

void MainWindow::portModified(int portIndex)
{
    ui->sinusoidTestButton->setEnabled(false);

    if (symulator.portIsActive())
        symulator.stopPort();

    if (portIndex > 0)
    {
        if (symulator.startPort(ui->availablePorts->itemText(portIndex)))
        {
            ui->portStatusLabel->setText("OK!");

            ui->sinusoidTestButton->setEnabled(true);
        }

        else
            ui->portStatusLabel->setText("Error");
    }

    else
        ui->portStatusLabel->setText("");
}
