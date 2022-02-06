#include "rd5r_limits.hh"
#include "radioid.hh"
#include "channel.hh"

RD5RLimits::RD5RLimits(QObject *parent)
  : RadioLimits(parent)
{
  _elements = QHash<QString, RadioLimitElement*>{
    /* Define limits for radio IDs. */
    { "radioIDs",
      new RadioLimitList(
            1, 1, new RadioLimitObjects {
              { DMRRadioID::staticMetaObject,
                new RadioLimitObject {
                  {"name", new RadioLimitString(1,16, RadioLimitString::ASCII) },
                  {"id", new RadioLimitUInt(0, 16777215)}
                } }
            })},

    /* Define limits for channel list. */
    { "channels",
      new RadioLimitList(
            1, 1024, new RadioLimitObjects{
              { AnalogChannel::staticMetaObject,
                new RadioLimitObject {
                  {"name", new RadioLimitString(1,16, RadioLimitString::ASCII)},
                  {"rxFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 470.}})},
                  {"txFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 470.}})},
                  {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
                  {"timeout", new RadioLimitUInt(0, 3825)},
                  /// @todo Implement RadioLimitBool for rxOnly check.
                  /// @todo Implement RadioLimitObjRef for scanlist reference.
                  {"vox", new RadioLimitUInt(0, 10)},
                  // openGD77 channel extension
                  // tyt channel extension
                } },
              { DigitalChannel::staticMetaObject,
                new RadioLimitObject {
                  {"name", new RadioLimitString(1,16, RadioLimitString::ASCII)},
                  {"rxFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 470.}})},
                  {"txFrequency", new RadioLimitFrequencies({{136., 174.}, {400., 470.}})},
                  {"power", new RadioLimitEnum{unsigned(Channel::Power::Low), unsigned(Channel::Power::High)}},
                  {"timeout", new RadioLimitUInt(0, 3825)},
                  /// @todo Implement RadioLimitBool for rxOnly check.
                  /// @todo Implement RadioLimitObjRef for scanlist reference.
                  {"vox", new RadioLimitUInt(0, 10)},
                  /// @todo Implement RadioLimitBool for rxOnly check.
                  // openGD77 channel extension
                  // tyt channel extension
                } }
            }) },

    /* Define limits for zone list. */
    { "zones",
      new RadioLimitList(
            1, 250, new RadioLimitObject{
              { "name", new RadioLimitString(1, 16, RadioLimitString::ASCII)}
              /// @todo Implement RadioLimitRefList for zone A&B
            }) }
  };
}