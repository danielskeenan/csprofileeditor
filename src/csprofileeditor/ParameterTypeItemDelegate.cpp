/**
 * @file ParameterTypeItemDelegate.cpp
 *
 * @author dankeenan
 * @date 4/30/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "ParameterTypeItemDelegate.h"
#include <QComboBox>
#include <csprofile/parameter/Parameter.h>
#include "EtcCsPersEditBridge.h"

namespace csprofileeditor {

QWidget *ParameterTypeItemDelegate::createEditor(QWidget *parent,
                                                 const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const {
  auto *combo_box = new QComboBox(parent);
  // One entry per parameter
  combo_box->addItem(EtcCsPersEditBridge::GetParameterTypeName(csprofile::parameter::Type::kNone),
                     static_cast<unsigned int>(csprofile::parameter::Type::kNone));
  combo_box->addItem(EtcCsPersEditBridge::GetParameterTypeName(csprofile::parameter::Type::kIntensity),
                     static_cast<unsigned int>(csprofile::parameter::Type::kIntensity));
  combo_box->addItem(EtcCsPersEditBridge::GetParameterTypeName(csprofile::parameter::Type::kPosition),
                     static_cast<unsigned int>(csprofile::parameter::Type::kPosition));
  combo_box->addItem(EtcCsPersEditBridge::GetParameterTypeName(csprofile::parameter::Type::kBeam),
                     static_cast<unsigned int>(csprofile::parameter::Type::kBeam));
  combo_box->addItem(EtcCsPersEditBridge::GetParameterTypeName(csprofile::parameter::Type::kColor),
                     static_cast<unsigned int>(csprofile::parameter::Type::kColor));

  return combo_box;
}

void ParameterTypeItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  auto *combo_box = dynamic_cast<QComboBox *>(editor);
  assert(combo_box != nullptr);
  const auto param_type = static_cast<csprofile::parameter::Type>(index.data(Qt::EditRole).toUInt());
  combo_box->setCurrentText(EtcCsPersEditBridge::GetParameterTypeName(param_type));
}

void ParameterTypeItemDelegate::setModelData(QWidget *editor,
                                             QAbstractItemModel *model,
                                             const QModelIndex &index) const {
  auto *combo_box = dynamic_cast<QComboBox *>(editor);
  assert(combo_box != nullptr);
  const auto param_type = combo_box->currentData().toUInt();
  model->setData(index, param_type);
}

} // csprofileeditor
