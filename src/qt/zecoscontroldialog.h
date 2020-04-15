// Copyright (c) 2017-2020 The ECODOLLAR developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ZECOSCONTROLDIALOG_H
#define ZECOSCONTROLDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "zecos/zerocoin.h"

class CZerocoinMint;
class WalletModel;

namespace Ui {
class ZEcosControlDialog;
}

class CZEcosControlWidgetItem : public QTreeWidgetItem
{
public:
    explicit CZEcosControlWidgetItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type) {}
    explicit CZEcosControlWidgetItem(int type = Type) : QTreeWidgetItem(type) {}
    explicit CZEcosControlWidgetItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type) {}

    bool operator<(const QTreeWidgetItem &other) const;
};

class ZEcosControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ZEcosControlDialog(QWidget *parent);
    ~ZEcosControlDialog();

    void setModel(WalletModel* model);

    static std::set<std::string> setSelectedMints;
    static std::set<CMintMeta> setMints;
    static std::vector<CMintMeta> GetSelectedMints();

private:
    Ui::ZEcosControlDialog *ui;
    WalletModel* model;

    void updateList();
    void updateLabels();

    enum {
        COLUMN_CHECKBOX,
        COLUMN_DENOMINATION,
        COLUMN_PUBCOIN,
        COLUMN_VERSION,
        COLUMN_CONFIRMATIONS,
        COLUMN_ISSPENDABLE
    };
    friend class CZEcosControlWidgetItem;

private slots:
    void updateSelection(QTreeWidgetItem* item, int column);
    void ButtonAllClicked();
};

#endif // ZECOSCONTROLDIALOG_H
