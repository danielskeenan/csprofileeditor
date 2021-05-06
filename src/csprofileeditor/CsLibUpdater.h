/**
 * @file CsLibUpdater.h
 *
 * @author dankeenan
 * @date 5/1/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_CSLIBUPDATER_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_CSLIBUPDATER_H_

#include <QDialog>
#include <QThread>
#include <cslibs/Db.h>
#include <QProgressBar>
#include <QLabel>
#include <QGridLayout>
#include <utility>
#include <csprofile/logging.h>

namespace csprofileeditor {

/**
 * Worker thread to perform a library update.
 *
 * Threads will emit ZProgressChanged when the progress has changed.
 *
 * @internal
 */
class UpdateWorker final : public QThread {
 Q_OBJECT
 public:
  explicit UpdateWorker(std::shared_ptr<cslibs::Db> db, QObject *parent = nullptr)
      : QThread(parent), db_(std::move(db)) {}

  [[nodiscard]] bool HasError() const {
    return has_error_;
  }

 Q_SIGNALS:
  void ZProgressChanged(unsigned long current, unsigned long total);

 private:
  bool has_error_ = false;
  std::shared_ptr<cslibs::Db> db_;

  void run() final;
};

/**
 * Update libraries
 */
class CsLibUpdater final : public QDialog {
 Q_OBJECT
 public:
  explicit CsLibUpdater(QWidget *parent = nullptr);
  ~CsLibUpdater() final;
  [[nodiscard]] static bool UpToDate();
  void open() override;

 protected:
  void closeEvent(QCloseEvent *event) override;

 private:
  std::vector<QProgressBar *> progress_bars_;
  std::vector<QLabel *> progress_labels_;
  std::vector<UpdateWorker *> workers_;
  unsigned int completed_count_ = 0;

  void AddUpdater(const QString &name, UpdateWorker *worker, int index, QGridLayout *layout);
  static bool DiscDbUpToDate();
  static bool EffectDbUpToDate();
  static bool GelDbUpToDate();
  static bool GoboDbUpToDate();
  void StopAllWorkers() noexcept;

 private Q_SLOTS:
  void SWorkerCompleted();
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_CSLIBUPDATER_H_
