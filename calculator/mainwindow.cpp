#include "mainwindow.h"

#include <QEvent>
#include <QFile>
#include <QFont>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QPushButton>
#include <QStyleHints>
#include <cmath>

#include "ui_mainwindow.h"

namespace {

constexpr int kDisplayInset = 16;
constexpr int kMaxDisplayPointSize = 40;
constexpr int kMinDisplayPointSize = 12;

int countDigits(const QString& text) {
    int count = 0;
    for (const QChar character : text) {
        if (character.isDigit()) {
            ++count;
        }
    }
    return count;
}

}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    resize(330, 520);
    setMinimumSize(280, 470);
    setFocusPolicy(Qt::StrongFocus);

    ui->displayLabel->installEventFilter(this);

    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, &MainWindow::applyColorScheme);
    applyColorScheme(QGuiApplication::styleHints()->colorScheme());

    const auto buttons = findChildren<QPushButton*>();
    for (QPushButton* button : buttons) {
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        button->setMinimumSize(0, 54);
        button->setFocusPolicy(Qt::NoFocus);
        button->setCursor(Qt::PointingHandCursor);
    }

    connect(ui->btnDigit0, &QPushButton::clicked, this, [this] { inputDigit(0); });
    connect(ui->btnDigit1, &QPushButton::clicked, this, [this] { inputDigit(1); });
    connect(ui->btnDigit2, &QPushButton::clicked, this, [this] { inputDigit(2); });
    connect(ui->btnDigit3, &QPushButton::clicked, this, [this] { inputDigit(3); });
    connect(ui->btnDigit4, &QPushButton::clicked, this, [this] { inputDigit(4); });
    connect(ui->btnDigit5, &QPushButton::clicked, this, [this] { inputDigit(5); });
    connect(ui->btnDigit6, &QPushButton::clicked, this, [this] { inputDigit(6); });
    connect(ui->btnDigit7, &QPushButton::clicked, this, [this] { inputDigit(7); });
    connect(ui->btnDigit8, &QPushButton::clicked, this, [this] { inputDigit(8); });
    connect(ui->btnDigit9, &QPushButton::clicked, this, [this] { inputDigit(9); });
    connect(ui->btnDot, &QPushButton::clicked, this, &MainWindow::inputDot);
    connect(ui->btnSign, &QPushButton::clicked, this, &MainWindow::inputToggleSign);
    connect(ui->btnPercent, &QPushButton::clicked, this, &MainWindow::inputPercent);
    connect(ui->btnClearEntry, &QPushButton::clicked, this, &MainWindow::clearEntry);
    connect(ui->btnClear, &QPushButton::clicked, this, &MainWindow::clearAll);
    connect(ui->btnBackspace, &QPushButton::clicked, this, &MainWindow::inputBackspace);
    connect(ui->btnReciprocal, &QPushButton::clicked, this, &MainWindow::inputReciprocal);
    connect(ui->btnSquare, &QPushButton::clicked, this, &MainWindow::inputSquare);
    connect(ui->btnSqrt, &QPushButton::clicked, this, &MainWindow::inputSquareRoot);
    connect(ui->btnPlus, &QPushButton::clicked, this, [this] { inputOperator(Operator::Add); });
    connect(ui->btnMinus, &QPushButton::clicked, this, [this] { inputOperator(Operator::Subtract); });
    connect(ui->btnMultiply, &QPushButton::clicked, this, [this] { inputOperator(Operator::Multiply); });
    connect(ui->btnDivide, &QPushButton::clicked, this, [this] { inputOperator(Operator::Divide); });
    connect(ui->btnEquals, &QPushButton::clicked, this, &MainWindow::inputEquals);

    refreshDisplay();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        inputDigit(event->key() - Qt::Key_0);
        break;
    case Qt::Key_Period:
    case Qt::Key_Comma:
        inputDot();
        break;
    case Qt::Key_Plus:
        inputOperator(Operator::Add);
        break;
    case Qt::Key_Minus:
        inputOperator(Operator::Subtract);
        break;
    case Qt::Key_Asterisk:
        inputOperator(Operator::Multiply);
        break;
    case Qt::Key_Slash:
        inputOperator(Operator::Divide);
        break;
    case Qt::Key_Equal:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        inputEquals();
        break;
    case Qt::Key_Backspace:
        inputBackspace();
        break;
    case Qt::Key_Escape:
        clearAll();
        break;
    case Qt::Key_Delete:
        clearEntry();
        break;
    case Qt::Key_Percent:
        inputPercent();
        break;
    case Qt::Key_F9:
        inputToggleSign();
        break;
    case Qt::Key_R:
        inputReciprocal();
        break;
    case Qt::Key_Q:
        inputSquare();
        break;
    case Qt::Key_At:
        inputSquareRoot();
        break;
    default:
        QMainWindow::keyPressEvent(event);
        return;
    }
    event->accept();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->displayLabel && event->type() == QEvent::Resize) {
        fitDisplayFont(ui->displayLabel->text());
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::inputDigit(int digit) {
    if (m_error || m_justEvaluated) {
        clearAll();
    }
    if (!m_typing) {
        m_entry = QString::number(digit);
        m_typing = true;
    }
    else if (m_entry == QStringLiteral("0")) {
        m_entry = QString::number(digit);
    }
    else if (m_entry == QStringLiteral("-0")) {
        m_entry = QLatin1Char('-') + QString::number(digit);
    }
    else if (countDigits(m_entry) < 16) {
        m_entry.append(QString::number(digit));
    }
    m_operandReady = true;
    refreshDisplay();
}

void MainWindow::inputDot() {
    if (m_error || m_justEvaluated) {
        clearAll();
    }
    if (!m_typing) {
        m_entry = QStringLiteral("0.");
        m_typing = true;
    }
    else if (!m_entry.contains(QLatin1Char('.'))) {
        m_entry.append(QLatin1Char('.'));
    }
    m_operandReady = true;
    refreshDisplay();
}

void MainWindow::inputOperator(Operator op) {
    if (m_error) {
        clearAll();
    }
    const double value = currentValue();
    if (m_pendingOperator != Operator::None && m_operandReady) {
        double result = 0.0;
        if (!compute(m_pendingOperator, m_accumulator, value, result)) {
            return;
        }
        m_accumulator = result;
    }
    else if (m_pendingOperator == Operator::None) {
        m_accumulator = value;
    }
    m_currentValue = m_accumulator;
    m_pendingOperator = op;
    m_lastOperator = Operator::None;
    m_typing = false;
    m_operandReady = false;
    m_justEvaluated = false;
    m_entry.clear();
    m_expression = formatValue(m_accumulator) + QLatin1Char(' ') + operatorSymbol(op);
    refreshDisplay();
}

void MainWindow::inputEquals() {
    if (m_error) {
        clearAll();
        return;
    }
    const double value = currentValue();
    if (m_pendingOperator != Operator::None) {
        double result = 0.0;
        if (!compute(m_pendingOperator, m_accumulator, value, result)) {
            return;
        }
        m_expression = formatValue(m_accumulator) + QLatin1Char(' ') + operatorSymbol(m_pendingOperator) + QLatin1Char(' ') + formatValue(value) + QStringLiteral(" =");
        m_lastOperator = m_pendingOperator;
        m_lastOperand = value;
        m_currentValue = result;
        m_pendingOperator = Operator::None;
    }
    else if (m_lastOperator != Operator::None) {
        double result = 0.0;
        if (!compute(m_lastOperator, value, m_lastOperand, result)) {
            return;
        }
        m_expression = formatValue(value) + QLatin1Char(' ') + operatorSymbol(m_lastOperator) + QLatin1Char(' ') + formatValue(m_lastOperand) + QStringLiteral(" =");
        m_currentValue = result;
    }
    else {
        m_currentValue = value;
        m_expression = formatValue(value) + QStringLiteral(" =");
    }
    m_accumulator = m_currentValue;
    m_typing = false;
    m_operandReady = false;
    m_justEvaluated = true;
    m_entry.clear();
    refreshDisplay();
}

void MainWindow::inputPercent() {
    if (m_error) {
        clearAll();
    }
    const double value = currentValue();
    double result = value / 100.0;
    if (m_pendingOperator == Operator::Add || m_pendingOperator == Operator::Subtract) {
        result = m_accumulator * value / 100.0;
    }
    if (!std::isfinite(result)) {
        showError(QStringLiteral("溢出"));
        return;
    }
    m_currentValue = result;
    m_typing = false;
    m_operandReady = true;
    m_entry.clear();
    refreshDisplay();
}

void MainWindow::inputToggleSign() {
    if (m_error) {
        clearAll();
    }
    if (m_typing) {
        if (m_entry.startsWith(QLatin1Char('-'))) {
            m_entry.remove(0, 1);
        }
        else {
            m_entry.prepend(QLatin1Char('-'));
        }
        m_operandReady = true;
    }
    else if (m_pendingOperator != Operator::None && !m_operandReady) {
        m_entry = QStringLiteral("-0");
        m_typing = true;
        m_operandReady = true;
    }
    else {
        m_currentValue = -m_currentValue;
    }
    refreshDisplay();
}

void MainWindow::inputBackspace() {
    if (m_error) {
        clearAll();
        return;
    }
    if (!m_typing) {
        return;
    }
    m_entry.chop(1);
    if (m_entry.isEmpty() || m_entry == QStringLiteral("-")) {
        m_entry.clear();
        m_currentValue = 0.0;
        m_typing = false;
    }
    refreshDisplay();
}

void MainWindow::inputReciprocal() {
    if (m_error) {
        clearAll();
    }
    const double value = currentValue();
    if (value == 0.0) {
        showError(QStringLiteral("不能除以零"));
        return;
    }
    applyUnaryResult(QStringLiteral("1/(") + formatValue(value) + QLatin1Char(')'), 1.0 / value);
}

void MainWindow::inputSquare() {
    if (m_error) {
        clearAll();
    }
    const double value = currentValue();
    applyUnaryResult(QStringLiteral("sqr(") + formatValue(value) + QLatin1Char(')'), value * value);
}

void MainWindow::inputSquareRoot() {
    if (m_error) {
        clearAll();
    }
    const double value = currentValue();
    if (value < 0.0) {
        showError(QStringLiteral("无效输入"));
        return;
    }
    applyUnaryResult(QStringLiteral("√(") + formatValue(value) + QLatin1Char(')'), std::sqrt(value));
}

void MainWindow::applyUnaryResult(const QString& expression, double result) {
    if (!std::isfinite(result)) {
        showError(QStringLiteral("溢出"));
        return;
    }
    m_currentValue = result;
    m_typing = false;
    m_operandReady = true;
    m_entry.clear();
    if (m_pendingOperator == Operator::None) {
        m_expression = expression;
        m_justEvaluated = true;
    }
    refreshDisplay();
}

void MainWindow::clearAll() {
    m_entry.clear();
    m_expression.clear();
    m_currentValue = 0.0;
    m_accumulator = 0.0;
    m_lastOperand = 0.0;
    m_pendingOperator = Operator::None;
    m_lastOperator = Operator::None;
    m_typing = false;
    m_operandReady = false;
    m_justEvaluated = false;
    m_error = false;
    refreshDisplay();
}

void MainWindow::clearEntry() {
    if (m_error) {
        clearAll();
        return;
    }
    m_entry.clear();
    m_currentValue = 0.0;
    m_typing = false;
    m_operandReady = false;
    m_justEvaluated = false;
    if (m_pendingOperator == Operator::None) {
        m_expression.clear();
    }
    refreshDisplay();
}

double MainWindow::currentValue() const {
    if (m_typing) {
        return m_entry.toDouble();
    }
    return m_currentValue;
}

void MainWindow::refreshDisplay() {
    if (m_error) {
        return;
    }
    if (m_typing) {
        setDisplayText(groupText(m_entry));
    }
    else {
        setDisplayText(formatValue(m_currentValue));
    }
    ui->expressionLabel->setText(m_expression);
}

void MainWindow::setDisplayText(const QString& text) {
    ui->displayLabel->setText(text);
    fitDisplayFont(text);
}

void MainWindow::fitDisplayFont(const QString& text) {
    const int availableWidth = ui->displayLabel->width() - kDisplayInset;
    const int availableHeight = ui->displayLabel->height() - kDisplayInset;
    if (availableWidth <= 0 || availableHeight <= 0) {
        return;
    }
    int pointSize = kMaxDisplayPointSize;
    QFont font(QStringLiteral("Segoe UI"), pointSize, QFont::DemiBold);
    QFontMetricsF metrics(font);
    while (pointSize > kMinDisplayPointSize && (metrics.horizontalAdvance(text) > availableWidth || metrics.height() > availableHeight)) {
        --pointSize;
        font.setPointSize(pointSize);
        metrics = QFontMetricsF(font);
    }
    if (font != ui->displayLabel->font()) {
        ui->displayLabel->setFont(font);
    }
}

void MainWindow::showError(const QString& message) {
    clearAll();
    m_error = true;
    ui->expressionLabel->clear();
    setDisplayText(message);
}

void MainWindow::applyColorScheme(Qt::ColorScheme scheme) {
    const QString path = scheme == Qt::ColorScheme::Dark ? QStringLiteral(":/calculator-dark.qss") : QStringLiteral(":/calculator-light.qss");
    QFile styleFile(path);
    if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qFatal("样式表文件无法打开");
    }
    setStyleSheet(QString::fromUtf8(styleFile.readAll()));
}

bool MainWindow::compute(Operator op, double left, double right, double& result) {
    switch (op) {
    case Operator::Add:
        result = left + right;
        break;
    case Operator::Subtract:
        result = left - right;
        break;
    case Operator::Multiply:
        result = left * right;
        break;
    case Operator::Divide:
        if (right == 0.0) {
            showError(QStringLiteral("不能除以零"));
            return false;
        }
        result = left / right;
        break;
    case Operator::None:
        return false;
    }
    if (!std::isfinite(result)) {
        showError(QStringLiteral("溢出"));
        return false;
    }
    return true;
}

QString MainWindow::operatorSymbol(Operator op) {
    switch (op) {
    case Operator::Add:
        return QStringLiteral("+");
    case Operator::Subtract:
        return QStringLiteral("−");
    case Operator::Multiply:
        return QStringLiteral("×");
    case Operator::Divide:
        return QStringLiteral("÷");
    case Operator::None:
        break;
    }
    return QString();
}

QString MainWindow::groupText(const QString& text) {
    QString integerPart = text;
    QString fractionPart;
    const int dotIndex = text.indexOf(QLatin1Char('.'));
    if (dotIndex >= 0) {
        integerPart = text.left(dotIndex);
        fractionPart = text.mid(dotIndex);
    }
    const bool negative = integerPart.startsWith(QLatin1Char('-'));
    if (negative) {
        integerPart.remove(0, 1);
    }
    for (int index = integerPart.length() - 3; index > 0; index -= 3) {
        integerPart.insert(index, QLatin1Char(','));
    }
    return (negative ? QStringLiteral("-") : QString()) + integerPart + fractionPart;
}

QString MainWindow::formatValue(double value) {
    if (!std::isfinite(value)) {
        return QStringLiteral("溢出");
    }
    if (value == 0.0) {
        return QStringLiteral("0");
    }
    const QString text = QString::number(value, 'g', 16);
    if (text.contains(QLatin1Char('e'))) {
        return text;
    }
    return groupText(text);
}
