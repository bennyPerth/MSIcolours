#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
  {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:

    void on_cmbMode_currentTextChanged(const QString &arg1);
    void on_cmbFavourites_currentTextChanged(const QString &arg1);

    void on_cmbColourL_currentTextChanged(const QString &arg1);
    void on_cmbIntensityL_currentTextChanged(const QString &arg1);

    void on_cmbColourM_currentTextChanged(const QString &arg1);
    void on_cmbIntensityM_currentTextChanged(const QString &arg1);

    void on_cmbColourR_currentTextChanged(const QString &arg1);
    void on_cmbIntensityR_currentTextChanged(const QString &arg1);

    void on_btnApply_clicked();

    void on_btnExit_clicked();

    void runSystemCommand(QString);

    void on_btnAddToFavourites_clicked();

    void loadSettings();
    void saveSettings();


  private:
    Ui::MainWindow *ui;
  };
#endif // MAINWINDOW_H
