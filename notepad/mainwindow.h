#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onAbout();
    void onFind();
    void onReplace();

private:
    static QString revealSecret(const char* encoded);
    void initStatusBar();

    Ui::MainWindow* ui;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_cursorLabel = nullptr;
    QLabel* m_authorLabel = nullptr;
};

#endif
