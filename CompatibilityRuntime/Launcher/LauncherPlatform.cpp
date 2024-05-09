#include "LauncherPlatform.h"
#include "LauncherApplicationInterface.h"

LauncherPlatform::LauncherPlatform(LauncherApplicationInterface *application) :m_application(application) {

}

LauncherPlatform::~LauncherPlatform() = default;

void LauncherPlatform::update() {
    m_application->update(this);
}
