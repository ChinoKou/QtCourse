#include "mainwindow.h"

#include <QByteArray>
#include <QLabel>

#include "aboutdialog.h"
#include "replacedialog.h"
#include "searchdialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initStatusBar();

    connect(ui->actAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actFind, &QAction::triggered, this, &MainWindow::onFind);
    connect(ui->actReplace, &QAction::triggered, this, &MainWindow::onReplace);
}

MainWindow::~MainWindow() {
    delete ui;
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
