#include "FW/document.h"
#include "FW/UI/ui_main_window.h"
#include "FW/UI/ui_record_explorer.h"
#include "FW/UI/ui_add_record.h"
#include <QFileDialog>
#include "FW/RC/record_struct.h"
#include "FW/SC/scene.h"
#include "FW/clipboard.h"
#include <QList>
#include "ui_mainwindow.h"
#include <FW/UI/ui_find_record.h>
#include <QWebFrame>
#include <FW/UI/ui_code_editor_container.h>

C_Events::C_Events( C_Document& document, C_UiMainWindow& main_window, QObject* parent )
    : QObject( parent )
{
    m_Document = &document;
    m_MainWindow = &main_window;
}

C_Events::~C_Events()
{
    // void
}

void C_Events::ConnectEvents()
{
    connect(
        this,
        C_Events::RecordsChanged,
        this,
        C_Events::OnRecordsChanged );

    connect(
        this,
        C_Events::SceneChanged,
        this,
        C_Events::OnSceneChanged );

    connect(
        this,
        C_Events::ClientScriptChanged,
        this,
        C_Events::OnClientScriptChanged );

    connect(
        this,
        C_Events::RecordExplorerChanged,
        this,
        C_Events::OnRecordExplorerChanged );

    connect(
        this,
        C_Events::FileExplorerChanged,
        this,
        C_Events::OnFileExplorerChanged );

    connect(
        this,
        C_Events::CodeEditorContainerChanged,
        this,
        C_Events::OnCodeEditorContainerChanged );
}

void C_Events::OnRecordExplorerChanged()
{
    MainWindow().UpdateMenubar();
}

void C_Events::OnFileExplorerChanged()
{
    MainWindow().UpdateMenubar();
}

void C_Events::OnRecordsChanged()
{
    MainWindow().UpdateRecordExplorer();
    Document().UpdateScript();
    Document().UpdateScene();
}

void C_Events::OnClientScriptChanged()
{
    MainWindow().UpdateClientScriptView();
}

void C_Events::OnCodeEditorContainerChanged()
{
    MainWindow().UpdateMenubar();
}

void C_Events::OnSceneChanged()
{
    MainWindow().UpdateSceneView();
}

void C_Events::OnActionLoadFile()
{
    QString file_name = QFileDialog::getOpenFileName( &MainWindow(),
                        tr( "Load File" ),
                        "",
                        tr( "JS Blocks Files (*.blk)" ) );

    if( file_name.isEmpty() )
        return;

    QFile file( file_name );
    Document().FileLoad( file );

}

void C_Events::OnActionSaveFile()
{
    QString file_name = QFileDialog::getSaveFileName( &MainWindow(),
                        tr( "Save File" ),
                        tr( "untitled.blk" ),
                        tr( "JS Blocks Files (*.blk)" ) );

    if( file_name.isEmpty() )
        return;

    QFile file( file_name );
    Document().FileSave( file );
}

void C_Events::OnActionLoadSQL()
{
    QString file_name = QFileDialog::getOpenFileName( &MainWindow(),
                        tr( "Load SQL Database File" ),
                        "",
                        tr( "SQL Files (*.sql)" ) );

    if( file_name.isEmpty() )
        return;

    Document().DatabaseLoad( file_name );
}

void C_Events::OnActionSaveSQL()
{
    QString file_name = QFileDialog::getSaveFileName( &MainWindow(),
                        tr( "Save SQL Database File" ),
                        tr( "untitled.sql" ),
                        tr( "SQL Files (*.sql)" ) );

    if( file_name.isEmpty() )
        return;

    Document().DatabaseSave( file_name );
}

void C_Events::OnActionSaveClientScript()
{
    QString file_name = QFileDialog::getSaveFileName( &Document().MainWindow(),
                        tr( "Save Java Script File" ),
                        tr( "untitled.js" ),
                        tr( "JS Files (*.js)" ) );

    if( file_name.isEmpty() )
        return;

    Document().Script().Generate( Document().Records() );
    emit Document().Events().ClientScriptChanged();
    C_Document::SaveTextFile( file_name, Document().Script().StringList().join( "" ) );
}

void C_Events::OnActionEditRecord()
{
    long action_flags =
        Document()
        .Context()
        .Records()
        .Flags() ;

    bool has_selection =
        Document()
        .MainWindow()
        .RecordExplorer()
        .HasSelection();

    if( ( action_flags & FLAG_ACTION_EDIT ) && has_selection )
    {
        auto record =
            static_cast<C_Record*>(
                *MainWindow()
                .RecordExplorer()
                .Selection()
                .begin()
            );

        record->ShowEditor( Document() );
    }
}

void C_Events::OnActionRemoveRecord()
{
    long action_flags =
        Document()
        .Context()
        .Records()
        .Flags() ;

    bool has_selection =
        Document()
        .MainWindow()
        .RecordExplorer()
        .HasSelection();

    if( ( action_flags & FLAG_ACTION_REMOVE ) && has_selection )
    {
        QList<C_Record*> selection_list =
            Document()
            .MainWindow()
            .RecordExplorer()
            .Selection();

        if( selection_list.size() == 1 )
        {
            if( !C_Document::AcceptMessage( tr( "Do you want to remove this record?" ) ) )
                return;
        }
        else
        {
            if( !C_Document::AcceptMessage( tr( "Do you want to remove these records?" ) ) )
                return;
        }

        for( auto record : selection_list )
            delete record;

        emit Document().Events().RecordsChanged();
    }
}

void C_Events::OnActionAddRecord()
{

    int position = -1;
    bool has_selection =
        Document()
        .MainWindow()
        .RecordExplorer()
        .HasSelection();

    if ( has_selection )
    {
        auto front =
            Document()
            .MainWindow()
            .RecordExplorer()
            .Selection()
            .front();
        position =
            Document()
            .Context()
            .Records()
            .GetIndex( front );
    }

    QWidget* dialog;

    if( position >= 0 )
        dialog = new C_UiAddRecord( Document(), position, &Document().MainWindow() );
    else
        dialog = new C_UiAddRecord( Document(), -1,  &Document().MainWindow() );

    dialog->show();
}

void C_Events::OnActionAddSceneItem()
{
    long action_flags =
        Document()
        .Context()
        .Records()
        .Flags();

    if( ( action_flags & FLAG_ACTION_ADD_SCENE ) &&
            MainWindow().RecordExplorer().HasSelection() )
    {
        QList<C_Record*> selection_list =
            Document()
            .MainWindow()
            .RecordExplorer()
            .Selection();

        for( auto record : selection_list )
        {
            Document()
            .Context()
            .Scene()
            .CreateItem( *record );
        }

        emit Document().Events().SceneChanged();
    }
}

void C_Events::OnActionCopyRecord()
{
    long action_flags =
        Document()
        .Context()
        .Records()
        .Flags();
    bool has_selection =
        Document()
        .MainWindow()
        .RecordExplorer()
        .HasSelection();

    if( ( action_flags & FLAG_ACTION_COPY ) && has_selection )
    {
        Document()
        .Clipboard()
        .Copy(
            Document()
            .MainWindow()
            .RecordExplorer()
            .Selection()
        );

        Document().MainWindow().UpdateMenubar();
    }
}

void C_Events::OnActionPasteRecord()
{

    long action_flags =
        Document()
        .Context()
        .Records()
        .Flags();

    bool has_selection =
        Document()
        .MainWindow()
        .RecordExplorer()
        .HasSelection();

    int position = -1;

    if( action_flags & FLAG_ACTION_PASTE )
    {

        if( has_selection )
        {
            auto selection_list =
                Document()
                .MainWindow()
                .RecordExplorer()
                .Selection();

            position =
                Document()
                .MainWindow()
                .RecordExplorer()
                .Records()
                .GetIndex( selection_list.front() );
        }

        Document()
        .Clipboard()
        .Paste(
            Document()
            .Context()
            .Records(),
            position
        );

        Document().Clipboard().Clear();
        emit Document().Events().RecordsChanged();
    }
}

void C_Events::OnActionCutRecord()
{
    long action_flags =
        Document()
        .Context()
        .Records()
        .Flags();

    bool has_selection =
        Document()
        .MainWindow()
        .RecordExplorer()
        .HasSelection();

    if( ( action_flags & FLAG_ACTION_CUT ) && has_selection )
    {
        auto selection_list =
            Document()
            .MainWindow()
            .RecordExplorer()
            .Selection();

        for( auto record : selection_list )
        {
            record->SetParent( 0 );
            auto items =
                Document()
                .Scene()
                .FromRecord( *record );

            for( auto item : items )
                delete item;
        }

        Document().Clipboard().Copy( selection_list );
        emit Document().Events().RecordsChanged();
    }
}

void C_Events::OnActionNewFile()
{
    Document().Clear();
}

void C_Events::OnActionFindRecord()
{
    QWidget* dialog = new C_UiFindRecord( Document(), &MainWindow() );
    dialog->show();
}

void C_Events::OnActionNewEditorFile()
{
    QString file_name = QFileDialog::getSaveFileName( &MainWindow(),
                        tr( "New File" ),
                        tr( "untitled.js" ),
                        tr( "JS Files (*.js)" ) );

    if( file_name.isEmpty() )
        return;

    C_Document::SaveTextFile( file_name, "//FILE: " + file_name.split( "/" ).back() );

    MainWindow().CodeEditorContainer().Append( file_name );
    emit CodeEditorContainerChanged();
}

void C_Events::OnActionCloseEditorFile()
{
    if( C_Document::AcceptMessage( tr( "Save changes?" ) ) )
        MainWindow().CodeEditorContainer().SaveCurrent();

    MainWindow().CodeEditorContainer().CloseCurrent();
    emit CodeEditorContainerChanged();
}

void C_Events::OnActionCloseAllEditorFiles()
{
    if( C_Document::AcceptMessage( tr( "Save changes?" ) ) )
        MainWindow().CodeEditorContainer().SaveAll();

    MainWindow().CodeEditorContainer().CloseAll();
    emit CodeEditorContainerChanged();
}

void C_Events::OnActionSaveEditorFile()
{
    MainWindow().CodeEditorContainer().SaveCurrent();
}

void C_Events::OnActionSaveAllEditorFiles()
{
    MainWindow().CodeEditorContainer().SaveAll();
}

void C_Events::OnActionLoadEditorFile()
{
    QString file_name = QFileDialog::getOpenFileName( &MainWindow(),
                        tr( "New File" ),
                        tr( "untitled.js" ),
                        tr( "JS Files (*.js)" ) );

    if( !QFileInfo( file_name ).exists() )
    {
        C_Document::Message( tr( "File doesn't exists" ) );
        return;
    }

    if( MainWindow().CodeEditorContainer().HasFile( file_name ) )
    {
        if( C_Document::AcceptMessage( tr( "File already loaded.\n\nLoad again?" ) ) )
        {
            MainWindow().CodeEditorContainer().Clear( file_name );
            MainWindow().CodeEditorContainer().Append( file_name );
            emit CodeEditorContainerChanged();
        }
        return;
    }

    MainWindow().CodeEditorContainer().Append( file_name );
    emit CodeEditorContainerChanged();
}

void C_Events::OnActionExit()
{
    Document().MainWindow().close();
}

void C_Events::OnActionRunClientScript()
{
    Document().MainWindow().UpdateWebView();
}

void C_Events::OnActionUpdateClientScript()
{
    Document().UpdateScript();
}



