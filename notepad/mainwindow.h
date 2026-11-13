#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QCloseEvent;
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

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onNewFile();
    void onOpenFile();
    bool onSaveFile();
    bool onSaveAsFile();
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onSelectAll();
    void updatePasteState();
    void onAbout();
    void onFind();
    void onReplace();
    void updateWindowTitle();

private:
    static QString revealSecret(const char* encoded);
    void initStatusBar();
    bool maybeSave();
    bool loadFile(const QString& filePath);
    bool saveFile(const QString& filePath);

    Ui::MainWindow* ui;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_cursorLabel = nullptr;
    QLabel* m_authorLabel = nullptr;
    QString m_currentFilePath;
};

#endif
