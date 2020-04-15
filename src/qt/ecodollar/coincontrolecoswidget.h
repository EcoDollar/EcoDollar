// Copyright (c) 2019 The ECODOLLAR developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COINCONTROLECOSWIDGET_H
#define COINCONTROLECOSWIDGET_H

#include <QDialog>

namespace Ui {
class CoinControlEcosWidget;
}

class CoinControlEcosWidget : public QDialog
{
    Q_OBJECT

public:
    explicit CoinControlEcosWidget(QWidget *parent = nullptr);
    ~CoinControlEcosWidget();

private:
    Ui::CoinControlEcosWidget *ui;
};

#endif // COINCONTROLECOSWIDGET_H
