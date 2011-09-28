/*
    KControl Module for general Telepathy integration configs
    Copyright (C) 2011  Martin Klapetek <martin.klapetek@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef TELEPATHY_KDED_CONFIG_H
#define TELEPATHY_KDED_CONFIG_H

#include <KCModule>

namespace Ui {
    class TelepathyKDEDUi;
}

class TelepathyKDEDConfig : public KCModule
{
    Q_OBJECT

public:
    TelepathyKDEDConfig(QWidget* parent, const QVariantList& args);
    virtual ~TelepathyKDEDConfig();

public Q_SLOTS:
    void load();
    void save();

private Q_SLOTS:
    void autoAwayChecked(bool checked);
    void autoXAChecked(bool checked);
    void settingsHasChanged();

private:
    Ui::TelepathyKDEDUi *ui;

};

#endif // TELEPATHY_KDED_CONFIG_H
