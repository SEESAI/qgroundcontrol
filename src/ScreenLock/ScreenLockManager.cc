#include "ScreenLockManager.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"
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
    for (int i = 0; i < manager->vehicles()->count(); i++) {
        _vehicleAdded(qobject_cast<Vehicle*>(manager->vehicles()->get(i)));
    }
}

void ScreenLockManager::_vehicleAdded(Vehicle* vehicle)
{
    _vehicles.insert(vehicle);
    connect(vehicle->vehicleLinkManager(), &VehicleLinkManager::communicationLostChanged,
            this, &ScreenLockManager::_communicationLostChanged);
    qCDebug(ScreenLockManagerLog) << "Vehicle added, tracking" << _vehicles.count() << "vehicles";
    _updateInhibition();
}

void ScreenLockManager::_vehicleRemoved(Vehicle* vehicle)
{
    _vehicles.remove(vehicle);
    disconnect(vehicle->vehicleLinkManager(), &VehicleLinkManager::communicationLostChanged,
               this, &ScreenLockManager::_communicationLostChanged);
    qCDebug(ScreenLockManagerLog) << "Vehicle removed, tracking" << _vehicles.count() << "vehicles";
    _updateInhibition();
}

void ScreenLockManager::_communicationLostChanged(bool communicationLost)
{
    qCDebug(ScreenLockManagerLog) << "Communication lost changed:" << communicationLost;
    _updateInhibition();
}

bool ScreenLockManager::_anyVehicleCommunicating() const
{
    for (Vehicle* vehicle : _vehicles) {
        if (!vehicle->vehicleLinkManager()->communicationLost()) {
            return true;
        }
    }
    return false;
}

void ScreenLockManager::_updateInhibition()
{
    bool shouldInhibit = _anyVehicleCommunicating();

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
