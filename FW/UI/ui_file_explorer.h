#ifndef UI_FILE_EXPLORER_VIEW_H
#define UI_FILE_EXPLORER_VIEW_H

#include <QWidget>
#include "FW/macro.h"
#include <QStringListModel>
class C_Document;

namespace Ui
{
class C_UiFileExplorer;
}

class C_UiFileExplorer : public QWidget
{
    Q_OBJECT

public:

    C_UiFileExplorer( C_Document& document, QWidget* parent = 0 );
    ~C_UiFileExplorer();

    M_VALUE                       ( Path,      QString )
    M_VALUE                       ( ModelData, QStringList )
    M_POINTER                     ( Model,     QStringListModel )
    M_POINTER                     ( Document,  C_Document )

    void                          Update();
    void                          Activate(QString file_name);

public slots:

    void OnRootButtonClicked();
    void OnUpButtonClicked();
    void OnDoubleClicked( const QModelIndex& index );
    void OnLineEditReturnPressed();

private:

    QString                       FullPath();

    Ui::C_UiFileExplorer* ui;
};

#endif // UI_FILE_EXPLORER_VIEW_H
