/**
 * @file AboutDialog.cpp
 *
 * @author dankeenan
 * @date 5/7/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QApplication>
#include <QTextBrowser>
#include <QFile>

namespace csprofileeditor {

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent) {
  setWindowTitle(tr("About"));
  resize(480, 480);
  auto *layout = new QVBoxLayout(this);

  // Tabs
  auto *tabs = new QTabWidget(this);
  layout->addWidget(tabs);
  tabs->addTab(CreateAboutTab(), tr("About"));
  tabs->addTab(CreateThirdPartyTab(), tr("3rd Party"));

  // Action buttons
  auto *actions = new QDialogButtonBox(QDialogButtonBox::Close, this);
  connect(actions, &QDialogButtonBox::rejected, this, &AboutDialog::close);

  layout->addWidget(actions);
}

QWidget *AboutDialog::CreateAboutTab() {
  auto *tab = new QWidget(this);
  auto *layout = new QVBoxLayout(tab);

  // Icon
  auto *icon = new QLabel(tab);
  icon->setPixmap(QIcon(":/app-icon").pixmap(128, 128));
  icon->setAlignment(Qt::AlignHCenter);
  layout->addWidget(icon, Qt::AlignHCenter);

  // Name
  auto *name = new QLabel(qApp->applicationDisplayName(), tab);
  QFont font = name->font();
  font.setPointSize(16);
  name->setFont(font);
  name->setAlignment(Qt::AlignHCenter);
  layout->addWidget(name, Qt::AlignHCenter);

  // Source code
  auto *source_code = new QLabel(tr("<a href='https://github.com/danielskeenan/csprofileeditor'>Source Code</a>"), tab);
  source_code->setAlignment(Qt::AlignHCenter);
  source_code->setOpenExternalLinks(true);
  layout->addWidget(source_code, Qt::AlignHCenter);

  // Issues
  auto *issue_tracker =
      new QLabel(tr("<a href='https://github.com/danielskeenan/csprofileeditor/issues'>Issue Tracker</a>"), tab);
  issue_tracker->setAlignment(Qt::AlignHCenter);
  issue_tracker->setOpenExternalLinks(true);
  layout->addWidget(issue_tracker, Qt::AlignHCenter);

  return tab;
}

QWidget *AboutDialog::CreateThirdPartyTab() {
  auto *tab = new QWidget(this);
  auto *layout = new QVBoxLayout(tab);

  auto *browser = new QTextBrowser(this);
  layout->addWidget(browser);
  // Get the notices
  QFile notice_file(":/3rd-party.html");
  notice_file.open(QIODevice::ReadOnly | QIODevice::Text);
  assert(notice_file.isOpen());
  browser->setHtml(notice_file.readAll());
  browser->setOpenExternalLinks(true);

  return tab;
}

} // csprofileeditor
