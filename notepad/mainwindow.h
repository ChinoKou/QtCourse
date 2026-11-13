#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColor>
#include <QMainWindow>
#include <QString>

class QCloseEvent;
class QLabel;
class QTimer;
class ReplaceDialog;
class SearchDialog;

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
    void onFont();
    void onFontColor();
    void onFontBackColor();
    void onEditorBackColor();
    void onToggleWrap(bool checked);
    void onToggleLineNumbers(bool checked);
    void onToggleAutoSave(bool checked);
    void onAutoSaveTimeout();
    void onToggleToolBar(bool checked);
    void onToggleStatusBar(bool checked);
    void updateStatusBar();
    void onAbout();
    void onFind();
    void onReplace();
    void updateWindowTitle();

private:
    static QString revealSecret(const char* encoded);
    void initStatusBar();
    void updateHeaderBadges();
    void applyEditorColors();
    bool maybeSave();
    bool loadFile(const QString& filePath);
    bool saveFile(const QString& filePath);

    Ui::MainWindow* ui;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_cursorLabel = nullptr;
    QLabel* m_encodingLabel = nullptr;
    QLabel* m_authorLabel = nullptr;
    SearchDialog* m_searchDialog = nullptr;
    ReplaceDialog* m_replaceDialog = nullptr;
    QTimer* m_autoSaveTimer = nullptr;
    QString m_currentFilePath;
    QColor m_fontColor;
    QColor m_editorBgColor;
};

#endif
