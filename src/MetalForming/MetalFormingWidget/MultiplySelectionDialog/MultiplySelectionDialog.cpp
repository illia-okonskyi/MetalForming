#include "MultiplySelectionDialog.h"

CMultiplySelectionDialog::CMultiplySelectionDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

CMultiplySelectionDialog::~CMultiplySelectionDialog()
{

}

void CMultiplySelectionDialog::onSelChanged()
{
    ui.btnOK->setEnabled(true);
}

void CMultiplySelectionDialog::onOK()
{
    m_iSelItem = ui.selItems->currentRow();
    accept();
}

void CMultiplySelectionDialog::initItems( const QStringList &labels )
{
    m_iSelItem = -1;
    ui.selItems->clear();
    ui.selItems->addItems(labels);
}

int CMultiplySelectionDialog::getSelItem() const
{
    return m_iSelItem;
}

void CMultiplySelectionDialog::onItemDoubleClicked( QListWidgetItem* item )
{
    onOK();
}
