// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "coinselectiontablemodel.h"

#include "bitcoinunits.h"
#include "guiutil.h"
#include "walletmodel.h"
#include "optionsmodel.h"

#include "base58.h"
#include "key_io.h"
#include "wallet/wallet.h"

#include <QFont>
#include <QDebug>

const QString CoinSelectionTableModel::ZCoinSelection = "Z";
const QString CoinSelectionTableModel::TCoinSelection = "T";

// Amount column is right-aligned it contains numbers
static int column_alignments[] = {
        Qt::AlignLeft|Qt::AlignVCenter, /* address */
        Qt::AlignRight|Qt::AlignVCenter /* amount */
    };

struct CoinSelectionTableEntry
{
    enum Type {
        ZCoinSelection,
        TCoinSelection,
        Hidden
    };

    Type type;
    QString address;
    CAmount amount;

    CoinSelectionTableEntry() {}
    CoinSelectionTableEntry(Type type, const QString &address, const CAmount &amount):
        type(type), address(address), amount(amount) {}
};

struct CoinSelectionTableEntryLessThan
{
    bool operator()(const CoinSelectionTableEntry &a, const CoinSelectionTableEntry &b) const
    {
        return a.address < b.address;
    }
    bool operator()(const CoinSelectionTableEntry &a, const QString &b) const
    {
        return a.address < b;
    }
    bool operator()(const QString &a, const CoinSelectionTableEntry &b) const
    {
        return a < b.address;
    }
};

/* Determine coinselection type from coinselection purpose */
static CoinSelectionTableEntry::Type translateCoinSelectionType(const QString &strPurpose)
{
    CoinSelectionTableEntry::Type coinselectionType = CoinSelectionTableEntry::Hidden;
    if (strPurpose == "tcoinselection")
        coinselectionType = CoinSelectionTableEntry::TCoinSelection;
    else if (strPurpose == "zcoinselection")
        coinselectionType = CoinSelectionTableEntry::ZCoinSelection;
    return coinselectionType;
}

// Private implementation
class CoinSelectionTablePriv
{
public:
    CWallet *wallet;
    WalletModel *walletModel;
    QList<CoinSelectionTableEntry> cachedCoinSelectionTable;
    CoinSelectionTableModel *parent;

    CoinSelectionTablePriv(CWallet *wallet, WalletModel *walletModel, CoinSelectionTableModel *parent):
        wallet(wallet), walletModel(walletModel), parent(parent) {}

    void refreshCoinSelectionTable()
    {
        cachedCoinSelectionTable.clear();
        {
            // T-CoinSelection
            std::vector<COutput> vecOutputs;
            std::map<QString, std::vector<COutput> > mapCoins;

            LOCK2(cs_main, wallet->cs_wallet);
            wallet->AvailableCoins(vecOutputs, false, NULL, true);
            KeyIO keyIO(Params());

            for (const COutput& out : vecOutputs) {
                if (out.tx->IsCoinBase())
                    continue;

                if (!out.fSpendable)
                    continue;

                CTxDestination address;
                if (ExtractDestination(out.tx->vout[out.i].scriptPubKey, address)) {
                    mapCoins[QString::fromStdString(keyIO.EncodeDestination(address))].push_back(out);
                }
            }
            for (const auto& [first, second] : mapCoins) {
                QString sWalletAddress = first;
                CAmount nSum = 0;
                for (const COutput& out : second)
                    nSum += CAmount(out.tx->vout[out.i].nValue);
                if (nSum > 0)
                {
                    CoinSelectionTableEntry::Type unspentType = translateCoinSelectionType(QString::fromStdString("tcoinselection"));
                    cachedCoinSelectionTable.append(CoinSelectionTableEntry(unspentType, sWalletAddress, nSum));
                }
            }

            // Z-CoinSelection
            std::set<libzcash::RawAddress> zaddrs = {};
            std::map<QString, std::vector<SproutNoteEntry> > mapZCoins;
            std::map<QString, std::vector<SaplingNoteEntry> > mapSaplingZCoins;
            int nMinDepth = 1;
            int nMaxDepth = 9999999;

            std::set<libzcash::SproutPaymentAddress> addresses = {};
            wallet->GetSproutPaymentAddresses(addresses);
            for (auto addr : addresses ) {
                if (wallet->HaveSproutSpendingKey(addr)) {
                    zaddrs.insert(addr);
                }
            }
	    std::set<libzcash::SaplingPaymentAddress> saplingzaddrs = {};
	    wallet->GetSaplingPaymentAddresses(saplingzaddrs);
	    libzcash::SaplingIncomingViewingKey ivk;
	    libzcash::SaplingExtendedFullViewingKey extfvk;
            for (auto addr : saplingzaddrs ) {
                if (wallet->GetSaplingIncomingViewingKey(addr, ivk) &&
		    wallet->GetSaplingFullViewingKey(ivk, extfvk) &&
		    wallet->HaveSaplingSpendingKey(extfvk)) {
                    zaddrs.insert(addr);
                }
            }

            if (zaddrs.size() > 0) {
                std::vector<SproutNoteEntry> sproutEntries;
		std::vector<SaplingNoteEntry> saplingEntries;
                wallet->GetFilteredNotes(sproutEntries, saplingEntries, zaddrs, nMinDepth, nMaxDepth, true, true, false);
		std::set<std::pair<libzcash::RawAddress, uint256>> nullifierSet = wallet->GetNullifiersForAddresses(zaddrs);
                KeyIO keyIO(Params());

                for (auto & entry : sproutEntries) {
                    mapZCoins[QString::fromStdString(keyIO.EncodePaymentAddress(entry.address))].push_back(entry);
                }
                for (const auto& [first, second] : mapZCoins) {
                    QString sWalletAddress = first;
                    CAmount nSum = 0;
                    for (const auto & entry : second)
                        nSum += CAmount(entry.note.value());
                    if (nSum > 0)
                    {
                        CoinSelectionTableEntry::Type unspentType = translateCoinSelectionType(QString::fromStdString("zcoinselection"));
                        cachedCoinSelectionTable.append(CoinSelectionTableEntry(unspentType, sWalletAddress, nSum));
                    }
                }

                for (SaplingNoteEntry & entry : saplingEntries) {
                    mapSaplingZCoins[QString::fromStdString(keyIO.EncodePaymentAddress(entry.address))].push_back(entry);
                }
                for (const auto& [first, second] : mapSaplingZCoins) {
                    QString sWalletAddress = first;
                    CAmount nSum = 0;
                    for (const SaplingNoteEntry& entry : second)
                        nSum += CAmount(entry.note.value());
                    if (nSum > 0)
                    {
                        CoinSelectionTableEntry::Type unspentType = translateCoinSelectionType(QString::fromStdString("zcoinselection"));
                        cachedCoinSelectionTable.append(CoinSelectionTableEntry(unspentType, sWalletAddress, nSum));
                    }
                }
            }
        }
        // qLowerBound() and qUpperBound() require our cachedCoinSelectionTable list to be sorted in asc order
        // Even though the map is already sorted this re-sorting step is needed because the originating map
        // is sorted by binary address, not by base58() address.
        qSort(cachedCoinSelectionTable.begin(), cachedCoinSelectionTable.end(), CoinSelectionTableEntryLessThan());
    }

    int size()
    {
        return cachedCoinSelectionTable.size();
    }

    CoinSelectionTableEntry *index(int idx)
    {
        if(idx >= 0 && idx < cachedCoinSelectionTable.size())
        {
            return &cachedCoinSelectionTable[idx];
        }
        else
        {
            return 0;
        }
    }
};

CoinSelectionTableModel::CoinSelectionTableModel(CWallet *wallet, WalletModel *parent) :
    QAbstractTableModel(parent), walletModel(parent), wallet(wallet), priv(0)
{
    columns << tr("Address") << tr("Amount");
    priv = new CoinSelectionTablePriv(wallet, walletModel, this);
    priv->refreshCoinSelectionTable();

    connect(walletModel->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
}

void CoinSelectionTableModel::updateDisplayUnit()
{
    Q_EMIT dataChanged(index(0, Amount), index(priv->size()-1, Amount));
}

CoinSelectionTableModel::~CoinSelectionTableModel()
{
    delete priv;
}

int CoinSelectionTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int CoinSelectionTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant CoinSelectionTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    CoinSelectionTableEntry *rec = static_cast<CoinSelectionTableEntry*>(index.internalPointer());

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case Address:
            return rec->address;
        case Amount:
            return BitcoinUnits::format(walletModel->getOptionsModel()->getDisplayUnit(), rec->amount);
        }
    }
    else if (role == Qt::FontRole)
    {
        QFont font;
        if(index.column() == Address)
        {
            font = GUIUtil::fixedPitchFont();
        }
        return font;
    }
    else if (role == TypeRole)
    {
        switch(rec->type)
        {
        case CoinSelectionTableEntry::ZCoinSelection:
            return ZCoinSelection;
        case CoinSelectionTableEntry::TCoinSelection:
            return TCoinSelection;
        default: break;
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant CoinSelectionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole && section < columns.size())
        {
            return columns[section];
        }
        else if (role == Qt::TextAlignmentRole)
        {
            return column_alignments[section];
        }
    }
    return QVariant();
}

Qt::ItemFlags CoinSelectionTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;
    CoinSelectionTableEntry *rec = static_cast<CoinSelectionTableEntry*>(index.internalPointer());

    Qt::ItemFlags retval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return retval;
}

QModelIndex CoinSelectionTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    CoinSelectionTableEntry *data = priv->index(row);
    if(data)
    {
        return createIndex(row, column, priv->index(row));
    }
    return QModelIndex();
}

int CoinSelectionTableModel::lookupAddress(const QString &address) const
{
    QModelIndexList lst = match(index(0, Address, QModelIndex()),
                                Qt::EditRole, address, 1, Qt::MatchExactly);
    if(lst.isEmpty())
    {
        return -1;
    }
    else
    {
        return lst.at(0).row();
    }
}

void CoinSelectionTableModel::emitDataChanged(int idx)
{
    Q_EMIT dataChanged(index(idx, 0, QModelIndex()), index(idx, columns.length()-1, QModelIndex()));
}

void CoinSelectionTableModel::refresh()
{
    Q_EMIT layoutAboutToBeChanged();
    priv->refreshCoinSelectionTable();
    Q_EMIT layoutChanged();
}
