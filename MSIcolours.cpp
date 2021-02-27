me#include "MSIcolours.h"
#include "ui_MSIcolours.h"

#include <QSettings>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QInputDialog>


// purpose of this app is to let user use a GUI to generate the msi-keyboard command

QString version = "a03";
QString appName = "MSIcolours";

// 27 February 2021 (a03)
// redesign of favourites logic (use key instead of MSI arguments)
// allow user to update the favourites settings

// 26 February 2021 (a02)
// use settings file for favourites

// 25 February 2021 (a01)
// added a favourites combo box
// select combo box values based on favourites when favourites is selected

// 23 February 2021 (a00)
// initial code


QStringList homeLocation = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
QString homePath = homeLocation[0] + "/";
QString homeMSIcoloursFolder = homePath + appName + "/";
QString homeFile = homeMSIcoloursFolder + appName +".ini";
QSettings settings(homeFile, QSettings::IniFormat);

QString binScript = homePath + "bin/changeColour";

QString currentMode = "normal";
QString currentColourL = "";
QString currentColourM = "";
QString currentColourR = "";
QString currentIntensityL = "";
QString currentIntensityM = "";
QString currentIntensityR = "";
QString currentFavourite = "";
QString lastUsed;
QString colourBits;


QMap<QString, QString> favouritesMap;

QList <QString> favouritesList;

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->lblStatus->setText("");
  QString windowTitle = appName + " (version "+version+")" + " Qt "+QT_VERSION_STR;
  setWindowTitle(windowTitle);

  loadSettings();
}

MainWindow::~MainWindow() {
  delete ui;
}


void MainWindow::on_cmbFavourites_currentTextChanged(const QString &favouritesKey) {
  QString favouritesData = favouritesMap[favouritesKey];
//  qDebug() << "favouritesData:" << favouritesData;
  QString localArg = favouritesData;
  currentFavourite = favouritesData;
  if (localArg.length() > 1) {
    runSystemCommand("msi-keyboard -m " + currentMode + " " +localArg);
    localArg = localArg.replace("-c ","").simplified();
    localArg = localArg.replace(" ", ",");
    // localArg is now similar to "left,red,low,middle,white,medium,right,blue,high"
    QStringList msiString1 = localArg.split(",");
    if (msiString1.length() >8) {
      QString leftColour = msiString1[1];
      QString leftIntensity = msiString1[2];
      QString middleColour = msiString1[4];
      QString middleIntensity = msiString1[5];
      QString rightColour = msiString1[7];
      QString rightIntensity = msiString1[8];

      // populate combo boxes with the selected favourite
      ui->cmbColourL->setCurrentText(leftColour);
      ui->cmbColourM->setCurrentText(middleColour);
      ui->cmbColourR->setCurrentText(rightColour);

      ui->cmbIntensityL->setCurrentText(leftIntensity);
      ui->cmbIntensityM->setCurrentText(middleIntensity);
      ui->cmbIntensityR->setCurrentText(rightIntensity);
      lastUsed = favouritesKey;
//      qDebug() << "on_cmbFavourites_currentTextChanged: lastUsed" << lastUsed;
      }
    else
      {qDebug() << "ignoring combobox updates as favourites string is not correct.";}
    }
}  // end of on_cmbFavourites_currentTextChanged

void MainWindow::on_cmbMode_currentTextChanged(const QString &arg1) {
  currentMode = arg1;
}

void MainWindow::on_cmbColourL_currentTextChanged(const QString &arg1) {
  currentColourL = arg1;
}
void MainWindow::on_cmbIntensityL_currentTextChanged(const QString &arg1) {
  currentIntensityL = arg1;
}

void MainWindow::on_cmbColourM_currentTextChanged(const QString &arg1) {
  currentColourM = arg1;
}
void MainWindow::on_cmbIntensityM_currentTextChanged(const QString &arg1) {
  currentIntensityM = arg1;
}

void MainWindow::on_cmbColourR_currentTextChanged(const QString &arg1) {
  currentColourR = arg1;
}
void MainWindow::on_cmbIntensityR_currentTextChanged(const QString &arg1) {
  currentIntensityR = arg1;
}


void MainWindow::on_btnApply_clicked() {
  // set colour <mode>:
  //  normal, gaming, breathe, demo, wave

  QString keyboardCommand = "msi-keyboard -m ";
  keyboardCommand += currentMode;
  colourBits = " -c left,";
  colourBits += currentColourL + ",";
  colourBits += currentIntensityL;

  colourBits += " -c middle,";
  colourBits += currentColourM + ",";
  colourBits += currentIntensityM;

  colourBits += " -c right,";
  colourBits += currentColourR + ",";
  colourBits += currentIntensityR;

  runSystemCommand(keyboardCommand+colourBits);
  currentFavourite = colourBits;
}

void MainWindow::on_btnExit_clicked() {
  saveSettings();

  if (!QFile::exists(binScript))
    {qDebug() << "script file not found!"; return;}

  // update the home/user/bin/changeColour script so it has the latest keyboard colours
  // the script is run after a suspend is resumed
  // the following file runs the changeColour script /usr/lib/systemd/system-sleep/resumeKeyboardColour

  QByteArray theWholeFile;
  QFile scriptFile(binScript);
  if (scriptFile.open(QIODevice::ReadOnly)) {
    theWholeFile = scriptFile.readAll();
    scriptFile.close();
    }  // end of if


  QString lastCommand = "msi-keyboard -m " + currentMode + " " + favouritesMap[lastUsed];
  QByteArray lastUsedArray  = lastCommand.toUtf8();
  int lastIndex = theWholeFile.lastIndexOf("msi-keyboard ");
  int endOfLineIndex = theWholeFile.indexOf("\n", lastIndex);
  theWholeFile.replace(lastIndex, endOfLineIndex-lastIndex, QByteArray(lastUsedArray) );

  if(scriptFile.open(QIODevice::WriteOnly)) {
    scriptFile.write(theWholeFile);
    scriptFile.close();
    }

}  // end of on_btnExit_clicked


void MainWindow::runSystemCommand(QString systemCommand) {
  QByteArray byteArray;
  const char* cString;
  byteArray = systemCommand.toUtf8();
  cString = byteArray.constData();

//  qDebug() << "systemCommand:" <<  cString;
  int returnCode =  system(cString);
  if (returnCode) {
    qDebug() << "returnCode:" << returnCode;
    }
}

void MainWindow::loadSettings() {
  QDir thisCurrentPath = QDir::currentPath();
  QString thisPath = thisCurrentPath.path();
  QString executionFile = thisPath + "/MSIcolours.ini";
  if (!QFile::exists(homeFile))
    {qDebug() << "no ini file!";
      QFile::copy(executionFile, homeFile);
      }
  // load the combo boxes
  settings.beginGroup("Misc");
  QStringList modeList = settings.value("modes", "normal").toStringList();
  ui->cmbMode->clear();
  foreach(QString mode, modeList) {
    ui->cmbMode->addItem(mode);
    }

  QStringList coloursList = settings.value("colours", "off").toStringList();
  ui->cmbColourL->clear();
  ui->cmbColourM->clear();
  ui->cmbColourR->clear();
  foreach(QString colour, coloursList) {
    ui->cmbColourL->addItem(colour);
    ui->cmbColourM->addItem(colour);
    ui->cmbColourR->addItem(colour);
    }

  QStringList intensitiesList = settings.value("intensities", "medium").toStringList();
  ui->cmbIntensityL->clear();
  ui->cmbIntensityM->clear();
  ui->cmbIntensityR->clear();
  foreach(QString intensity, intensitiesList) {
    ui->cmbIntensityL->addItem(intensity);
    ui->cmbIntensityM->addItem(intensity);
    ui->cmbIntensityR->addItem(intensity);
    }
  settings.endGroup();

  settings.beginGroup("favourites");
  QStringList keys = settings.allKeys();
  ui->cmbFavourites->clear();
  favouritesMap.clear();
  int rowNumber = 0;
  foreach(QString key, keys) {
    QStringList items = settings.value(key, "").toStringList();
    QString longItem = "-c ";
    rowNumber = 0;
    foreach(QString item, items) {
      rowNumber++;
      longItem += item;
      if (rowNumber%3 != 0) {
        longItem += ",";
        } else {
          if (rowNumber != 9) // don't add '-c' at end of string
            {longItem += " -c ";}
        }
      if (key == "lastUsed")
        {lastUsed = item;}
      }  // end of a key

    if (key != "lastUsed") {
      ui->cmbFavourites->insertItem(rowNumber, key, longItem);
      favouritesMap[key] = longItem;
      }
    }  // end of favourite keys
  ui->cmbFavourites->setCurrentText(lastUsed);
  on_cmbFavourites_currentTextChanged(lastUsed);

  settings.endGroup();

}  // end of loadSettings

// only save favourites
void MainWindow::saveSettings() {
  settings.beginGroup("favourites");
  settings.setValue("lastUsed", lastUsed);
  settings.endGroup();
}  // end of saveSettings

void MainWindow::on_btnAddToFavourites_clicked() {
  if (colourBits.length() < 1) {
      ui->lblStatus->setText("must do an apply before ading a favourite key");
      return;}

  settings.beginGroup("favourites");
  // get a key name from user
  bool ok;
  // Ask for a new favourites key
  QString newFavouriteKey = QInputDialog::getText(0, "Add a new favourite",
                 "please enter a key name", QLineEdit::Normal, "", &ok);
  if (ok && !newFavouriteKey.isEmpty())
    {qDebug() << newFavouriteKey;}
  else
    {  ui->lblStatus->setText(newFavouriteKey + "key name is invalid"); return;}

  // create the stringlist based on the current favourite
  colourBits = colourBits.replace("-c","");
  colourBits = colourBits.replace("-c ","").simplified();
  colourBits = colourBits.replace(" ", ",");
  QStringList tempStringList;
  tempStringList = colourBits.split(",");
  settings.setValue(newFavouriteKey, tempStringList);
  settings.endGroup();
  ui->lblStatus->setText(newFavouriteKey + " added to favourites");
  lastUsed = newFavouriteKey;
}




