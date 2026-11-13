#include "aboutdialog.h"

#include <QByteArray>

#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);
    ui->labelDevName->setText(revealSecret("i9P0gP/wjO7qm9nqiP3micHMUEEYTk+RyNaE69mAyP9CUVZaW0VRQlhUWl9G"));
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
