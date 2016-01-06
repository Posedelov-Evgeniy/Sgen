#include "iopicker.h"
#include "ui_iopicker.h"

IOPicker::IOPicker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IOPicker)
{
    ui->setupUi(this);
    inputs = new CInputs();
}

IOPicker::~IOPicker()
{
    delete inputs;
    delete ui;
}

void IOPicker::savePickersSettings(QSettings *settings)
{
    settings->setValue("main/buffer_size", SndController::Instance()->getSystemBufferMsSize());
    settings->setValue("main/channels_count", SndController::Instance()->getChannelsCount());
    settings->setValue("main/driver_out", out_guid);

    settings->beginGroup("inputs");
    settings->remove("");
    settings->endGroup();

    settings->setValue("inputs/inputs_count", inputs->count());
    CInputs::const_iterator iter_var = inputs->constBegin();
    while (iter_var != inputs->constEnd()) {
        settings->setValue("inputs/active_"+QString::number(iter_var.key()), iter_var.value().active);
        settings->setValue("inputs/id_"+QString::number(iter_var.key()), iter_var.value().device_id);
        settings->setValue("inputs/guid_"+QString::number(iter_var.key()), iter_var.value().device_guid);
        settings->setValue("inputs/fname_"+QString::number(iter_var.key()), iter_var.value().function_name);
        settings->setValue("inputs/buffer_"+QString::number(iter_var.key()), iter_var.value().recording_buffer_size);
        settings->setValue("inputs/latency_"+QString::number(iter_var.key()), iter_var.value().latency);
        settings->setValue("inputs/drift_"+QString::number(iter_var.key()), iter_var.value().drift);
        ++iter_var;
    }
}

void IOPicker::loadPickersSettings(QSettings *settings)
{
    int channels_cnt = settings->value("main/channels_count", 2).toInt();
    if (channels_cnt<=0 || channels_cnt>8) channels_cnt=2;
    on_spinBox_channels_valueChanged(channels_cnt);
    on_spinBox_play_buffer_valueChanged(settings->value("main/buffer_size", 1000).toInt());
    out_guid = settings->value("main/driver_out", "").toString();
    playing_devices(ui->comboBox_soundcard, out_guid);
    if (out_index>=0) {
        SndController::Instance()->setOutDriverIndex(out_index);
    }

    QString guid;

    ui->checkBox_in_active->setChecked(settings->value("inputs/active_0", false).toBool());
    ui->spinBox_in_buffer->setValue(settings->value("inputs/buffer_0", 100).toInt());
    ui->spinBox_in_latency->setValue(settings->value("inputs/latency_0", 50).toInt());
    ui->lineEdit_in_function->setText(settings->value("inputs/fname_0", "mic1").toString());
    guid = settings->value("inputs/guid_0", "").toString();
    recording_devices(ui->comboBox_in_soundcard, guid, 0);

    ui->checkBox_in_active_2->setChecked(settings->value("inputs/active_1", false).toBool());
    ui->spinBox_in_buffer_2->setValue(settings->value("inputs/buffer_1", 100).toInt());
    ui->spinBox_in_latency_2->setValue(settings->value("inputs/latency_1", 50).toInt());
    ui->lineEdit_in_function_2->setText(settings->value("inputs/fname_1", "mic2").toString());
    guid = settings->value("inputs/guid_1", "").toString();
    recording_devices(ui->comboBox_in_soundcard_2, guid, 1);
}

void IOPicker::setOutChannelsCount(unsigned int new_channels_count)
{
    ui->spinBox_channels->setValue(new_channels_count);
    SndController::Instance()->setChannelsCount(new_channels_count);
}

void IOPicker::setOutBuffer(unsigned int new_buffer_size)
{
    ui->spinBox_play_buffer->setValue(new_buffer_size);
    SndController::Instance()->setSystemBufferMsSize(new_buffer_size);
}

int IOPicker::getOutIndex()
{
    return out_index;
}

void IOPicker::on_spinBox_play_buffer_valueChanged(int arg1)
{
    emit play_buffer_changed(arg1);
    SndController::Instance()->setSystemBufferMsSize(arg1);
}

void IOPicker::on_spinBox_channels_valueChanged(int arg1)
{
    emit channels_count_changed(arg1);
    SndController::Instance()->setChannelsCount(arg1);
}

void IOPicker::on_toolButton_refresh_clicked()
{
    playing_devices(ui->comboBox_soundcard, out_guid);
}

void IOPicker::on_toolButton_in_refresh_clicked()
{
    recording_devices(ui->comboBox_in_soundcard, "", 0);
}

void IOPicker::on_toolButton_in_refresh_2_clicked()
{
    recording_devices(ui->comboBox_in_soundcard_2, "", 1);
}

void IOPicker::playing_devices(QComboBox *combobox, QString sel_guid)
{
    out_index = -1;
    if (sel_guid.isEmpty()) {
        sel_guid = combobox->currentIndex()>=0 ? combobox->itemData(combobox->currentIndex()).toString() : "";
    }
    combobox->clear();
    SndController *controller = SndController::Instance();
    FMOD::System *system = controller->getFmodSystem();
    FMOD_RESULT result;
    int numDrivers = 0;
    result = system->getNumDrivers(&numDrivers);
    controller->ERRCHECK(result);

    printf("PLAYING DEVICES:\r\n");
    for (int i=0; i<numDrivers; i++) {
        char name[256];
        char guid_str[64];
        int systemrate;
        int channels;
        FMOD_GUID guid;
        result = system->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, NULL, &channels);
        controller->ERRCHECK(result);

        sprintf(guid_str, "%08X-%04X-%04X-%04X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0] << 8 | guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

        combobox->addItem(QString(name), QString(guid_str));

        if (QString(guid_str) == sel_guid) {
            combobox->setCurrentIndex(i);
            out_index = i;
            out_guid = QString(guid_str);
        }

        printf("%c %2d. %s", out_index==i ? '>' : ' ', i, name);
        printf(" %dKHz %dch {%s}", systemrate / 1000, channels, guid_str);
        printf("\r\n");
    }

    if (out_index<0 && combobox->count()>0) {
        out_index = 0;
        out_guid = combobox->itemData(out_index).toString();
        combobox->setCurrentIndex(out_index);
    }
}

void IOPicker::recording_devices(QComboBox *combobox, QString sel_guid, int in_number)
{
    if (sel_guid.isEmpty()) {
        sel_guid = combobox->currentIndex()>=0 ? combobox->itemData(combobox->currentIndex()).toString() : "";
    }

    combobox->clear();
    SndController *controller = SndController::Instance();
    FMOD::System *system = controller->getFmodSystem();
    FMOD_RESULT result;
    int numDrivers = 0;
    int cindex = -1;

    result = system->getRecordNumDrivers(NULL, &numDrivers);
    controller->ERRCHECK(result);

    printf("RECORDING DEVICES:\r\n");
    for (int i=0; i<numDrivers; i++) {
        char name[256];
        char guid_str[64];
        int sampleRate;
        int channels;
        FMOD_GUID guid;
        FMOD_DRIVER_STATE state;
        result = system->getRecordDriverInfo(i, name, sizeof(name), &guid, &sampleRate, NULL, &channels, &state);
        controller->ERRCHECK(result);

        sprintf(guid_str, "%08X-%04X-%04X-%04X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0] << 8 | guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

        combobox->addItem(QString(name), QString(guid_str));

        bool isRecording = false;
        system->isRecording(i, &isRecording);

        if (QString(guid_str) == sel_guid) {
            combobox->setCurrentIndex(i);
            cindex = i;
        }

        printf("%c %2d. %s", cindex==i ? '>' : ' ', i, name);
        printf(" %dKHz %dch {%s}", sampleRate / 1000, channels, guid_str);
        printf(" (%s) (%s)", (state & FMOD_DRIVER_STATE_CONNECTED) ? "Connected" : "Unplugged", isRecording ? "Recording" : "Not recording");
        printf("\r\n");
    }

    if (cindex<0 && combobox->count()>0) {
        cindex = 0;
        sel_guid = combobox->itemData(cindex).toString();
        combobox->setCurrentIndex(cindex);
    }

    InputDeviceInfo devinfo;
    devinfo.device_guid = sel_guid;
    devinfo.device_id = cindex;
    devinfo.drift = 0;

    if (in_number==0) {
        devinfo.active = ui->checkBox_in_active->isChecked();
        devinfo.function_name = ui->lineEdit_in_function->text();
        devinfo.latency = ui->spinBox_in_latency->value();
        devinfo.recording_buffer_size = ui->spinBox_in_buffer->value();
    } else if (in_number==1) {
        devinfo.active = ui->checkBox_in_active_2->isChecked();
        devinfo.function_name = ui->lineEdit_in_function_2->text();
        devinfo.latency = ui->spinBox_in_latency_2->value();
        devinfo.recording_buffer_size = ui->spinBox_in_buffer_2->value();
    } else {
        devinfo.active = false;
        devinfo.function_name = "";
        devinfo.latency = 50;
        devinfo.recording_buffer_size = 25;
    }

    inputs->insert(in_number, devinfo);
}

void IOPicker::on_checkBox_in_active_toggled(bool checked)
{
    ui->widget_in_1->setEnabled(checked);
}

void IOPicker::on_checkBox_in_active_2_toggled(bool checked)
{
    ui->widget_in_2->setEnabled(checked);
}

void IOPicker::on_comboBox_soundcard_currentIndexChanged(int index)
{
    if (index>=0) {
        out_index = index;
        out_guid = ui->comboBox_soundcard->itemData(out_index).toString();
        SndController::Instance()->setOutDriverIndex(out_index);
    }
}
