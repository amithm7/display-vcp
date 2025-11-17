#include <QDebug>
#include <QKeyEvent>
#include <QProcess>
#include <QTimer>

// Qt Widgets https://doc.qt.io/qt-6/qtwidgets-index.html
#include <QApplication>
// ---
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>
#include <QWidget>

#include "core/constants.h"
#include "core/ddcutil-wrapper.h"
#include <KAboutData>
#include <KStatusNotifierItem>

class CustomWidget : public QWidget {
  Q_OBJECT

public:
  explicit CustomWidget(QWidget *parent = nullptr) : QWidget(parent) {
    // setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // Ensure the widget can receive focus
    // setFocusPolicy(Qt::StrongFocus);

    // Install event filter to monitor global focus changes
    qApp->installEventFilter(this);
  }

  ~CustomWidget() { qApp->removeEventFilter(this); }

protected:
  // Required to add styling to the widget
  // https://doc.qt.io/qt-5/stylesheet-reference.html
  void paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
  }

  // Close on Esc key (mimicking QMenu behavior)
  void keyPressEvent(QKeyEvent *event) override {
    if (event->key() == Qt::Key_Escape) {
      close();
      event->accept();
    } else {
      QWidget::keyPressEvent(event);
    }
  }

  // Close when focus is lost due to clicking outside the application
  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::FocusOut) {
      QFocusEvent *focusEvent = static_cast<QFocusEvent *>(event);
      if (focusEvent->reason() == Qt::ActiveWindowFocusReason) {
        close();
      }
    }
    return QWidget::eventFilter(obj, event);
  }
};

const QString CURRENT_BRIGHTNESS_TEXT = "Brightness: ";
const QString CURRENT_CONTRAST_TEXT = "Contrast: ";

void adjustProperty(QString vcpCode, short delta, short &currentValue,
                    std::pair<short, short> range, auto *increaseBtn, auto *decreaseBtn,
                    auto *currentValueLabel, QString PROPERTY_LABEL) {

  increaseBtn->setEnabled(false);
  decreaseBtn->setEnabled(false);

  short newValue = currentValue + delta;

  auto [minValue, maxValue] = range;
  if (newValue > maxValue)
    newValue = maxValue;
  else if (newValue < minValue)
    newValue = minValue;

  setVCPValueAsync(vcpCode, newValue,
                   [increaseBtn, decreaseBtn, currentValueLabel, PROPERTY_LABEL, minValue, maxValue,
                    &currentValue, newValue](int exitCode) mutable {
                     if (exitCode != 0) {
                       qDebug() << "Failed to change property:" << exitCode;
                     } else {
                       qDebug() << "Property changed successfully!";
                       currentValue = newValue;
                     }

                     QTimer::singleShot(0, [increaseBtn, decreaseBtn, currentValueLabel,
                                            PROPERTY_LABEL, minValue, maxValue, &currentValue]() {
                       if (currentValue == minValue)
                         decreaseBtn->setEnabled(false);
                       else if (currentValue == maxValue)
                         increaseBtn->setEnabled(false);
                       else {
                         increaseBtn->setEnabled(true);
                         decreaseBtn->setEnabled(true);
                       }

                       currentValueLabel->setText(PROPERTY_LABEL + QString::number(currentValue));
                     });
                   });
}

QMenu *createContextMenu(const short &currentBrightness, const short &currentContrast,
                         QApplication &app) {
  QMenu *contextMenu = new QMenu();

  QAction *brightnessAction =
      contextMenu->addAction(CURRENT_BRIGHTNESS_TEXT + QString::number(currentBrightness));
  brightnessAction->setEnabled(false);

  // QAction *increaseBrightnessAction =
  //     contextMenu->addAction("+" + QString::number(Constants::Display::Brightness::STEP));
  // QAction *decreaseBrightnessAction =
  //     contextMenu->addAction("-" + QString::number(Constants::Display::Brightness::STEP));

  // QObject::connect(
  //     increaseBrightnessAction, &QAction::triggered,
  //     [increaseBrightnessAction, decreaseBrightnessAction, brightnessAction,
  //     &currentBrightness]() {
  //       adjustProperty(
  //           QString::number(Constants::MCCS::VCPCode::std::BRIGHTNESS, 16),
  //           Constants::Display::Brightness::STEP, currentBrightness,
  //           {Constants::Display::CONTINUOUS_FEATURE_MIN, Constants::Display::Brightness::MAX},
  //           increaseBrightnessAction, decreaseBrightnessAction, brightnessAction,
  //           CURRENT_BRIGHTNESS_TEXT);
  //     });
  // QObject::connect(
  //     decreaseBrightnessAction, &QAction::triggered,
  //     [increaseBrightnessAction, decreaseBrightnessAction, brightnessAction,
  //     &currentBrightness]() {
  //       adjustProperty(
  //           QString::number(Constants::MCCS::VCPCode::std::BRIGHTNESS, 16),
  //           -Constants::Display::Brightness::STEP, currentBrightness,
  //           {Constants::Display::CONTINUOUS_FEATURE_MIN, Constants::Display::Brightness::MAX},
  //           increaseBrightnessAction, decreaseBrightnessAction, brightnessAction,
  //           CURRENT_BRIGHTNESS_TEXT);
  //     });

  QAction *contrastAction =
      contextMenu->addAction(CURRENT_CONTRAST_TEXT + QString::number(currentContrast));
  contrastAction->setEnabled(false);

  QAction *quitAction = contextMenu->addAction("Quit");
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

  // Update the current brightness and contrast when the context menu is shown
  QObject::connect(
      contextMenu, &QMenu::aboutToShow,
      [brightnessAction, contrastAction, &currentBrightness, &currentContrast]() {
        brightnessAction->setText(CURRENT_BRIGHTNESS_TEXT + QString::number(currentBrightness));
        contrastAction->setText(CURRENT_CONTRAST_TEXT + QString::number(currentContrast));
      });

  return contextMenu;
}

auto createContinuousPropertyWidget(const QString &labelText, short &currentValue, short step,
                                    std::pair<short, short> range, QString vcpCode) {
  QWidget *propertyWidget = new QWidget();

  QHBoxLayout *propertyLayout = new QHBoxLayout(propertyWidget);
  propertyWidget->setLayout(propertyLayout);

  QLabel *propertyLabel = new QLabel(labelText + QString::number(currentValue));
  QPushButton *increaseButton = new QPushButton("+" + QString::number(step));
  QPushButton *decreaseButton = new QPushButton("-" + QString::number(step));
  propertyLayout->addWidget(decreaseButton);
  propertyLayout->addSpacing(10);

  propertyLayout->addWidget(propertyLabel);
  propertyLabel->setFixedWidth(120);
  propertyLabel->setAlignment(Qt::AlignCenter);

  propertyLayout->addSpacing(10);
  propertyLayout->addWidget(increaseButton);

  if (currentValue == range.first)
    decreaseButton->setEnabled(false);
  else if (currentValue == range.second)
    increaseButton->setEnabled(false);

  QObject::connect(increaseButton, &QPushButton::clicked,
                   [increaseButton, decreaseButton, propertyLabel, &currentValue, step, range,
                    vcpCode, labelText]() {
                     adjustProperty(vcpCode, step, currentValue, range, increaseButton,
                                    decreaseButton, propertyLabel, labelText);
                   });
  QObject::connect(decreaseButton, &QPushButton::clicked,
                   [increaseButton, decreaseButton, propertyLabel, &currentValue, step, range,
                    vcpCode, labelText]() {
                     adjustProperty(vcpCode, -step, currentValue, range, increaseButton,
                                    decreaseButton, propertyLabel, labelText);
                   });

  // Refresh the current value periodically
  QTimer *timer = new QTimer();
  QObject::connect(
      timer, &QTimer::timeout,
      [propertyLabel, labelText, &currentValue, vcpCode, increaseButton, decreaseButton]() {
        // Skip the refresh to avoid races/overwrites.
        if (!increaseButton->isEnabled() && !decreaseButton->isEnabled()) {
          qDebug() << "Property change in progress. Skipping refresh.";
          return;
        }

        // Prevent waking up the monitor if it's off
        QProcess process;
        process.start("xset", {"q"});
        process.waitForFinished();

        QString output = process.readAllStandardOutput();
        if (output.contains("Monitor is Off")) {
          qDebug() << "Monitor is off. Skipping refresh.";
          return;
        }

        getVCPValueAsync(vcpCode, [propertyLabel, labelText, &currentValue](short newValue) {
          if (newValue != -1) {
            currentValue = newValue;
            propertyLabel->setText(labelText + QString::number(currentValue));
          }
        });
      });
  timer->start(Constants::Display::REFRESH_INTERVAL);

  return propertyWidget;
};

auto createModeWidget(short &currentMode, QWidget *brightnessWidget, QWidget *contrastWidget) {
  QWidget *modeWidget = new QWidget();
  QGridLayout *modeLayout = new QGridLayout(modeWidget);
  modeWidget->setLayout(modeLayout);

  // Using a pointer to persist beyond function scope
  auto modeButtons =
      std::make_shared<std::map<short, QPushButton *>>(std::map<short, QPushButton *>{
          {Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::USER,
           new QPushButton("User")},
          {Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::STANDARD,
           new QPushButton("Standard")},
          {Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::ECO,
           new QPushButton("Eco")},
          {Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::GRAPHICS,
           new QPushButton("Graphics")},
          {Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::GAME_ACTION,
           new QPushButton("G-Action")},
          {Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::GAME_RACING,
           new QPushButton("G-Racing")},
          {Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::GAME_SPORTS,
           new QPushButton("G-Sports")},
          {Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::HDR,
           new QPushButton("HDR")}});

  // Disable current mode button
  (*modeButtons)[currentMode]->setEnabled(false);

  // Changing values in ECO / STD / Graphics will switch to the USER mode
  brightnessWidget->setEnabled(
      currentMode == Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::USER ||
      currentMode >= Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::GAME_ACTION);
  contrastWidget->setEnabled(
      currentMode == Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::USER ||
      currentMode >= Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::GAME_ACTION);

  auto changeMode = [brightnessWidget, contrastWidget, modeButtons, &currentMode](short newMode) {
    for (auto &kv : *modeButtons)
      kv.second->setEnabled(false);

    setVCPValueAsync(
        QString::number(Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::Code::MODE, 16),
        newMode,
        [modeButtons, brightnessWidget, contrastWidget, &currentMode,
         newMode](int exitCode) mutable {
          QTimer::singleShot(0, [modeButtons, brightnessWidget, contrastWidget, &currentMode,
                                 newMode, exitCode]() {
            if (exitCode != 0) {
              qDebug() << "Failed to change the mode:" << exitCode;
            } else {
              qDebug() << "Mode changed successfully!";

              brightnessWidget->setEnabled(
                  newMode ==
                      Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::USER ||
                  newMode >=
                      Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::GAME_ACTION);
              contrastWidget->setEnabled(
                  newMode ==
                      Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::USER ||
                  newMode >=
                      Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::GAME_ACTION);

              currentMode = newMode;
            }

            for (auto &kv : *modeButtons) {
              kv.second->setEnabled(kv.first != currentMode);
            }
          });
        });
  };

  short i = 0, cols = 4;
  for (auto &[mode, button] : *modeButtons) {
    modeLayout->addWidget(button, i / cols, i % cols);
    i++;
    QObject::connect(button, &QPushButton::clicked, [mode, changeMode]() { changeMode(mode); });
  }

  return modeWidget;
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QString programName = "display-vcp";
  QString displayName = "Display VCP";
  QString programVersion = "0.1";
  QString programDescription = "Virtual Control Panel app to control display features "
                               "like brightness, contrast, etc. available on KDE system tray";

  // Application metadata
  KAboutData aboutData(programName, displayName, programVersion, programDescription,
                       KAboutLicense::Unknown);
  KAboutData::setApplicationData(aboutData);

  // Create a status notifier item (system tray icon)
  KStatusNotifierItem *trayIcon = new KStatusNotifierItem();
  trayIcon->setTitle(displayName);
  trayIcon->setToolTipTitle(displayName);
  trayIcon->setCategory(KStatusNotifierItem::Hardware);

  // Use a KDE icon name or path to your icon. See /usr/share/icons/
  trayIcon->setIconByName("monitor");
  trayIcon->setIconByName("video-display");
  trayIcon->setIconByName("video-display-symbolic");
  trayIcon->setIconByName("video-display-brightness");

  // Disable the default actions (including the default Quit action)
  trayIcon->setStandardActionsEnabled(false);

  short currentBrightness =
      getVCPValue(QString::number(Constants::MCCS::VCPCode::std::BRIGHTNESS, 16));
  if (currentBrightness == -1) {
    qDebug() << "Failed to get the current brightness!";
    currentBrightness = Constants::Display::Brightness::DEFAULT;
  }

  short currentContrast = getVCPValue(QString::number(Constants::MCCS::VCPCode::std::CONTRAST, 16));
  if (currentContrast == -1) {
    qDebug() << "Failed to get the current contrast!";
    currentContrast = Constants::Display::Contrast::DEFAULT;
  }

  short currentMode = getVCPValue(
      QString::number(Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::Code::MODE, 16));

  if (currentMode == -1) {
    qDebug() << "Failed to get the current mode!";
    currentMode = Constants::MCCS::VCPCode::Manufacturer::AcerXV272UV3::ModeValue::USER;
  }

  trayIcon->setContextMenu(createContextMenu(currentBrightness, currentContrast, app));

#pragma region Main control UI

  CustomWidget *mainWidget = new CustomWidget();
  mainWidget->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
  mainWidget->setFont(QFont("Monospace"));

  // Create a layout for the main widget
  QVBoxLayout *frameLayout = new QVBoxLayout(mainWidget);
  frameLayout->setContentsMargins(0, 0, 0, 0);
  mainWidget->setLayout(frameLayout);

  // Create a frame to contain the main layout
  QFrame *mainFrame = new QFrame(mainWidget);
  mainFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
  mainFrame->setLineWidth(2);
  frameLayout->addWidget(mainFrame);

  // Create the main layout inside the frame
  QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
  mainLayout->setContentsMargins(20, 20, 20, 20);
  mainFrame->setLayout(mainLayout);

  QLabel *titleLabel = new QLabel(displayName + " v" + programVersion);

  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(14);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);

  mainLayout->addWidget(titleLabel);

  QWidget *brightnessWidget = createContinuousPropertyWidget(
      CURRENT_BRIGHTNESS_TEXT, currentBrightness, Constants::Display::Brightness::STEP,
      {Constants::Display::CONTINUOUS_FEATURE_MIN, Constants::Display::Brightness::MAX},
      QString::number(Constants::MCCS::VCPCode::std::BRIGHTNESS, 16));
  QWidget *contrastWidget = createContinuousPropertyWidget(
      CURRENT_CONTRAST_TEXT, currentContrast, Constants::Display::Contrast::STEP,
      {Constants::Display::CONTINUOUS_FEATURE_MIN, Constants::Display::Contrast::MAX},
      QString::number(Constants::MCCS::VCPCode::std::CONTRAST, 16));
  mainLayout->addWidget(brightnessWidget);
  mainLayout->addWidget(contrastWidget);

  QWidget *modeWidget = createModeWidget(currentMode, brightnessWidget, contrastWidget);

  mainLayout->addWidget(modeWidget);

  // Okay button to dismiss the widget menu
  QWidget *okayWidget = new QWidget();
  mainLayout->addWidget(okayWidget);
  QHBoxLayout *okayLayout = new QHBoxLayout(okayWidget);
  okayWidget->setLayout(okayLayout);
  QPushButton *okayButton = new QPushButton("Okay");
  okayLayout->addWidget(okayButton);
  okayLayout->addStretch(); // flex space
  QObject::connect(okayButton, &QPushButton::clicked, [mainWidget]() { mainWidget->close(); });

#pragma endregion

  // Left-click on the tray icon
  // 1. Show main control UI
  QObject::connect(trayIcon, &KStatusNotifierItem::activateRequested, [mainWidget]() {
    if (mainWidget->isVisible()) {
      mainWidget->close();
      return;
    }

    mainWidget->show();
    mainWidget->move(QCursor::pos());

    // // Calculate the position to show the widget near the system tray icon
    // QRect screenGeometry = QGuiApplication::screenAt(QCursor::pos())->geometry();
    // QPoint cursorPos = QCursor::pos();
    // int x = cursorPos.x(), y = cursorPos.y();
    // // Adjust the position to ensure the widget is fully visible on the screen
    // if (x + mainWidget->width() > screenGeometry.right())
    //   x = screenGeometry.right() - mainWidget->width();
    // if (y + mainWidget->height() > screenGeometry.bottom())
    //   y = screenGeometry.bottom() - mainWidget->height();
    // mainWidget->move(x, y);

    mainWidget->setFocus(); // Ensure it has focus when shown
    mainWidget->activateWindow();
  });
  // 2. Show the context menu
  // QObject::connect(trayIcon, &KStatusNotifierItem::activateRequested, [trayIcon]() {
  //   // Show the context menu at the cursor position
  //   trayIcon->contextMenu()->popup(QCursor::pos());
  // });

  // Show the tray icon
  trayIcon->setStatus(KStatusNotifierItem::Active);

  return app.exec();
}

// meta object compiler
#include "main.moc"