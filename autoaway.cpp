/*
    Auto away-presence setter-class
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

#include "autoaway.h"
#include "ktp_kded_debug.h"

#include <KTp/global-presence.h>

#include <KIdleTime>
#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>

AutoAway::AutoAway(KTp::GlobalPresence *globalPresence, QObject *parent)
    : TelepathyKDEDModulePlugin(globalPresence, parent),
      m_awayTimeoutId(-1),
      m_extAwayTimeoutId(-1)
{
    reloadConfig();

    connect(KIdleTime::instance(), SIGNAL(timeoutReached(int)),
            this, SLOT(timeoutReached(int)));

    connect(KIdleTime::instance(), SIGNAL(resumingFromIdle()),
            this, SLOT(backFromIdle()));
}

AutoAway::~AutoAway()
{
}

QString AutoAway::pluginName() const
{
    return QString::fromLatin1("auto-away");
}

void AutoAway::timeoutReached(int id)
{
    if (!isEnabled()) {
        return;
    }
    KIdleTime::instance()->catchNextResumeEvent();
    if (id == m_awayTimeoutId) {
        if (m_globalPresence->currentPresence().type() != Tp::Presence::away().type() &&
            m_globalPresence->currentPresence().type() != Tp::Presence::xa().type() &&
            m_globalPresence->currentPresence().type() != Tp::Presence::hidden().type() &&
            m_globalPresence->currentPresence().type() != Tp::Presence::offline().type()) {
            m_awayMessage.replace(QLatin1String("%time"), QDateTime::currentDateTimeUtc().toString(QLatin1String("hh:mm:ss (%t)")), Qt::CaseInsensitive);
            setRequestedPresence(Tp::Presence::away(m_awayMessage));
            setActive(true);
        }
    } else if (id == m_extAwayTimeoutId) {
        if (m_globalPresence->currentPresence().type() == Tp::Presence::away().type()) {
            m_xaMessage.replace(QLatin1String("%time"), QDateTime::currentDateTimeUtc().toString(QLatin1String("hh:mm:ss (%t)")), Qt::CaseInsensitive);
            setRequestedPresence(Tp::Presence::xa(m_xaMessage));
            setActive(true);
        }
    }
}

void AutoAway::backFromIdle()
{
    qCDebug(KTP_KDED_MODULE);
    setActive(false);
}

void AutoAway::reloadConfig()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("ktelepathyrc"));
    config.data()->reparseConfiguration();

    KConfigGroup kdedConfig = config->group("KDED");

    bool autoAwayEnabled = kdedConfig.readEntry("autoAwayEnabled", true);
    bool autoXAEnabled = kdedConfig.readEntry("autoXAEnabled", true);

    m_awayMessage = kdedConfig.readEntry(QLatin1String("awayMessage"), QString());
    m_xaMessage = kdedConfig.readEntry(QLatin1String("xaMessage"), QString());

    //remove all our timeouts and only readd them if auto-away is enabled
    //WARNING: can't use removeAllTimeouts because this runs inside KDED, it would remove other KDED timeouts as well
    KIdleTime::instance()->removeIdleTimeout(m_awayTimeoutId);
    KIdleTime::instance()->removeIdleTimeout(m_extAwayTimeoutId);

    if (autoAwayEnabled) {
        int awayTime = kdedConfig.readEntry("awayAfter", 5);
        m_awayTimeoutId = KIdleTime::instance()->addIdleTimeout(awayTime * 60 * 1000);
        setEnabled(true);
    } else {
        setEnabled(false);
    }
    if (autoAwayEnabled && autoXAEnabled) {
        int xaTime = kdedConfig.readEntry("xaAfter", 15);
        m_extAwayTimeoutId = KIdleTime::instance()->addIdleTimeout(xaTime * 60 * 1000);
    }
}
