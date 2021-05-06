/**
 * @file CsLibUpdater.cpp
 *
 * @author dankeenan
 * @date 5/1/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "CsLibUpdater.h"
#include "EtcCsPersEditBridge.h"
#include <cslibs/disc/DiscDb.h>
#include <cslibs/effect/EffectDb.h>
#include <cslibs/gel/GelDb.h>
#include <cslibs/gobo/GoboDb.h>
#include <QGridLayout>
#include <cmath>
#include <QMessageBox>

namespace csprofileeditor {

bool CsLibUpdater::UpToDate() {
  return DiscDbUpToDate() && EffectDbUpToDate() && GelDbUpToDate() && GoboDbUpToDate();
}

CsLibUpdater::CsLibUpdater(QWidget *parent) : QDialog(parent) {
  setWindowTitle(tr("Library Updater"));
  setMinimumWidth(640);

  auto *layout = new QGridLayout(this);

  int row = 0;
  auto *label = new QLabel(tr("Loading console media for the first time.  Please wait."), this);
  label->setWordWrap(true);
  layout->addWidget(label, row++, 0, 1, 3);
  if (!DiscDbUpToDate()) {
    AddUpdater(tr("Discs"), new UpdateWorker(EtcCsPersEditBridge::GetDiscDb(), this), row++, layout);
  }
  if (!EffectDbUpToDate()) {
    AddUpdater(tr("Effects"), new UpdateWorker(EtcCsPersEditBridge::GetEffectDb(), this), row++, layout);
  }
  if (!GelDbUpToDate()) {
    AddUpdater(tr("Gels"), new UpdateWorker(EtcCsPersEditBridge::GetGelDb(), this), row++, layout);
  }
  if (!GoboDbUpToDate()) {
    AddUpdater(tr("Gobos"), new UpdateWorker(EtcCsPersEditBridge::GetGoboDb(), this), row++, layout);
  }
}

CsLibUpdater::~CsLibUpdater() {
  StopAllWorkers();
}

void CsLibUpdater::AddUpdater(const QString &name, UpdateWorker *worker, int index, QGridLayout *layout) {
  workers_.push_back(worker);

  // Label
  auto *label = new QLabel(name, this);

  // Progress labels
  auto *progress_label = new QLabel(this);
  progress_labels_.push_back(progress_label);

  // Progress bar
  auto *progress_bar = new QProgressBar(this);
  progress_bar->setMinimum(0);
  progress_bar->setMaximum(0);
  progress_bar->setTextVisible(false);
  connect(worker,
          &UpdateWorker::ZProgressChanged,
          this,
          [progress_label, progress_bar](unsigned long current, unsigned long total) {
            progress_label->setText(tr("%1%").arg(std::floor(
                static_cast<double>(current) / static_cast<double>(total) * 100.0)));
            progress_bar->setMaximum(total);
            progress_bar->setValue(current);
          },
          Qt::QueuedConnection);
  connect(worker, &UpdateWorker::finished, this, &CsLibUpdater::SWorkerCompleted);
  progress_bars_.push_back(progress_bar);

  layout->addWidget(label, index, 0);
  layout->addWidget(progress_bar, index, 1);
  layout->addWidget(progress_label, index, 2);
}

void CsLibUpdater::SWorkerCompleted() {
  ++completed_count_;
  if (completed_count_ == workers_.size()) {
    const bool has_error = std::any_of(workers_.cbegin(), workers_.cend(),
                                       [](const UpdateWorker *worker) { return worker->HasError(); });
    if (has_error) {
      QMessageBox::critical(this,
                            tr("Error updating libraries"),
                            tr("An error occurred updating libraries.  Try again later."));
      reject();
    } else {
      accept();
    }
  }
}

void CsLibUpdater::StopAllWorkers() noexcept {
  for (auto *thread : workers_) {
    if (thread->isRunning()) {
      thread->terminate();
    }
  }
}

void CsLibUpdater::open() {
  std::for_each(workers_.begin(), workers_.end(), [](UpdateWorker *worker) {
    worker->start();
  });
  QDialog::open();
}

void CsLibUpdater::closeEvent(QCloseEvent *event) {
  StopAllWorkers();
  QDialog::closeEvent(event);
}

bool CsLibUpdater::DiscDbUpToDate() {
  auto disc_db = EtcCsPersEditBridge::GetDiscDb();
  if (!disc_db) {
    return false;
  }

  return disc_db->UpToDate();
}

bool CsLibUpdater::EffectDbUpToDate() {
  auto effect_db = EtcCsPersEditBridge::GetEffectDb();
  if (!effect_db) {
    return false;
  }

  return effect_db->UpToDate();
}

bool CsLibUpdater::GelDbUpToDate() {
  auto gel_db = EtcCsPersEditBridge::GetGelDb();
  if (!gel_db) {
    return false;
  }

  return gel_db->UpToDate();
}

bool CsLibUpdater::GoboDbUpToDate() {
  auto gobo_db = EtcCsPersEditBridge::GetGoboDb();
  if (!gobo_db) {
    return false;
  }

  return gobo_db->UpToDate();
}

void UpdateWorker::run() {
  if (!db_) {
    has_error_ = true;
  }
  try {
    db_->Update([this](unsigned long current, unsigned long total) {
      Q_EMIT(ZProgressChanged(current, total));
    });
  } catch (const std::exception &e) {
    csprofile::logging::error(e.what());
    has_error_ = true;
  }
  quit();
}
} // csprofileeditor
