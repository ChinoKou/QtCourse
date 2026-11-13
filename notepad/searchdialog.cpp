#include "searchdialog.h"

#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget* parent) : QDialog(parent), ui(new Ui::SearchDialog) {
    ui->setupUi(this);
    ui->radioDown->setChecked(true);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
}

SearchDialog::~SearchDialog() {
    delete ui;
}
