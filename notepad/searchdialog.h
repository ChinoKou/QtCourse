#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

class QPlainTextEdit;

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog {
    Q_OBJECT

public:
    explicit SearchDialog(QPlainTextEdit* editor, QWidget* parent = nullptr);
    ~SearchDialog() override;

    void setTargetText(const QString& text);

private slots:
    void onFindNext();

private:
    Ui::SearchDialog* ui;
    QPlainTextEdit* m_editor = nullptr;
};

#endif
