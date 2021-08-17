#include "inputsumwidget.h"

static InputSumWidget* m_instance = nullptr;

void InputSumWidget::setUpWidgets()
{
    m_layout->addWidget(m_path,0,0);
    m_layout->addWidget(m_inputPath,0,1);
    m_layout->addWidget(m_format,1,0);
    m_layout->addWidget(m_inputFormat,1,1);
    m_layout->addWidget(m_operation,2,0);
    m_layout->addWidget(m_inputOperation,2,1);
    m_layout->addWidget(m_connectToTheNextStage, 3,0,1,2);
    m_layout->addWidget(m_submit, 4,0,1,2);

    m_inputPath->setText("/home/semen/qtProjects/test/");
    m_inputFormat->setText("pdf");
    m_inputOperation->setText("ctrlSum");


    setLayout(m_layout);
}



void InputSumWidget::callback(const std::string& path, const std::string& format, const std::string& operation)
{

    std::string pathLib(m_pathForScan + QDir::separator().toLatin1() + "libexecutionSumWidget.so");
    m_lib = dlopen(pathLib.c_str(), RTLD_NOW);

    if (m_lib == nullptr)
    {
        throw std::runtime_error(dlerror());
        return;
    }
    dlerror();

    typedef void *(*GetInputWidget)(std::string);
    GetInputWidget getInputWidget = nullptr;

    getInputWidget = reinterpret_cast<GetInputWidget>(dlsym(m_lib, "getWidgetInstance")); // приводим к указателю на фукцнию
    if (getInputWidget == nullptr)
    {
        throw std::runtime_error(dlerror());

        return;
    }
    dlerror();

    executionWidget = reinterpret_cast<AbstractExecutionWidget *>(getInputWidget(m_pathForScan));
    if (executionWidget == nullptr)
    {
        throw std::runtime_error("Не удалось открыть библиотеку");
        return;
    }
    qDebug() << "Pointer of executionWidget in InputSumWidget::callback" << executionWidget;
    executionWidget->setData(path, format, operation);
    executionWidget->show();
    executionWidget->startExecution();
}

void InputSumWidget::releaseExecutionWidgetInstance(AbstractExecutionWidget *instance)
{
    typedef void (*ReleaseInputWidget)(QWidget* );
    ReleaseInputWidget releaseInputWidget = nullptr;

    releaseInputWidget = reinterpret_cast<ReleaseInputWidget>(dlsym(m_lib, "releaseWidgetInstance"));
    if (releaseInputWidget == nullptr)
    {
        throw std::runtime_error(dlerror());
        return;
    }
    dlerror();

    releaseInputWidget(instance);
}



InputSumWidget::InputSumWidget(QWidget *parent, std::string pathForScan)
    : AbstractInputWidget(parent), m_pathForScan(pathForScan)
{
    memoryAllocation();
    setUpWidgets();
    setMinimumWidth(380);

    connect(m_submit,&QPushButton::clicked,
            this    ,&InputSumWidget::submitPressed);

}



InputSumWidget::~InputSumWidget()
{
    if (executionWidget != nullptr)
        releaseExecutionWidgetInstance(executionWidget);
    if (m_lib != nullptr)
        dlclose(m_lib);
    m_lib = nullptr;
    executionWidget = nullptr;

    qDebug() << "InputSumWidget destructor";
}


void InputSumWidget::submitPressed()
{
    try {
        if (m_connectToTheNextStage->isChecked())
        {
            callback(m_inputPath->text().toStdString(),m_inputFormat->text().toStdString(), m_inputOperation->text().toStdString());
            this->close();
        }
        else
        {
            QMessageBox::warning(this, "Error", "Please, checked the connection to the next stage");
        }

    }  catch (...) {
        handle_eptr(std::current_exception());
        std::cout << "EXCEPTION!!! in InputSumWidget::submitPressed()" << std::endl;
    }
}


void InputSumWidget::memoryAllocation()
{
     m_path           = new QLabel("Input path ",this);
     m_format         = new QLabel("Input format ", this);
     m_operation      = new QLabel("Input operation", this);
     m_inputPath      = new QLineEdit(this);
     m_inputFormat    = new QLineEdit(this);
     m_inputOperation = new QLineEdit(this);
     m_submit         = new QPushButton("Submit", this);
     m_connectToTheNextStage = new QCheckBox("Connect to the next stage", this);
     m_layout         = new QGridLayout(this);
}


void *getWidgetInstance(std::string pathForScan)
{
    if(m_instance == nullptr)
        m_instance = new InputSumWidget(nullptr, pathForScan);
    return m_instance;
}



void releaseWidgetInstance(QWidget* instance)
{
    instance = reinterpret_cast<InputSumWidget *>(instance);
    if(instance != nullptr)
        {
            delete instance;
            m_instance = nullptr;
        }
}

std::string *getInfo()
{
    return (new std::string("Это библиотека предназначена для отображения входного приложения, которое будет передавать введенные пользователем данные в обработку"));
}
