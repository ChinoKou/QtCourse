#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
class QKeyEvent;
class QResizeEvent;
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    enum class Operator { None, Add, Subtract, Multiply, Divide };

    void inputDigit(int digit);
    void inputDot();
    void inputOperator(Operator op);
    void inputEquals();
    void inputPercent();
    void inputToggleSign();
    void inputBackspace();
    void inputReciprocal();
    void inputSquare();
    void inputSquareRoot();
    void applyUnaryResult(const QString& expression, double result);
    void clearAll();
    void clearEntry();

    double currentValue() const;
    void refreshDisplay();
    void setDisplayText(const QString& text);
    void fitDisplayFont(const QString& text);
    void showError(const QString& message);
    void applyColorScheme(Qt::ColorScheme scheme);
    bool compute(Operator op, double left, double right, double& result);

    static QString operatorSymbol(Operator op);
    static QString groupText(const QString& text);
    static QString formatValue(double value);

    Ui::MainWindow* ui;

    QString m_entry;
    QString m_expression;
    double m_currentValue = 0.0;
    double m_accumulator = 0.0;
    double m_lastOperand = 0.0;
    Operator m_pendingOperator = Operator::None;
    Operator m_lastOperator = Operator::None;
    bool m_typing = false;
    bool m_operandReady = false;
    bool m_justEvaluated = false;
    bool m_error = false;
};

#endif  // MAINWINDOW_H
