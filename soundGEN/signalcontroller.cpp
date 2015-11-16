#include "signalcontroller.h"

SignalController::SignalController(QObject *parent) :
    QObject(parent)
{
    all_functions_loaded = false;
    channels_count = 0;
    frequency = 0;
    oldParseHash = "";
    signal_functions = "";
    transition = SCVT_LinearBuff;
    transition_time = 1;
    transition_counter = 0;

    buff_variables = new CVariables();
    old_variables = new CVariables();
    new_variables = new CVariables();
    variables = new CVariables();
    expressions = new CExpressions();
    update_func = NULL;
    base_play_sound_function = NULL;
    variable_value_function = NULL;

    inner_variables = new QStringList();
    inner_variables->append("t");
    inner_variables->append("k");
    inner_variables->append("f");
    inner_variables->append("int");
    inner_variables->append("float");
    inner_variables->append("bool");
    inner_variables->append("double");
    inner_variables->append("static");
    inner_variables->append("std");
    inner_variables->append("namespace");
    inner_variables->append("PlaySignalFunction");
    inner_variables->append("VariablesFunction");
    inner_variables->append("BaseSignalFunction");
    inner_variables->append("BaseVariablesFunction");
    inner_variables->append("extern");
    inner_variables->append("return");
    inner_variables->append("break");
    inner_variables->append("continue");
}

SignalController::~SignalController()
{
    delete inner_variables;
    delete variables;
    delete new_variables;
    delete old_variables;
    delete buff_variables;
    delete expressions;
}

QString SignalController::getCurrentParseHash()
{
    QCryptographicHash hash(QCryptographicHash::Sha1);

    hash.addData(EnvironmentInfo::getConfigsPath().toLatin1());
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        hash.addData(QString::number(QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/main.dll")).toLatin1());
    #else
        hash.addData(QString::number(QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/main.so")).toLatin1());
    #endif
    hash.addData(QString::number(channels_count).toLatin1());
    hash.addData(QString::number(lib.isLoaded()).toLatin1());
    hash.addData(signal_functions.toLatin1());
    hash.addData(text_functions.toLatin1());
    for(int i=0;i<channels_count;i++) {
        hash.addData(channels.at(i)->function_text.toLatin1());
    }
    hash.addData(QString::number(variables->count()).toLatin1());
    hash.addData(QString::number(expressions->count()).toLatin1());

    return hash.result().toHex();
}

bool SignalController::checkHash(bool emptyCheck)
{
    QString parseHash = "";

    if (emptyCheck || !oldParseHash.isEmpty()) {
        parseHash = getCurrentParseHash();
        if (!emptyCheck || oldParseHash.isEmpty()) {
            qDebug() << tr("Hash is: ") << parseHash;
        }
    }

    if (!emptyCheck && !oldParseHash.isEmpty() && !parseHash.isEmpty() && oldParseHash==parseHash) {
        qDebug() << tr("Hashes equals - no need to parse functions");
        return true;
    }

    if (!parseHash.isEmpty()) {
        oldParseHash = parseHash;
    }

    return false;
}

void SignalController::setChannelsCount(unsigned int count)
{
    if (count<channels.size()) {
        while (count>=0 && count<channels.size()) {
            delete channels.last();
            channels.remove(channels.size()-1);
        }
    } else if (count>channels.size()) {
        while (count>channels.size()) {
            GenChannelInfo *info = new GenChannelInfo();
            info->amp = 1;
            info->freq = 500;
            info->channel_fct = NULL;
            info->function_text = "sin(k*t)";
            info->k = info->freq*2.0*M_PI;
            info->fr = 0;
            info->ar = 0;
            channels.append(info);
        }
    }

    channels_count = count;
    setInnerVariables();
}

bool SignalController::parseFunctions()
{
    if (checkHash(false)) return true;

    QString error = "";
    bool add_base_functions = false;
    all_functions_loaded = false;
    unsigned int i;

    if (lib.isLoaded()) {
        lib.unload();
    }

    QDir dir(EnvironmentInfo::getConfigsPath());
    dir.mkdir("efr");

    if (QFile::exists(EnvironmentInfo::getConfigsPath()+"/base_functions.h")) {
        if (QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/base_functions.cpp"))
        {
            QFile::remove(EnvironmentInfo::getConfigsPath()+"/efr/base_functions.cpp");
        }
        QFile::copy(EnvironmentInfo::getConfigsPath()+"/base_functions.cpp", EnvironmentInfo::getConfigsPath()+"/efr/base_functions.cpp");
        if (QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/base_functions.h"))
        {
            QFile::remove(EnvironmentInfo::getConfigsPath()+"/efr/base_functions.h");
        }
        QFile::copy(EnvironmentInfo::getConfigsPath()+"/base_functions.h", EnvironmentInfo::getConfigsPath()+"/efr/base_functions.h");
        add_base_functions = true;
    }

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        QString main_file_name = "main.cpp";
        QString spec_func_pref = "extern \"C\" __declspec(dllexport)";
        QString spec_namespace = "using namespace std;\n";
        QString lib_file = "main.dll";
    #else
        QString main_file_name = "main.c";
        QString spec_func_pref = "extern \"C\"";
        QString spec_namespace = "";
        QString lib_file = "main.so";
    #endif

    if (QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/"+lib_file) && !QFile::remove(EnvironmentInfo::getConfigsPath()+"/efr/"+lib_file))
    {
        qDebug() <<  tr("Can't remove %filename%").replace("%filename%", lib_file) << endl;
        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
            int lib_n = 0;
            do {
                lib_file = "main_"+QString::number(lib_n)+".dll";
                lib_n++;
            } while (QFile::exists(EnvironmentInfo::getConfigsPath()+"/efr/"+lib_file) && !QFile::remove(EnvironmentInfo::getConfigsPath()+"/efr/"+lib_file));
        #endif
    }

    QProcess* pConsoleProc = new QProcess;

    QFile file(EnvironmentInfo::getConfigsPath()+"/efr/main.h");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out(&file);
    out << "#include <math.h>\n";
    out << "#include <string.h>\n";
    out << "#include <stdio.h>\n";
    if (add_base_functions) out << "#include \"base_functions.h\"\n";
    out << "typedef double (*PlaySignalFunction) (int,unsigned int,double);\n";
    out << "typedef double (*VariablesFunction) (unsigned int);\n";

    out << spec_func_pref << " void update_vars(PlaySignalFunction __bFunction, VariablesFunction __cFunction);\n";

    for(i=0;i<channels_count;i++) {
        out << spec_func_pref << " double signal_func_"+QString::number(i)+"(double t);\n";
    }
    file.close();

    QFile file2(EnvironmentInfo::getConfigsPath()+"/efr/"+main_file_name);
    file2.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out2(&file2);
    out2 << "#include \"main.h\"\n";
    out2 << spec_namespace;
    out2 << "\n";
    out2 << "static PlaySignalFunction BaseSignalFunction;\n";
    out2 << "static VariablesFunction BaseVariablesFunction;\n";
    if (!signal_functions.isEmpty()) {
        out2 << "\n" + signal_functions + "\n";
    }
    if (!text_functions.isEmpty()) {
        out2 << "\n" + text_functions + "\n";
    }

    CVariables::const_iterator iter_var = variables->constBegin();
    int ind;
    while (iter_var != variables->constEnd()) {
        out2 << "static double " << iter_var.key() << " = " << iter_var.value() << ";\n";
        ++iter_var;
    }

    out2 << "\n";
    out2 << spec_func_pref << " void update_vars(PlaySignalFunction __bFunction, VariablesFunction __cFunction)\n";
    out2 << "{\n";
    out2 << "    BaseSignalFunction = __bFunction;\n";
    out2 << "    BaseVariablesFunction = __cFunction;\n";
    iter_var = variables->constBegin();
    ind = 0;
    while (iter_var != variables->constEnd()) {
        out2 << "    " << iter_var.key() << " = BaseVariablesFunction(" << ind << ");\n";
        ++iter_var;
        ++ind;
    }
    out2 << "};\n";

    CExpressions::const_iterator iter_expr;
    QString ftext;

    for(i=0;i<channels_count;i++) {
        ftext = channels.at(i)->function_text;

        out2 << spec_func_pref << " double signal_func_"+QString::number(i)+"(double t) \n";
        out2 << "{\n";

        iter_expr = expressions->constBegin();
        while (iter_expr != expressions->constEnd()) {
            out2 << "    double " << iter_expr.key() << " = (" << iter_expr.value() << ");\n";
            ++iter_expr;
        }

        out2 << "    double k = k_" << i << ";\n";
        out2 << "    double f = f_" << i << ";\n";

        out2 << "    return amp_" << i << "*((double) ("+ftext+"));\n";
        out2 << "};\n";
    }
    out2 << "\n";
    out2 << "int main(int argc, char *argv[]) {return 0;};\n";
    file2.close();

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
        QString tcmd;
        QString vcdir = EnvironmentInfo::getVCPath();
        if (!vcdir.isEmpty())
        {
            pConsoleProc->setWorkingDirectory(vcdir);
            pConsoleProc->start("vcvarsall.bat x86_amd64");
            pConsoleProc->waitForFinished();
            qDebug() << pConsoleProc->workingDirectory() << endl;
        }

        pConsoleProc->setWorkingDirectory(EnvironmentInfo::getConfigsPath()+"/efr");
        qDebug() << pConsoleProc->workingDirectory() << endl;

        if (add_base_functions) {
            pConsoleProc->start("cl.exe /c /EHsc base_functions.cpp");
            pConsoleProc->waitForFinished();
            qDebug() <<  pConsoleProc->readAll() << endl;
            pConsoleProc->start("lib base_functions.obj");
            pConsoleProc->waitForFinished();
            qDebug() <<  pConsoleProc->readAll() << endl;
            tcmd = "cl.exe /LD main.cpp /DLL /link base_functions.lib /OUT:" + lib_file;
        } else {
            tcmd = "cl.exe /LD main.cpp /link /DLL /OUT:" + lib_file;
        }
    #else
        QFile file3(EnvironmentInfo::getConfigsPath()+"/efr/Makefile");
        file3.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        QTextStream out3(&file3);

        #if defined(__LP64__) || defined(__x86_64__) || defined(__ppc64__)
            int platform = 64;
        #else
            int platform = 32;
        #endif

        out3 << "CC = g++\n";
        out3 << "MODULES= main.o base_functions.o\n";
        out3 << "INC=-I/usr/local/sbin -I/usr/local/bin -I/usr/sbin -I/usr/bin -I/sbin -I/bin\n";
        out3 << "OBJECTS=\n";
        out3 << "RCLOBJECTS= main.c main.h base_functions.cpp base_functions.h\n";
        out3 << "all: "+lib_file+"\n";
        out3 << lib_file+":$(MODULES)\n";
        out3 << "	$(CC) -shared $(MODULES) -o "+lib_file+"\n";
        out3 << "base_functions.o: $(RCLOBJECTS)\n";
        out3 << "	g++ -m" << platform << " -Wall -fPIC -c base_functions.cpp -o base_functions.o\n";
        out3 << "main.o: $(RCLOBJECTS)\n";
        out3 << "	g++ -m" << platform << " -Wall -fPIC -c main.c -o main.o\n";
        out3 << "clean:\n";
        out3 << "	rm -f *.o\n";
        out3 << "	rm -f "+lib_file+"\n";
        file3.close();

        QString tcmd = "make -C \""+EnvironmentInfo::getConfigsPath()+"/efr\" -f Makefile";
    #endif
    qDebug() <<  tcmd << endl;

    pConsoleProc->start(tcmd, QProcess::ReadOnly);
    if(pConsoleProc->waitForFinished()==true)
    {
       QByteArray b = pConsoleProc->readAllStandardError();
       #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
           error = pConsoleProc->readAll();
           qDebug() <<  error << endl;
           if (error.indexOf("fatal error", 0, Qt::CaseInsensitive)<0 && error.indexOf("error c", 0, Qt::CaseInsensitive)<0) {
               error = "";
           }
       #else
           error = QString(b);
       #endif
       qDebug() <<  error << endl;
    }
    pConsoleProc->close();
    delete pConsoleProc;

    if (error.isEmpty()) {
        lib.setFileName(EnvironmentInfo::getConfigsPath()+"/efr/"+lib_file);
        all_functions_loaded = true;
        for(i=0;i<channels_count;i++) {
            channels.at(i)->channel_fct = (GenSignalFunction)(lib.resolve(qPrintable("signal_func_"+QString::number(i))));
            all_functions_loaded = all_functions_loaded && channels.at(i)->channel_fct;
        }
        if (all_functions_loaded) {
            update_func = (UpdateVariablesFunction)(lib.resolve("update_vars"));
        }
        checkHash(true);
    }

    if (!all_functions_loaded) {
        oldParseHash = "";
        for(i=0;i<channels_count;i++) {
            channels.at(i)->channel_fct = NULL;
        }
        update_func = NULL;
    }

    return all_functions_loaded;
}

unsigned int SignalController::getChannelsCount()
{
    return channels_count;
}

void SignalController::setFunctionsStr(QString new_f) {
    text_functions = new_f;
}

void SignalController::setFunctionStr(unsigned int channel, QString new_text)
{
    channels.at(channel)->function_text = new_text;
}

void SignalController::setAmp(unsigned int channel, double new_amp)
{
    QMutexLocker locker(&buffer_mutex);
    variable_changed = true;
    channels.at(channel)->amp = new_amp;
    setVariable("amp_"+QString::number(channel), new_amp, false);
}

void SignalController::setFreq(unsigned int channel, double new_freq)
{
    QMutexLocker locker(&buffer_mutex);
    variable_changed = true;
    channels.at(channel)->freq = new_freq;
    channels.at(channel)->k = new_freq*2.0*M_PI;
    setVariable("k_"+QString::number(channel), channels.at(channel)->k, false);
    setVariable("f_"+QString::number(channel), channels.at(channel)->freq, false);
}

GenSignalFunction SignalController::getChannelFunction(unsigned int channel)
{
    if (lib.isLoaded() && channel>=0 && channel<channels.size())
        return channels.at(channel)->channel_fct;
    else
        return 0;
}

CVariables* SignalController::getVariables()
{
    return variables;
}

CExpressions* SignalController::getExpressions()
{
    return expressions;
}

double SignalController::getTransitionKoef(double transition_tk)
{
    if (transition==SCVT_SquareBuff || transition==SCVT_SquareVar)
        return transition_tk*transition_tk;
    return transition_tk;
}

void SignalController::fillBuffer(void *data, unsigned int datalen)
{
    unsigned int count, maxcount, offset;
    qint32 *buffer = (qint32*)data;
    qint32 max_val = std::numeric_limits<qint32>::max()-1;
    double curr;

    buffer_mutex.lock();

    datalen = datalen/sizeof(qint32);
    maxcount = datalen/channels_count;

    if (all_functions_loaded)
    {

        if (variable_changed && new_variables->size()>0 && transition!=SCVT_None) {
            curr = 0;
            double t_k;

            if (transition==SCVT_LinearBuff || transition==SCVT_SquareBuff) {

                if (!transition_counter) {
                    setNewVariables();
                    variableUpdated();
                }

                for(unsigned int i=0; i<channels_count; i++)
                {
                    curr = 0;
                    for (count=0; count<maxcount; count++)
                    {
                        curr = getResult(i, t+count/frequency);
                        buffer[count*channels_count + i] = (qint32)(curr * max_val);
                    }
                }

                setOldVariables();
                variableUpdated();
                for (count=0; count<maxcount; count++) {
                    t_k = getTransitionKoef((frequency*transition_counter + count)/(frequency*transition_time));
                    if(t_k>1) t_k=1;
                    for(unsigned int i=0; i<channels_count; i++) {
                        offset =  count*channels_count + i;
                        curr = getResult(i, t+count/frequency);
                        buffer[offset] = (quint32) (floor(curr * max_val)*(1-t_k) + buffer[offset]*t_k);
                    }
                }

                if (t_k < 1) {
                    setNewVariables();
                    variableUpdated();
                }

            } else if (transition == SCVT_LinearVar || transition==SCVT_SquareVar) {
                CVariables::const_iterator iter_var;
                for (count=0; count<maxcount; count++)
                {
                    t_k = getTransitionKoef((frequency*transition_counter + count)/(frequency*transition_time));
                    if(t_k>1) t_k=1;
                    iter_var = new_variables->constBegin();
                    while (iter_var != new_variables->constEnd()) {
                        curr = old_variables->value(iter_var.key())*(1-t_k) + iter_var.value()*t_k;
                        variables->insert(iter_var.key(), curr);
                        ++iter_var;
                    }
                    variableUpdated();
                    for(unsigned int i=0; i<channels_count; i++)
                    {
                        offset =  count*channels_count + i;
                        curr = getResult(i, t+count/frequency);
                        buffer[offset] = (qint32)(curr * max_val);
                    }
                }
            }

            transition_counter += (maxcount-1) / frequency;

            if (abs(transition_counter-transition_time)<1E-10 || transition_counter>transition_time) {
                flushNewVariables();
                variableUpdated();
            }
        } else {
            if (transition==SCVT_None && variable_changed && new_variables->size()>0) {
                flushAllVariables();
            }

            for(unsigned int i=0; i<channels_count; i++)
            {
                curr = 0;
                for (count=0; count<maxcount; count++)
                {
                    curr = getResult(i, t+count/frequency);
                    buffer[count*channels_count + i] = (qint32)(curr * max_val);
                }
            }
        }

        t += maxcount/frequency;
    }

    buffer_mutex.unlock();
}

double SignalController::getFrequency() const
{
    return frequency;
}

void SignalController::setFrequency(double value)
{
    frequency = value;
}

QStringList* SignalController::getInnerVariables() const
{
    return inner_variables;
}

double SignalController::getInstFreq(unsigned int channel)
{
    return channels.at(channel)->fr;
}

double SignalController::getInstAmp(unsigned int channel)
{
    return channels.at(channel)->ar;
}

SignalControllerVariablesTransition SignalController::getTransitionType() const
{
    return transition;
}

void SignalController::setTransitionType(const SignalControllerVariablesTransition &value)
{
    transition = value;
}

double SignalController::getTransitionTime() const
{
    return transition_time;
}

void SignalController::setTransitionTime(double value)
{
    transition_time = value;
}

void SignalController::resetParams()
{
    for(unsigned int i=0; i<channels_count; i++) {
        GenChannelInfo *info = channels.at(i);
        info->amp = 1;
        info->freq = 500;
        info->k = info->freq*2.0*M_PI;
        info->ar = 0;
        info->fr = 0;
        info->function_text = "sin(k*t)";
    }
    resetT();
}

void SignalController::resetT()
{
    t = t_real = 0.0;
}

void SignalController::variableUpdated()
{
    if (update_func && base_play_sound_function && variable_value_function) {
        update_func(base_play_sound_function, variable_value_function);
    }
}

void SignalController::setSignalFunctions(QString value)
{
    signal_functions = value;
}

QString SignalController::getSignalFunctions() const
{
    return signal_functions;
}

void SignalController::setVariableValueFunction(const VariablesFunction &value)
{
    variable_value_function = value;
}

void SignalController::flushAllVariables()
{
    if(new_variables->size()>0) flushNewVariables();
    if(new_variables->size()>0) flushNewVariables();
    variableUpdated();
}

void SignalController::setBasePlaySoundFunction(const PlaySoundFunction &value)
{
    base_play_sound_function = value;
}

VariablesFunction SignalController::getVariableValueFunction() const
{
    return variable_value_function;
}

PlaySoundFunction SignalController::getBasePlaySoundFunction() const
{
    return base_play_sound_function;
}

void SignalController::setVariable(QString varname, double varvalue, bool with_locker)
{
    QMutexLocker locker(with_locker ? &buffer_mutex : NULL);
    variable_changed = true;

    if (variables->contains(varname)) {
        if (transition_counter) {
            buff_variables->insert(varname, varvalue);
        } else {
            new_variables->insert(varname, varvalue);
            old_variables->insert(varname, variables->value(varname));
        }
    } else {
        variables->insert(varname, varvalue);
    }
}

void SignalController::setNewVariables()
{
    CVariables::const_iterator iter_var = new_variables->constBegin();
    while (iter_var != new_variables->constEnd()) {
        variables->insert(iter_var.key(), iter_var.value());
        ++iter_var;
    }
}

void SignalController::setOldVariables()
{
    CVariables::const_iterator iter_var = old_variables->constBegin();
    while (iter_var != old_variables->constEnd()) {
        variables->insert(iter_var.key(), iter_var.value());
        ++iter_var;
    }
}

void SignalController::clearVariables()
{
    variables->clear();
    setInnerVariables();
}

void SignalController::setInnerVariables()
{
    for(int i=0;i<channels_count;i++) {
        variables->insert("amp_"+QString::number(i), channels.at(i)->amp);
        variables->insert("k_"+QString::number(i), channels.at(i)->k);
        variables->insert("f_"+QString::number(i), channels.at(i)->freq);
    }
}

void SignalController::flushNewVariables()
{
    setNewVariables();
    transition_counter = 0;
    new_variables->clear();
    old_variables->clear();

    if (!buff_variables->isEmpty()) {
        CVariables::const_iterator iter_var = variables->constBegin();
        while (iter_var != variables->constEnd()) {
            old_variables->insert(iter_var.key(), iter_var.value());
            ++iter_var;
        }
        iter_var = buff_variables->constBegin();
        while (iter_var != buff_variables->constEnd()) {
            new_variables->insert(iter_var.key(), iter_var.value());
            ++iter_var;
        }
        buff_variables->clear();
    } else {
        variable_changed = false;
    }
}

bool SignalController::isVariableNameOK(QString varname)
{
    return !varname.isEmpty() && !getInnerVariables()->contains(varname, Qt::CaseInsensitive);
}

double SignalController::getResult(unsigned int channel, double current_t)
{
    GenChannelInfo *info = channels.at(channel);
    return info->channel_fct(current_t);
}
