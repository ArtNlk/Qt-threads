#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

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

    void onSplitterMoved();

private:
    Ui::MainWindow *ui;

    float scaleFactor;

    QImage originalImage;

    static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode);

    bool loadFile(const QString &fileName);
};
#endif // MAINWINDOW_H
