#include <QApplication>
#include "csprofileeditor_config.h"
#include "MainWindow.h"
#include <csprofile/logging.h>

using namespace csprofileeditor;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setOrganizationName(csprofileeditor::config::kProjectOrganizationName);
  app.setOrganizationDomain(csprofileeditor::config::kProjectOrganizationDomain);
  app.setApplicationName(csprofileeditor::config::kProjectName);
  app.setApplicationDisplayName(csprofileeditor::config::kProjectDisplayName);
  app.setApplicationVersion(csprofileeditor::config::kProjectVersion);

  csprofile::logging::init_logging(spdlog::level::info);

  MainWindow main_window;
  main_window.show();

  return QApplication::exec();
}
