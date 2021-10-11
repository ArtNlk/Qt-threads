#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QThread>

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

    void onWorkerCompleted();

private slots:
    void on_applyButton_clicked();

private:
    Ui::MainWindow *ui;

    float scaleFactor;

    QImage originalImage;
    QImage resultImage;

    QThread workerThread;
    ThreadWorker worker;

    static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode);

    bool loadFile(const QString &fileName);

    void resizeEvent(QResizeEvent*);
};
#endif // MAINWINDOW_H
