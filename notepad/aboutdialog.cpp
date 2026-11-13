#include "aboutdialog.h"

#include <QByteArray>

#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    ui->labelDevValue->setText(revealSecret("h/b8g8XI"));
    ui->labelIdValue->setText(revealSecret("XF9GUURQUFxWRFFAUw=="));
    connect(ui->btnOk, &QPushButton::clicked, this, &QDialog::accept);
}

AboutDialog::~AboutDialog() {
    delete ui;
}

QString AboutDialog::revealSecret(const char* encoded) {
    QByteArray raw = QByteArray::fromBase64(encoded);
    const QByteArray key("notepad");
    for (int i = 0; i < raw.size(); ++i) {
        raw[i] = static_cast<char>(raw.at(i) ^ key.at(i % key.size()));
    }
    return QString::fromUtf8(raw);
}
