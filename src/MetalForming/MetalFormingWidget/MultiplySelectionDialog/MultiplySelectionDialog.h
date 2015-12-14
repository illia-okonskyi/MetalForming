#ifndef MULTIPLYSELECTIONDIALOG_H
#define MULTIPLYSELECTIONDIALOG_H

#include <QDialog>
#include "ui_MultiplySelectionDialog.h"

#include <QtCore/QStringList>

class CMultiplySelectionDialog : public QDialog
{
    Q_OBJECT

public:
    CMultiplySelectionDialog(QWidget *parent = 0);
    ~CMultiplySelectionDialog();

    void initItems(const QStringList &labels);
    int getSelItem() const;

protected slots:
    void onSelChanged();
    void onOK();
    void onItemDoubleClicked(QListWidgetItem* item);

private:
    Ui::CMultiplySelectionDialogClass ui;
private:
    int m_iSelItem;
};

#endif // MULTIPLYSELECTIONDIALOG_H
