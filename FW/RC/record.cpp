#include "FW/RC/record.h"
#include "FW/document.h"
#include "FW/UI/ui_main_window.h"
#include "FW/UI/ui_text_editor_container.h"
#include "FW/UI/ui_record_value_editor.h"
#include "FW/UI/PR/ui_record_properties.h"
#include "FW/ST/state_reader.h"
#include "FW/ST/state_writer.h"
#include <QCoreApplication>

/////////////////////////////////////////////////////////////////////////////////
/// STATIC

long C_RecordFactory::m_IdCount = 0;


QString C_RecordFactory::GenerateId()
{
    return QString::number( m_IdCount++ );
}

QString C_RecordFactory::IdCount()
{
    return QString::number( m_IdCount );
}

/////////////////////////////////////////////////////////////////////////////////
/// NON STATIC

C_Record::C_Record( QString id, QString name, QString value, C_Variant* parent, C_RecordStruct* ) :
    C_Variant( parent ),
    m_Id( id ),
    m_Name( name ),
    m_Class( "Record" ),
    m_Value( value )
{
    SetFlags( FLAG_ACTION_ALL );
}

C_Record::C_Record( C_StateWriter& state, C_Variant* parent, C_RecordStruct* ):
    C_Record( "", "", "", parent )
{
    SetState( state );
}

C_Record::~C_Record()
{
    // void
}

void C_Record::SetValue( QString value )
{
    m_Value = value;
}

QString C_Record::Value()
{
    return m_Value;
}


void C_Record::GetState( C_StateReader& state )
{
    QStringList row;
    row.append( m_Id );
    row.append( m_Name );
    row.append( m_Value );
    row.append( m_Class );
    state.Read( row );
}

void C_Record::SetState( C_StateWriter& state, C_RecordStruct* )
{
    QStringList row;
    state.Write( row );

    if( state.Flags() & FLAG_STATE_NEWID )
        m_Id    = C_RecordFactory::GenerateId();
    else
        m_Id    = row[0];

    m_Name  = row[1];
    m_Value = row[2];
    m_Class = row[3];
}

void C_Record::EditProperties( C_Document& document )
{
    QWidget* dialog = new C_UiRecordProperties( *this, document, &document.MainWindow() );
    dialog->show();
}

void C_Record::OpenInEditor( C_Document& document )
{
    // DEFAULT OPEN OPERATION

    auto& main_window = document.MainWindow();
    QString editor_id = "RECORD:TEXT:" + Id();
    QString editor_name = Name();

    if( main_window.TextEditorContainer().HasId( editor_id ) )
    {
        if( C_Document::AcceptMessage( QCoreApplication::translate( "C_Record", "Record already opened.\n\nLoad again?" ) ) )
        {
            main_window.TextEditorContainer().Close( editor_id );
            main_window.TextEditorContainer().Append( new C_UiRecordValueEditor( editor_id, editor_name, *this ) );
            emit document.Events().TextEditorContainerChanged();
            main_window.SetCurrentTab( MAINWINDOW_TAB_EDITOR );
        }

        return;
    }

    main_window.TextEditorContainer().Append( new C_UiRecordValueEditor( editor_id, editor_name, *this ) );
    emit document.Events().TextEditorContainerChanged();
    main_window.SetCurrentTab( MAINWINDOW_TAB_EDITOR );
}

C_RecordStruct* C_Record::Struct()
{
    return 0;
}

QStringList C_Record::Script()
{
    return QStringList();
}

QStringList C_Record::Html()
{
    return QStringList();
}

QString C_Record::FullName() const
{
    auto* record_struct = static_cast<C_RecordStruct*>( Parent() );

    if( record_struct->Name() != "root" )
    {
        C_Record* record = static_cast<C_Record*>( record_struct->Parent() );
        return record->FullName() + "." + Name();
    }

    return Name();
}

C_RecordFactory::C_RecordFactory() :
    m_RecordClass( "Record" )
{
    // void
}

C_Record* C_RecordFactory::CreateInstance( QString name, QString value, C_Variant* parent , C_RecordStruct* root )
{
    return new C_Record( C_RecordFactory::GenerateId(), name, value, parent, root );
}

C_Record* C_RecordFactory::CreateInstance( C_StateWriter& state, C_Variant* parent , C_RecordStruct* root )
{
    return new C_Record( state, parent, root );
}






