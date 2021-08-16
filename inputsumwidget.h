#ifndef INPUTSUMWIDGET_H
#define INPUTSUMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QGridLayout>
#include <QTimer>
#include <QCheckBox>

#include <QMessageBox>
#include <QDebug>
//#include <QApplication>
#include <QDir>

#include <iostream>
#include <thread>
#include <dlfcn.h>

#include "/home/semen/qtProjects/QtProjects/AbstractClasses/abstractclasses.h"

class InputSumWidget : public AbstractInputWidget
{
    Q_OBJECT

public:
    InputSumWidget(QWidget *parent = nullptr, std::string pathForScan = "");
    ~InputSumWidget();

public slots:
    void submitPressed() override;
//    void updateProgress() override;

private:
    QLabel * m_path;
    QLabel * m_format;
    QLabel * m_operation;

    QLineEdit * m_inputPath;
    QLineEdit * m_inputFormat;
    QLineEdit * m_inputOperation;

    QPushButton  * m_submit;
    QCheckBox    * m_connectToTheNextStage;
//    QProgressBar * m_progress;
//    QTimer       * m_timer;
    QGridLayout  * m_layout;

    std::string m_pathForScan;

    void* m_lib = nullptr;


private:
    void memoryAllocation() override;
//    void setUpProgress();
    void setUpWidgets() override;
    void callback(const std::string& path, const std::string& format, const std::string& operation) override;
    void releaseExecutionWidgetInstance(AbstractExecutionWidget *instance);

};

extern "C" void *getWidgetInstance(std::string pathForScan = "");
extern "C" void  releaseWidgetInstance(QWidget* instance);
extern "C" std::string *getInfo();

#endif // INPUTSUMWIDGET_H
