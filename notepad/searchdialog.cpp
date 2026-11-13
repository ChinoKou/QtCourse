#include "searchdialog.h"

#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextDocument>

#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QPlainTextEdit* editor, QWidget* parent)
    : QDialog(parent), ui(new Ui::SearchDialog), m_editor(editor) {
    ui->setupUi(this);
    ui->radioDown->setChecked(true);
    connect(ui->btnFindNext, &QPushButton::clicked, this, &SearchDialog::onFindNext);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
}

SearchDialog::~SearchDialog() {
    delete ui;
}

void SearchDialog::setTargetText(const QString& text) {
    if (!text.isEmpty()) {
        ui->lineEditTarget->setText(text);
        ui->lineEditTarget->selectAll();
    }
    ui->lineEditTarget->setFocus();
}

void SearchDialog::onFindNext() {
    if (m_editor == nullptr) {
        return;
    }
    const QString target = ui->lineEditTarget->text();
    if (target.isEmpty()) {
        ui->labelStatus->setText(QStringLiteral("提示：查找内容不能为空"));
        QMessageBox::information(this, QStringLiteral("查找"), QStringLiteral("请输入要查找的内容。"));
        ui->lineEditTarget->setFocus();
        return;
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
        return;
    }
    const QTextCursor savedCursor = m_editor->textCursor();
    m_editor->moveCursor(backward ? QTextCursor::End : QTextCursor::Start);
    if (m_editor->find(target, flags)) {
        const QTextCursor c = m_editor->textCursor();
        ui->labelStatus->setText(QStringLiteral("已循环匹配：第 %1 行，第 %2 列")
                                     .arg(c.blockNumber() + 1)
                                     .arg(c.columnNumber() + 1));
        return;
    }
    m_editor->setTextCursor(savedCursor);
    ui->labelStatus->setText(QStringLiteral("未找到匹配项：\"%1\"").arg(target));
    QMessageBox::information(this, QStringLiteral("查找"),
                             QStringLiteral("找不到 \"%1\"").arg(target));
}
