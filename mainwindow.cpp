#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStyle>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupActions();
    populateMenusAndToolBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupActions()
{
    QStyle *st = style();

    // Open Image：同一份 QAction 同時加入 menuFile 與 toolBar，執行相同功能
    m_openImageAction = new QAction(st->standardIcon(QStyle::SP_DialogOpenButton), tr("Open Image"), this);
    m_openImageAction->setShortcut(QKeySequence::Open); // Ctrl+O
    m_openImageAction->setStatusTip(tr("開啟圖片檔案"));
    connect(m_openImageAction, &QAction::triggered, this, &MainWindow::onOpenImage);

    // Save：設定 Ctrl+S 快捷鍵
    m_saveAction = new QAction(st->standardIcon(QStyle::SP_DialogSaveButton), tr("Save"), this);
    m_saveAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S)); // Ctrl+S
    m_saveAction->setStatusTip(tr("儲存目前圖片"));
    m_saveAction->setEnabled(false);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSaveImage);

    // Exit
    m_exitAction = new QAction(st->standardIcon(QStyle::SP_DialogCloseButton), tr("Exit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit); // Ctrl+Q
    m_exitAction->setStatusTip(tr("離開程式"));
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::onExitApp);

    // Image 選單動作
    m_zoomInAction = new QAction(st->standardIcon(QStyle::SP_ArrowUp), tr("Zoom In"), this);
    m_zoomInAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Equal)); // Ctrl+=
    m_zoomInAction->setStatusTip(tr("放大圖片"));
    m_zoomInAction->setEnabled(false);
    connect(m_zoomInAction, &QAction::triggered, this, &MainWindow::onZoomIn);

    m_zoomOutAction = new QAction(st->standardIcon(QStyle::SP_ArrowDown), tr("Zoom Out"), this);
    m_zoomOutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus)); // Ctrl+-
    m_zoomOutAction->setStatusTip(tr("縮小圖片"));
    m_zoomOutAction->setEnabled(false);
    connect(m_zoomOutAction, &QAction::triggered, this, &MainWindow::onZoomOut);

    m_resetZoomAction = new QAction(st->standardIcon(QStyle::SP_BrowserReload), tr("Reset Zoom"), this);
    m_resetZoomAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0)); // Ctrl+0
    m_resetZoomAction->setStatusTip(tr("重設為原始大小"));
    m_resetZoomAction->setEnabled(false);
    connect(m_resetZoomAction, &QAction::triggered, this, &MainWindow::onResetZoom);

    // Edit 選單動作
    m_clearAction = new QAction(st->standardIcon(QStyle::SP_TrashIcon), tr("Clear"), this);
    m_clearAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L)); // Ctrl+L
    m_clearAction->setStatusTip(tr("清除目前顯示的圖片"));
    m_clearAction->setEnabled(false);
    connect(m_clearAction, &QAction::triggered, this, &MainWindow::onClearImage);
}

void MainWindow::populateMenusAndToolBar()
{
    // File Menu
    ui->menuFile->addAction(m_openImageAction);
    ui->menuFile->addAction(m_saveAction);
    ui->menuFile->addSeparator();
    ui->menuFile->addAction(m_exitAction);

    // Image Menu
    ui->menuImage->addAction(m_zoomInAction);
    ui->menuImage->addAction(m_zoomOutAction);
    ui->menuImage->addAction(m_resetZoomAction);

    // Edit Menu
    ui->menuEdit->addAction(m_clearAction);

    // ToolBar（放入同一個 m_openImageAction 與常用動作）
    ui->toolBar->addAction(m_openImageAction);
    ui->toolBar->addAction(m_saveAction);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(m_zoomInAction);
    ui->toolBar->addAction(m_zoomOutAction);
    ui->toolBar->addAction(m_resetZoomAction);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(m_clearAction);
}

void MainWindow::onOpenImage()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Image"),
        QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp *.gif);;All Files (*)")
        );

    if (fileName.isEmpty()) {
        return;
    }

    QPixmap loaded(fileName);
    if (loaded.isNull()) {
        QMessageBox::warning(this, tr("開啟失敗"), tr("無法載入所選的圖片檔案。"));
        return;
    }

    m_originalPixmap = loaded;
    m_currentFilePath = fileName;
    m_scaleFactor = 1.0;

    m_saveAction->setEnabled(true);
    m_zoomInAction->setEnabled(true);
    m_zoomOutAction->setEnabled(true);
    m_resetZoomAction->setEnabled(true);
    m_clearAction->setEnabled(true);

    updateImageDisplay();
    ui->statusbar->showMessage(tr("已開啟：%1").arg(fileName), 3000);
}

void MainWindow::onSaveImage()
{
    if (m_originalPixmap.isNull()) {
        QMessageBox::information(this, tr("提示"), tr("目前沒有可儲存的圖片。"));
        return;
    }

    const QString savePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Image"),
        m_currentFilePath.isEmpty() ? QStringLiteral("output.png") : m_currentFilePath,
        tr("PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp)")
        );

    if (savePath.isEmpty()) {
        return;
    }

    if (!m_originalPixmap.save(savePath)) {
        QMessageBox::warning(this, tr("儲存失敗"), tr("無法將圖片儲存至指定路徑。"));
        return;
    }

    m_currentFilePath = savePath;
    ui->statusbar->showMessage(tr("已儲存至：%1").arg(savePath), 3000);
}

void MainWindow::onExitApp()
{
    close();
}

void MainWindow::onZoomIn()
{
    if (m_originalPixmap.isNull()) {
        return;
    }
    m_scaleFactor *= 1.25;
    updateImageDisplay();
}

void MainWindow::onZoomOut()
{
    if (m_originalPixmap.isNull()) {
        return;
    }
    m_scaleFactor *= 0.8;
    if (m_scaleFactor < 0.1) {
        m_scaleFactor = 0.1;
    }
    updateImageDisplay();
}

void MainWindow::onResetZoom()
{
    if (m_originalPixmap.isNull()) {
        return;
    }
    m_scaleFactor = 1.0;
    updateImageDisplay();
}

void MainWindow::onClearImage()
{
    m_originalPixmap = QPixmap();
    m_currentFilePath.clear();
    m_scaleFactor = 1.0;

    ui->imageLabel->setPixmap(QPixmap());
    ui->imageLabel->setText(tr("已清除圖片，請點擊「Open Image」重新開啟"));

    m_saveAction->setEnabled(false);
    m_zoomInAction->setEnabled(false);
    m_zoomOutAction->setEnabled(false);
    m_resetZoomAction->setEnabled(false);
    m_clearAction->setEnabled(false);

    ui->statusbar->showMessage(tr("已清除圖片"), 2000);
}

void MainWindow::updateImageDisplay()
{
    if (m_originalPixmap.isNull()) {
        return;
    }

    const QSize targetSize = m_originalPixmap.size() * m_scaleFactor;
    const QPixmap scaledPixmap = m_originalPixmap.scaled(
        targetSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    ui->imageLabel->setText(QString());
    ui->imageLabel->setPixmap(scaledPixmap);
    ui->imageLabel->resize(scaledPixmap.size());
}