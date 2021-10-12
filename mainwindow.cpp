#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardPaths>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QWheelEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    loaded(false),
    processed(false)
{
    ui->setupUi(this);

    qDebug() << "Main thread id: " << QThread::currentThreadId();

    this->setWindowTitle(tr("Sobel filter"));

    for(int i = 1; i < 16; i++)
    {
        ui->kernelSize->addItem(QString::number(i*2 - 1),QVariant(i*2 - 1));
    }

    ui->imgLabel->setScaledContents(true);

    ui->imgArea->takeWidget();
    ui->imgArea->setWidget(ui->imgLabel);

    setCentralWidget(ui->splitter);

    connect(ui->action_Open,&QAction::triggered,this,&MainWindow::onOpenAction);
    connect(ui->action_Save,&QAction::triggered,this,&MainWindow::onSaveAction);
    connect(ui->action_Exit,&QAction::triggered,this,&MainWindow::onExitAction);

    worker.moveToThread(&workerThread);
    connect(&workerThread,&QThread::finished,&worker,&QObject::deleteLater);
    connect(&worker,&ThreadWorker::completed,this,&MainWindow::onWorkerCompleted);
    connect(this,&MainWindow::process,&worker,&ThreadWorker::process);
}

MainWindow::~MainWindow()
{
    workerThread.quit();
    workerThread.wait();
    delete ui;
}

void MainWindow::onOpenAction()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    if(dialog.exec() == QDialog::Accepted && loadFile(dialog.selectedFiles().constFirst()))
    {
        ui->applyButton->setEnabled(true);
        this->setWindowTitle(QString("%1 - [%2]").arg(this->windowTitle()).arg(dialog.selectedFiles().constFirst()));
    }
}

void MainWindow::onSaveAction()
{
    QFileDialog dialog(this,tr("Save file"));
    initializeImageFileDialog(dialog,QFileDialog::AcceptSave);

    if(dialog.exec() == QDialog::Accepted)
    {
        QImageWriter writer(dialog.selectedFiles().constFirst());

        writer.write(resultImage);
    }
}

void MainWindow::onExitAction()
{
    this->close();
}

void MainWindow::onWorkerCompleted(QImage result)
{
    qDebug() << "Worker done";
    processed = true;
    resultImage = result;
    ui->imgLabel->setPixmap(QPixmap::fromImage(resultImage).scaled(resultImage.size()*scaleFactor));
    //ui->imgLabel->adjustSize();

    ui->settingsBox->setEnabled(true);
    ui->action_Save->setEnabled(true);

    qDebug() << "Input size: " << originalImage.size() << " output size: " << resultImage.size();
}

void MainWindow::initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    for (const QByteArray &mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/png");
    dialog.setAcceptMode(acceptMode);
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

bool MainWindow::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    originalImage = reader.read();
    if (originalImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    scaleFactor = 1.0;

    ui->imgArea->setVisible(true);
    ui->imgLabel->setPixmap(QPixmap::fromImage(originalImage));
    //printAct->setEnabled(true);
    //fitToWindowAct->setEnabled(true);
    //updateActions();

    ui->imgLabel->adjustSize();

    loaded = true;

    return true;
}

void MainWindow::updateImageSize(double factor)
{
    scaleFactor *= factor;
    QPixmap newMap;
    if(processed){
        newMap = QPixmap::fromImage(resultImage).scaled(resultImage.size() * scaleFactor, Qt::KeepAspectRatio, Qt::FastTransformation);
    }else{
        newMap = QPixmap::fromImage(originalImage).scaled(originalImage.size() * scaleFactor, Qt::KeepAspectRatio, Qt::FastTransformation);
    }
    ui->imgLabel->setPixmap(newMap);

    adjustScrollBar(ui->imgArea->horizontalScrollBar(), factor);
    adjustScrollBar(ui->imgArea->verticalScrollBar(), factor);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    ui->settingsFrame->setMaximumWidth(this->width()/3);
}

void MainWindow::on_applyButton_clicked()
{
    workerThread.start();
    int gradDir = ui->applyVertical->isChecked() ? ThreadWorker::Vertical : ThreadWorker::None;
    if(ui->applyHorizontal->isChecked())
    {
        gradDir  = gradDir | ThreadWorker::Horizontal;
    }
   emit process(originalImage,ui->kernelSize->currentData().toInt(),cv::BORDER_DEFAULT, gradDir, ui->normalize->isChecked());
    ui->settingsBox->setDisabled(true);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(!loaded) return;
    if(e->key() == Qt::Key_Plus)
    {
        qDebug() << "zoom in";
        updateImageSize(1.1);
    }
    else if(e->key() == Qt::Key_Minus)
    {
        qDebug() << "zoom out";
        updateImageSize(0.9);
    }
}

