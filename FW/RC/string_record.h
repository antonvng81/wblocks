#ifndef C_STRING_RECORD_H
#define C_STRING_RECORD_H

#include "FW/macro.h"
#include "FW/RC/record.h"

class C_StateReader;
class C_StateWriter;

class C_StringRecord : public C_Record
{
public:

    explicit C_StringRecord( C_StateWriter& state, C_Variant* parent = 0, C_RecordStruct* root = 0 );
    C_StringRecord( QString id, QString name, QString value, C_Variant* parent = 0, C_RecordStruct* root = 0 );
    ~C_StringRecord() override;

    QWidget*            PropertyWidget(C_Controller& controller ) override;

private:

    friend class        C_StringRecordFactory;
};

class C_StringRecordFactory : public C_RecordFactory
{
public:

    SINGLETON_CLASS     ( C_StringRecordFactory )

    C_StringRecordFactory();

    C_Record*           CreateInstance( QString name, QString value, C_Variant* parent = 0, C_RecordStruct* root = 0 ) override;
    C_Record*           CreateInstance( C_StateWriter& state, C_Variant* parent = 0, C_RecordStruct* root = 0 ) override;
};

#endif // C_STRING_RECORD_H
