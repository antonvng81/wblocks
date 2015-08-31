#include "FW/RC/record_struct.h"
#include "FW/SC/scene.h"
#include "FW/clipboard.h"
#include "FW/document.h"
#include "FW/UI/ui_main_window.h"
#include "FW/UI/ui_record_explorer.h"
#include "FW/UI/ui_add_record.h"
#include "FW/UI/ui_editor_container.h"
#include "FW/htmlbuilder.h"
#include "FW/database.h"
#include "FW/clipboard.h"
#include "FW/config.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QFileDialog>
#include <QWebFrame>
#include <QMessageBox>
#include <FW/UI/ED/ui_text_editor.h>

///////////////////////////////////////////////////////////////////////////////////////////////
/// STATIC

QString C_Controller::LoadTextFile( QString file_name )
{
    QFile file( file_name );

    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        file.close();
        return QString();
    }

    QString text = file.readAll();
    file.close();
    return text;
}

void C_Controller::SaveTextFile( QString file_name, QString text )
{
    QFile file( file_name );

    if( !file.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        file.close();
        return;
    }

    QTextStream out( &file );
    out << text;
    file.close();
}

bool C_Controller::AcceptMessage( QString msg )
{
    QMessageBox msgBox;
    msgBox.setText( msg );
    msgBox.setStandardButtons( QMessageBox::Ok | QMessageBox::Cancel );
    msgBox.setDefaultButton( QMessageBox::Cancel );
    msgBox.exec();

    if( msgBox.result() == QMessageBox::Cancel )
        return false;

    return true;
}

void C_Controller::Message( QString msg )
{
    QMessageBox msgBox;
    msgBox.setText( msg );
    msgBox.exec();
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// NON STATIC

C_Controller::C_Controller(): C_Variant()
{
    m_HtmlBuilder       = new C_HtmlBuilder( this );
    m_Database          = new C_Database( this );
    m_Clipboard         = new C_Clipboard( this );
}

C_Controller::~C_Controller()
{
    // void
}

void C_Controller::ConnectSlots()
{
    connect(
        this,
        C_Controller::DirectoryChanged,
        this,
        C_Controller::OnDirectoryChanged );

    connect(
        this,
        C_Controller::RecordsChanged,
        this,
        C_Controller::OnRecordsChanged );

    connect(
        this,
        C_Controller::SceneChanged,
        this,
        C_Controller::OnSceneChanged );

    connect(
        this,
        C_Controller::HtmlCodeChanged,
        this,
        C_Controller::OnHtmlCodeChanged );

    connect(
        this,
        C_Controller::RecordExplorerChanged,
        this,
        C_Controller::OnRecordExplorerChanged );

    connect(
        this,
        C_Controller::FileExplorerChanged,
        this,
        C_Controller::OnFileExplorerChanged );

    connect(
        this,
        C_Controller::TextEditorContainerChanged,
        this,
        C_Controller::OnEditorContainerChanged );
}

void C_Controller::SetPropertyWidgetRecord( C_Record& record )
{
    MainWindow().SetPropertyWidget( record.PropertyWidget( *this ) );
}

void C_Controller::OpenRecordEditorWidget( C_Record& record )
{
    QString id = "RECORD:" + record.Id();

    if( MainWindow().TextEditorContainer().HasId( id ) )
    {
        if( C_Controller::AcceptMessage( QCoreApplication::translate( "C_Controller", "Record already opened.\n\nLoad again?" ) ) )
        {
            MainWindow().TextEditorContainer().Close( id );
            MainWindow().OpenEditorWidget( record.EditorWidget( id, *this ) );
        }

        return;
    }

    MainWindow().OpenEditorWidget( record.EditorWidget( id, *this ) );
}

void C_Controller::OpenFileEditorWidget( QString file_name )
{

    QString id = "FILE:" + file_name;

    if( MainWindow().TextEditorContainer().HasId( id ) )
    {
        if( !C_Controller::AcceptMessage( tr( "File already open. Load again?" ) ) )
            return;

        MainWindow().TextEditorContainer().Close( id );
    }

    C_UiTextEditor* text_editor;

    text_editor = new C_UiTextEditor( id, file_name, file_name.split( "/" ).back(), [file_name]( C_UiEditor & editor_base )
    {
        C_UiTextEditor& editor = static_cast<C_UiTextEditor&>( editor_base );
        C_Controller::SaveTextFile( file_name, editor.Text() );
    } );

    MainWindow().TextEditorContainer().Append( text_editor );
    MainWindow().TextEditorContainer().SetCurrent( id );
    MainWindow().SetCurrentTab( MAINWINDOW_TAB_EDITOR );
}


void C_Controller::OnDirectoryChanged()
{
    MainWindow().UpdateFileExplorer();
}

void C_Controller::OnRecordExplorerChanged()
{
    MainWindow().UpdateMenubar();
}

void C_Controller::OnFileExplorerChanged()
{
    MainWindow().UpdateMenubar();
}

void C_Controller::OnRecordsChanged()
{
    MainWindow().UpdateRecordExplorer();
    MainWindow().UpdateSceneView();

    Document().UpdateHtml();
}

void C_Controller::OnHtmlCodeChanged()
{
    MainWindow().UpdateHtmlCodeView();
}

void C_Controller::OnEditorContainerChanged()
{
    MainWindow().UpdateMenubar();
}

void C_Controller::OnSceneChanged()
{
    MainWindow().UpdateSceneView();
}

void C_Controller::OnActionLoadProjectFile()
{
    QString file_name =
        QFileDialog::getOpenFileName(
            &MainWindow(),
            tr( "Load File" ),
            Document().FileName(),
            tr( "Project Files (*.prj)" )
        );

    if( file_name.isEmpty() )
    {
        qDebug() << "File Selection failed";
        return;
    }

    int error = Document().LoadFile( file_name );

    if( !error )
    {
        qDebug() << "Load project successfull";

        Config().SetProjectFileName( Document().FileName() );
        Config().SetProjectPath( Document().Path() );
        QDir().setCurrent( Config().ProjectPath() );
        emit DirectoryChanged();
        MainWindow().SetTitle( Document().FileName() );

        return;
    }

    qDebug() << "Load project error. Code " << error;
}

void C_Controller::OnActionSaveProjectFile()
{
    QString file_name =
        QFileDialog::getSaveFileName(
            &MainWindow(),
            tr( "Save File" ),
            Document().FileName(),
            tr( "Project Files (*.prj)" )
        );

    if( file_name.isEmpty() )
    {
        qDebug() << "File Selection failed";
        return;
    }

    int error = Document().SaveFile( file_name );
    MainWindow().UpdateFileExplorer();

    if( !error )
    {
        qDebug() << "Save project successfull";

        Config().SetProjectFileName( Document().FileName() );
        Config().SetProjectPath( Document().Path() );
        QDir().setCurrent( Config().ProjectPath() );
        emit DirectoryChanged();
        MainWindow().SetTitle( Document().FileName() );

        return;
    }

    qDebug() << "Save project error. Code " << error;
}

void C_Controller::OnActionLoadProjectSQL()
{
    QString file_name =
        QFileDialog::getOpenFileName(
            &MainWindow(),
            tr( "Load SQL Database File" ),
            "",
            tr( "SQL Files (*.sql)" )
        );

    if( file_name.isEmpty() )
    {
        qDebug() << "File Selection failed";
        return;
    }

    int error = Document().LoadSQL( file_name );

    if( !error )
    {
        qDebug() << "Load project SQL successfull";

        Config().SetProjectFileName( Document().FileName() );
        Config().SetProjectPath( Document().Path() );
        QDir().setCurrent( Config().ProjectPath() );
        emit DirectoryChanged();
        MainWindow().SetTitle( Document().FileName() );

        return;
    }

    qDebug() << "Load project SQL error. Code " << error;
}

void C_Controller::OnActionSaveProjectSQL()
{
    QString file_name =
        QFileDialog::getSaveFileName(
            &MainWindow(),
            tr( "Save SQL Database File" ),
            tr( "untitled.sql" ),
            tr( "SQL Files (*.sql)" )
        );

    if( file_name.isEmpty() )
    {
        qDebug() << "File Selection failed";
        return;
    }

    int error = Document().SaveSQL( file_name );
    MainWindow().UpdateFileExplorer();

    if( !error )
    {
        qDebug() << "Load project SQL successfull";

        Config().SetProjectFileName( Document().FileName() );
        Config().SetProjectPath( Document().Path() );
        QDir().setCurrent( Config().ProjectPath() );
        emit DirectoryChanged();
        MainWindow().SetTitle( Document().FileName() );

        return;
    }

    qDebug() << "Load project SQL error. Code " << error;
}

void C_Controller::OnActionSaveHtmlCode()
{
    QString file_name =
        QFileDialog::getSaveFileName(
            &MainWindow(),
            tr( "Save Html File" ),
            tr( "untitled.html" ),
            tr( "Html Files (*.html)" )
        );

    if( file_name.isEmpty() )
    {
        qDebug() << "File Selection failed";
        return;
    }

    Document().UpdateHtml();

    C_Controller::SaveTextFile(
        file_name,
        Document().Html()
    );

    MainWindow().UpdateFileExplorer();
}

void C_Controller::OnActionChangePropertyWidget()
{
    long action_flags = Document().Context().Records().Flags() ;
    bool has_selection = MainWindow().RecordExplorer().HasSelection();

    if( ( action_flags & FLAG_ACTION_PROPERTIES ) && has_selection )
    {
        C_Record* record =
            static_cast<C_Record*>(
                *MainWindow().RecordExplorer().Selection().begin()
            );

        if( !( record->Flags() & FLAG_ACTION_PROPERTIES ) )
        {
            qDebug() << "FLAG_ACTION_PROPERTIES is disabled on Record:"
                     << record->Name();
            return;
        }

        record->PropertyWidget( *this );
    }
    else
    {
        qDebug() << "FLAG_ACTION_PROPERTIES is disabled on Struct:"
                 << Document().Context().Records().Name();
    }
}

void C_Controller::OnActionRemoveRecord()
{
    long action_flags = Document().Context().Records().Flags() ;
    bool has_selection = MainWindow().RecordExplorer().HasSelection();

    if( ( action_flags & FLAG_ACTION_REMOVE ) && has_selection )
    {
        auto selection_list = MainWindow().RecordExplorer().Selection();

        if( selection_list.size() == 1 )
        {
            if( !C_Controller::AcceptMessage( tr( "Remove Record?" ) ) )
                return;
        }
        else
        {
            if( !C_Controller::AcceptMessage( tr( "Do you want to remove these records?" ) ) )
                return;
        }

        for( C_Record* record : selection_list )
        {
            if( !( record->Flags() & FLAG_ACTION_REMOVE ) )
            {
                qDebug() << "FLAG_ACTION_REMOVE is disabled on Record:"
                         << record->Name();
                return;
            }
        }

        for( C_Record* record : selection_list )
            delete record;

        emit RecordsChanged();
    }
    else
    {
        qDebug() << "FLAG_ACTION_REMOVE is disabled on Struct:"
                 << Document().Context().Records().Name();
    }
}

void C_Controller::OnActionAddRecord()
{
    long action_flags = Document().Context().Records().Flags() ;

    if( !( action_flags & FLAG_ACTION_ADD ) )
    {
        qDebug() << "FLAG_ACTION_ADD is disabled on Struct:"
                 << Document().Context().Records().Name();
        return;
    }

    int position = -1;

    if ( MainWindow().RecordExplorer().HasSelection() )
    {
        C_Record* front = MainWindow().RecordExplorer().Selection().front();
        position = Document().Context().Records().GetIndex( front );
    }

    QWidget* dialog;

    if( position >= 0 )
    {
        dialog = new C_UiAddRecord( *this, Document().Context(), position, &MainWindow() );
        dialog->show();
        return;
    }

    dialog = new C_UiAddRecord( *this, Document().Context(), -1,  &MainWindow() );
    dialog->show();
}

void C_Controller::OnActionAddSceneItem()
{
    long action_flags = Document().Context().Records().Flags();

    if( ( action_flags & FLAG_ACTION_ADD_SCENE ) &&
            MainWindow().RecordExplorer().HasSelection() )
    {
        auto selection_list = MainWindow().RecordExplorer().Selection();

        for( C_Record* record : selection_list )
        {
            if( !( record->Flags() & FLAG_ACTION_ADD_SCENE ) )
            {
                qDebug() << "FLAG_ACTION_ADD_SCENE is disabled on Record:"
                         << record->Name();
                return;
            }
        }

        for( C_Record* record : selection_list )
            Document().Context().Scene().CreateItem( *record );

        MainWindow().SetCurrentTab( MAINWINDOW_TAB_SCENE );
        emit SceneChanged();
    }
    else
    {
        qDebug() << "FLAG_ACTION_ADD_SCENE is disabled on Struct:"
                 << Document().Context().Records().Name();
        return;
    }
}

void C_Controller::OnActionCopyRecord()
{
    long action_flags = Document().Context().Records().Flags();
    bool has_selection = MainWindow().RecordExplorer().HasSelection();

    if( ( action_flags & FLAG_ACTION_COPY ) && has_selection )
    {
        auto selection_list = MainWindow().RecordExplorer().Selection();

        for( C_Record* record : selection_list )
        {
            if( !( record->Flags() & FLAG_ACTION_CUT ) )
            {
                qDebug() << "FLAG_ACTION_CUT is disabled on Record:"
                         << record->Name();
                return;
            }
        }

        Clipboard().Copy( selection_list );
        MainWindow().UpdateMenubar();
    }
    else
    {
        qDebug() << "FLAG_ACTION_COPY is disabled on Struct:"
                 << Document().Context().Records().Name();
        return;
    }
}

void C_Controller::OnActionPasteRecord()
{
    long action_flags = Document().Context().Records().Flags();
    bool has_selection = MainWindow().RecordExplorer().HasSelection();
    int position = -1;

    if( action_flags & FLAG_ACTION_PASTE )
    {
        if( has_selection )
        {
            QList<C_Record*> selection_list = MainWindow().RecordExplorer().Selection();
            position = Document().Context().Records().GetIndex( selection_list.front() );
        }

        Clipboard().Paste(
            Document().Context().Records(),
            position
        );

        Clipboard().Clear();
        emit RecordsChanged();
    }
    else
    {
        qDebug() << "FLAG_ACTION_PASTE is disabled on Struct:"
                 << Document().Context().Records().Name();
        return;
    }
}

void C_Controller::OnActionCutRecord()
{
    long action_flags = Document().Context().Records().Flags();
    bool has_selection = MainWindow().RecordExplorer().HasSelection();

    if( ( action_flags & FLAG_ACTION_CUT ) && has_selection )
    {
        QList<C_Record*> selection_list = MainWindow().RecordExplorer().Selection();

        for( C_Record* record : selection_list )
        {
            if( !( record->Flags() & FLAG_ACTION_CUT ) )
            {
                qDebug() << "FLAG_ACTION_CUT is disabled on Record:"
                         << record->Name();
                return;
            }
        }

        for( C_Record* record : selection_list )
        {
            record->SetParent( 0 );

            auto items = Document().Scene().FromRecord( *record );

            for( auto item : items )
                delete item;
        }

        Clipboard().Copy( selection_list );
        emit RecordsChanged();
    }
    else
    {
        qDebug() << "FLAG_ACTION_CUT is disabled on Struct:"
                 << Document().Context().Records().Name();
        return;
    }
}

void C_Controller::OnActionOpenRecordInEditor()
{
    long action_flags = Document().Context().Records().Flags() ;
    bool has_selection = MainWindow().RecordExplorer().HasSelection();

    if( ( action_flags & FLAG_ACTION_OPEN ) && has_selection )
    {
        C_Record* record =
            static_cast<C_Record*>(
                *MainWindow().RecordExplorer().Selection().begin()
            );

        if( !( record->Flags() & FLAG_ACTION_OPEN ) )
        {
            qDebug() << "FLAG_ACTION_OPEN is disabled on Record:"
                     << record->Name();
            return;
        }

        OpenRecordEditorWidget( *record );
    }
    else
    {
        qDebug() << "FLAG_ACTION_OPEN is disabled on Struct:"
                 << Document().Context().Records().Name();
    }
}

void C_Controller::OnActionNewProjectFile()
{
    Document().Clear();
    MainWindow().SetTitle("");
}

void C_Controller::OnActionNewFile()
{
    QString file_name =
        QFileDialog::getSaveFileName(
            &MainWindow(),
            tr( "New File" ),
            tr( "untitled.js" ),
            tr( "JS Files (*.js)" )
        );

    if( file_name.isEmpty() )
        return;

    if( QFileInfo( file_name ).exists() )
    {
        if( !C_Controller::AcceptMessage( tr( "File already exists. Overwrite?" ) ) )
            return;
    }

    C_Controller::SaveTextFile( file_name, "//FILE: " + file_name.split( "/" ).back() );
    emit FileExplorerChanged();

    OpenFileEditorWidget( file_name );
}

void C_Controller::OnActionCloseFile()
{
    if( C_Controller::AcceptMessage( tr( "Save changes?" ) ) )
    {
        MainWindow().TextEditorContainer().SaveCurrent();
        emit FileExplorerChanged();
    }

    MainWindow().TextEditorContainer().CloseCurrent();
    emit TextEditorContainerChanged();
}

void C_Controller::OnActionCloseAllFiles()
{
    if( C_Controller::AcceptMessage( tr( "Save changes?" ) ) )
    {
        MainWindow().TextEditorContainer().SaveAll();
        emit FileExplorerChanged();
    }

    MainWindow().TextEditorContainer().CloseAll();
    emit TextEditorContainerChanged();
}

void C_Controller::OnActionSaveFile()
{
    MainWindow().TextEditorContainer().SaveCurrent();
}

void C_Controller::OnActionSaveAllFiles()
{
    MainWindow().TextEditorContainer().SaveAll();
}

void C_Controller::OnActionLoadFile()
{
    QString file_name =
        QFileDialog::getOpenFileName(
            &MainWindow(),
            tr( "New File" ),
            tr( "untitled.js" ),
            tr( "JS Files (*.js)" )
        );

    if( !QFileInfo( file_name ).exists() )
    {
        C_Controller::Message( tr( "File doesn't exists" ) );
        return;
    }

    OpenFileEditorWidget( file_name );
}

void C_Controller::OnActionExit()
{
    MainWindow().close();
}

void C_Controller::OnActionRunHtmlCode()
{
    Document().UpdateHtml();

    MainWindow().UpdateWebView();
    MainWindow().SetCurrentTab( MAINWINDOW_TAB_OUTPUT );
}

void C_Controller::OnActionUpdateHtmlCode()
{
    Document().UpdateHtml();

    MainWindow().UpdateHtmlCodeView();
    MainWindow().SetCurrentTab( MAINWINDOW_TAB_CLIENT );
}



