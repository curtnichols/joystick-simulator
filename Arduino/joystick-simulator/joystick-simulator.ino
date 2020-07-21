// Echos inputs back to the serial port.

// These are boards built on an ATMEGA32U4, which can mimic USB HID devices.
#define SPARKFUN_PRO_MICRO_QWIIC
//#define ACROBOTIC_PRO_MICRO_BEETLE

#if defined(SPARKFUN_PRO_MICRO_QWIIC) && defined(ACROBOTIC_PRO_MICRO_BEETLE)
  #error "Target only one board at a time."
#elif defined(SPARKFUN_PRO_MICRO_QWIIC)
  constexpr int FEEDBACK_LED = 17; // RX LED
  #define LED_ON  LOW
  #define LED_OFF HIGH
#elif defined(ACROBOTIC_PRO_MICRO_BEETLE)
  constexpr int FEEDBACK_LED = 13; // LED
  #define LED_ON  HIGH
  #define LED_OFF LOW
#else
  #error "No target defined."
#endif

constexpr int16_t loop_period = 100;      // milliseconds
constexpr uint16_t initial_light_on = 10; // loop periods

uint16_t light_on_count = initial_light_on;

void setup()
{
  pinMode(FEEDBACK_LED, OUTPUT);  // Set RX LED as an output
  Serial.begin(115200);
}

void HandleSerialInput();

void loop()
{
  digitalWrite(FEEDBACK_LED, light_on_count ? LED_ON : LED_OFF);
  HandleSerialInput();

  if (light_on_count > 0) {
    --light_on_count;
  }

  delay(loop_period); // more or less
}

enum State { Start, Accumulating, Ignoring };
State state = Start;

constexpr size_t command_buffer_length = 32;

State AccumulateUserInput(State state, char ch);

void HandleSerialInput()
{
  while (Serial.available() > 0) {
    char ch = Serial.read();
    state = AccumulateUserInput(state, ch);
    if (state == Start) {
      light_on_count = 10;
    }
  }
}

bool IsCommandTerminator(char ch) {
  return ch == '\r' || ch == '\n';
}

struct CommandBuffer {
  char buffer[command_buffer_length];
  int8_t in_use;

  CommandBuffer() { Reset(); }
  
  bool Append(char ch) {
    if (in_use == 0 && isspace(ch)) {
      return true;
    }
    else if (in_use < command_buffer_length - 1) {
      buffer[in_use++] = ch;
      return true;
    }
    else {
      return false;
    }
  }

  void Reset() {
    memset(buffer, 0, command_buffer_length);
    in_use = 0;
  }

  const char* c_str() const { return buffer; }

  int8_t Length() const { return in_use; }
};

void HandleCommand(const CommandBuffer& command_buffer);

State AccumulateUserInput(State state, char ch) {
  static CommandBuffer command_buffer;
  
  State newState = state;
  
  switch (state) {
    case Start:
      if (IsCommandTerminator(ch)) {
        break;
      }
      
      Serial.print("> ");
      newState = Accumulating;
      // Fall through to Accumulating

    case Accumulating:
      Serial.print(ch);
      if (IsCommandTerminator(ch)) {
        
        HandleCommand(command_buffer);
        newState = Start;
        command_buffer.Reset();
      }
      else {
        command_buffer.Append(ch);
      }
      break;

    case Ignoring:
      if (IsCommandTerminator(ch)) {
        newState = Start;
        command_buffer.Reset();
      }
      break;
  }

  return newState;
}

void ShowMenu();

#define COMMAND_BATTLE "battle"

void HandleCommand(const CommandBuffer& command_buffer) {
  if (command_buffer.Length() < 1) {
    Serial.println("Empty command");
    return;
  }

  if (command_buffer.Length() == 1 && *command_buffer.c_str() == '?') {
    ShowMenu();
    return;
  }

  if (strcasecmp(command_buffer.c_str(), COMMAND_BATTLE) == 0) {
    Serial.println("You battle the orc; you die");
  }
  else {
    Serial.print("Unimplemented function: "); Serial.println(command_buffer.c_str());
    Serial.println("Enter ? to get a command menu.");
  }
}

void ShowMenu() {
  Serial.println("Menu of commands:");
  Serial.println(" ?\tShow this menu");
  Serial.println(" " COMMAND_BATTLE "\tBattle the orc");
}
