#include "searchdialog.h"

#include <QMessageBox>
#include <QPlainTextEdit>
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
}

void SearchDialog::onFindNext() {
    if (m_editor == nullptr) {
        return;
    }
    const QString target = ui->lineEditTarget->text();
    if (target.isEmpty()) {
        return;
    }
    QTextDocument::FindFlags flags;
    if (ui->checkCaseSensitive->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (ui->radioUp->isChecked()) {
        flags |= QTextDocument::FindBackward;
    }
    if (!m_editor->find(target, flags)) {
        QMessageBox::information(this, QStringLiteral("查找"),
                                 QStringLiteral("找不到 \"%1\"").arg(target));
    }
}
