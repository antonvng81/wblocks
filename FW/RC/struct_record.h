#ifndef C_STRUCT_RECORD_H
#define C_STRUCT_RECORD_H

#include "FW/RC/record.h"
#include "FW/macro.h"

class C_RecordStruct;

class C_StateWriter;
class C_StateReader;

class C_StructRecord : public C_Record
{
public:

    explicit C_StructRecord( C_StateWriter& state, C_Variant* parent = 0, C_RecordStruct* root = 0 );
    C_StructRecord( QString id, QString name, QString value, C_Variant* parent = 0, C_RecordStruct* root = 0 );
    ~C_StructRecord() override;

    QString             Class()  const override;
    C_RecordStruct*     Struct() override;
    QString             Script() override;

    void                GetState( C_StateReader& state ) override;
    void                SetState(C_StateWriter& state , C_RecordStruct* root = 0) override;
    void                EditProperties( C_Document& document ) override;

    QString             Value() override;
    void                SetValue( QString ) override;

    M_POINTER           ( Records, C_RecordStruct )

private:

    friend class        C_UiStructEditor;
    friend class        C_StructRecordFactory;
};

class C_StructRecordFactory : public C_RecordFactory
{
public:

    SINGLETON_CLASS   ( C_StructRecordFactory )

    C_Record*           CreateInstance(QString name, QString value, C_Variant* parent = 0 , C_RecordStruct* root = 0) override;
    C_Record*           CreateInstance( C_StateWriter& table, C_Variant* parent = 0, C_RecordStruct* root = 0 ) override;

    QString             RecordClass() const override;
};

#endif // C_STRUCT_RECORD_H
