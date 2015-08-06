#ifndef C_SCRIPTRECORD_H
#define C_SCRIPTRECORD_H

#include "FW/variant.h"
#include "FW/macro.h"
#include "FW/RC/real_record.h"
#include <QFile>

class C_Record;
class C_RecordFactory;
class C_DataState;
class C_DataState;
class C_RecordStruct;
class C_StringRecord;
class C_BoolRecord;

class C_ScriptRecord: public C_Record{

public:

    explicit            C_ScriptRecord(QString name, C_Variant* parent=0);

    QString             Id()        const override;
    QString             Name()      const override;
    QString             Class()     const override;
    QString             Script()    const override;
    C_RecordStruct*     Struct()    const override;

    void 			    GetState(C_DataState& state) override;
    void                SetState(C_DataState& value_list) override;

    void                ShowEditor(C_Document& document) override;

    C_POINTER           (Records,    C_RecordStruct)
    C_POINTER           (FileName,   C_StringRecord)
    C_POINTER           (Code,       C_StringRecord)
    C_POINTER           (IsFromFile, C_BoolRecord)

    QString             Value() const override;
    void                SetValue(QString) override;

private:

    friend class        C_UiScriptEditor;
    friend class        C_ScriptRecordFactory;
};

class C_ScriptRecordFactory : public C_RecordFactory {
public:

    C_SINGLETON_CLASS   (C_ScriptRecordFactory)

    QString             RecordClass() const override;

    C_Record*           CreateInstance(QString name, QString value, C_Variant* parent=0) override;
    C_Record*           CreateInstance(C_DataState& state, C_Variant* parent=0) override;

};

#endif // C_SCRIPTRECORD_H