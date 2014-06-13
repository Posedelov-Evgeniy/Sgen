/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../soundGEN/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[15];
    char stringdata[308];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 13),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 13),
QT_MOC_LITERAL(4, 40, 14),
QT_MOC_LITERAL(5, 55, 26),
QT_MOC_LITERAL(6, 82, 21),
QT_MOC_LITERAL(7, 104, 23),
QT_MOC_LITERAL(8, 128, 23),
QT_MOC_LITERAL(9, 152, 29),
QT_MOC_LITERAL(10, 182, 4),
QT_MOC_LITERAL(11, 187, 31),
QT_MOC_LITERAL(12, 219, 31),
QT_MOC_LITERAL(13, 251, 31),
QT_MOC_LITERAL(14, 283, 23)
    },
    "MainWindow\0sound_stopped\0\0sound_started\0"
    "sound_starting\0on_functionsButton_clicked\0"
    "on_pushButton_clicked\0on_pushButton_2_clicked\0"
    "on_pushButton_3_clicked\0"
    "on_doubleSpinBox_valueChanged\0arg1\0"
    "on_doubleSpinBox_3_valueChanged\0"
    "on_doubleSpinBox_2_valueChanged\0"
    "on_doubleSpinBox_4_valueChanged\0"
    "on_MainWindow_destroyed\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08,
       3,    0,   75,    2, 0x08,
       4,    0,   76,    2, 0x08,
       5,    0,   77,    2, 0x08,
       6,    0,   78,    2, 0x08,
       7,    0,   79,    2, 0x08,
       8,    0,   80,    2, 0x08,
       9,    1,   81,    2, 0x08,
      11,    1,   84,    2, 0x08,
      12,    1,   87,    2, 0x08,
      13,    1,   90,    2, 0x08,
      14,    0,   93,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->sound_stopped(); break;
        case 1: _t->sound_started(); break;
        case 2: _t->sound_starting(); break;
        case 3: _t->on_functionsButton_clicked(); break;
        case 4: _t->on_pushButton_clicked(); break;
        case 5: _t->on_pushButton_2_clicked(); break;
        case 6: _t->on_pushButton_3_clicked(); break;
        case 7: _t->on_doubleSpinBox_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->on_doubleSpinBox_3_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->on_doubleSpinBox_2_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: _t->on_doubleSpinBox_4_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->on_MainWindow_destroyed(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
