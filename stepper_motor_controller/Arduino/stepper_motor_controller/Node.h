#ifndef ___NODE__H___
#define ___NODE__H___

#include <stdint.h>
#include <Arduino.h>
#include <NadaMQ.h>
#include <TimerOne.h>
#include <CArrayDefs.h>
#include "RPCBuffer.h"  // Define packet sizes
#include "StepperMotorController/Properties.h"  // Define package name, URL, etc.
#include <BaseNodeRpc/BaseNode.h>
#include <BaseNodeRpc/BaseNodeRpc.h>
#include <BaseNodeRpc/BaseNodeEeprom.h>
#include <BaseNodeRpc/BaseNodeI2c.h>
#include <BaseNodeRpc/BaseNodeConfig.h>
#include <BaseNodeRpc/BaseNodeState.h>
#include <BaseNodeRpc/BaseNodeSerialHandler.h>
#include <BaseNodeRpc/BaseNodeI2cHandler.h>
#include <BaseNodeRpc/I2cHandler.h>
#include <BaseNodeRpc/SerialHandler.h>
#include <pb_validate.h>
#include <pb_eeprom.h>
#include "stepper_motor_controller_config_validate.h"
#include "stepper_motor_controller_state_validate.h"
#include "StepperMotorController/config_pb.h"
#include "StepperMotorController/state_pb.h"


namespace stepper_motor_controller {
const size_t FRAME_SIZE = (3 * sizeof(uint8_t)  // Frame boundary
                           - sizeof(uint16_t)  // UUID
                           - sizeof(uint16_t)  // Payload length
                           - sizeof(uint16_t));  // CRC

const char HARDWARE_VERSION_[] = "0.1";
class Node;

typedef nanopb::EepromMessage<stepper_motor_controller_Config,
                              config_validate::Validator<Node> > config_t;
typedef nanopb::Message<stepper_motor_controller_State,
                        state_validate::Validator<Node> > state_t;

class Node :
  public BaseNode,
  public BaseNodeEeprom,
  public BaseNodeI2c,
  public BaseNodeConfig<config_t>,
  public BaseNodeState<state_t>,
#ifndef DISABLE_SERIAL
  public BaseNodeSerialHandler,
#endif  // #ifndef DISABLE_SERIAL
  public BaseNodeI2cHandler<base_node_rpc::i2c_handler_t> {
public:
  typedef PacketParser<FixedPacket> parser_t;

  static const uint16_t BUFFER_SIZE = 128;  // >= longest property string

  static const uint8_t STEP_PIN = 3;
  static const uint8_t DIR_PIN = 4;
  static const uint8_t MS1_PIN = 5;
  static const uint8_t MS2_PIN = 6;
  static const uint8_t MS3_PIN = 7;

  static uint16_t steps_;
  static bool is_running_;

  uint8_t buffer_[BUFFER_SIZE];

  Node() : BaseNode(), BaseNodeConfig<config_t>(stepper_motor_controller_Config_fields),
           BaseNodeState<state_t>(stepper_motor_controller_State_fields) {}

  UInt8Array get_buffer() { return UInt8Array_init(sizeof(buffer_), buffer_); }
  /* This is a required method to provide a temporary buffer to the
   * `BaseNode...` classes. */

  void begin();

  void set_i2c_address(uint8_t value);  // Override to validate i2c address

  /****************************************************************************
   * # User-defined methods #
   *
   * Add new methods below.  When Python package is generated using the
   * command, `paver sdist` from the project root directory, the signatures of
   * the methods below will be scanned and code will automatically be generated
   * to support calling the methods from Python over a serial connection.
   *
   * e.g.
   *
   *     bool less_than(float a, float b) { return a < b; }
   *
   * See [`arduino_rpc`][1] and [`base_node_rpc`][2] for more details.
   *
   * [1]: https://github.com/wheeler-microfluidics/arduino_rpc
   * [2]: https://github.com/wheeler-microfluidics/base_node_rpc
   */
  UInt8Array hardware_version() { return UInt8Array_init(strlen(HARDWARE_VERSION_),
                      (uint8_t *)&HARDWARE_VERSION_[0]); }

  static void _timer_callback() {
    steps_--;
    digitalWrite(STEP_PIN, HIGH);
    digitalWrite(STEP_PIN, LOW);
    if (steps_ == 0) {
      stop();
    }
  }

  static void stop() {
    is_running_ = false;
    Timer1.stop();
  }

  bool direction() { return digitalRead(DIR_PIN); }

  void move(int16_t steps, float steps_per_second) {
    stop();
    is_running_ = true;
    digitalWrite(DIR_PIN, steps > 0);
    steps_ = abs(steps);
    Timer1.setPeriod(1e6 / steps_per_second); // set timer period in us
    Timer1.restart();
  }

  bool is_running() { return is_running_; }

  uint16_t steps_remaining() { return steps_; }

  bool on_config_microstep_setting_changed(uint32_t microstep_setting) {
    /* This method is triggered whenever a microstep is included in a config
    * update. */
    if (microstep_setting == 1) {
      digitalWrite(MS1_PIN, 0);
      digitalWrite(MS2_PIN, 0);
      digitalWrite(MS3_PIN, 0);
    } else if (microstep_setting == 16) {
      digitalWrite(MS1_PIN, 1);
      digitalWrite(MS2_PIN, 1);
      digitalWrite(MS3_PIN, 1);
    } else {
      return false;
    }
    return true;
  }
};

}  // namespace stepper_motor_controller


#endif  // #ifndef ___NODE__H___
