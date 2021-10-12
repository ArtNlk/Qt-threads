#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QThread>
#include <QScrollBar>

#include "threadworker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadFile();

public slots:
    void onOpenAction();
    void onSaveAction();
    void onExitAction();

    void onWorkerCompleted(QImage result);

signals:
    void process(QImage input, int kernelSize, int borderType, int direction, bool normalize);

private slots:
    void on_applyButton_clicked();

private:
    Ui::MainWindow *ui;

    bool loaded;
    bool processed;

    float scaleFactor;

    QImage originalImage;
    QImage resultImage;

    QThread workerThread;
    ThreadWorker worker;

    static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode);

    bool loadFile(const QString &fileName);

    void updateImageSize(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent *e);
};
#endif // MAINWINDOW_H
