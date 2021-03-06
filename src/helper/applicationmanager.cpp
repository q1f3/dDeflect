#include "applicationmanager.h"
//#include "DSourceCodeParser/dsourcecodeparser.h"

// #include <ApplicationManager/DLogger/dlogger.h>
#include <core/file_types/elffile.h>
#include <QUrl>
#include <QtWidgets/QMessageBox>
/*
ApplicationManager::ApplicationManager(QObject *parent) :
    QObject(parent), jsonParser(), sourceParser(), m_targetPath("Choose a C++ source file or an executive file.")
{
    methodsCount = 0;
    // Metody 32 bitowe
    jsonParser.setPath(DSettings::getSettings().getDescriptionsPath<Registers_x86>());
    LOG_MSG(DSettings::getSettings().getDescriptionsPath<Registers_x86>());
    // qDebug()<<DSettings::getSettings().getDescriptionsPath<Registers_x86>();
    QDir wrappers(DSettings::getSettings().getDescriptionsPath<Registers_x86>());
    Q_ASSERT(wrappers.exists());
    QDirIterator it(DSettings::getSettings().getDescriptionsPath<Registers_x86>(), QStringList() << "*.json", QDir::Files);

    QFileInfoList files = wrappers.entryInfoList();
    foreach(const QFileInfo &fi, files) {
        QString Path = fi.absoluteFilePath();
        if(fi.completeSuffix()=="json"){
            Wrapper<Registers_x86> *w = jsonParser.loadInjectDescription<Registers_x86>(fi.fileName());
            if(w!=NULL){
                m_x86methodsList.append(w);
                Method *m = new Method(w);
                if(w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method)
                    m_methodsx86.append(m);
                if(w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler)
                    m_handlersx86.append(m);
            }
            else
                // qDebug() << "JSON read failed" << fi.fileName().toStdString().c_str();
                LOG_ERROR(QString("JSON read failed %1").arg(fi.fileName().toStdString().c_str()));
        }
    }
    // Metody 64 bitowe
    jsonParser.setPath(DSettings::getSettings().getDescriptionsPath<Registers_x64>());
    // qDebug()<<DSettings::getSettings().getDescriptionsPath<Registers_x64>();
    LOG_MSG(DSettings::getSettings().getDescriptionsPath<Registers_x64>());
    QDir wrappers64(DSettings::getSettings().getDescriptionsPath<Registers_x64>());
    Q_ASSERT(wrappers64.exists());
    QDirIterator it64(DSettings::getSettings().getDescriptionsPath<Registers_x64>(), QStringList() << "*.json", QDir::Files);

    QFileInfoList files64 = wrappers64.entryInfoList();
    foreach(const QFileInfo &fi, files64) {
        QString Path = fi.absoluteFilePath();
        if(fi.completeSuffix()=="json"){
            Wrapper<Registers_x64> *w = jsonParser.loadInjectDescription<Registers_x64>(fi.fileName());
            if(w!=NULL){
                m_x64methodsList.append(w);
                Method *m = new Method(w);
                if(w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method)
                    m_methodsx64.append(m);
                if(w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler)
                    m_handlersx64.append(m);
            }
        }
    }
    // TODO: Lista do uzupełnienia o wszystkie rozszerzenia, albo stworzyć plik ze stringami i innymi danymi
    sourceExtensionList<<"cpp"<<"cxx"<<"c";
    setState(IDLE);
    setArchType(X86);

    // load source methods
    QString dsc_src_path = DSettings::getSettings().getDescriptionsSourcePath();

    jsonParser.setPath(dsc_src_path);
    QDir source_dsc(dsc_src_path);
    Q_ASSERT(source_dsc.exists());
    QFileInfoList files_dsc_src = source_dsc.entryInfoList();

    SourceCodeDescription scd;
    SourceCodeDescription *pscd;
    foreach(const QFileInfo &fi, files_dsc_src) {
        if (fi.completeSuffix() == QString("json")) {
            if (!jsonParser.loadSourceCodeDescription(fi.fileName(), scd))
                continue;

            pscd = new(std::nothrow) SourceCodeDescription;
            if (!pscd)
                continue;
            scd.copy(pscd);

            m_sourceMethods.push_back(pscd);
        }
    }

    connect(this,SIGNAL(archTypeChanged()),this,SLOT(updateCurrMethods()));
    connect(this,SIGNAL(currCmChanged()),this,SLOT(updateCurrMethods()));
    connect(this,SIGNAL(sysChanged()),this,SLOT(updateCurrMethods()));

    connect(this,SIGNAL(archTypeChanged()),this,SLOT(updateCurrHandlers()));
    connect(this,SIGNAL(currCmChanged()),this,SLOT(updateCurrHandlers()));
    connect(this,SIGNAL(sysChanged()),this,SLOT(updateCurrHandlers()));

    connect(this, SIGNAL(sysChanged()), this, SLOT(updateCurrSourceMethods()));
}

ApplicationManager::~ApplicationManager()
{
    delete sourceParser;
}

void ApplicationManager::setState(ApplicationManager::State state)
{
    m_state = state;
    emit stateChanged(state);
}

void ApplicationManager::setArchType(ApplicationManager::Arch t)
{
    m_archType = t;
    emit archTypeChanged();
}

void ApplicationManager::setCurrCm(ApplicationManager::CallingMethod cm)
{
    m_currCm = cm;
    emit currCmChanged();
}

void ApplicationManager::setSys(ApplicationManager::System s)
{
    m_sys = s;
    emit sysChanged();
}

ApplicationManager::State ApplicationManager::state() const
{
    return m_state;
}

ApplicationManager::Arch ApplicationManager::archType() const
{
    return m_archType;
}

ApplicationManager::CallingMethod ApplicationManager::currCm() const
{
    return m_currCm;
}

ApplicationManager::System ApplicationManager::sys() const
{
    return m_sys;
}

QQmlListProperty<Method> ApplicationManager::x64methods()
{
    return QQmlListProperty<Method>(this,m_methodsx64);
}

QQmlListProperty<Method> ApplicationManager::x86handlers()
{
    return QQmlListProperty<Method>(this,m_handlersx86);
}

QQmlListProperty<Method> ApplicationManager::x64handlers()
{
    return QQmlListProperty<Method>(this,m_handlersx64);
}

QQmlListProperty<Method> ApplicationManager::currMethods()
{
    return QQmlListProperty<Method>(this,m_currMethods);
}

QQmlListProperty<Method> ApplicationManager::currHandlers()
{
    return QQmlListProperty<Method>(this,m_currHandlers);
}

QQmlListProperty<SourceCodeDescription> ApplicationManager::currSourceMethods()
{
    return QQmlListProperty<SourceCodeDescription>(this,m_currSourceMethods);
}

QVariantList ApplicationManager::x86MethodsNames()
{
    return m_x86MethodsNames;
}

void ApplicationManager::fileOpened(QString path)
{
    m_targetPath = QUrl(path).toLocalFile();
    //emit targetPathChanged()
    setState(getFileType(m_targetPath));
    //setState(ApplicationManager::PE);
}

void ApplicationManager::applyClicked()
{
    //insertMethods();
}

void ApplicationManager::saveClicked()
{
    if(archType()==X86)
        methodsCount = x86methodsToInsert.count();
    else
        methodsCount = x64methodsToInsert.count();
}

void ApplicationManager::secureClicked()
{
    QString path = m_targetPath;

    if(m_sys == Windows)
    {
        if(m_archType == ApplicationManager::X86)
        {
            if(!x86threadWrappersToInject.empty()) {
                DAddingMethods<Registers_x86>::InjectDescription *th = new DAddingMethods<Registers_x86>::InjectDescription();
                th->cm = DAddingMethods<Registers_x86>::CallingMethod::OEP;
                foreach(Wrapper<Registers_x86> * w, m_x86methodsList) {
                    if(w->wrapper_type == Wrapper<Registers_x86>::WrapperType::ThreadWrapper
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows) {
                        th->adding_method = w;
                        break;
                    }
                }

                ThreadWrapper<Registers_x86> *tw = dynamic_cast<ThreadWrapper<Registers_x86>*>(th->adding_method);

                foreach(Wrapper<Registers_x86>* wrp, x86threadWrappersToInject) {
                    tw->thread_actions.append(wrp);
                }

                x86methodsToInsert.append(th);
            }
        }
        else
        {
            if(!x64threadWrappersToInject.empty()) {
                DAddingMethods<Registers_x64>::InjectDescription *th = new DAddingMethods<Registers_x64>::InjectDescription();
                th->cm = DAddingMethods<Registers_x64>::CallingMethod::OEP;
                foreach(Wrapper<Registers_x64> * w, m_x64methodsList) {
                    if(w->wrapper_type == Wrapper<Registers_x64>::WrapperType::ThreadWrapper
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows) {
                        th->adding_method = w;
                        break;
                    }
                }

                ThreadWrapper<Registers_x64> *tw = dynamic_cast<ThreadWrapper<Registers_x64>*>(th->adding_method);

                foreach(Wrapper<Registers_x64>* wrp, x64threadWrappersToInject) {
                    tw->thread_actions.append(wrp);
                }

                x64methodsToInsert.append(th);
            }
        }
    }
    else
    {
        if(m_archType == ApplicationManager::X86)
        {
            foreach(DAddingMethods<Registers_x86>::InjectDescription *id, x86methodsToInsert) {
                switch(id->cm) {
                case DAddingMethods<Registers_x86>::CallingMethod::OEP:
                    foreach(Wrapper<Registers_x86> * w, m_x86methodsList) {
                        if(w->wrapper_type == Wrapper<Registers_x86>::WrapperType::OepWrapper &&
                                w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux) {
                            OEPWrapper<Registers_x86> *wrp = dynamic_cast<OEPWrapper<Registers_x86>*>(Wrapper<Registers_x86>::copy(w));
                            wrp->oep_action = id->adding_method;
                            wrp->detect_handler = id->adding_method->detect_handler;
                            id->adding_method->detect_handler = nullptr;
                            id->adding_method = wrp;
                            break;
                        }
                    }

                    break;

                case DAddingMethods<Registers_x86>::CallingMethod::Thread:
                    foreach(Wrapper<Registers_x86> * w, m_x86methodsList) {
                        if(w->wrapper_type == Wrapper<Registers_x86>::WrapperType::ThreadWrapper &&
                                w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux) {
                            ThreadWrapper<Registers_x86> *wrp = dynamic_cast<ThreadWrapper<Registers_x86>*>(Wrapper<Registers_x86>::copy(w));
                            wrp->thread_actions = { id->adding_method };
                            wrp->detect_handler = id->adding_method->detect_handler;
                            id->adding_method->detect_handler = nullptr;
                            id->adding_method = wrp;
                            break;
                        }
                    }
                    break;

                default: // tram
                    foreach(Wrapper<Registers_x86> * w, m_x86methodsList) {
                        if(w->wrapper_type == Wrapper<Registers_x86>::WrapperType::TrampolineWrapper &&
                                w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux) {
                            TrampolineWrapper<Registers_x86> *wrp = dynamic_cast<TrampolineWrapper<Registers_x86>*>(Wrapper<Registers_x86>::copy(w));
                            wrp->tramp_action = id->adding_method;
                            wrp->detect_handler = id->adding_method->detect_handler;
                            id->adding_method->detect_handler = nullptr;
                            id->adding_method = wrp;
                            break;
                        }
                    }
                    break;
                }
            }

            foreach(Wrapper<Registers_x86>* mth, x86threadWrappersToInject) {
                DAddingMethods<Registers_x86>::InjectDescription *th = new DAddingMethods<Registers_x86>::InjectDescription();
                th->cm = DAddingMethods<Registers_x86>::CallingMethod::Thread;
                foreach(Wrapper<Registers_x86> * w, m_x86methodsList) {
                    if(w->wrapper_type == Wrapper<Registers_x86>::WrapperType::ThreadWrapper
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux) {
                        ThreadWrapper<Registers_x86> *wrp = dynamic_cast<ThreadWrapper<Registers_x86>*>(Wrapper<Registers_x86>::copy(w));
                        wrp->detect_handler = mth->detect_handler;
                        mth->detect_handler = nullptr;
                        wrp->thread_actions = { mth };
                        th->adding_method = wrp;
                        break;
                    }
                }
                x86methodsToInsert.append(th);
            }
        }
        else
        {
            foreach(DAddingMethods<Registers_x64>::InjectDescription *id, x64methodsToInsert) {
                switch(id->cm) {
                case DAddingMethods<Registers_x64>::CallingMethod::OEP:
                    foreach(Wrapper<Registers_x64> * w, m_x64methodsList) {
                        if(w->wrapper_type == Wrapper<Registers_x64>::WrapperType::OepWrapper &&
                                w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux) {
                            OEPWrapper<Registers_x64> *wrp = dynamic_cast<OEPWrapper<Registers_x64>*>(Wrapper<Registers_x64>::copy(w));
                            wrp->oep_action = id->adding_method;
                            wrp->detect_handler = id->adding_method->detect_handler;
                            id->adding_method->detect_handler = nullptr;
                            id->adding_method = wrp;
                            break;
                        }
                    }

                    break;

                case DAddingMethods<Registers_x64>::CallingMethod::Thread:
                    foreach(Wrapper<Registers_x64> * w, m_x64methodsList) {
                        if(w->wrapper_type == Wrapper<Registers_x64>::WrapperType::ThreadWrapper &&
                                w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux) {
                            ThreadWrapper<Registers_x64> *wrp = dynamic_cast<ThreadWrapper<Registers_x64>*>(Wrapper<Registers_x64>::copy(w));
                            wrp->thread_actions = { id->adding_method };
                            wrp->detect_handler = id->adding_method->detect_handler;
                            id->adding_method->detect_handler = nullptr;
                            id->adding_method = wrp;
                            break;
                        }
                    }
                    break;

                default: // tram
                    foreach(Wrapper<Registers_x64> * w, m_x64methodsList) {
                        if(w->wrapper_type == Wrapper<Registers_x64>::WrapperType::TrampolineWrapper &&
                                w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux) {
                            TrampolineWrapper<Registers_x64> *wrp = dynamic_cast<TrampolineWrapper<Registers_x64>*>(Wrapper<Registers_x64>::copy(w));
                            wrp->tramp_action = id->adding_method;
                            wrp->detect_handler = id->adding_method->detect_handler;
                            id->adding_method->detect_handler = nullptr;
                            id->adding_method = wrp;
                            break;
                        }
                    }
                    break;
                }
            }

            foreach(Wrapper<Registers_x64>* mth, x64threadWrappersToInject) {
                DAddingMethods<Registers_x64>::InjectDescription *th = new DAddingMethods<Registers_x64>::InjectDescription();
                th->cm = DAddingMethods<Registers_x64>::CallingMethod::Thread;
                foreach(Wrapper<Registers_x64> * w, m_x64methodsList) {
                    if(w->wrapper_type == Wrapper<Registers_x64>::WrapperType::ThreadWrapper
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux) {
                        ThreadWrapper<Registers_x64> *wrp = dynamic_cast<ThreadWrapper<Registers_x64>*>(Wrapper<Registers_x64>::copy(w));
                        wrp->detect_handler = mth->detect_handler;
                        mth->detect_handler = nullptr;
                        wrp->thread_actions = { mth };
                        th->adding_method = wrp;
                        break;
                    }
                }
                x64methodsToInsert.append(th);
            }
        }
    }

    QFile f(path);
    if(!f.open(QFile::ReadOnly)) {
        QMessageBox::critical(nullptr, "Error", "Secure failed! Cannot open file.");
        return;
    }
    QByteArray data = f.readAll();
    f.close();

    BinaryFile *bin = nullptr;
    QFileInfo in(path);

    QString out_name = in.baseName() + QString("_secured");
    if(in.completeSuffix().length() > 0)
        out_name.append(".").append(in.completeSuffix());
    QString new_path = QFileInfo(in.absoluteDir(), out_name).absoluteFilePath();
    QFile out(new_path);

    switch(m_state)
    {
    case ApplicationManager::PE:
    {
        bin = new PEFile(data);
        if(!checkBinaryFile(*bin)) {
            QMessageBox::critical(nullptr, "Error", "Secure failed! Invalid file.");
            return;
        }

        if(bin->is_x86()) {
            PEAddingMethods<Registers_x86> am(dynamic_cast<PEFile*>(bin));
            if(!am.secure(x86methodsToInsert)) {
                QMessageBox::critical(nullptr, "Error", "Secure failed!");
                return;
            }
        }
        else {
            PEAddingMethods<Registers_x64> am(dynamic_cast<PEFile*>(bin));
            if(!am.secure(x64methodsToInsert)) {
                QMessageBox::critical(nullptr, "Error", "Secure failed!");
                return;
            }
        }

        break;
    }

    case ApplicationManager::ELF:
    {
        bin = new ::ELF(data);
        if(!checkBinaryFile(*bin)) {
            QMessageBox::critical(nullptr, "Error", "Secure failed! Invalid file.");
            return;
        }

        if(bin->is_x86()) {
            ELFAddingMethods<Registers_x86> am(dynamic_cast< ::ELF*>(bin));
            if(!am.secure(x86methodsToInsert)) {
                QMessageBox::critical(nullptr, "Error", "Secure failed!");
                return;
            }
        }
        else {
            ELFAddingMethods<Registers_x64> am(dynamic_cast< ::ELF*>(bin));
            if(!am.secure(x64methodsToInsert)) {
                QMessageBox::critical(nullptr, "Error", "Secure failed!");
                return;
            }
        }

        break;
    }

    case ApplicationManager::SOURCE:
        // TODO: source
        break;

    default:
        QMessageBox::critical(nullptr, "Error", "Secure failed!");
        break;
    }

    if(bin) {
        if(!out.open(QFile::WriteOnly))
        {
            QMessageBox::critical(nullptr, "Error", "Secure failed! Cannot open out file.");
            return;
        }

        out.write(bin->getData());

        out.close();
    }

    QMessageBox::information(nullptr, "Success!", QString("Methods injected.\nFile saved as: ") + new_path);
    m_targetPath = new_path;

    // TODO: source
}

void ApplicationManager::obfuscateClicked(int cov, int minl, int maxl)
{
    QString path = m_targetPath;
    QFile f(path);
    if(!f.open(QFile::ReadOnly)) {
        QMessageBox::critical(nullptr, "Error", "Obfuscation failed! Cannot open file.");
        return;
    }
    QByteArray data = f.readAll();
    f.close();

    BinaryFile *bin = nullptr;
    QFileInfo in(path);

    QString out_name = in.baseName() + QString("_obfuscated");
    if(in.completeSuffix().length() > 0)
        out_name.append(".").append(in.completeSuffix());
    QString new_path = QFileInfo(in.absoluteDir(), out_name).absoluteFilePath();
    QFile out(new_path);

    switch(m_state)
    {
    case ApplicationManager::PE:
    {
        bin = new PEFile(data);
        if(!checkBinaryFile(*bin)) {
            QMessageBox::critical(nullptr, "Error", "Obfuscation failed! Invalid file.");
            return;
        }

        if(bin->is_x86()) {
            PEAddingMethods<Registers_x86> am(dynamic_cast<PEFile*>(bin));
            if(!am.obfuscate(cov, minl, maxl)) {
                QMessageBox::critical(nullptr, "Error", "Obfuscation failed!");
                return;
            }
        }
        else {
            PEAddingMethods<Registers_x64> am(dynamic_cast<PEFile*>(bin));
            if(!am.obfuscate(cov, minl, maxl)) {
                QMessageBox::critical(nullptr, "Error", "Obfuscation failed!");
                return;
            }
        }

        break;
    }

    case ApplicationManager::ELF:
    {
        bin = new ::ELF(data);
        if(!checkBinaryFile(*bin)) {
            QMessageBox::critical(nullptr, "Error", "Obfuscation failed! Invalid file.");
            return;
        }

        if(bin->is_x86()) {
            ELFAddingMethods<Registers_x86> am(dynamic_cast< ::ELF*>(bin));
            if(!am.obfuscate(cov, minl, maxl)) {
                QMessageBox::critical(nullptr, "Error", "Obfuscation failed!");
                return;
            }
        }
        else {
            ELFAddingMethods<Registers_x64> am(dynamic_cast< ::ELF*>(bin));
            if(!am.obfuscate(cov, minl, maxl)) {
                QMessageBox::critical(nullptr, "Error", "Obfuscation failed!");
                return;
            }
        }

        break;
    }

    case ApplicationManager::SOURCE:
        // TODO: source
        break;

    default:
        QMessageBox::critical(nullptr, "Error", "Obfuscation failed!");
        break;
    }

    if(bin) {
        if(!out.open(QFile::WriteOnly))
        {
            QMessageBox::critical(nullptr, "Error", "Obfuscation failed! Cannot open out file.");
            return;
        }

        out.write(bin->getData());

        out.close();
    }

    QMessageBox::information(nullptr, "Success!", QString("Code obfuscated.\nFile saved as: ") + new_path);
    m_targetPath = new_path;

    // TODO: source
}

void ApplicationManager::packClicked(int lvl, int opt)
{
    bool ok = false;
    lvl++;

    QString path = m_targetPath;
    QFileInfo in(path);

    QString out_name = in.baseName() + QString("_packed");
    if(in.completeSuffix().length() > 0)
        out_name.append(".").append(in.completeSuffix());

    path = QFileInfo(in.absoluteDir(), out_name).absoluteFilePath();
    QFile::copy(m_targetPath, path);


    if(m_archType == ApplicationManager::X86)
        ok = DAddingMethods<Registers_x86>::pack(path,
                                                 static_cast<DAddingMethods<Registers_x86>::CompressionLevel>(lvl),
                                                 static_cast<DAddingMethods<Registers_x86>::CompressionOptions>(opt));
    else
        ok = DAddingMethods<Registers_x64>::pack(path,
                                                 static_cast<DAddingMethods<Registers_x64>::CompressionLevel>(lvl),
                                                 static_cast<DAddingMethods<Registers_x64>::CompressionOptions>(opt));

    if(!ok)
        QMessageBox::critical(nullptr, "Error", "Packing failed!");
    else
        QMessageBox::information(nullptr, "Success!", QString("Packing finished.\nFile saved as: ") + path);

    m_targetPath = path;
}

void ApplicationManager::insertMethods(const QString& functionName, const QString &scdName)
{
    bool isFound = false;
    SourceCodeDescription * rightScd;
    foreach (SourceCodeDescription* pscd, m_sourceMethods) {
        if(pscd->name()==scdName){
            rightScd = pscd;
            isFound = true;
            break;
        }
    }
    if(!isFound){
        LOG_ERROR("Method not found");
        return;
    }

    QFile f(rightScd->path);
    if (!f.open(QFile::ReadOnly | QFile::Text)){
        LOG_ERROR("Cant open file");
        return;
    }
    QTextStream in(&f);
    QString s = in.readAll();
    sourceParser->insertMethods(m_targetPath,functionName,s);
}

QStringList ApplicationManager::getDeclarations()
{
    QStringList l = sourceParser->getFunctions(m_targetPath);
    foreach(QString s, l){
        m_x86MethodsNames.append(QVariant::fromValue(s));
    }

    return l;
}

void ApplicationManager::updateCurrMethods()
{
    foreach(Method* m, m_currMethods)
        delete m;

    m_currMethods.clear();
    if(m_sys==Linux){
        if(m_archType == X86){
            switch(m_currCm){
            case OEP:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::OEP)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux )
                        m_currMethods.append(new Method(w));
                }
                break;
            case Thread:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::Thread)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case Trampoline:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::Trampoline)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case INIT:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::INIT)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case INIT_ARRAY:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::INIT_ARRAY)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case CTORS:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::CTORS)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case TLS:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::TLS)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            default:
                break;

            }
        }else{
            switch(m_currCm){
            case OEP:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::OEP)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case Thread:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::Thread)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case Trampoline:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::Trampoline)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case INIT:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::INIT)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case INIT_ARRAY:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::INIT_ARRAY)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case CTORS:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::CTORS)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            case TLS:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::TLS)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currMethods.append(new Method(w));
                }
                break;
            default:
                break;

            }
        }
    } else {
        if(m_archType == X86){
            switch(m_currCm){
            case OEP:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::OEP)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows )
                        m_currMethods.append(new Method(w));
                }
                break;
            case Thread:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::Thread)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case Trampoline:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::Trampoline)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case INIT:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::INIT)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case INIT_ARRAY:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::INIT_ARRAY)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case CTORS:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::CTORS)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case TLS:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::TLS)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            default:
                break;

            }
        }else{
            switch(m_currCm){
            case OEP:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::OEP)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case Thread:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::Thread)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case Trampoline:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::Trampoline)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case INIT:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::INIT)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case INIT_ARRAY:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::INIT_ARRAY)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case CTORS:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::CTORS)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            case TLS:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::TLS)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Method
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currMethods.append(new Method(w));
                }
                break;
            default:
                break;

            }
        }
    }
    emit currMethodsChanged();
}

void ApplicationManager::updateCurrHandlers()
{
    foreach(Method* m, m_currHandlers)
        delete m;

    m_currHandlers.clear();
    if(m_sys==Linux){
        if(m_archType == X86){
            switch(m_currCm){
            case OEP:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::OEP)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux )
                        m_currHandlers.append(new Method(w));
                }
                break;
            case Thread:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::Thread)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case Trampoline:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::Trampoline)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case INIT:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::INIT)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case INIT_ARRAY:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::INIT_ARRAY)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case CTORS:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::CTORS)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case TLS:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::TLS)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            default:
                break;

            }
        }else{
            switch(m_currCm){
            case OEP:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::OEP)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case Thread:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::Thread)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case Trampoline:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::Trampoline)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case INIT:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::INIT)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case INIT_ARRAY:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::INIT_ARRAY)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case CTORS:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::CTORS)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case TLS:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::TLS)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Linux)
                        m_currHandlers.append(new Method(w));
                }
                break;
            default:
                break;

            }
        }
    } else {
        if(m_archType == X86){
            switch(m_currCm){
            case OEP:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::OEP)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows )
                        m_currHandlers.append(new Method(w));
                }
                break;
            case Thread:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::Thread)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case Trampoline:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::Trampoline)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case INIT:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::INIT)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case INIT_ARRAY:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::INIT_ARRAY)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case CTORS:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::CTORS)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case TLS:
                foreach(Wrapper<Registers_x86>* w, m_x86methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x86>::CallingMethod::TLS)
                            && w->wrapper_type==Wrapper<Registers_x86>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x86>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            default:
                break;

            }
        }else{
            switch(m_currCm){
            case OEP:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::OEP)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case Thread:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::Thread)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case Trampoline:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::Trampoline)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case INIT:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::INIT)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case INIT_ARRAY:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::INIT_ARRAY)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case CTORS:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::CTORS)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            case TLS:
                foreach(Wrapper<Registers_x64>* w, m_x64methodsList){
                    if(w->allowed_methods.contains(DAddingMethods<Registers_x64>::CallingMethod::TLS)
                            && w->wrapper_type==Wrapper<Registers_x64>::WrapperType::Handler
                            && w->system_type == DAddingMethods<Registers_x64>::SystemType::Windows)
                        m_currHandlers.append(new Method(w));
                }
                break;
            default:
                break;

            }
        }
    }
    emit currHandlersChanged();
}

void ApplicationManager::updateCurrSourceMethods() {
    foreach (SourceCodeDescription *scd, m_currSourceMethods)
        delete scd;

    m_currSourceMethods.clear();

    SourceCodeDescription *pscd;
    foreach (SourceCodeDescription *scd, m_sourceMethods) {
        if (!scd)
            continue;
        if (scd->sys_type == static_cast<SourceCodeDescription::SystemType>(m_sys)) {
            pscd = new(std::nothrow) SourceCodeDescription;
            if (!pscd)
                continue;
            scd->copy(pscd);
            m_currSourceMethods.push_back(pscd);
        }
    }

    emit currSourceMethodsChanged();
}

void ApplicationManager::changeList(const QString &methodsName,const QString& handlersName, int index)
{
    // TODO : CLEAR MEMORY
    bool methodFound=false;
    bool handlerFound=false;
    if(currCm()==Thread)
    {
        if(archType()==X86){
            Wrapper<Registers_x86> *newWrapper;
            Wrapper<Registers_x86> *newHandler;
            foreach(Wrapper<Registers_x86> *w ,m_x86methodsList)
                if(w->name==methodsName) {
                    newWrapper = Wrapper<Registers_x86>::copy(w);
                    methodFound = true;
                    break;
                }

            if (!methodFound) {
                LOG_ERROR("Methods not found");
                return;
            }

            foreach(Wrapper<Registers_x86> *w ,m_x86methodsList)
                if(w->name==handlersName) {
                    newHandler = Wrapper<Registers_x86>::copy(w);
                    handlerFound = true;
                    break;
                }

            if(!handlerFound) {
                LOG_ERROR("Handlers not found!");
                return;
            }

            if(newWrapper->ret == Registers_x86::None)
                newHandler = nullptr;

            newWrapper->detect_handler = newHandler;

            if(index<x86threadWrappersToInject.size() && index >= 0)
                x86threadWrappersToInject[index]= newWrapper;
            else LOG_ERROR("index out of bound");

            return;
        }
        else {
            Wrapper<Registers_x64> *newWrapper;
            Wrapper<Registers_x64> *newHandler;
            foreach(Wrapper<Registers_x64> *w ,m_x64methodsList)
                if(w->name==methodsName){
                    newWrapper = Wrapper<Registers_x64>::copy(w);
                    methodFound = true;
                    break;
                }

            if (!methodFound) {
                LOG_ERROR("Methods not found");
                return;
            }

            foreach(Wrapper<Registers_x64> *w ,m_x64methodsList)
                if(w->name==handlersName){
                    newHandler = Wrapper<Registers_x64>::copy(w);
                    handlerFound = true;
                    break;
                }

            if(!handlerFound) {
                LOG_ERROR("Handlers not found!");
                return;
            }

            if(newWrapper->ret == Registers_x64::None)
                newHandler = nullptr;

            newWrapper->detect_handler = newHandler;
            if(index<x64threadWrappersToInject.size() && index >= 0)
                x64threadWrappersToInject[index]= newWrapper;
            else LOG_ERROR("index out of bound");

            return;
        }
    }
    else
        if(archType()==X86){
            Wrapper<Registers_x86> *newWrapper;
            Wrapper<Registers_x86> *newHandler;
            foreach(Wrapper<Registers_x86> *w ,m_x86methodsList)
                if(w->name==methodsName){
                    newWrapper = Wrapper<Registers_x86>::copy(w);
                    methodFound = true;
                    break;
                }

            if (!methodFound) {
                LOG_ERROR("Methods not found");
                return;
            }

            foreach(Wrapper<Registers_x86> *w ,m_x86methodsList)
                if(w->name==handlersName){
                    newHandler = Wrapper<Registers_x86>::copy(w);
                    handlerFound = true;
                    break;
                }

            if(!handlerFound) {
                LOG_ERROR("Handlers not found!");
                return;
            }

            if(newWrapper->ret == Registers_x86::None)
                newHandler = nullptr;

            newWrapper->detect_handler = newHandler;
            if(index<x86methodsToInsert.size() && index >= 0)
                x86methodsToInsert[index+methodsCount]->adding_method = newWrapper;
            else LOG_ERROR("index out of bound");

            return;
        }
        else {
            Wrapper<Registers_x64> *newWrapper;
            Wrapper<Registers_x64> *newHandler;
            foreach(Wrapper<Registers_x64> *w ,m_x64methodsList)
                if(w->name==methodsName){
                    newWrapper = Wrapper<Registers_x64>::copy(w);
                    methodFound = true;
                    break;
                }

            if (!methodFound) {
                LOG_ERROR("Methods not found");
                return;
            }

            foreach(Wrapper<Registers_x64> *w ,m_x64methodsList)
                if(w->name==handlersName){
                    newHandler = Wrapper<Registers_x64>::copy(w);
                    handlerFound = true;
                    break;
                }

            if(!handlerFound) {
                LOG_ERROR("Handlers not found!");
                return;
            }

            if(newWrapper->ret == Registers_x64::None)
                newHandler = nullptr;

            newWrapper->detect_handler = newHandler;
            if(index<x64methodsToInsert.size() && index>=0)
                x64methodsToInsert[index+methodsCount]->adding_method = newWrapper;
            else LOG_ERROR("index out of bound");

            return;
        }
}

void ApplicationManager::insertNewToList(const QString &name)
{
    if(currCm()==Thread){
        if(archType()==X86){
            Wrapper<Registers_x86> * w = new Wrapper<Registers_x86>();
            x86threadWrappersToInject.append(w);
        } else {
            Wrapper<Registers_x64> * w = new Wrapper<Registers_x64>();
            x64threadWrappersToInject.append(w);
        }
    } else
        if(archType()==X86){
            DAddingMethods<Registers_x86>::InjectDescription *id = new DAddingMethods<Registers_x86>::InjectDescription();
            id->cm = (DAddingMethods<Registers_x86>::CallingMethod)currCm();
            id->change_x_only = false;

            Wrapper<Registers_x86> * w = new Wrapper<Registers_x86>();
            id->adding_method = w;

            x86methodsToInsert.append(id);
            //changeList("Code checksum","Exit",methodsToInsert.size()-1);
        } else {
            DAddingMethods<Registers_x64>::InjectDescription *id = new DAddingMethods<Registers_x64>::InjectDescription();
            id->cm = (DAddingMethods<Registers_x64>::CallingMethod)currCm();
            id->change_x_only = false;

            Wrapper<Registers_x64> * w = new Wrapper<Registers_x64>();
            id->adding_method = w;

            x64methodsToInsert.append(id);
        }
}

void ApplicationManager::clearList()
{

}

ApplicationManager::State ApplicationManager::getFileType(QString path)
{
    //QString newPath = path.remove("file:///");
    QFile f(path);

    if(!f.open(QFile::ReadOnly))
    {
        LOG_ERROR("Cannot open file!");
        return ApplicationManager::IDLE;
    }

    QByteArray data = f.readAll();
    f.close();

    PEFile pe(data);
    if(pe.is_valid()){
        setSys(Windows);
        setArchType(pe.is_x64() ? X64 : X86);
        return ApplicationManager::PE;
    }
    ::ELF elf(data);
    if(elf.is_valid()){
        setSys(Linux);
        setArchType(elf.is_x64() ? X64 : X86);
        return ApplicationManager::ELF;
    }
    // Source !
    getDeclarations();
    return ApplicationManager::SOURCE;
}

bool ApplicationManager::checkBinaryFile(BinaryFile &f)
{
    if(!f.is_valid())
        return false;

    if(f.is_x86() && m_archType != ApplicationManager::X86)
        return false;

    if(f.is_x64() && m_archType != ApplicationManager::X64)
        return false;

    return true;
}

QQmlListProperty<Method> ApplicationManager::x86methods()
{
    return QQmlListProperty<Method>(this,m_methodsx86);
}
*/

