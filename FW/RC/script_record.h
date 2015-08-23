#ifndef C_SCRIPT_RECORD_H
#define C_SCRIPT_RECORD_H

#include "FW/RC/record.h"
#include "FW/macro.h"

class C_RecordStruct;
class C_StringRecord;
class C_BoolRecord;

class C_StateWriter;
class C_StateReader;
class C_FileRecord;

class C_ScriptRecord: public C_Record
{

public:

    explicit C_ScriptRecord( C_StateWriter& state, C_Variant* parent = 0 );
    C_ScriptRecord( QString id, QString name, QString value, C_Variant* parent = 0 );
    ~C_ScriptRecord() override;

    QString             Class() const override;
    QString             Script() const override;
    C_RecordStruct*     Struct() const override;
    void 			    GetState( C_StateReader& state ) override;
    void                SetState( C_StateWriter& value_list ) override;
    void                ShowEditor( C_Document& document ) override;
    QString             Value() const override;
    void                SetValue( QString ) override;

    M_POINTER           ( Records,    C_RecordStruct )
    M_POINTER           ( File,       C_FileRecord )
    M_POINTER           ( Code,       C_StringRecord )

private:

    friend class        C_UiScriptEditor;
    friend class        C_ScriptRecordFactory;
};

class C_ScriptRecordFactory : public C_RecordFactory
{
public:

    SINGLETON_CLASS   ( C_ScriptRecordFactory )

    QString             RecordClass() const override;

    C_Record*           CreateInstance( QString name, QString value, C_Variant* parent = 0 ) override;
    C_Record*           CreateInstance( C_StateWriter& state, C_Variant* parent = 0 ) override;
};

#endif // C_SCRIPT_RECORD_H
