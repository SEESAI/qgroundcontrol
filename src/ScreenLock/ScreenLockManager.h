#pragma once

#include "QGCToolbox.h"

#include <QSet>

class Vehicle;

/// Manages screen lock inhibition on Linux via D-Bus.
/// Prevents the display from sleeping when a vehicle is actively communicating.
class ScreenLockManager : public QGCTool
{
    Q_OBJECT

public:
    ScreenLockManager(QGCApplication* app, QGCToolbox* toolbox);
    ~ScreenLockManager();

    void setToolbox(QGCToolbox* toolbox) override;

private slots:
    void _vehicleAdded(Vehicle* vehicle);
    void _vehicleRemoved(Vehicle* vehicle);
    void _communicationLostChanged(bool communicationLost);

private:
    void _updateInhibition();
    bool _anyVehicleCommunicating() const;
    void _inhibit();
    void _uninhibit();

    bool        _inhibiting = false;
    uint32_t    _inhibitCookie = 0;
    QSet<Vehicle*> _vehicles;
};
