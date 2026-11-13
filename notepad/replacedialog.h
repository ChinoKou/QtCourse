#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

class QPlainTextEdit;

namespace Ui {
class ReplaceDialog;
}

class ReplaceDialog : public QDialog {
    Q_OBJECT

public:
    explicit ReplaceDialog(QPlainTextEdit* editor, QWidget* parent = nullptr);
    ~ReplaceDialog() override;

    void setTargetText(const QString& text);

private slots:
    void onFindNext();
    void onReplace();
    void onReplaceAll();

private:
    bool findMatch();

    Ui::ReplaceDialog* ui;
    QPlainTextEdit* m_editor = nullptr;
};

#endif
