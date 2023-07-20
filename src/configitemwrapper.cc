#include "configitemwrapper.hh"
#include <cmath>
#include "logger.hh"
#include "utils.hh"
#include <QColor>
#include <QPalette>
#include <QWidget>


/* ********************************************************************************************* *
 * Implementation of GenericListWrapper
 * ********************************************************************************************* */
GenericListWrapper::GenericListWrapper(AbstractConfigObjectList *list, QObject *parent)
  : QAbstractListModel(parent), _list(list)
{
  if (nullptr == _list)
    return;

  connect(_list, SIGNAL(destroyed(QObject*)), this, SLOT(onListDeleted()));
  connect(_list, SIGNAL(elementAdded(int)), this, SLOT(onItemAdded(int)));
  connect(_list, SIGNAL(elementModified(int)), this, SLOT(onItemModified(int)));
  connect(_list, SIGNAL(elementRemoved(int)), this, SLOT(onItemRemoved(int)));
}

int
GenericListWrapper::rowCount(const QModelIndex &index) const {
  Q_UNUSED(index)
  if (nullptr == _list)
    return 0;
  return _list->count();
}

int
GenericListWrapper::columnCount(const QModelIndex &index) const {
  Q_UNUSED(index)
  if (nullptr == _list)
    return 0;
  return 1;
}

bool
GenericListWrapper::moveUp(int row) {
  if ((0>=row) || (row>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  _list->moveUp(row);
  endMoveRows();
  emit modified();
  return true;
}

bool
GenericListWrapper::moveUp(int first, int last) {
  if ((0>=first) || (last>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), first-1);
  _list->moveUp(first, last);
  endMoveRows();
  emit modified();
  return true;
}

bool
GenericListWrapper::moveDown(int row) {
  if ((0>row) || ((row+1)>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  _list->moveDown(row);
  endMoveRows();
  emit modified();
  return true;
}

bool
GenericListWrapper::moveDown(int first, int last) {
  if ((0>first) || ((last+1)>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), last+2);
  _list->moveDown(first, last);
  endMoveRows();
  emit modified();
  return true;
}


void
GenericListWrapper::onListDeleted() {
  beginResetModel();
  _list = nullptr;
  endResetModel();
}

void
GenericListWrapper::onItemAdded(int idx) {
  beginInsertRows(QModelIndex(), idx, idx);
  endInsertRows();
}

void
GenericListWrapper::onItemRemoved(int idx) {
  beginRemoveRows(QModelIndex(), idx, idx);
  //logDebug() << "Signal removal of item at idx=" << idx;
  endRemoveRows();
}

void
GenericListWrapper::onItemModified(int idx) {
  emit dataChanged(index(idx),index(idx));
}


/* ********************************************************************************************* *
 * Implementation of GenericTableWrapper
 * ********************************************************************************************* */
GenericTableWrapper::GenericTableWrapper(AbstractConfigObjectList *list, QObject *parent)
  : QAbstractTableModel(parent), _list(list)
{
  if (nullptr == _list)
    return;

  connect(_list, SIGNAL(destroyed(QObject*)), this, SLOT(onListDeleted()));
  connect(_list, SIGNAL(elementAdded(int)), this, SLOT(onItemAdded(int)));
  connect(_list, SIGNAL(elementModified(int)), this, SLOT(onItemModified(int)));
  connect(_list, SIGNAL(elementRemoved(int)), this, SLOT(onItemRemoved(int)));
}

int
GenericTableWrapper::rowCount(const QModelIndex &index) const {
  Q_UNUSED(index)
  if (nullptr == _list)
    return 0;
  return _list->count();
}

bool
GenericTableWrapper::moveUp(int row) {
  if ((0>=row) || (row>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
  _list->moveUp(row);
  endMoveRows();
  emit modified();
  return true;
}

bool
GenericTableWrapper::moveUp(int first, int last) {
  if ((0>=first) || (last>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), first-1);
  _list->moveUp(first, last);
  endMoveRows();
  emit modified();
  return true;
}

bool
GenericTableWrapper::moveDown(int row) {
  if ((0>row) || ((row+1)>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
  _list->moveDown(row);
  endMoveRows();
  emit modified();
  return true;
}

bool
GenericTableWrapper::moveDown(int first, int last) {
  if ((0>first) || ((last+1)>=_list->count()))
    return false;
  beginMoveRows(QModelIndex(), first, last, QModelIndex(), last+2);
  _list->moveDown(first, last);
  endMoveRows();
  emit modified();
  return true;
}

void
GenericTableWrapper::onListDeleted() {
  beginResetModel();
  _list = nullptr;
  endResetModel();
}

void
GenericTableWrapper::onItemAdded(int idx) {
  beginInsertRows(QModelIndex(), idx, idx);
  endInsertRows();
}

void
GenericTableWrapper::onItemRemoved(int idx) {
  beginRemoveRows(QModelIndex(), idx, idx);
  //logDebug() << "Signal removal of item at idx=" << idx;
  endRemoveRows();
}

void
GenericTableWrapper::onItemModified(int idx) {
  emit dataChanged(index(idx,0),index(idx,columnCount()-1));
}


/* ********************************************************************************************* *
 * Implementation of ChannelListWrapper
 * ********************************************************************************************* */
ChannelListWrapper::ChannelListWrapper(ChannelList *list, QObject *parent)
  : GenericTableWrapper(list, parent)
{
  // pass...
}

int
ChannelListWrapper::columnCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return 21;
}

QVariant
ChannelListWrapper::data(const QModelIndex &index, int role) const {
  if (nullptr == _list)
    return QVariant();

  if ((! index.isValid()) || (index.row()>=_list->count()))
    return QVariant();

  if (Qt::ForegroundRole == role) {
    const QPalette &palette = qobject_cast<QWidget *>(QObject::parent())->palette();
    QColor active   = palette.color(QPalette::Active, QPalette::Text);
    QColor inactive = palette.color(QPalette::Inactive, QPalette::Text);
    bool isDigital = dynamic_cast<ChannelList *>(_list)->channel(index.row())->is<DMRChannel>();
    switch(index.column()) {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
      return active;
    case 11: case 12: case 13: case 14:
      return (isDigital ? active : inactive);
    case 15:
      return active;
    case 16:
      return (isDigital ? active : inactive);
    case 17: case 18: case 19: case 20:
      return (isDigital ? inactive : active);
    }
  }

  if ((Qt::DisplayRole!=role) && (Qt::EditRole!=role))
    return QVariant();

  Channel *channel = dynamic_cast<ChannelList *>(_list)->channel(index.row());

  switch (index.column()) {
  case 0:
    if (channel->is<FMChannel>())
      return tr("FM");
    else if (channel->is<DMRChannel>())
      return tr("DMR");
    else if (channel->is<M17Channel>())
      return tr("M17");
    else
      return QVariant();
  case 1:
    return channel->name();
  case 2:
    return channel->rxFrequency().format(Frequency::Format::MHz);
  case 3:
    return channel->txFrequency().format(Frequency::Format::MHz);
  case 4:
    if (channel->defaultPower())
      return tr("[Default]");
    switch (channel->power()) {
    case Channel::Power::Max: return tr("Max"); break;
    case Channel::Power::High: return tr("High"); break;
    case Channel::Power::Mid: return tr("Mid"); break;
    case Channel::Power::Low: return tr("Low"); break;
    case Channel::Power::Min: return tr("Min"); break;
    }
    break;
  case 5:
    if (channel->defaultTimeout())
      return tr("[Default]");
    if (channel->timeoutDisabled())
      return tr("Off");
    return QString::number(channel->timeout());
  case 6:
    return channel->rxOnly() ? tr("On") : tr("Off");
  case 7:
    if (DMRChannel *dmr = channel->as<DMRChannel>()) {
      switch (dmr->admit()) {
      case DMRChannel::Admit::Always: return tr("Always"); break;
      case DMRChannel::Admit::Free: return tr("Free"); break;
      case DMRChannel::Admit::ColorCode: return tr("Color"); break;
      }
    } else if (FMChannel *fm = channel->as<FMChannel>()) {
      switch (fm->admit()) {
      case FMChannel::Admit::Always: return tr("Always"); break;
      case FMChannel::Admit::Free: return tr("Free"); break;
      case FMChannel::Admit::Tone: return tr("Tone"); break;
      }
    } else if (channel->is<M17Channel>()) {
      return tr("[None]");
    }
    break;
  case 8:
    if (channel->scanList()) {
      return channel->scanList()->name();
    } else {
      return tr("[None]");
    }
  case 9: { // Collect zones, the channel is a member of
      QStringList zones;
      for(int i=0;i<channel->config()->zones()->count(); i++) {
        Zone *zone = channel->config()->zones()->zone(i);
        if (zone->contains(channel))
          zones.append(zone->name());
      }
      return zones.join(", ");
    } break;
  case 10:
    if (DMRChannel *digi = channel->as<DMRChannel>()) {
      return digi->colorCode();
    } else if (M17Channel *m17 = channel->as<M17Channel>()) {
            return m17->accessNumber();
    } else if (channel->is<FMChannel>()) {
      return tr("[None]");
    }
    break;
  case 11:
    if (DMRChannel *dmr = channel->as<DMRChannel>()) {
      return (DMRChannel::TimeSlot::TS1 == dmr->timeSlot()) ? 1 : 2;
    } else if (channel->is<FMChannel>() || channel->is<M17Channel>()) {
      return tr("[None]");
    }
    break;
  case 12:
    if (DMRChannel *dmr = channel->as<DMRChannel>()) {
      if (dmr->groupListObj()) {
        return dmr->groupListObj()->name();
      } else {
        return tr("-");
      }
    } else if (channel->is<FMChannel>() || channel->is<M17Channel>()) {
      return tr("[None]");
    }
    break;
  case 13:
    if (DMRChannel *digi = channel->as<DMRChannel>()) {
      if (digi->txContactObj())
        return digi->txContactObj()->name();
      else
        return tr("-");
    } else if (M17Channel *m17 = channel->as<M17Channel>()) {
      if (m17->contact())
        return m17->contact()->name();
      else
        return tr("-");
    } else if (channel->is<FMChannel>()) {
      return tr("[None]");
    }
    break;
  case 14:
    if (DMRChannel *digi = channel->as<DMRChannel>()) {
      if ((nullptr == digi->radioIdObj()) || (DefaultRadioID::get() == digi->radioIdObj()))
        return tr("[Default]");
      return digi->radioIdObj()->name();
    } else if (channel->is<FMChannel>() || channel->is<M17Channel>()) {
      return tr("[None]");
    }
    break;
  case 15:
    if (DMRChannel *dmr = channel->as<DMRChannel>()) {
      if (dmr->aprsObj())
        return dmr->aprsObj()->name();
      else
        return tr("-");
    } else if (M17Channel *m17 = channel->as<M17Channel>()) {
      if (m17->gpsEnabled())
        return tr("Enabled");
      return tr("Disabled");
    } else if (FMChannel *fm = channel->as<FMChannel>()) {
      if (fm->aprsSystem())
        return fm->aprsSystem()->name();
      else
        return tr("-");
    }
    break;
  case 16:
    if (DMRChannel *digi = channel->as<DMRChannel>()) {
      if (nullptr == digi->roamingZone())
        return tr("-");
      else if (DefaultRoamingZone::get() == digi->roamingZone())
        return tr("[Default]");
      return digi->roamingZone()->name();
    } else if (channel->is<FMChannel>() || channel->is<M17Channel>()) {
      return tr("[None]");
    }
    break;
  case 17:
    if (channel->is<DMRChannel>() || channel->is<M17Channel>()) {
      return tr("[None]");
    } else if (FMChannel *analog = channel->as<FMChannel>()) {
      if (analog->defaultSquelch())
        return tr("[Default]");
      if (analog->squelchDisabled())
        return tr("Open");
      else
        return analog->squelch();
    }
    break;
  case 18:
    if (channel->is<DMRChannel>() || channel->is<M17Channel>()) {
      return tr("[None]");
    } else if (FMChannel *analog = channel->as<FMChannel>()) {
      if (Signaling::SIGNALING_NONE == analog->rxTone()) {
        return tr("Off");
      } else
        return Signaling::codeLabel(analog->rxTone());
    }
    break;
  case 19:
    if (channel->is<DMRChannel>() || channel->is<M17Channel>()) {
      return tr("[None]");
    } else if (FMChannel *analog = channel->as<FMChannel>()) {
      if (Signaling::SIGNALING_NONE == analog->txTone()) {
        return tr("Off");
      } else
        return Signaling::codeLabel(analog->txTone());
    }
    break;
  case 20:
    if (channel->is<DMRChannel>() || channel->is<M17Channel>()) {
      return tr("[None]");
    } else if (FMChannel *analog = channel->as<FMChannel>()) {
      if (FMChannel::Bandwidth::Wide == analog->bandwidth()) {
        return tr("Wide");
      } else
        return tr("Narrow");
    }
    break;

  default:
    break;
  }

  return QVariant();
}

QVariant
ChannelListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation))
    return QVariant();
  switch (section) {
  case 0: return tr("Type");
  case 1: return tr("Name");
  case 2: return tr("Rx Frequency");
  case 3: return tr("Tx Frequency");
  case 4: return tr("Power");
  case 5: return tr("Timeout");
  case 6: return tr("Rx Only");
  case 7: return tr("Admit");
  case 8: return tr("Scanlist");
  case 9: return tr("Zones");
  case 10: return tr("CC");
  case 11: return tr("TS");
  case 12: return tr("RX Group List");
  case 13: return tr("TX Contact");
  case 14: return tr("DMR ID");
  case 15: return tr("GPS/APRS");
  case 16: return tr("Roaming");
  case 17: return tr("Squelch");
  case 18: return tr("Rx Tone");
  case 19: return tr("Tx Tone");
  case 20: return tr("Bandwidth");
    default:
      break;
  }
  return QVariant();
}


/* ********************************************************************************************* *
 * Implementation of ChannelRefListWrapper
 * ********************************************************************************************* */
ChannelRefListWrapper::ChannelRefListWrapper(ChannelRefList *list, QObject *parent)
  : GenericListWrapper(list, parent)
{
  // pass...
}

QVariant
ChannelRefListWrapper::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (! index.isValid()) || (index.row()>=_list->count()))
    return QVariant();
  return _list->get(index.row())->as<Channel>()->name();
}

QVariant
ChannelRefListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((0!=section) || (Qt::Horizontal!=orientation) || (Qt::DisplayRole!=role))
    return QVariant();
  return tr("Channel");
}


/* ********************************************************************************************* *
 * Implementation of RoamingChannelListWrapper
 * ********************************************************************************************* */
RoamingChannelListWrapper::RoamingChannelListWrapper(RoamingChannelList *list, QObject *parent)
  : GenericTableWrapper(list, parent)
{
  // pass...
}

int
RoamingChannelListWrapper::columnCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return 5;
}

QVariant
RoamingChannelListWrapper::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (! index.isValid()) || (index.row() >= _list->count()))
    return QVariant();

  RoamingChannel *ch = _list->get(index.row())->as<RoamingChannel>();

  // Dispatch by column
  switch (index.column()) {
  case 0: return ch->name();
  case 1: return ch->rxFrequency().format(Frequency::Format::MHz);
  case 2: return ch->txFrequency().format(Frequency::Format::MHz);
  case 3:
    if (ch->colorCodeOverridden())
      return ch->colorCode();
    return tr("[Selected]");
  case 4:
    if (ch->timeSlotOverridden()) {
      switch(ch->timeSlot()) {
      case DMRChannel::TimeSlot::TS1: return 1;
      case DMRChannel::TimeSlot::TS2: return 2;
      }
    }
    return tr("[Selected]");
  default: break;
  }

  return QVariant();
}

QVariant
RoamingChannelListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::Horizontal!=orientation) || (Qt::DisplayRole!=role))
    return QVariant();
  switch (section) {
  case 0: return tr("Name");
  case 1: return tr("RX Frequency");
  case 2: return tr("TX Frequency");
  case 3: return tr("CC");
  case 4: return tr("TS");
  default: break;
  }
  return QVariant();
}


/* ********************************************************************************************* *
 * Implementation of RoamingChannelRefListWrapper
 * ********************************************************************************************* */
RoamingChannelRefListWrapper::RoamingChannelRefListWrapper(RoamingChannelRefList *list, QObject *parent)
  : GenericListWrapper(list, parent)
{
  // pass...
}

QVariant
RoamingChannelRefListWrapper::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (! index.isValid()) || (index.row() >= _list->count()))
    return QVariant();
  return _list->get(index.row())->as<RoamingChannel>()->name();
}

QVariant
RoamingChannelRefListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((0!=section) || (Qt::Horizontal!=orientation) || (Qt::DisplayRole!=role))
    return QVariant();
  return tr("Roaming Channel");
}


/* ********************************************************************************************* *
 * Implementation of ContactListWrapper
 * ********************************************************************************************* */
ContactListWrapper::ContactListWrapper(ContactList *list, QObject *parent)
  : GenericTableWrapper(list, parent)
{
  // pass...
}

int
ContactListWrapper::columnCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return 4;
}

QVariant
ContactListWrapper::data(const QModelIndex &index, int role) const {
  if ((!index.isValid()) || (index.row()>=_list->count()))
    return QVariant();

  if (Qt::DisplayRole == role) {
    Contact *contact = _list->get(index.row())->as<Contact>();
    if (contact->is<DTMFContact>()) {
      DTMFContact *dtmf = contact->as<DTMFContact>();
      switch (index.column()) {
        case 0:
          return tr("DTMF");
        case 1:
          return dtmf->name();
        case 2:
          return dtmf->number();
        case 3:
          return (dtmf->ring() ? tr("On") : tr("Off"));
        default:
          return QVariant();
      }
    } else if (contact->is<DMRContact>()) {
      DMRContact *dmr = contact->as<DMRContact>();
      switch (index.column()) {
        case 0:
          switch (dmr->type()) {
            case DMRContact::PrivateCall: return tr("Private Call");
            case DMRContact::GroupCall: return tr("Group Call");
            case DMRContact::AllCall: return tr("All Call");
          }
        break;
        case 1:
          return dmr->name();
        case 2:
          return dmr->number();
        case 3:
          return (dmr->ring() ? tr("On") : tr("Off"));
        default:
          return QVariant();
      }
    } else if (contact->is<M17Contact>()) {
      M17Contact *m17 = contact->as<M17Contact>();
      switch (index.column()) {
      case 0:
        return tr("M17");
      case 1:
        return m17->name();
      case 2:
        if (m17->isBroadcast())
          return tr("[Broadcast]");
        return m17->call();
      case 3:
        return (m17->ring() ? tr("On") : tr("Off"));
      default:
        return QVariant();
      }
    }
  }

  return QVariant();
}


QVariant
ContactListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole != role) || (Qt::Horizontal != orientation)) {
    return QVariant();
  }
  if (0 == section) {
    return tr("Type");
  } else if (1 == section) {
    return tr("Name");
  } else if (2 == section) {
    return tr("Number");
  } else if (3 == section) {
    return tr("RX Tone");
  }
  return QVariant();
}


/* ********************************************************************************************* *
 * Implementation of ZoneListWrapper
 * ********************************************************************************************* */
ZoneListWrapper::ZoneListWrapper(ZoneList *list, QObject *parent)
  : GenericListWrapper(list, parent)
{
  // pass...
}

QVariant
ZoneListWrapper::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_list->count()) || (0 != index.column()))
    return QVariant();
  Zone *zone = _list->get(index.row())->as<Zone>();
  return zone->name();
}

QVariant
ZoneListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Zone");
}


/* ********************************************************************************************* *
 * Implementation of PositioningSystemListWrapper
 * ********************************************************************************************* */
PositioningSystemListWrapper::PositioningSystemListWrapper(PositioningSystems *list, QObject *parent)
  : GenericTableWrapper(list, parent)
{
  // pass...
}

int
PositioningSystemListWrapper::columnCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return 6;
}

QVariant
PositioningSystemListWrapper::data(const QModelIndex &index, int role) const {
  if ((! index.isValid()) || (index.row()>=_list->count()))
    return QVariant();
  if ((Qt::DisplayRole!=role) && (Qt::EditRole!=role))
    return QVariant();

  PositioningSystem *sys = _list->get(index.row())->as<PositioningSystem>();

  switch (index.column()) {
  case 0:
    if (sys->is<GPSSystem>())
      return tr("DMR");
    else if (sys->is<APRSSystem>())
      return tr("APRS");
    else
      return tr("OOps!");
  case 1:
    return sys->name();
  case 2:
    if (sys->is<GPSSystem>()) {
      if (! sys->as<GPSSystem>()->hasContact())
        return tr("[None]");
      return sys->as<GPSSystem>()->contactObj()->name();
    } else if (sys->is<APRSSystem>())
      return tr("%1-%2").arg(sys->as<APRSSystem>()->destination())
          .arg(sys->as<APRSSystem>()->destSSID());
    break;
  case 3:
    return sys->period();
  case 4:
    if (sys->is<GPSSystem>()) {
      if ((! sys->as<GPSSystem>()->hasRevertChannel()) || sys->as<GPSSystem>()->revert()->is<SelectedChannel>())
        return tr("[Selected]");
      return sys->as<GPSSystem>()->revertChannel()->name();
    } else if (sys->is<APRSSystem>())
      return ((nullptr != sys->as<APRSSystem>()->revertChannel()) ?
                sys->as<APRSSystem>()->revertChannel()->name() : tr("OOPS!"));
    break;
  case 5:
    if (sys->is<GPSSystem>())
      return tr("[None]");
    else if (sys->is<APRSSystem>())
      return sys->as<APRSSystem>()->message();

  default:
    break;
  }

  return QVariant();
}

QVariant
PositioningSystemListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation))
    return QVariant();
  switch (section) {
  case 0: return tr("Type");
  case 1: return tr("Name");
  case 2: return tr("Destination");
  case 3: return tr("Period [s]");
  case 4: return tr("Channel");
  case 5: return tr("Message");
  default:
    break;
  }
  return QVariant();
}


/* ********************************************************************************************* *
 * Implementation of ScanListsWrapper
 * ********************************************************************************************* */
ScanListsWrapper::ScanListsWrapper(ScanLists *list, QObject *parent)
  : GenericListWrapper(list, parent)
{
  // pass...
}

QVariant
ScanListsWrapper::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_list->count()) || (0 != index.column()))
    return QVariant();
  return _list->get(index.row())->as<ScanList>()->name();
}

QVariant
ScanListsWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Scan-List");
}


/* ********************************************************************************************* *
 * Implementation of GroupListsWrapper
 * ********************************************************************************************* */
GroupListsWrapper::GroupListsWrapper(RXGroupLists *list, QObject *parent)
  : GenericListWrapper(list, parent)
{
  // pass...
}

QVariant
GroupListsWrapper::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (! index.isValid()) || (index.row()>=_list->count()))
    return QVariant();
  return _list->get(index.row())->as<RXGroupList>()->name();
}

QVariant
GroupListsWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((0!=section) || (Qt::Horizontal!=orientation) || (Qt::DisplayRole!=role))
    return QVariant();
  return tr("RX Group Lists");
}


/* ********************************************************************************************* *
 * Implementation of GroupListWrapper
 * ********************************************************************************************* */
GroupListWrapper::GroupListWrapper(RXGroupList *list, QObject *parent)
  : GenericListWrapper(list->contacts(), parent)
{
  // pass...
}

QVariant
GroupListWrapper::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (! index.isValid()) || (index.row()>=_list->count()))
    return QVariant();
  return _list->get(index.row())->as<DMRContact>()->name();
}

QVariant
GroupListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((0!=section) || (Qt::Horizontal!=orientation) || (Qt::DisplayRole!=role))
    return QVariant();
  return tr("Contact");
}


/* ********************************************************************************************* *
 * Implementation of RoamingListWrapper
 * ********************************************************************************************* */
RoamingListWrapper::RoamingListWrapper(RoamingZoneList *list, QObject *parent)
  : GenericListWrapper(list, parent)
{
  // pass...
}

QVariant
RoamingListWrapper::data(const QModelIndex &index, int role) const {
  if ((Qt::DisplayRole!=role) || (index.row()>=_list->count()) || (0 != index.column()))
    return QVariant();
  RoamingZone *zone = _list->get(index.row())->as<RoamingZone>();
  return tr("%1 (containing %2 channels)").arg(zone->name()).arg(zone->count());
}

QVariant
RoamingListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation) || (0 != section))
    return QVariant();
  return tr("Roaming zone");
}


/* ********************************************************************************************* *
 * Implementation of RadioIdListWrapper
 * ********************************************************************************************* */
RadioIdListWrapper::RadioIdListWrapper(RadioIDList *list, QObject *parent)
  : GenericTableWrapper(list, parent)
{
  // pass...
}

int
RadioIdListWrapper::columnCount(const QModelIndex &idx) const {
  Q_UNUSED(idx);
  return 3;
}

QVariant
RadioIdListWrapper::data(const QModelIndex &index, int role) const {
  if ((! index.isValid()) || (index.row()>=_list->count()))
    return QVariant();
  if ((Qt::DisplayRole!=role) && (Qt::EditRole!=role))
    return QVariant();

  DMRRadioID *id = _list->get(index.row())->as<DMRRadioID>();

  switch (index.column()) {
  case 0:
    return ("DMR");
  case 1:
    return id->name();
  case 2:
    return id->number();
  default:
    break;
  }

  return QVariant();
}

QVariant
RadioIdListWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole!=role) || (Qt::Horizontal!=orientation))
    return QVariant();
  switch (section) {
  case 0: return tr("Type");
  case 1: return tr("Name");
  case 2: return tr("Number");
  default:
    break;
  }
  return QVariant();
}


