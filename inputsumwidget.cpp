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
    m_layout->addWidget(m_progress, 4,0,1,2);

    m_inputPath->setText("/home/semen/qtProjects/test/");
    m_inputFormat->setText("pdf");
    m_inputOperation->setText("ctrlSum");

    m_progress->hide();

    setLayout(m_layout);
}



void InputSumWidget::callback(const std::string& path, const std::string& format, const std::string& operation)
{
    void* lib;
    std::string pathLib(m_pathForScan + QDir::separator().toLatin1() + "libexecutionSumWidget.so");
    lib = dlopen(pathLib.c_str(), RTLD_NOW);

    if (lib == nullptr)
    {
        throw std::runtime_error(dlerror());
        return;
    }
    dlerror();

    typedef void *(*GetInputWidget)(std::string);
    GetInputWidget getInputWidget = nullptr;

    getInputWidget = reinterpret_cast<GetInputWidget>(dlsym(lib, "getWidgetInstance")); // приводим к указателю на фукцнию
    if (getInputWidget == nullptr)
    {
        throw std::runtime_error(dlerror());
//        qDebug() << "Cannot load create function: " << dlerror() << '\n';
        return;
    }
    dlerror();

    executionWidget = reinterpret_cast<AbstractExecutionWidget *>(getInputWidget(m_pathForScan));
    if (executionWidget == nullptr)
    {
        throw std::runtime_error("Не удалось открыть библиотеку");
        return;
    }
    executionWidget->setData(path, format, operation);
    executionWidget->show();
    executionWidget->startExecution();
    dlclose(lib);
}


void InputSumWidget::setUpProgress()
{
    m_submit->hide();

    m_progress->show();
    m_progress->setMaximum(0);
    m_progress->setMaximum(100);

    m_timer->start(50);
}



InputSumWidget::InputSumWidget(QWidget *parent, std::string pathForScan)
    : AbstractInputWidget(parent), m_pathForScan(pathForScan)
{
    memoryAllocation();
    setUpWidgets();
    setMinimumWidth(380);

    connect(m_submit,&QPushButton::clicked,
            this    ,&InputSumWidget::submitPressed);

    connect(m_timer, &QTimer::timeout      ,
            this   , &InputSumWidget::updateProgress);
}



InputSumWidget::~InputSumWidget()
{
    if (executionWidget != nullptr)
    {
        executionWidget->close();
        delete executionWidget;
        executionWidget = nullptr;
    }
//    if (m_instance != nullptr)

    qDebug() << "InputSumWidget destructor";
//    executionWidget->setParent(this);
}


void InputSumWidget::submitPressed()
{
    try {
//        std::cout << "Main thread id: " << std::this_thread::get_id() << std::endl;


//        std::thread th(&ReadDirectory::checkInputData, &m_readDirectory, m_inputPath->text().toStdString(), m_inputFormat->text().toStdString(), m_inputOperation->text().toStdString());

//        th.join();
//        m_outputWidget = new OutputWidget(m_readDirectory.resultOfMd5Sum(), m_readDirectory.countOfFiles(), m_inputPath->text().toStdString(),
//                                          m_readDirectory.otherFiles());
        if (m_connectToTheNextStage->isChecked())
        {
//            setUpProgress();
            callback(m_inputPath->text().toStdString(),m_inputFormat->text().toStdString(), m_inputOperation->text().toStdString());
            this->close();
        }
        else
        {
            QMessageBox::warning(this, "Error", "Please, checked the connection to the next stage");
        }

    }  catch (...) {
        handle_eptr(std::current_exception());
        std::cout << "EXCEPTION!!!" << std::endl;
    }
}

void InputSumWidget::updateProgress()
{
    m_progress->setValue(m_progress->value() + 1);
    if (m_progress->value() == m_progress->maximum())
    {
//        m_outputWidget->show();

        m_timer->stop();

        m_progress->setValue(0);
        m_progress->hide();
        m_submit->show();
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
     m_progress       = new QProgressBar(this);
     m_timer          = new QTimer(this);
     m_layout         = new QGridLayout(this);
}


void *getWidgetInstance(std::string pathForScan)
{
    if(!m_instance)
        m_instance = new (std::nothrow) InputSumWidget(nullptr, pathForScan);
    return m_instance;
}



void releaseWidgetInstance(QWidget* instance)
{
    instance = reinterpret_cast<InputSumWidget *>(instance);
    if(instance != nullptr)
        {
            delete instance;
            instance = nullptr;
        }
}

std::string *getInfo()
{
    return (new std::string("Это библиотека предназначена для отображения входного приложения, которое будет передавать введенные пользователем данные в обработку"));
}
