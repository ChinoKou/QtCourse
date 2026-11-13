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
    ui->lineEditTarget->setFocus();
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
    const bool backward = ui->radioUp->isChecked();
    if (backward) {
        flags |= QTextDocument::FindBackward;
    }
    if (m_editor->find(target, flags)) {
        const QTextCursor c = m_editor->textCursor();
        ui->labelStatus->setText(QStringLiteral("已匹配：第 %1 行，第 %2 列")
                                     .arg(c.blockNumber() + 1)
                                     .arg(c.columnNumber() + 1));
        return true;
    }
    const QTextCursor savedCursor = m_editor->textCursor();
    m_editor->moveCursor(backward ? QTextCursor::End : QTextCursor::Start);
    if (m_editor->find(target, flags)) {
        const QTextCursor c = m_editor->textCursor();
        ui->labelStatus->setText(QStringLiteral("已循环匹配：第 %1 行，第 %2 列")
                                     .arg(c.blockNumber() + 1)
                                     .arg(c.columnNumber() + 1));
        return true;
    }
    m_editor->setTextCursor(savedCursor);
    return false;
}

void ReplaceDialog::onFindNext() {
    const QString target = ui->lineEditTarget->text();
    if (target.isEmpty()) {
        ui->labelStatus->setText(QStringLiteral("提示：查找内容不能为空"));
        QMessageBox::information(this, QStringLiteral("查找"), QStringLiteral("请输入要查找的内容。"));
        ui->lineEditTarget->setFocus();
        return;
    }
    if (!findMatch()) {
        ui->labelStatus->setText(QStringLiteral("未找到匹配项：\"%1\"").arg(target));
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
        ui->labelStatus->setText(QStringLiteral("提示：查找内容不能为空"));
        QMessageBox::information(this, QStringLiteral("替换"), QStringLiteral("请输入要查找的内容。"));
        ui->lineEditTarget->setFocus();
        return;
    }
    const QString replacement = ui->lineEditReplace->text();
    const Qt::CaseSensitivity cs = ui->checkCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QTextCursor cursor = m_editor->textCursor();
    bool replaced = false;
    if (cursor.hasSelection() && cursor.selectedText().compare(target, cs) == 0) {
        cursor.insertText(replacement);
        m_editor->setTextCursor(cursor);
        replaced = true;
    }
    if (!findMatch()) {
        ui->labelStatus->setText(replaced ? QStringLiteral("已完成当前替换，无更多匹配项")
                                          : QStringLiteral("未找到匹配项：\"%1\"").arg(target));
        if (!replaced) {
            QMessageBox::information(this, QStringLiteral("替换"),
                                     QStringLiteral("找不到 \"%1\"").arg(target));
        }
    }
}

void ReplaceDialog::onReplaceAll() {
    if (m_editor == nullptr) {
        return;
    }
    const QString target = ui->lineEditTarget->text();
    if (target.isEmpty()) {
        ui->labelStatus->setText(QStringLiteral("提示：查找内容不能为空"));
        QMessageBox::information(this, QStringLiteral("替换"), QStringLiteral("请输入要查找的内容。"));
        ui->lineEditTarget->setFocus();
        return;
    }
    const QString replacement = ui->lineEditReplace->text();
    QTextDocument::FindFlags flags;
    if (ui->checkCaseSensitive->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    const QTextCursor savedCursor = m_editor->textCursor();
    QTextCursor undoGroup = m_editor->textCursor();
    undoGroup.beginEditBlock();
    m_editor->moveCursor(QTextCursor::Start);
    int count = 0;
    while (m_editor->find(target, flags)) {
        QTextCursor match = m_editor->textCursor();
        match.insertText(replacement);
        m_editor->setTextCursor(match);
        ++count;
    }
    undoGroup.endEditBlock();
    if (count == 0) {
        m_editor->setTextCursor(savedCursor);
        ui->labelStatus->setText(QStringLiteral("未找到匹配项：\"%1\"").arg(target));
        QMessageBox::information(this, QStringLiteral("替换"),
                                 QStringLiteral("找不到 \"%1\"").arg(target));
    }
    else {
        ui->labelStatus->setText(QStringLiteral("全部替换完成：共替换 %1 处").arg(count));
        QMessageBox::information(this, QStringLiteral("替换"),
                                 QStringLiteral("已完成全部替换，共替换 %1 处。").arg(count));
    }
}
