// Pin mappings
#define GAMEBOY_RD 25
#define GAMEBOY_A00 27
#define GAMEBOY_A02 29
#define GAMEBOY_A04 31
#define GAMEBOY_A06 33
#define GAMEBOY_A08 35
#define GAMEBOY_A10 37
#define GAMEBOY_A12 39
#define GAMEBOY_A14 41
#define GAMEBOY_D00 43
#define GAMEBOY_D02 45
#define GAMEBOY_D04 47
#define GAMEBOY_D06 49
#define GAMEBOY_WT 24
#define GAMEBOY_A01 26
#define GAMEBOY_A03 28
#define GAMEBOY_A05 30
#define GAMEBOY_A07 32
#define GAMEBOY_A09 34
#define GAMEBOY_A11 36
#define GAMEBOY_A13 38
#define GAMEBOY_A15 40
#define GAMEBOY_D01 42
#define GAMEBOY_D03 44
#define GAMEBOY_D05 46
#define GAMEBOY_D07 48

// Address pins
const static uint32_t ADDRESS_PINS[16] = 
{
    GAMEBOY_A00,
    GAMEBOY_A01,
    GAMEBOY_A02,
    GAMEBOY_A03,
    GAMEBOY_A04,
    GAMEBOY_A05,
    GAMEBOY_A06,
    GAMEBOY_A07,
    GAMEBOY_A08,
    GAMEBOY_A09,
    GAMEBOY_A10,
    GAMEBOY_A11,
    GAMEBOY_A12,
    GAMEBOY_A13,
    GAMEBOY_A14,
    GAMEBOY_A15
};

// Data pins
const static uint32_t DATA_PINS[8] = 
{
    GAMEBOY_D00,
    GAMEBOY_D01,
    GAMEBOY_D02,
    GAMEBOY_D03,
    GAMEBOY_D04,
    GAMEBOY_D05,
    GAMEBOY_D06,
    GAMEBOY_D07
};

#define BANK_SWITCH_ADDRESS 0x2100

#define SERIAL_BAUD_RATE 115200

void setup() 
{
    // Read/Write pin as outputs by default
    pinMode(GAMEBOY_WT, OUTPUT);
    pinMode(GAMEBOY_RD, OUTPUT);

    // Address pins as outputs
    for (uint32_t i = 0; i < sizeof(ADDRESS_PINS)/sizeof(ADDRESS_PINS[0]); i++)
    {
        pinMode(ADDRESS_PINS[i], OUTPUT);
    }

    // Data pins as inputs
    for (uint32_t i = 0; i < sizeof(DATA_PINS)/sizeof(DATA_PINS[0]); i++)
    {
        pinMode(DATA_PINS[i], INPUT);
    }

    // Start serial connection to host
    Serial.begin(SERIAL_BAUD_RATE);
}

void writeAddress(uint16_t address)
{
    // Write each of the bits into the address pins
    for (uint32_t i = 0; i < sizeof(ADDRESS_PINS)/sizeof(ADDRESS_PINS[0]); i++)
    {
        digitalWrite(ADDRESS_PINS[i], address & (1 << i) ? HIGH : LOW);
    }
}

uint8_t readData()
{
    uint8_t data = 0;

    // Read each of the data pins and construct the byte data
    for (uint32_t i = 0; i < sizeof(DATA_PINS)/sizeof(DATA_PINS[0]); i++)
    {
        data |= digitalRead(DATA_PINS[i]) << i;
    }

    return data;
}

void selectBank(uint32_t bank)
{
    // Set to write mode
    digitalWrite(GAMEBOY_RD, HIGH);
    digitalWrite(GAMEBOY_WT, LOW);

    // Change the pin typing
    for (uint32_t i = 0; i < sizeof(DATA_PINS)/sizeof(DATA_PINS[0]); i++)
    {
        pinMode(DATA_PINS[i], OUTPUT);
    }

    // Write the bank address 
    writeAddress(BANK_SWITCH_ADDRESS);

    delay(5);

    // Write the bank to switch to
    for (uint32_t i = 0; i < sizeof(DATA_PINS)/sizeof(DATA_PINS[0]); i++)
    {
        digitalWrite(DATA_PINS[i], bank & (1 << i) ? HIGH : LOW);
    }

    delay(5);

    digitalWrite(GAMEBOY_RD, LOW); 
    digitalWrite(GAMEBOY_WT, HIGH);

    // Set the data to LOW
    for (uint32_t i = 0; i < sizeof(DATA_PINS)/sizeof(DATA_PINS[0]); i++)
    {
        digitalWrite(DATA_PINS[i], LOW);
    }

    // Set pins back as inputs
    for (uint32_t i = 0; i < sizeof(DATA_PINS)/sizeof(DATA_PINS[0]); i++)
    {
        pinMode(DATA_PINS[i], INPUT);
    }

    delay(5);
}

static uint32_t currentBank = 1;

void loop() 
{
    uint32_t input = 0;
    uint32_t selectedBank = currentBank;
    uint16_t selectedAddress = 0;

    uint8_t data = 0;

    // Set to Read Mode
    digitalWrite(GAMEBOY_RD, LOW); 
    digitalWrite(GAMEBOY_WT, HIGH);

    while(!Serial.available()){}
    Serial.readBytes((uint8_t*)&input, sizeof(uint32_t));

    selectedAddress = (uint16_t) input & 0xFFFF;
    selectedBank = (input >> 16) + 1;

    // If we are reading from the banked rom range, make sure 
    // we are in the appropiate bank
    if (selectedAddress >= 0x4000 && currentBank != selectedBank)
    {
        selectBank(selectedBank);
        currentBank = selectedBank;
    }

    for (uint32_t i = 0; i < sizeof(uint32_t); i ++)
    {
        writeAddress(selectedAddress + i);
        delay(5);
        data = readData();
        Serial.write((uint8_t *)&data, 1);
    }
}