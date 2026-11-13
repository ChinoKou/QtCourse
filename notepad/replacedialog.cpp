#include "replacedialog.h"

#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextDocument>

#include "ui_replacedialog.h"

ReplaceDialog::ReplaceDialog(QPlainTextEdit* editor, QWidget* parent)
    : QDialog(parent), ui(new Ui::ReplaceDialog), m_editor(editor) {
    ui->setupUi(this);
    ui->radioDown->setChecked(true);
    connect(ui->btnFindNext, &QPushButton::clicked, this, &ReplaceDialog::onFindNext);
    connect(ui->btnReplace, &QPushButton::clicked, this, &ReplaceDialog::onReplace);
    connect(ui->btnReplaceAll, &QPushButton::clicked, this, &ReplaceDialog::onReplaceAll);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
}

ReplaceDialog::~ReplaceDialog() {
    delete ui;
}

void ReplaceDialog::setTargetText(const QString& text) {
    if (!text.isEmpty()) {
        ui->lineEditTarget->setText(text);
        ui->lineEditTarget->selectAll();
    }
}

bool ReplaceDialog::findMatch() {
    if (m_editor == nullptr) {
        return false;
    }
    const QString target = ui->lineEditTarget->text();
    if (target.isEmpty()) {
        return false;
    }
    QTextDocument::FindFlags flags;
    if (ui->checkCaseSensitive->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (ui->radioUp->isChecked()) {
        flags |= QTextDocument::FindBackward;
    }
    return m_editor->find(target, flags);
}

void ReplaceDialog::onFindNext() {
    const QString target = ui->lineEditTarget->text();
    if (target.isEmpty()) {
        return;
    }
    if (!findMatch()) {
        QMessageBox::information(this, QStringLiteral("查找"),
                                 QStringLiteral("找不到 \"%1\"").arg(target));
    }
}

void ReplaceDialog::onReplace() {
    if (m_editor == nullptr) {
        return;
    }
    const QString target = ui->lineEditTarget->text();
    if (target.isEmpty()) {
        return;
    }
    const QString replacement = ui->lineEditReplace->text();
    const Qt::CaseSensitivity cs = ui->checkCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QTextCursor cursor = m_editor->textCursor();
    if (cursor.hasSelection() && cursor.selectedText().compare(target, cs) == 0) {
        cursor.insertText(replacement);
    }
    onFindNext();
}

void ReplaceDialog::onReplaceAll() {
    if (m_editor == nullptr) {
        return;
    }
    const QString target = ui->lineEditTarget->text();
    if (target.isEmpty()) {
        return;
    }
    const QString replacement = ui->lineEditReplace->text();
    QTextDocument::FindFlags flags;
    if (ui->checkCaseSensitive->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    m_editor->moveCursor(QTextCursor::Start);
    int count = 0;
    while (m_editor->find(target, flags)) {
        m_editor->textCursor().insertText(replacement);
        ++count;
    }
    if (count == 0) {
        QMessageBox::information(this, QStringLiteral("替换"),
                                 QStringLiteral("找不到 \"%1\"").arg(target));
    }
    else {
        QMessageBox::information(this, QStringLiteral("替换"),
                                 QStringLiteral("已完成全部替换，共替换 %1 处。").arg(count));
    }
}
