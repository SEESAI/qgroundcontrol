#include "ScreenLockManager.h"
#include "MultiVehicleManager.h"
#include "QGCLoggingCategory.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

QGC_LOGGING_CATEGORY(ScreenLockManagerLog, "ScreenLockManagerLog")

ScreenLockManager::ScreenLockManager(QGCApplication* app, QGCToolbox* toolbox)
    : QGCTool(app, toolbox)
{
}

ScreenLockManager::~ScreenLockManager()
{
    if (_inhibiting) {
        _uninhibit();
    }
}

void ScreenLockManager::setToolbox(QGCToolbox* toolbox)
{
    QGCTool::setToolbox(toolbox);

    MultiVehicleManager* manager = toolbox->multiVehicleManager();
    connect(manager, &MultiVehicleManager::vehicleAdded, this, &ScreenLockManager::_vehicleAdded);
    connect(manager, &MultiVehicleManager::vehicleRemoved, this, &ScreenLockManager::_vehicleRemoved);

    // Handle vehicles that may already be connected
    _vehicleCount = manager->vehicles()->count();
    _updateInhibition();
}

void ScreenLockManager::_vehicleAdded(Vehicle* vehicle)
{
    Q_UNUSED(vehicle);
    _vehicleCount++;
    qCDebug(ScreenLockManagerLog) << "Vehicle added, count:" << _vehicleCount;
    _updateInhibition();
}

void ScreenLockManager::_vehicleRemoved(Vehicle* vehicle)
{
    Q_UNUSED(vehicle);
    _vehicleCount--;
    qCDebug(ScreenLockManagerLog) << "Vehicle removed, count:" << _vehicleCount;
    _updateInhibition();
}

void ScreenLockManager::_updateInhibition()
{
    bool shouldInhibit = (_vehicleCount > 0);

    if (shouldInhibit && !_inhibiting) {
        _inhibit();
        _inhibiting = true;
    } else if (!shouldInhibit && _inhibiting) {
        _uninhibit();
        _inhibiting = false;
    }
}

void ScreenLockManager::_inhibit()
{
    qCDebug(ScreenLockManagerLog) << "Inhibiting screen lock";

    QDBusInterface iface("org.freedesktop.ScreenSaver",
                         "/org/freedesktop/ScreenSaver",
                         "org.freedesktop.ScreenSaver",
                         QDBusConnection::sessionBus());

    if (iface.isValid()) {
        QDBusReply<uint32_t> reply = iface.call("Inhibit", "QGroundControl", "Vehicle connected");
        if (reply.isValid()) {
            _inhibitCookie = reply.value();
            qCDebug(ScreenLockManagerLog) << "D-Bus screen inhibit successful, cookie:" << _inhibitCookie;
        } else {
            qCWarning(ScreenLockManagerLog) << "D-Bus Inhibit call failed:" << reply.error().message();
        }
    } else {
        qCWarning(ScreenLockManagerLog) << "D-Bus ScreenSaver interface not available";
    }
}

void ScreenLockManager::_uninhibit()
{
    qCDebug(ScreenLockManagerLog) << "Releasing screen lock inhibition";

    if (_inhibitCookie != 0) {
        QDBusInterface iface("org.freedesktop.ScreenSaver",
                             "/org/freedesktop/ScreenSaver",
                             "org.freedesktop.ScreenSaver",
                             QDBusConnection::sessionBus());

        if (iface.isValid()) {
            iface.call("UnInhibit", _inhibitCookie);
            qCDebug(ScreenLockManagerLog) << "D-Bus screen inhibit released";
        }
        _inhibitCookie = 0;
    }
}
