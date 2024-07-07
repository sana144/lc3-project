#include "assembler.h"

FileClass lc3File("MEMORY.bin");

QString toBinary(int value, int bits){
    if (value < 0) {
        value = (1 << bits) + value; // Two's complement calculation if value is negative
        return QString::fromStdString(std::bitset<16>(value).to_string()).right(bits).rightJustified(bits, '1');
    }
    else {
        return QString::fromStdString(std::bitset<16>(value).to_string()).right(bits).rightJustified(bits, '0');
    }
}

uint16_t HandleOrgDirective(const QString &line, uint16_t currentAddress) {
    QRegularExpression re("\\s+");
    QVector<QString> tokens = line.split(re, Qt::SkipEmptyParts);

    if (tokens.size() < 2) {
        qWarning() << "Invalid ORG directive: " << line;
        return currentAddress;
    }

    QString addrString = tokens[1];
    bool ok;
    uint16_t newAddress = addrString.toInt(&ok, 16);
    if (!ok) {
        qWarning() << "Error converting address:" << addrString;
    } else {
        qDebug() << "Setting address to:" << newAddress;
        return newAddress;
    }

    return currentAddress; // Default to current address if conversion fails
}
void HandleLabels(const QString &line, uint16_t &address, QMap<QString, uint16_t> &labels) {
    QRegularExpression re("\\s+");
    QVector<QString> tokens = line.split(re, Qt::SkipEmptyParts);

    if (tokens.isEmpty())
        return;

    if (tokens[0].endsWith(',')) {
        labels[tokens[0].chopped(1)] = address;
        if (tokens.size() > 1) {
            address++;
        }
    } else {
        address++;
    }
}
QMap<QString, uint16_t> AssembleFirstPass(const QVector<QString> &lines) {
    QMap<QString, uint16_t> labels;
    uint16_t address = 0x3000; // Starting address

    for (const QString &line : lines) {
        if (line.isEmpty() || line.startsWith(';'))
            continue; // Ignore empty lines and comments

        QRegularExpression re("\\s+");
        QVector<QString> tokens = line.split(re, Qt::SkipEmptyParts);

        if (tokens[0] == "ORG") {
            address = HandleOrgDirective(line, address);
        } else if (tokens[0] == "END") {
            break; // Stop processing at the end directive
        } else {
            HandleLabels(line, address, labels);
        }
    }

    return labels;
}

QString AssembleInstruction(const QString &instruction, const QMap<QString, uint16_t> &labels, uint16_t currentAddress){
    // assemble instructions to binary
    QRegularExpression re("\\s+");
    QVector<QString> tokens = instruction.split(re, Qt::SkipEmptyParts);

    for (int i = 0; i < tokens.size(); ++i) {
        tokens[i] = tokens[i].remove(',').trimmed();
    }

    QString opcode = tokens[0];

    QString result;

    if (opcode == "ADD") {
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);
        QString sr1 = toBinary(tokens[2].mid(1).toInt(), 3);
        if (tokens[3].startsWith('R')) { // last part -> register mode
            QString sr2 = toBinary(tokens[3].mid(1).toInt(), 3);
            result = "0001" + dr + sr1 + "000" + sr2;
            return result;
        }
        else {
            // last part -> immediate mode
            int imm5 = tokens[3].mid(1).toInt();
            result = "0001" + dr + sr1 + "1" + toBinary(imm5, 5);
            return result;
        }
    }

    else if (opcode == "AND") {
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);
        QString sr1 = toBinary(tokens[2].mid(1).toInt(), 3);

        if (tokens[3].startsWith('R')) {//register mode
            QString sr2 = toBinary(tokens[3].mid(1).toInt(), 3);
            result = "0101" + dr + sr1 + "000" + sr2;
            return result;
        }
        else {
            // Immediate mode
            int imm5 = tokens[3].mid(1).toInt();
            result = "0101" + dr + sr1 + "1" + toBinary(imm5, 5);
            return result;
        }
    }

    else if (opcode.startsWith("BR")) {
        // considering flags
        QString n = tokens[0].contains('n') ? "1" : "0";
        QString z = tokens[0].contains('z') ? "1" : "0";
        QString p = tokens[0].contains('p') ? "1" : "0";

        int offset = labels.value(tokens[1]) - currentAddress - 1;
        QString offsetBinary = toBinary(offset, 9);

        result= "0000" + n + z + p + offsetBinary;
        return result;
    }

    else if (opcode == "JMP") {
        QString BaseR = tokens[1];

        int BaseRNum = BaseR.mid(1).toInt();

        result = "1100000" + toBinary(BaseRNum, 3) + "000000";

        return result;
    }

    else if (opcode == "JSR") {
        QString label = tokens[1];

        uint16_t addressofsubroutine = labels.value(label);

        int16_t offset = addressofsubroutine - currentAddress - 1;

        QString offsetBinary = toBinary(offset, 11);

        result = "01001" + offsetBinary;

        return result;
    }

    else if (opcode == "JSRR") {
        QString BaseR = tokens[1];

        int BaseRNum = BaseR.mid(1).toInt();

        result="0100000"+toBinary(BaseRNum, 3)+"000000";

        return result;
    }

    else if (opcode == "LD") {
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);

        int offset = labels.value(tokens[2]) - currentAddress - 1;

        result = "0010" + dr + toBinary(offset, 9);

        return result;
    }

    else if (opcode == "LDI") {
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);

        int offset = labels.value(tokens[2]) - currentAddress - 1;

        result= "1010" + dr + toBinary(offset, 9);

        return result;
    }

    else if (opcode == "LDR") {
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);

        QString baser = toBinary(tokens[2].mid(1).toInt(), 3);

        int offset = tokens[3].mid(1).toInt();

        result = "0110" + dr + baser + toBinary(offset, 6);

        return result;
    }

    else if (opcode == "LEA") {
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);

        int offset = labels.value(tokens[2]) - currentAddress - 1;

        result = "1110" + dr + toBinary(offset, 9);

        return result;
    }

    else if (opcode == "NOT") {
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);

        QString sr = toBinary(tokens[2].mid(1).toInt(), 3);

        result = "1001" + dr + sr + "111111";

        return result;
    }

    else if (opcode == "RET") {
        return "1100000111000000"; // Fixed code
    }

    else if (opcode == "ST") {
        QString sr = toBinary(tokens[1].mid(1).toInt(), 3);

        int offset = labels.value(tokens[2]) - currentAddress - 1;

        QString offsetbinary = toBinary(offset, 9);

        result = "0011" + sr + offsetbinary;

        return result;
    }

    else if (opcode == "STI") {
        QString sr = toBinary(tokens[1].mid(1).toInt(), 3);

        QString offset = toBinary(labels.value(tokens[2]) - currentAddress - 1, 9);

        result = "1011" + sr + offset;

        return result;
    }

    else if (opcode == "STR") {
        QString sr = toBinary(tokens[1].mid(1).toInt(), 3);

        QString baseR = toBinary(tokens[2].mid(1).toInt(), 3);

        int offset6 = tokens[3].mid(1).toInt();

        result = "0111" + sr + baseR + toBinary(offset6, 6);

        return result;
    }

    else if (opcode == "HALT") {
        return "1111000000100101"; // TRAP .. Fixed code
    }

    else if (opcode == "WORD") {
        QString result;
        for (int i = 1; i < tokens.size(); ++i) {
            bool ok;
            uint16_t value = tokens[i].toInt(&ok, 16);
            if (!ok) {
                QMessageBox::warning(nullptr, "Invalid Value", "Invalid .WORD value: " + tokens[i]);
                continue;
            }
            result += toBinary(value, 16) + "\n";
        }
        return result.trimmed();
    }

    else if (opcode == "BYTE") {
        QString result;
        for (int i = 1; i < tokens.size(); ++i) {
            bool ok;
            uint8_t value = tokens[i].toInt(&ok, 16);
            if (!ok || value > 0xFF) {
                QMessageBox::warning(nullptr, "Invalid Value", "Invalid .BYTE value: " + tokens[i]);
                continue;
            }
            result += toBinary(value, 8).rightJustified(16, '0') + "\n"; // Pad to 16 bits
        }
        return result.trimmed();
    }

    else if (opcode == "DEC") {
        bool ok;
        int16_t value = tokens[1].toInt(&ok);
        if (!ok) {
            QMessageBox::warning(nullptr, "Invalid Value", "Invalid .DEC value: " + tokens[1]);
            return "";
        }
        return toBinary(value, 16);
    }

    else if (opcode == "HEX") {
        bool ok;
        uint16_t value = tokens[1].toUInt(&ok, 16);
        if (!ok) {
            QMessageBox::warning(nullptr, "Invalid Value", "Invalid .HEX value: " + tokens[1]);
            return "";
        }
        return toBinary(value, 16);
    }

    return "";
}

QVector<QString> DeletionComments(const QString &line, const QChar &delimiter) {
    QVector<QString> tokens;
    QString token;
    bool isComment = false;

    for (QChar ch : line) {
        if (ch == ';')
        {
            isComment = true;
            break;
        }

        if (ch == delimiter)
        {
            if (!token.trimmed().isEmpty())
            {
                tokens.append(token.trimmed());
                token.clear();
            }
        }
        else
        {
            token += ch;
        }
    }
    if (!token.trimmed().isEmpty())
    {
        tokens.append(token.trimmed());
    }

    return tokens;
}

bool isRegister(const QString &token) {
    int regNum = token.at(1).digitValue();
    return token.startsWith('R') && regNum >= 0 && regNum <= 7;
}
bool IsValidInstruction(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels) {
    if (tokens.isEmpty())
        return false;

    QString opcode = tokens[0].trimmed();

    if (opcode == "ADD" || opcode == "AND")
    {
        if (tokens.size() != 4)
            return false;
        if (!isRegister(tokens[1]) || !isRegister(tokens[2]))
            return false;
        if (tokens[3].startsWith('R'))
        {
            if (!isRegister(tokens[3]))
                return false;
        }
        else
        {
            bool ok;
            int imm = tokens[3].mid(1).toInt(&ok);
            if (!ok || imm < -16 || imm > 15)
                return false;
        }
    }

    else if (opcode.startsWith("BR"))
    {
        if (tokens.size() != 2)
            return false;
        if (!labels.contains(tokens[1]))
            return false;
    }

    else if (opcode == "JMP" || opcode == "JSRR")
    {
        if (tokens.size() != 2)
            return false;
        if (!isRegister(tokens[1]))
            return false;
    }

    else if (opcode == "JSR")
    {
        if (tokens.size() != 2)
            return false;
        if (!labels.contains(tokens[1]))
            return false;
    }

    else if (opcode == "LD" || opcode == "LDI" || opcode == "LEA" || opcode == "ST" || opcode == "STI")
    {
        if (tokens.size() != 3)
            return false;
        if (!isRegister(tokens[1]))
            return false;
        if (!labels.contains(tokens[2]))
            return false;
    }

    else if (opcode == "LDR" || opcode == "STR")
    {
        if (tokens.size() != 4)
            return false;
        if (!isRegister(tokens[1]) || !isRegister(tokens[2]))
            return false;
        bool ok;
        int offset = tokens[3].toInt(&ok);
        if (!ok || offset < -32 || offset > 31)
            return false;
    }

    else if (opcode == "NOT")
    {
        if (tokens.size() != 3)
            return false;
        if (!isRegister(tokens[1]) || !isRegister(tokens[2]))
            return false;
    }

    else if (opcode == "RET" || opcode == "HALT" || opcode == "END")
    {
        if (tokens.size() != 1)
            return false;
    }

    else if (opcode == "WORD" || opcode == "BYTE")
    {
        if (tokens.size() != 2)
            return false;
        bool ok;
        tokens[1].toUInt(&ok);
        if (!ok)
            return false;
    }

    else if (opcode == "DEC")
    {
        if (tokens.size() != 2)
            return false;
        bool ok;
        tokens[1].toInt(&ok);
        if (!ok)
            return false;
    }

    else if (opcode == "HEX")
    {
        if (tokens.size() != 2)
            return false;
        bool ok;
        tokens[1].toUInt(&ok, 16);
        if (!ok)
            return false;
    }

    else
    {
       QMessageBox::warning(nullptr, "Error", "Invalid opcode: " + opcode);
        return false;
    }

    return true;
}

void AssemblesecondPass(const QVector<QString> &lines, const QMap<QString, uint16_t> &labels, Memory &memory){
    // convert assembly instructions to machine code
    uint16_t address = 0x3000; // Start address
    for (const QString &line : lines) { // if line is comment --> ignore
        if (line.isEmpty() || line.startsWith(';'))
            continue;

        QRegularExpression t("\\s+");
        QVector<QString> tokens = line.split(t, Qt::SkipEmptyParts);//tokenization based on any number of spaces

        if (tokens.size() > 1 && tokens[0] == "ORG") {
            QString addrString = tokens[1];
            bool ok;
            uint16_t newAddress = static_cast<uint16_t>(addrString.toInt(&ok, 16));
            if (!ok) {
                qWarning() << "Error converting address:" << addrString;
            }
            else {
                address = newAddress; // Set start address
                qDebug() << "Setting address to:" << address;
            }
        }

        else if (tokens.size() >= 1 && tokens[0] == "END"){
            break;
        }

        else if (tokens[0].endsWith(',')) { // this line starts with lable

            if (tokens.size() > 1) {
                // Extract instruction part after the (label,)
                QString instruction = line.mid(line.indexOf(',') + 1).trimmed();

                //deletion comments for passing to assembleInstruction function
                QVector<QString> tokens2 = DeletionComments(instruction, ' ');

                if (IsValidInstruction(tokens2, labels)) {
                    QString binaryInstruction = AssembleInstruction(instruction, labels, address);
                    bool ok;
                    uint16_t machineCode = static_cast<uint16_t>(binaryInstruction.toUInt(&ok, 2));
                    if (!ok)
                    {
                        QMessageBox::warning(nullptr, "Error", "Failed to convert binary instruction to machine code");
                        continue;
                    }
                    memory.write(address, machineCode);
                    address++;
                }
                else {
                    QMessageBox::warning(nullptr, "Error", "Skipping invalid instruction: " + line);
                }
            }
        }

        else { // instruction line without lable
            QVector<QString> tokens2 = DeletionComments(line, ' ');
            if (IsValidInstruction(tokens2, labels)) {
                QString binaryInstruction = AssembleInstruction(line, labels, address);
                bool ok;
                uint16_t machineCode = static_cast<uint16_t>(binaryInstruction.toUInt(&ok, 2));
                if (!ok) {
                   QMessageBox::warning(nullptr, "Error", "Failed to convert binary instruction to machine code");
                    continue;
                }
                memory.write(address, machineCode); // Write machine code to memory
                address++;
            }
            else {
                QMessageBox::warning(nullptr, "Error", "Skipping invalid instruction: " + line);
            }
        }
    }
}

int StartAssemble(QString inputFilename) {

    // Assemble the input code
    QVector<QString> assemblyCode = ReadInputFile(inputFilename);
    if (assemblyCode.isEmpty()) {
        QMessageBox::warning(nullptr, "Assembly Error", "Failed to assemble. Exiting...");
        return 0;
    }

    QMap<QString, uint16_t> labels = AssembleFirstPass(assemblyCode);

    Memory tempMemory(0xFFFF); // Create memory with size 0xFFFF (64KB)

    AssemblesecondPass(assemblyCode, labels, tempMemory);
    // Write assembled code to output file
    lc3File.WriteToFile(tempMemory, 0x3000, 0x3000 + assemblyCode.size() - 1);

    QMessageBox msgBox;
    msgBox.setStyleSheet("QMessageBox { background-color: rgb(0, 79, 115); }"
                         "QLabel { color: rgb(255, 255, 255); }"
                         "QPushButton { color: rgb(255, 255, 255); background-color: rgb(0, 79, 115); border: none; }");

    msgBox.setText("Assembly completed. Output written to MEMORY.bin");
    msgBox.setInformativeText(":)");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return 1;
}
