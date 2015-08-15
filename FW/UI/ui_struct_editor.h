#ifndef UI_STRUCT_EDITOR_H
#define UI_STRUCT_EDITOR_H

#include "FW/macro.h"
#include <QDialog>

class C_StructRecord;
class C_Document;

namespace Ui
{
class C_UiStructEditor;
}

class C_UiStructEditor : public QDialog
{
    Q_OBJECT

public:

    C_UiStructEditor( C_StructRecord& record, C_Document& document, QWidget* parent = 0 );
    ~C_UiStructEditor() override;

    ATR_POINTER         ( Record,   C_StructRecord )
    ATR_POINTER         ( Document, C_Document )


private slots:

    void                 OnRemoveButtonClicked();
    void                 OnButtonBoxAccepted();

private:

    Ui::C_UiStructEditor* ui;
};

#endif // UI_STRUCT_EDITOR_H
