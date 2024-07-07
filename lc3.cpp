#include "lc3.h"
#include "ui_lc3.h"
#include <QFileDialog>

Registers registers;

QString fileName;

int index;

lc3::lc3(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::lc3) {
    ui->setupUi(this);
    SetDisplayMemory();
}

lc3::~lc3()
{
    delete ui;
}

void lc3::SetDisplayRegisters()
{
    //Setting registers
    ui->R0_in->setText("0x" + QString::number(registers.getR(0), 16).toUpper());
    ui->R1_in->setText("0x" + QString::number(registers.getR(1), 16).toUpper());
    ui->R2_in->setText("0x" + QString::number(registers.getR(2), 16).toUpper());
    ui->R3_in->setText("0x" + QString::number(registers.getR(3), 16).toUpper());
    ui->R4_in->setText("0x" + QString::number(registers.getR(4), 16).toUpper());
    ui->R5_in->setText("0x" + QString::number(registers.getR(5), 16).toUpper());
    ui->R6_in->setText("0x" + QString::number(registers.getR(6), 16).toUpper());
    ui->R7_in->setText("0x" + QString::number(registers.getR(7), 16).toUpper());
    ui->MAR_in->setText("0x" + QString::number(registers.getMAR(), 16).toUpper());
    ui->MDR_in->setText("0x" + QString::number(registers.getMDR(), 16).toUpper());
    ui->Negative_in->setText("0x" + QString::number((registers.getCC() >> 2) & 0x1, 16).toUpper());
    ui->PC_in->setText("0x" + QString::number(registers.getPC(), 16).toUpper());
    ui->Positive_in->setText("0x" + QString::number(registers.getCC() & 0x1, 16).toUpper());
    ui->Zero_in->setText("0x" + QString::number((registers.getCC() >> 1) & 0x1, 16).toUpper());
    ui->IR_in->setText("0x" + QString::number(registers.getIR(), 16).toUpper());
}

void lc3::UpdateDisplayMemory(int index) {
    if (index < ui->memoryTable->rowCount()) {
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString("0x%1").arg(memory.read(index), 4, 16, QChar('0')).toUpper());
        ui->memoryTable->setItem(index, 1, valueItem);
        ui->memoryTable->scrollToItem(valueItem, QAbstractItemView::PositionAtCenter);//scroll to updated part of memory
    }
}

void lc3::on_Upload_code_clicked() {
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Assembly Files (*.asm)"));
    if (!fileName.isEmpty())
    {
        if (fileName.endsWith(".asm", Qt::CaseInsensitive)){
            QMessageBox msgBox;
            msgBox.setStyleSheet("QMessageBox { background-color: rgb(0, 79, 115); }"
                                 "QLabel { color: rgb(255, 255, 255); }"
                                 "QPushButton { color: rgb(255, 255, 255); background-color: rgb(0, 79, 115); border: none; }");

            msgBox.setText(tr("File Selected"));
            msgBox.setInformativeText(tr("You selected:\n%1").arg(fileName));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        }
        else
        {
            QMessageBox::warning(this, tr("Invalid File"), tr("Please select a file with an .asm extension."));
        }
    }
    else
    {
        QMessageBox::warning(this, tr("No File Selected"), tr("No file was selected."));
    }
}

void lc3::SetDisplayMemory() {
    ui->memoryTable->setRowCount(0xFFFF);
    ui->memoryTable->setColumnCount(2);
    QStringList headers = {"Address", "Value"};
    ui->memoryTable->setHorizontalHeaderLabels(headers);
    ui->memoryTable->verticalHeader()->setVisible(false); // Hide the vertical header

    for (size_t i = 0; i < 0xFFFF; ++i) {
        QTableWidgetItem *addressItem = new QTableWidgetItem(QString("0x%1").arg(i, 4, 16, QChar('0')).toUpper());
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString("0x%1").arg(memory.read(i), 4, 16, QChar('0')).toUpper());
        ui->memoryTable->setItem(i, 0, addressItem);
        ui->memoryTable->setItem(i, 1, valueItem);
    }

    ui->memoryTable->setStyleSheet("QHeaderView::section {"
                                   "background-color: rgb(0, 79, 115);"
                                   "font: 9pt 'Segoe Script';"
                                   "color: rgb(255, 255, 255);"
                                   "border: 1px solid black;"
                                   "}"
                                   "QTableWidget {"
                                   "background-color: rgb(0, 79, 115);"
                                   "font: 9pt 'Segoe Script';"
                                   "color: rgb(255, 255, 255);"
                                   "}");

    ui->memoryTable->horizontalHeader()->setFixedHeight(30);
    ui->memoryTable->setRowHeight(0, 30);
    ui->memoryTable->setRowHeight(1, 30);
}

void lc3::on_ASSEMBLE_clicked() {
    if(StartAssemble(fileName)){
        sc=1;
        lc3File.ReadFromFile(0x3000);
        registers.setPC(0x3000);
        index = 0x3000;
        SetDisplayMemory();
        UpdateDisplayMemory(index);
    }

}

void lc3::on_Next_clicked()
{
    if (sc == -1) {
        // HALT
        if (HultInstruction()) {
            QMessageBox msgBox;
            msgBox.setStyleSheet("QMessageBox { background-color: rgb(0, 79, 115); }"
                                 "QLabel { color: rgb(255, 255, 255); }"
                                 "QPushButton { color: rgb(255, 255, 255); background-color: rgb(0, 79, 115); border: none; }");

            msgBox.setText(tr("Program Done"));
            msgBox.setInformativeText("The program has reached the HALT instruction and is done.");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

            //QMessageBox::information(this, "Program Done", "The program has reached the HALT instruction and is done.");
        }
    }
    else if (sc == 1) {
        Fetch(memory);
        if (HultInstruction()) {
            QMessageBox::information(this, "Program Done", "The program has reached the HALT instruction and is done.");
            sc = -1;
        }
        else  {
            SetDisplayRegisters();
            ui->Phase->setText("Fetch");
            sc++;
        }
    }
    else if (sc == 2) {
        Decode();
        SetDisplayRegisters();
        ui->Phase->setText("Decode");
        sc++;
    }
    else if (sc == 3) {
        EvalAddress(memory);
        SetDisplayRegisters();
        ui->Phase->setText("Evaluate Address");
        sc++;
    }
    else if (sc == 4)
    {
        FetchOperands(memory);
        SetDisplayRegisters();
        ui->Phase->setText("Fetch Operands");
        sc++;

    }
    else if(sc == 5){
        Execute();
        SetDisplayRegisters();
        ui->Phase->setText("Execute");
        sc++;
    }
    else if (sc == 6) {
        Store(memory);
        SetDisplayRegisters();
        ui->Phase->setText("Store");
        UpdateDisplayMemory(index);
        sc = 1;
    }
}

