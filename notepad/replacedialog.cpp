#include "replacedialog.h"

#include "ui_replacedialog.h"

ReplaceDialog::ReplaceDialog(QWidget* parent) : QDialog(parent), ui(new Ui::ReplaceDialog) {
    ui->setupUi(this);
    ui->radioDown->setChecked(true);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
}

ReplaceDialog::~ReplaceDialog() {
    delete ui;
}
