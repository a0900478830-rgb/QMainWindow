#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QPixmap>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenImage();
    void onSaveImage();
    void onExitApp();
    void onZoomIn();
    void onZoomOut();
    void onResetZoom();
    void onClearImage();

private:
    void setupActions();
    void populateMenusAndToolBar();
    void updateImageDisplay();

    Ui::MainWindow *ui;

    QPixmap m_originalPixmap;
    QString m_currentFilePath;
    double m_scaleFactor = 1.0;

    QAction *m_openImageAction = nullptr;
    QAction *m_saveAction = nullptr;
    QAction *m_exitAction = nullptr;
    QAction *m_zoomInAction = nullptr;
    QAction *m_zoomOutAction = nullptr;
    QAction *m_resetZoomAction = nullptr;
    QAction *m_clearAction = nullptr;
};

#endif // MAINWINDOW_H