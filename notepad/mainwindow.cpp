#include "mainwindow.h"

#include <QByteArray>
#include <QClipboard>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGuiApplication>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QStringConverter>
#include <QTextDocument>
#include <QTextStream>

#include "aboutdialog.h"
#include "replacedialog.h"
#include "searchdialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initStatusBar();

    connect(ui->actNew, &QAction::triggered, this, &MainWindow::onNewFile);
    connect(ui->actOpen, &QAction::triggered, this, &MainWindow::onOpenFile);
    connect(ui->actSave, &QAction::triggered, this, &MainWindow::onSaveFile);
    connect(ui->actSaveAs, &QAction::triggered, this, &MainWindow::onSaveAsFile);
    connect(ui->actExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actUndo, &QAction::triggered, this, &MainWindow::onUndo);
    connect(ui->actRedo, &QAction::triggered, this, &MainWindow::onRedo);
    connect(ui->actCut, &QAction::triggered, this, &MainWindow::onCut);
    connect(ui->actCopy, &QAction::triggered, this, &MainWindow::onCopy);
    connect(ui->actPaste, &QAction::triggered, this, &MainWindow::onPaste);
    connect(ui->actSelectAll, &QAction::triggered, this, &MainWindow::onSelectAll);
    connect(ui->actAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actFind, &QAction::triggered, this, &MainWindow::onFind);
    connect(ui->actReplace, &QAction::triggered, this, &MainWindow::onReplace);
    connect(ui->textEdit->document(), &QTextDocument::modificationChanged, this, &MainWindow::updateWindowTitle);
    connect(ui->textEdit, &QPlainTextEdit::undoAvailable, ui->actUndo, &QAction::setEnabled);
    connect(ui->textEdit, &QPlainTextEdit::redoAvailable, ui->actRedo, &QAction::setEnabled);
    connect(ui->textEdit, &QPlainTextEdit::copyAvailable, ui->actCut, &QAction::setEnabled);
    connect(ui->textEdit, &QPlainTextEdit::copyAvailable, ui->actCopy, &QAction::setEnabled);
    connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::updatePasteState);

    ui->actUndo->setEnabled(false);
    ui->actRedo->setEnabled(false);
    ui->actCut->setEnabled(false);
    ui->actCopy->setEnabled(false);
    updatePasteState();
    updateWindowTitle();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (maybeSave()) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void MainWindow::onNewFile() {
    if (!maybeSave()) {
        return;
    }
    ui->textEdit->clear();
    m_currentFilePath.clear();
    ui->textEdit->document()->setModified(false);
    updateWindowTitle();
    ui->statusbar->showMessage(QStringLiteral("已新建空白文档"), 3000);
}

void MainWindow::onOpenFile() {
    if (!maybeSave()) {
        return;
    }
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("打开文件"),
        m_currentFilePath.isEmpty() ? QString() : QFileInfo(m_currentFilePath).absolutePath(),
        QStringLiteral("文本文件 (*.txt);;C++ 源码 (*.h *.hpp *.cpp);;所有文件 (*.*)"));
    if (!filePath.isEmpty()) {
        loadFile(filePath);
    }
}

bool MainWindow::onSaveFile() {
    if (m_currentFilePath.isEmpty()) {
        return onSaveAsFile();
    }
    return saveFile(m_currentFilePath);
}

bool MainWindow::onSaveAsFile() {
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("另存为"),
        m_currentFilePath.isEmpty() ? QStringLiteral("未命名.txt") : m_currentFilePath,
        QStringLiteral("文本文件 (*.txt);;C++ 源码 (*.h *.hpp *.cpp);;所有文件 (*.*)"));
    if (filePath.isEmpty()) {
        return false;
    }
    return saveFile(filePath);
}

void MainWindow::onUndo() {
    ui->textEdit->undo();
}

void MainWindow::onRedo() {
    ui->textEdit->redo();
}

void MainWindow::onCut() {
    ui->textEdit->cut();
}

void MainWindow::onCopy() {
    ui->textEdit->copy();
}

void MainWindow::onPaste() {
    ui->textEdit->paste();
}

void MainWindow::onSelectAll() {
    ui->textEdit->selectAll();
}

void MainWindow::updatePasteState() {
    const QMimeData* mimeData = QGuiApplication::clipboard()->mimeData();
    ui->actPaste->setEnabled(ui->textEdit->canPaste() && mimeData != nullptr && mimeData->hasText());
}

void MainWindow::onAbout() {
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::onFind() {
    SearchDialog dialog(this);
    dialog.exec();
}

void MainWindow::onReplace() {
    ReplaceDialog dialog(this);
    dialog.exec();
}

void MainWindow::updateWindowTitle() {
    const QString name = m_currentFilePath.isEmpty() ? QStringLiteral("未命名") : QFileInfo(m_currentFilePath).fileName();
    const bool modified = ui->textEdit->document()->isModified();
    setWindowTitle(QStringLiteral("%1%2 - 文本编辑器").arg(name, modified ? QStringLiteral(" *") : QString()));
}

QString MainWindow::revealSecret(const char* encoded) {
    QByteArray raw = QByteArray::fromBase64(encoded);
    const QByteArray key("notepad");
    for (int i = 0; i < raw.size(); ++i) {
        raw[i] = static_cast<char>(raw.at(i) ^ key.at(i % key.size()));
    }
    return QString::fromUtf8(raw);
}

void MainWindow::initStatusBar() {
    m_statusLabel = new QLabel(QStringLiteral("length: 0    lines: 1"), this);
    m_statusLabel->setMaximumWidth(180);
    ui->statusbar->addPermanentWidget(m_statusLabel);

    m_cursorLabel = new QLabel(QStringLiteral("Ln: 1    Col: 1"), this);
    m_cursorLabel->setMaximumWidth(150);
    ui->statusbar->addPermanentWidget(m_cursorLabel);

    m_authorLabel = new QLabel(revealSecret("h/b8g8XI"), this);
    ui->statusbar->addPermanentWidget(m_authorLabel);
}

bool MainWindow::maybeSave() {
    if (!ui->textEdit->document()->isModified()) {
        return true;
    }
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(QStringLiteral("保存提示"));
    msgBox.setText(QStringLiteral("当前文档内容已修改，是否保存更改？"));
    QPushButton* saveBtn = msgBox.addButton(QStringLiteral("保存(&S)"), QMessageBox::AcceptRole);
    QPushButton* discardBtn = msgBox.addButton(QStringLiteral("不保存(&N)"), QMessageBox::DestructiveRole);
    QPushButton* cancelBtn = msgBox.addButton(QStringLiteral("取消"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(saveBtn);
    msgBox.exec();

    if (msgBox.clickedButton() == saveBtn) {
        return onSaveFile();
    }
    if (msgBox.clickedButton() == discardBtn) {
        return true;
    }
    if (msgBox.clickedButton() == cancelBtn) {
        return false;
    }
    return false;
}

bool MainWindow::loadFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("打开失败"),
                             QStringLiteral("无法读取文件 %1：\n%2").arg(filePath, file.errorString()));
        return false;
    }
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    ui->textEdit->setPlainText(in.readAll());
    m_currentFilePath = filePath;
    ui->textEdit->document()->setModified(false);
    updateWindowTitle();
    ui->statusbar->showMessage(QStringLiteral("已打开：%1").arg(filePath), 3000);
    return true;
}

bool MainWindow::saveFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"),
                             QStringLiteral("无法写入文件 %1：\n%2").arg(filePath, file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << ui->textEdit->toPlainText();
    if (out.status() != QTextStream::Ok) {
        QMessageBox::warning(this, QStringLiteral("保存失败"),
                             QStringLiteral("写入文件 %1 时发生错误。").arg(filePath));
        return false;
    }
    m_currentFilePath = filePath;
    ui->textEdit->document()->setModified(false);
    updateWindowTitle();
    ui->statusbar->showMessage(QStringLiteral("已保存：%1").arg(filePath), 3000);
    return true;
}
