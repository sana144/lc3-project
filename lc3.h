#ifndef LC3_H
#define LC3_H

#include <QMainWindow>
#include "instructions.h"
#include "ui_lc3.h"
#include "memory.h"
#include "fileclass.h"
#include "assembler.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QScrollBar>

extern Registers registers;

QT_BEGIN_NAMESPACE
namespace Ui { class lc3; }
QT_END_NAMESPACE

extern int index;

class lc3 : public QMainWindow {
    Q_OBJECT
public:
    lc3(QWidget *parent = nullptr);
    ~lc3();
    QVector<QLabel*> memoryLabels;
    int sc=1;
    void SetDisplayRegisters();
    void UpdateDisplayMemory(int index);
    void scrollToUpdatedMemory();
    void SetDisplayMemory();

private slots:

    void on_Upload_code_clicked();

    void on_ASSEMBLE_clicked();

    void on_Next_clicked();

private:
    Ui::lc3 *ui;

};
bool readFromFile(Memory&, uint16_t, const QString&);
#endif // LC3_H
