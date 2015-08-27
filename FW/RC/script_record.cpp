#include "FW/RC/script_record.h"
#include "FW/UI/PR/ui_script_record_properties.h"
#include "FW/UI/ui_main_window.h"
#include "FW/document.h"
#include "FW/UI/ui_record_value_editor.h"
#include "FW/UI/ui_text_editor_container.h"
#include "FW/ST/state_reader.h"
#include "FW/ST/state_writer.h"
#include <QCoreApplication>


C_ScriptRecord::C_ScriptRecord( QString id, QString name, QString value, C_Variant* parent, C_RecordStruct* root ):
    C_Record( id, name, value, parent, root )
{
    m_Class = "Script";
}

C_ScriptRecord::C_ScriptRecord( C_StateWriter& state, C_Variant* parent, C_RecordStruct* root ):
    C_Record( "", "", "", parent, root )
{
    m_Class = "Script";
    SetState( state, root );
}

C_ScriptRecord::~C_ScriptRecord()
{
    //void
}

QStringList C_ScriptRecord::Script()
{
    return QStringList( Value() );
}

void C_ScriptRecord::EditProperties( C_Document& document )
{
    QWidget* dialog = new C_UiScriptRecordProperties( *this, document, &document.MainWindow() );
    dialog->show();
}

void C_ScriptRecord::OpenInEditor( C_Document& document )
{
    // DEFAULT OPEN OPERATION (WILL BE EXTENDED)

    auto& main_window = document.MainWindow();
    QString editor_id = "RECORD:TEXT:" + Id();
    QString editor_name = Name();

    if( main_window.TextEditorContainer().HasId( editor_id ) )
    {
        if( C_Document::AcceptMessage(
                    QCoreApplication::translate( "C_ScriptRecord", "Record already opened.\n\nLoad again?" ) ) )
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

C_ScriptRecordFactory::C_ScriptRecordFactory()
{
    m_RecordClass = "Script";
}

C_Record* C_ScriptRecordFactory::CreateInstance( QString name, QString value, C_Variant* parent, C_RecordStruct* root )
{
    C_ScriptRecord* record = new C_ScriptRecord( C_RecordFactory::GenerateId(), name, value, parent, root );
    return record;
}

C_Record* C_ScriptRecordFactory::CreateInstance( C_StateWriter& state, C_Variant* parent, C_RecordStruct* root )
{
    C_ScriptRecord* record = new C_ScriptRecord( state, parent, root );
    return record;
}



