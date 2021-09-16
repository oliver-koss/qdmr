#include "codeplug.hh"
#include "config.hh"
#include <QtEndian>


/* ********************************************************************************************* *
 * Implementation of CodePlug::Flags
 * ********************************************************************************************* */
CodePlug::Flags::Flags()
  : updateCodePlug(true), autoEnableGPS(false), autoEnableRoaming(false)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of CodePlug::Element
 * ********************************************************************************************* */
CodePlug::Element::Element(uint8_t *ptr, size_t size)
  : _data(ptr), _size(size)
{
  // pass...
}

CodePlug::Element::Element(const Element &other)
  : _data(other._data), _size(other._size)
{
  // pass...
}

CodePlug::Element::~Element() {
  // pass...
}

bool
CodePlug::Element::isValid() const {
  return nullptr != _data;
}

void
CodePlug::Element::clear() {
  // pass...
}

bool
CodePlug::Element::getBit(uint offset, uint bit) const {
  uint8_t *ptr = (_data+offset);
  return (1<<bit) & (*ptr);
}

void
CodePlug::Element::setBit(uint offset, uint bit, bool value) {
  uint8_t *ptr = (_data+offset);
  if (value)
    (*ptr) |= (1<<bit);
  else
    (*ptr) &= ~(1<<bit);
}

void
CodePlug::Element::clearBit(uint offset, uint bit) {
  uint8_t *ptr = (_data+offset);
  (*ptr) &= ~(1<<bit);
}

uint8_t
CodePlug::Element::getUInt2(uint offset, uint bit) const {
  return (((*(_data+offset)) >> bit) & 0b11);
}
void
CodePlug::Element::setUInt2(uint offset, uint bit, uint8_t value) {
  *(_data+offset) &= ~(0b11 << bit);
  *(_data+offset) |= ((value & 0b11)<<bit);
}

uint8_t
CodePlug::Element::getUInt3(uint offset, uint bit) const {
  return (((*(_data+offset)) >> bit) & 0b111);
}
void
CodePlug::Element::setUInt3(uint offset, uint bit, uint8_t value) {
  *(_data+offset) &= ~(0b111 << bit);
  *(_data+offset) |= ((value & 0b111)<<bit);
}

uint8_t
CodePlug::Element::getUInt4(uint offset, uint bit) const {
  return (((*(_data+offset)) >> bit) & 0b1111);
}
void
CodePlug::Element::setUInt4(uint offset, uint bit, uint8_t value) {
  *(_data+offset) &= ~(0b1111 << bit);
  *(_data+offset) |= ((value & 0b1111)<<bit);
}

uint8_t
CodePlug::Element::getUInt5(uint offset, uint bit) const {
  return (((*(_data+offset)) >> bit) & 0b11111);
}
void
CodePlug::Element::setUInt5(uint offset, uint bit, uint8_t value) {
  *(_data+offset) &= ~(0b11111 << bit);
  *(_data+offset) |= ((value & 0b11111)<<bit);
}

uint8_t
CodePlug::Element::getUInt6(uint offset, uint bit) const {
  return (((*(_data+offset)) >> bit) & 0b111111);
}
void
CodePlug::Element::setUInt6(uint offset, uint bit, uint8_t value) {
  *(_data+offset) &= ~(0b111111 << bit);
  *(_data+offset) |= ((value & 0b111111)<<bit);
}

uint8_t
CodePlug::Element::getUInt8(uint offset) const {
  return _data[offset];
}
void
CodePlug::Element::setUInt8(uint offset, uint8_t value) {
  _data[offset] = value;
}

uint16_t
CodePlug::Element::getUInt16_be(uint offset) const {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  return qFromBigEndian(*ptr);
}
uint16_t
CodePlug::Element::getUInt16_le(uint offset) const {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  return qFromLittleEndian(*ptr);
}
void
CodePlug::Element::setUInt16_be(uint offset, uint16_t value) {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  (*ptr) = qToBigEndian(value);
}
void
CodePlug::Element::setUInt16_le(uint offset, uint16_t value) {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  (*ptr) = qToLittleEndian(value);
}

uint32_t
CodePlug::Element::getUInt24_be(uint offset) const {
  uint8_t *ptr = _data+offset;
  return uint32_t(ptr[2]) + (uint32_t(ptr[1])<<8) + (uint32_t(ptr[0])<<16);
}
uint32_t
CodePlug::Element::getUInt24_le(uint offset) const {
  uint8_t *ptr = _data+offset;
  return uint32_t(ptr[0]) + (uint32_t(ptr[1])<<8) + (uint32_t(ptr[2])<<16);
}
void
CodePlug::Element::setUInt24_be(uint offset, uint32_t value) {
  uint8_t *ptr = _data+offset;
  ptr[0] = ((value >> 16) & 0xff);
  ptr[1] = ((value >> 8)  & 0xff);
  ptr[2] = ((value >> 0)  & 0xff);
}
void
CodePlug::Element::setUInt24_le(uint offset, uint32_t value) {
  uint8_t *ptr = _data+offset;
  ptr[0] = ((value >> 0)  & 0xff);
  ptr[1] = ((value >> 8)  & 0xff);
  ptr[2] = ((value >> 16) & 0xff);
}

uint32_t
CodePlug::Element::getUInt32_be(uint offset) const {
  uint32_t *ptr = (uint32_t *)(_data+offset);
  return qFromBigEndian(*ptr);
}
uint32_t
CodePlug::Element::getUInt32_le(uint offset) const {
  uint32_t *ptr = (uint32_t *)(_data+offset);
  return qFromLittleEndian(*ptr);
}
void
CodePlug::Element::setUInt32_be(uint offset, uint32_t value) {
  uint32_t *ptr = (uint32_t *)(_data+offset);
  (*ptr) = qToBigEndian(value);
}
void
CodePlug::Element::setUInt32_le(uint offset, uint32_t value) {
  uint32_t *ptr = (uint32_t *)(_data+offset);
  (*ptr) = qToLittleEndian(value);
}

uint8_t
CodePlug::Element::getBCD2(uint offset) const {
  uint8_t val = getUInt8(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10;
}
void
CodePlug::Element::setBCD2(uint offset, uint8_t val) {
  uint8_t a  = (val / 10) % 10;
  uint8_t b  = (val /  1) % 10;
  setUInt8(offset, (a << 4) + b);
}

uint16_t
CodePlug::Element::getBCD4_be(uint offset) const {
  uint32_t val = getUInt16_be(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10 + ((val>>8) & 0xf)*100 + ((val>>12) & 0xf)*1000;
}
void
CodePlug::Element::setBCD4_be(uint offset, uint16_t val) {
  uint32_t a  = (val / 1000) % 10;
  uint32_t b  = (val /  100) % 10;
  uint32_t c  = (val /   10) % 10;
  uint32_t d  = (val /    1) % 10;
  setUInt16_be(offset, (a << 12) + (b << 8) + (c << 4) + d);
}
uint16_t
CodePlug::Element::getBCD4_le(uint offset) const {
  uint32_t val = getUInt16_le(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10 + ((val>>8) & 0xf)*100 + ((val>>12) & 0xf)*1000;
}
void
CodePlug::Element::setBCD4_le(uint offset, uint16_t val) {
  uint32_t a  = (val / 1000) % 10;
  uint32_t b  = (val /  100) % 10;
  uint32_t c  = (val /   10) % 10;
  uint32_t d  = (val /    1) % 10;
  setUInt16_le(offset, (a << 12) + (b << 8) + (c << 4) + d);
}

uint32_t
CodePlug::Element::getBCD8_be(uint offset) const {
  uint32_t val = getUInt32_be(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10 + ((val>>8) & 0xf)*100 + ((val>>12) & 0xf)*1000 +
      ((val>>16) & 0xf)*10000 + ((val>>20) & 0xf)*100000 + ((val>>24) & 0xf)*1000000 +
      ((val>>28) & 0xf)*10000000;
}
void
CodePlug::Element::setBCD8_be(uint offset, uint32_t val) {
  uint32_t a  = (val / 10000000) % 10;
  uint32_t b  = (val /  1000000) % 10;
  uint32_t c  = (val /   100000) % 10;
  uint32_t d  = (val /    10000) % 10;
  uint32_t e  = (val /     1000) % 10;
  uint32_t f  = (val /      100) % 10;
  uint32_t g  = (val /       10) % 10;
  uint32_t h  = (val /        1) % 10;
  setUInt32_be(offset, (a << 28) + (b << 24) + (c << 20) + (d << 16) + (e << 12) + (f << 8) + (g << 4) + h);
}
uint32_t
CodePlug::Element::getBCD8_le(uint offset) const {
  uint32_t val = getUInt32_le(offset);
  return (val & 0xf) + ((val>>4) & 0xf)*10 + ((val>>8) & 0xf)*100 + ((val>>12) & 0xf)*1000 +
      ((val>>16) & 0xf)*10000 + ((val>>20) & 0xf)*100000 + ((val>>24) & 0xf)*1000000 +
      ((val>>28) & 0xf)*10000000;
}
void
CodePlug::Element::setBCD8_le(uint offset, uint32_t val) {
  uint32_t a  = (val / 10000000) % 10;
  uint32_t b  = (val /  1000000) % 10;
  uint32_t c  = (val /   100000) % 10;
  uint32_t d  = (val /    10000) % 10;
  uint32_t e  = (val /     1000) % 10;
  uint32_t f  = (val /      100) % 10;
  uint32_t g  = (val /       10) % 10;
  uint32_t h  = (val /        1) % 10;
  setUInt32_le(offset, (a << 28) + (b << 24) + (c << 20) + (d << 16) + (e << 12) + (f << 8) + (g << 4) + h);
}

QString
CodePlug::Element::readASCII(uint offset, uint maxlen, uint8_t eos) const {
  QString txt;
  uint8_t *ptr = (uint8_t *)(_data+offset);
  for (uint i=0; (i<maxlen)&&(ptr[i])&&(eos!=ptr[i]); i++) {
    txt.append(QChar::fromLatin1(ptr[i]));
  }
  return txt;
}
void
CodePlug::Element::writeASCII(uint offset, const QString &txt, uint maxlen, uint8_t eos) {
  uint8_t *ptr = (uint8_t *)(_data+offset);
  for (uint i=0; i<maxlen; i++) {
    if (i < uint(txt.length()))
      ptr[i] = txt.at(i).toLatin1();
    else
      ptr[i] = eos;
  }
}

QString
CodePlug::Element::readUnicode(uint offset, uint maxlen, uint16_t eos) const {
  QString txt;
  uint16_t *ptr = (uint16_t *)(_data+offset);
  for (uint i=0; (i<maxlen)&&(eos!=ptr[i]); i++) {
    txt.append(QChar(ptr[i]));
  }
  return txt;
}
void
CodePlug::Element::writeUnicode(uint offset, const QString &txt, uint maxlen, uint16_t eos) {
  uint16_t *ptr = (uint16_t *)(_data+offset);
  for (uint i=0; i<maxlen; i++) {
    if (i < uint(txt.length()))
      ptr[i] = txt.at(i).unicode();
    else
      ptr[i] = eos;
  }
}


/* ********************************************************************************************* *
 * Implementation of CodePlug::Context
 * ********************************************************************************************* */
CodePlug::Context::Context()
  : _tables()
{
  // Add tables for common elements
  addTable(&RadioID::staticMetaObject);
  addTable(&DigitalContact::staticMetaObject);
  addTable(&DTMFContact::staticMetaObject);
  addTable(&RXGroupList::staticMetaObject);
  addTable(&Channel::staticMetaObject);
  addTable(&Zone::staticMetaObject);
  addTable(&ScanList::staticMetaObject);
  addTable(&GPSSystem::staticMetaObject);
  addTable(&APRSSystem::staticMetaObject);
  addTable(&RoamingZone::staticMetaObject);
}

bool
CodePlug::Context::hasTable(const QMetaObject *obj) const {
  // Find a matching table
  if (_tables.contains(obj->className()))
    return true;
  else if (obj->superClass())
    return hasTable(obj->superClass());
  return false;
}

CodePlug::Context::Table &
CodePlug::Context::getTable(const QMetaObject *obj) {
  if (_tables.contains(obj->className()))
    return _tables[obj->className()];
  return getTable(obj->superClass());
}

bool
CodePlug::Context::addTable(const QMetaObject *obj) {
  if (hasTable(obj))
    return false;
  _tables.insert(obj->className(), Table());
  return true;
}

ConfigObject *
CodePlug::Context::obj(const QMetaObject *elementType, uint idx) {
  if (! hasTable(elementType))
    return nullptr;
  return getTable(elementType).objects.value(idx, nullptr);
}

int
CodePlug::Context::index(ConfigObject *obj) {
  if (! hasTable(obj->metaObject()))
    return -1;
  return getTable(obj->metaObject()).indices.value(obj, -1);
}

bool
CodePlug::Context::add(ConfigObject *obj, uint idx) {
  if (!hasTable(obj->metaObject()))
    return false;
  if (getTable(obj->metaObject()).indices.contains(obj))
    return false;
  if (getTable(obj->metaObject()).objects.contains(idx))
    return false;
  getTable(obj->metaObject()).objects.insert(idx, obj);
  getTable(obj->metaObject()).indices.insert(obj, idx);
  return true;
}


/* ********************************************************************************************* *
 * Implementation of CodePlug
 * ********************************************************************************************* */
CodePlug::CodePlug(QObject *parent)
  : DFUFile(parent)
{
	// pass...
}

CodePlug::~CodePlug() {
	// pass...
}
