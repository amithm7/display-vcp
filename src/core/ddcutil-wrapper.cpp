#include <QDebug>
#include <QProcess>

short getVCPValue(QString vcpCode) {
  vcpCode = vcpCode.toUpper();

  QProcess process;
  QString command = "ddcutil";
  QStringList arguments = {"--display=1", "--terse", "getvcp", vcpCode};

  // process.start("ls", QStringList() << "-l");
  process.start(command, arguments);
  process.waitForFinished();

  if (process.exitCode() != 0)
    return -1;

  // https://www.ddcutil.com/command_getvcp/#option-terse-brief
  QString output = process.readAllStandardOutput();

  // Simple Non-continuous => VCP feature-code SNC hex-value
  if (output.contains("VCP " + vcpCode + " SNC")) {
    QStringList parts = output.split("VCP " + vcpCode + " SNC");
    if (parts.size() > 1) {
      QString value = parts[1].trimmed().split(" ").first().split("x").last();
      return value.toShort(nullptr, 16);
    }
  }

  // Complex Non-continuous => VCP feature-code CNC mh-hex ml-hex sh-hex sl-hex
  if (output.contains("VCP " + vcpCode + " CNC")) {
    QStringList parts = output.split("VCP " + vcpCode + " CNC");

    if (parts.size() > 1) {
      QStringList values = parts[1].trimmed().split(" ");
      if (values.size() > 0) {
        QString highByte = values[2].split("x")[1];                                 // sh-hex
        QString lowByte = values[3].split("x")[1];                                  // sl-hex
        return (highByte.toShort(nullptr, 16) << 8) + lowByte.toShort(nullptr, 16); // set value
      }
    }
  }

  // Continuous [0, max-value] => VCP feature-code C cur-value-decimal max-value-decimal
  if (output.contains("VCP " + vcpCode + " C")) {
    QStringList parts = output.split("VCP " + vcpCode + " C");
    if (parts.size() > 1) {
      QStringList values = parts[1].trimmed().split(" ");
      if (values.size() > 0) {
        QString value = values.first();
        return value.toShort();
      }
    }
  }

  // // Table VCP code => VCP feature-code T hex-string
  // if (output.contains("VCP " + vcpCode + " T")) {
  //   QStringList parts = output.split("T");
  //   if (parts.size() > 1) {
  //     QString value = parts[1].trimmed();
  //     return value;
  //   }
  // }

  // Unknown format
  return -1;
}

int setVCPValue(QString vcpCode, short value) {
  QProcess process;
  QString command = "ddcutil";
  QStringList arguments = {"--display=1", "setvcp", vcpCode, QString::number(value)};

  process.start(command, arguments);
  if (process.waitForFinished()) {
    if (process.exitCode() != 0)
      qDebug() << "Failed to set VCP value:" << process.readAllStandardError();
  } else
    qDebug() << "Failed to start the process:" << process.errorString();

  return process.exitCode();
}