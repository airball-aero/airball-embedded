namespace airball {

/**
 * A DeviceKeyboardListener listens for keyboard input on a running embedded
 * device. This allows an operator to gain control by connecting a USB keyboard
 * and typing commands. Most importantly, it gives the operator a way to exit
 * the GUI of the device which is not likely to be accidentally activated during
 * use in flight.
 */
class DeviceKeyboardListener {
public:
  /**
   * Create a new DeviceKeyboardListener which listens to std::cin on a separate
   * thread.
   */
  DeviceKeyboardListener();
  ~DeviceKeyboardListener();

  /**
   * Run the main loop, listening for keyboard input.
   */
  void run();
private:
};

}  // namespace airball