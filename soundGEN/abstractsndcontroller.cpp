#include "abstractsndcontroller.h"

AbstractSndController::AbstractSndController(QObject *parent) :
    SignalController(parent)
{

}

void AbstractSndController::ERRCHECK(FMOD_RESULT op_result)
{
    if (op_result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", op_result, FMOD_ErrorString(op_result));
        exit(-1);
    }
}
