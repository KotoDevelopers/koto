// Copyright (c) 2011-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "addressbookdialog.h"
#include "ui_addressbookdialog.h"

#include "addresstablemodel.h"
#include "bitcoingui.h"
#include "editaddressdialog.h"
#include "guiutil.h"
#include "platformstyle.h"

#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QSortFilterProxyModel>

AddressBookDialog::AddressBookDialog(const PlatformStyle *platformStyle, Mode mode, Tabs tab, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressBookDialog),
    model(0),
    mode(mode),
    tab(tab)
{
    ui->setupUi(this);

    if (!platformStyle->getImagesOnButtons()) {
        ui->newReceivingZAddress->setIcon(QIcon());
        ui->newSendingTAddress->setIcon(QIcon());
    } else {
        ui->newReceivingZAddress->setIcon(platformStyle->MultiColorIcon(":/images/add1"));
        ui->newSendingTAddress->setIcon(platformStyle->MultiColorIcon(":/images/add1"));
    }

    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setFocus();

    switch(mode)
    {
    case ForTSelection:
        switch(tab)
        {
        case SendingTab: setWindowTitle(tr("Choose the address to send coins to")); break;
        case ReceivingTab: setWindowTitle(tr("Choose the address to receive coins with")); break;
        }
        break;
    case ForZSelection:
        switch(tab)
        {
        case SendingTab: setWindowTitle(tr("Sending z-addresses")); break;
        case ReceivingTab: setWindowTitle(tr("Receiving z-addresses")); break;
        }
        break;
    }

    switch(tab)
    {
    case SendingTab:
        ui->labelExplanation->setText(tr("These are your Koto addresses for sending payments. Always check the amount and the receiving address before sending coins."));
        break;
    case ReceivingTab:
        ui->labelExplanation->setText(tr("These are your Koto addresses for receiving payments. It is recommended to use a new receiving address for each transaction."));
        break;
    }

    // Context menu actions
    QAction *copyAddressAction = new QAction(tr("&Copy Address"), this);
    QAction *copyLabelAction = new QAction(tr("Copy &Label"), this);

    // Build context menu
    contextMenu = new QMenu(this);
    contextMenu->addAction(copyAddressAction);
    contextMenu->addAction(copyLabelAction);
    contextMenu->addSeparator();

    // Connect signals for context menu actions
    connect(copyAddressAction, SIGNAL(triggered()), this, SLOT(onCopyAddressAction()));
    connect(copyLabelAction, SIGNAL(triggered()), this, SLOT(onCopyLabelAction()));

    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenu(QPoint)));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void AddressBookDialog::on_newSendingTAddress_clicked()
{
    if(!model)
        return;

    EditAddressDialog dlg(EditAddressDialog::NewSendingAddress, this);
    dlg.setModel(model);
    if(dlg.exec())
    {
        newAddressToSelect = dlg.getAddress();
    }
}

void AddressBookDialog::on_newReceivingZAddress_clicked()
{
    newAddressToSelect = model->addRow(AddressTableModel::ZReceive, "", "");
}

void AddressBookDialog::selectNewAddress(const QModelIndex &parent, int begin, int /*end*/)
{
    QModelIndex idx = proxyModel->mapFromSource(model->index(begin, AddressTableModel::Address, parent));

    if(idx.isValid() && (idx.data(Qt::EditRole).toString() == newAddressToSelect))
    {
        // Select row of newly created address, once
        ui->tableView->setFocus();
        ui->tableView->selectRow(idx.row());
        newAddressToSelect.clear();
    }
}

AddressBookDialog::~AddressBookDialog()
{
    delete ui;
}

void AddressBookDialog::onCopyAddressAction()
{
    GUIUtil::copyEntryData(ui->tableView, AddressTableModel::Address);
}

void AddressBookDialog::onCopyLabelAction()
{
    GUIUtil::copyEntryData(ui->tableView, AddressTableModel::Label);
}

void AddressBookDialog::contextualMenu(const QPoint &point)
{
    QModelIndex index = ui->tableView->indexAt(point);
    if(index.isValid())
    {
        contextMenu->exec(QCursor::pos());
    }
}

void AddressBookDialog::setModel(AddressTableModel *model)
{
    this->model = model;
    if(!model)
        return;

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    switch(mode)
    {
    case ForTSelection:
        switch(tab)
        {
        case ReceivingTab:
            // Receive filter
            proxyModel->setFilterRole(AddressTableModel::TypeRole);
            proxyModel->setFilterFixedString(AddressTableModel::Receive);
            ui->newReceivingZAddress->setVisible(false);
            ui->newSendingTAddress->setVisible(false);
            break;
        case SendingTab:
            // Send filter
            proxyModel->setFilterRole(AddressTableModel::TypeRole);
            proxyModel->setFilterFixedString(AddressTableModel::Send);
            ui->newReceivingZAddress->setVisible(false);
            ui->newSendingTAddress->setVisible(true);
            break;
        }
        break;
    case ForZSelection:
        switch(tab)
        {
        case SendingTab:
            ui->newReceivingZAddress->setVisible(false);
            ui->newSendingTAddress->setVisible(false);
            break;
        case ReceivingTab:
            // Receive filter (z-addresses)
            proxyModel->setFilterRole(AddressTableModel::TypeRole);
            proxyModel->setFilterFixedString(AddressTableModel::ZReceive);
            ui->newReceivingZAddress->setVisible(true);
            ui->newSendingTAddress->setVisible(false);
            break;
        }
        break;
    }

    ui->tableView->setModel(proxyModel);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);

    // Set column widths
#if QT_VERSION < 0x050000
    ui->tableView->horizontalHeader()->setResizeMode(AddressTableModel::Label, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setResizeMode(AddressTableModel::Address, QHeaderView::ResizeToContents);
#else
    ui->tableView->horizontalHeader()->setSectionResizeMode(AddressTableModel::Label, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(AddressTableModel::Address, QHeaderView::ResizeToContents);
#endif

    // Select row for newly created address
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(selectNewAddress(QModelIndex,int,int)));
}

void AddressBookDialog::done(int retval)
{
    QTableView *table = ui->tableView;
    if(!table->selectionModel() || !table->model())
        return;

    // Figure out which address was selected, and return it
    QModelIndexList indexes = table->selectionModel()->selectedRows(AddressTableModel::Address);

    Q_FOREACH (const QModelIndex& index, indexes) {
        QVariant address = table->model()->data(index);
        returnValue = address.toString();
    }

    if(returnValue.isEmpty())
    {
        // If no address entry selected, return rejected
        retval = Rejected;
    }

    QDialog::done(retval);
}
