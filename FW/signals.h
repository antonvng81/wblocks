#ifndef C_SIGNALS_H
#define C_SIGNALS_H

#include "FW/macro.h"

#include <QObject>

class C_Record;
class C_UiMainWindow;
class C_Document;

class C_Signals : public QObject
{
    Q_OBJECT

public:
                        C_Signals(C_Document& document,C_UiMainWindow& window, QObject* parent=0);

    C_POINTER           (Document,   C_Document)
    C_POINTER           (MainWindow, C_UiMainWindow)

    void                ConnectSignals();

public slots:

    void                OnRecordsChanged();
    void                OnScriptChanged();
    void                OnSceneChanged();

    void                OnActionDocumentLoad();
    void                OnActionDocumentSave();
    void                OnActionSQLDocumentLoad();
    void                OnActionSQLDocumentSave();

signals:

    void                RecordsChanged();
    void                ScriptChanged();
    void                SceneChanged();

};

#endif // C_SIGNALS_H