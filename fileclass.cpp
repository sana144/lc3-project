#include "fileclass.h"
#include <QMessageBox>
Memory memory(0xFFFF);

FileClass::FileClass() {}

FileClass::FileClass(QString filename) {
    file.setFileName(filename);
}

void FileClass::WriteToFile(const Memory& memory, uint16_t startAddress, uint16_t endAddress) {
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, "Error", "Cannot open file for writing: MEMORY.bin");

        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_0);

    for (uint16_t address = startAddress; address <= endAddress; ++address) {
        uint16_t value = memory.read(address);
        out << value; // Write machine code to the binary file
    }

    file.close();
}

bool FileClass::ReadFromFile(uint16_t startAddress) {
    // read instructions from a binary file --> fill the memory
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Error", "Cannot open file for reading: MEMORY.bin");
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_0);

    uint16_t address = startAddress;
    while (!in.atEnd()) {
        uint16_t value;
        in >> value;
        memory.write(address, value); // Write value to memory at the current address
        address++;
    }

    file.close();
    return true;
}

QVector<QString> ReadInputFile(const QString &filename) { // read input file
    QVector<QString> lines;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))  {
        QMessageBox::warning(nullptr, "File Error", "Can't open file for reading: " + filename + "\nError: " + file.errorString());
        return lines;
    }
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        lines.append(line.trimmed());
    }
    file.close(); // close file after reading
    return lines; // return lines of input file
}
