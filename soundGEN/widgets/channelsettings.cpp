#include "channelsettings.h"
#include "ui_channelsettings.h"

ChannelSettings::ChannelSettings(QWidget *parent, unsigned int base_channel_index, unsigned int channels_count) :
    QWidget(parent),
    ui(new Ui::ChannelSettings)
{
    channel_index = base_channel_index;
    ui->setupUi(this);
    dialog_functions = new DialogFunctions(parent);
    sc = (SndController*) SndController::Instance();

    function_edit = new UTextEdit();
    function_edit->document()->setPlainText("sin(k*t)");
    ui->function_layout->insertWidget(1, function_edit);
    channel_drawer = new functionGraphicDrawer();
    ui->settings_base_horizontal_layout->addWidget(channel_drawer);

    #if defined(__ANDROID__)
    QPalette Pal = function_edit->palette();
    Pal.setColor(QPalette::Background, Qt::white);
    function_edit->setAutoFillBackground(true);
    function_edit->setPalette(Pal);
    ui->doubleSpinBox_freq->setDecimals(3);
    #endif

    setChannelsCount(channels_count);

    QObject::connect(sc, SIGNAL(cycle_start()), this, SLOT(cycle_starting()));

    QObject::connect(dialog_functions, SIGNAL(accepted()), this, SLOT(paste_function_accepted()));

    QObject::connect(function_edit, SIGNAL(textChangedC()), this, SLOT(options_changing()));
    QObject::connect(ui->doubleSpinBox_amp, SIGNAL(valueChanged(double)), this, SLOT(options_changing()));
    QObject::connect(ui->doubleSpinBox_freq, SIGNAL(valueChanged(double)), this, SLOT(options_changing()));
    QObject::connect(channel_drawer, SIGNAL(changed()), this, SLOT(channel_options_changing()));
}

void ChannelSettings::setChannelsCount(unsigned int channels_count)
{
    QString ltext;
    if (channels_count==1) {
        ltext = tr("Sound");
    } else if (channels_count==2) {
        ltext = channel_index==0 ? tr("Left channel") : tr("Right channel");
    } else {
        ltext = tr("Channel ") + QString::number(channel_index);
    }
    ui->label->setText(ltext);
}

ChannelSettings::~ChannelSettings()
{
    channel_drawer->deleteLater();
    delete channel_drawer;
    delete function_edit;
    delete dialog_functions;
    delete ui;
}

QString ChannelSettings::getFunction()
{
    return function_edit->document()->toPlainText();
}

void ChannelSettings::setFunction(QString ftext)
{
    function_edit->document()->setPlainText(ftext);
}

double ChannelSettings::getAmp()
{
    return ui->doubleSpinBox_amp->value();
}

void ChannelSettings::setAmp(double amp)
{
    ui->doubleSpinBox_amp->setValue(amp);
}

double ChannelSettings::getFreq()
{
    return ui->doubleSpinBox_freq->value();
}

void ChannelSettings::setFreq(double freq)
{
    ui->doubleSpinBox_freq->setValue(freq);
}

functionGraphicDrawer *ChannelSettings::getDrawer()
{
    return channel_drawer;
}

void ChannelSettings::setVisibleAmp(double amp)
{
    ui->lcdNumber_amp->display(amp);
}

void ChannelSettings::setVisibleFreq(double freq)
{
    ui->lcdNumber_freq->display(freq);
}

void ChannelSettings::cycle_starting()
{
    setVisibleAmp(sc->getInstAmp(channel_index));
    setVisibleFreq(sc->getInstFreq(channel_index));
}

void ChannelSettings::options_changing()
{
    emit options_changed(-1);
}

void ChannelSettings::channel_options_changing()
{
    emit options_changed(channel_index);
}

void ChannelSettings::init_snd_channel_params()
{
    sc->setFunctionStr(channel_index, getFunction());
    sc->setAmp(channel_index, getAmp());
    sc->setFreq(channel_index, getFreq());
    channel_drawer->setAmp(getAmp());
    channel_drawer->setFreq(getFreq());
}

void ChannelSettings::run_graphic()
{
    channel_drawer->setGraphicFunction(sc->getChannelFunction(channel_index));
    channel_drawer->run();
}

void ChannelSettings::stop_graphic()
{
    channel_drawer->setGraphicFunction(0);
    channel_drawer->stop();
}

void ChannelSettings::on_doubleSpinBox_amp_valueChanged(double arg1)
{
    sc->setAmp(channel_index, arg1);
    channel_drawer->setAmp(arg1);
}

void ChannelSettings::on_doubleSpinBox_freq_valueChanged(double arg1)
{
    sc->setFreq(channel_index, arg1);
    channel_drawer->setFreq(arg1);
}

void ChannelSettings::on_dialog_functions_btn_clicked()
{
    dialog_functions->open();
}

void ChannelSettings::paste_function_accepted()
{
    if (!dialog_functions->getPickedFunction().isEmpty()) {
        function_edit->textCursor().insertText(dialog_functions->getPickedFunction());
    }
}
