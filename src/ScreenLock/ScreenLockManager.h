#pragma once

#include "QGCToolbox.h"

class Vehicle;

/// Manages screen lock inhibition on Linux via D-Bus.
/// Prevents the display from sleeping when a vehicle is connected.
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

private:
    void _updateInhibition();
    void _inhibit();
    void _uninhibit();

    bool     _inhibiting = false;
    int      _vehicleCount = 0;
    uint32_t _inhibitCookie = 0;
};
