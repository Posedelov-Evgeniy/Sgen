#ifndef SIGNALCONTROLLER_H
#define SIGNALCONTROLLER_H

#include <QApplication>
#include <QObject>
#include <QTimer>
#include <QtGui>
#include <QtCore/QCoreApplication>
#include <QProcess>
#include <QVector>
#include <QMap>
#include <QCryptographicHash>
#include "classes/environmentinfo.h"

typedef double (*PlaySoundFunction) (int,unsigned int,double);
typedef double (*VariablesFunction) (unsigned int);
typedef double (*GenSignalFunction) (double, double, double);
typedef void (*UpdateVariablesFunction) (PlaySoundFunction, VariablesFunction);
typedef QMap<QString, double> CVariables;
typedef QMap<QString, QString> CExpressions;

struct GenChannelInfo {
    double freq;
    double k;
    double amp;
    double fr;
    double ar;
    QString function_text;
    GenSignalFunction channel_fct;
};

enum SignalControllerVariablesTransition { SCVT_None, SCVT_LinearBuff, SCVT_SquareBuff, SCVT_LinearVar, SCVT_SquareVar};

class SignalController: public QObject
{
    Q_OBJECT
public:
    SignalController(QObject *parent = 0);
    ~SignalController();

    CVariables *getVariables();
    CExpressions *getExpressions();

    void setAmp(unsigned int channel, double new_amp);
    void setFreq(unsigned int channel, double new_freq);

    void fillBuffer(void *data, unsigned int datalen);

    QStringList *getInnerVariables() const;
    void setVariable(QString varname, double varvalue);
    bool isVariableNameOK(QString varname);

    void setFunctionsStr(QString new_f);
    void setFunctionStr(unsigned int channel, QString new_text);

    virtual void setChannelsCount(unsigned int count);
    virtual unsigned int getChannelsCount();
    virtual GenSignalFunction getChannelFunction(unsigned int channel);

    virtual void setFrequency(double value);
    virtual double getFrequency() const;
    double getInstFreq(unsigned int channel);
    double getInstAmp(unsigned int channel);
    SignalControllerVariablesTransition getTransitionType() const;
    void setTransitionType(const SignalControllerVariablesTransition &value);
    double getTransitionTime() const;
    void setTransitionTime(double value);

protected:
    double t, t_real;
    SignalControllerVariablesTransition transition;
    double transition_time, transition_counter;
    QString oldParseHash;
    QLibrary lib;
    bool all_functions_loaded;
    unsigned int channels_count;
    QVector<GenChannelInfo*> channels;
    CVariables *variables, *new_variables, *old_variables, *buff_variables;
    CExpressions *expressions;
    QStringList *inner_variables;
    QString text_functions, signal_functions;
    double frequency;

    QMutex buffer_mutex;
    bool variable_changed;
    UpdateVariablesFunction update_func;

    bool parseFunctions();
    double getResult(unsigned int channel, double current_t);
    void resetParams();
    void resetT();
    void variableUpdated();

    void setSignalFunctions(QString value);
    QString getSignalFunctions() const;
    PlaySoundFunction getBasePlaySoundFunction() const;
    void setBasePlaySoundFunction(const PlaySoundFunction &value);
    VariablesFunction getVariableValueFunction() const;
    void setVariableValueFunction(const VariablesFunction &value);
private:
    PlaySoundFunction base_play_sound_function;
    VariablesFunction variable_value_function;

    QString getCurrentParseHash();
    bool checkHash(bool emptyCheck);
    void flushNewVariables();
    void setNewVariables();
    void setOldVariables();
    double getTransitionKoef(double transition_tk);
};

#endif // SIGNALCONTROLLER_H
