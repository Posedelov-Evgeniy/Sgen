#include "iopicker.h"
#include "ui_iopicker.h"

IOPicker::IOPicker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IOPicker)
{
    ui->setupUi(this);
    playing_devices(ui->comboBox_soundcard);
    recording_devices(ui->comboBox_in_soundcard);
    recording_devices(ui->comboBox_in_soundcard_2);
}

IOPicker::~IOPicker()
{
    delete ui;
}

void IOPicker::on_spinBox_play_buffer_valueChanged(int arg1)
{

}

void IOPicker::on_spinBox_channels_valueChanged(int arg1)
{

}

void IOPicker::on_toolButton_refresh_clicked()
{
    playing_devices(ui->comboBox_soundcard);
}

void IOPicker::on_toolButton_in_refresh_clicked()
{
    recording_devices(ui->comboBox_in_soundcard);
}

void IOPicker::on_toolButton_in_refresh_2_clicked()
{
    recording_devices(ui->comboBox_in_soundcard_2);
}

void IOPicker::playing_devices(QComboBox *combobox, QString sel_guid)
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
            cindex = i;
        }

        printf("%c %2d. %s", cindex==i ? '>' : ' ', i, name);
        printf(" %dKHz %dch {%s}", systemrate / 1000, channels, guid_str);
        printf("\r\n");
    }
}

void IOPicker::recording_devices(QComboBox *combobox, QString sel_guid)
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
}

