/****************************************************************************
** Meta object code from reading C++ file 'sndcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../soundGEN/sndcontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sndcontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SndController_t {
    QByteArrayData data[7];
    char stringdata[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_SndController_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_SndController_t qt_meta_stringdata_SndController = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 8),
QT_MOC_LITERAL(2, 23, 0),
QT_MOC_LITERAL(3, 24, 7),
QT_MOC_LITERAL(4, 32, 7),
QT_MOC_LITERAL(5, 40, 3),
QT_MOC_LITERAL(6, 44, 4)
    },
    "SndController\0starting\0\0started\0stopped\0"
    "run\0stop\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SndController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06,
       3,    0,   40,    2, 0x06,
       4,    0,   41,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       5,    0,   42,    2, 0x0a,
       6,    0,   43,    2, 0x0a,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SndController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SndController *_t = static_cast<SndController *>(_o);
        switch (_id) {
        case 0: _t->starting(); break;
        case 1: _t->started(); break;
        case 2: _t->stopped(); break;
        case 3: _t->run(); break;
        case 4: _t->stop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SndController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SndController::starting)) {
                *result = 0;
            }
        }
        {
            typedef void (SndController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SndController::started)) {
                *result = 1;
            }
        }
        {
            typedef void (SndController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SndController::stopped)) {
                *result = 2;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SndController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SndController.data,
      qt_meta_data_SndController,  qt_static_metacall, 0, 0}
};


const QMetaObject *SndController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SndController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SndController.stringdata))
        return static_cast<void*>(const_cast< SndController*>(this));
    return QObject::qt_metacast(_clname);
}

int SndController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void SndController::starting()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SndController::started()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void SndController::stopped()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
