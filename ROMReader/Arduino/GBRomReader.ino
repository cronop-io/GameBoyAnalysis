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

#define BANK_SWITCH_ADDRESS 0x2100

void setup() 
{
    pinMode(GAMEBOY_WT, OUTPUT);
    pinMode(GAMEBOY_RD, OUTPUT);

    pinMode(GAMEBOY_A00, OUTPUT);
    pinMode(GAMEBOY_A02, OUTPUT);
    pinMode(GAMEBOY_A04, OUTPUT);
    pinMode(GAMEBOY_A06, OUTPUT);
    pinMode(GAMEBOY_A08, OUTPUT);
    pinMode(GAMEBOY_A10, OUTPUT);
    pinMode(GAMEBOY_A12, OUTPUT);
    pinMode(GAMEBOY_A14, OUTPUT);
    pinMode(GAMEBOY_A01, OUTPUT);
    pinMode(GAMEBOY_A03, OUTPUT);
    pinMode(GAMEBOY_A05, OUTPUT);
    pinMode(GAMEBOY_A07, OUTPUT);
    pinMode(GAMEBOY_A09, OUTPUT);
    pinMode(GAMEBOY_A11, OUTPUT);
    pinMode(GAMEBOY_A13, OUTPUT);
    pinMode(GAMEBOY_A15, OUTPUT);

    pinMode(GAMEBOY_D00, INPUT);
    pinMode(GAMEBOY_D02, INPUT);
    pinMode(GAMEBOY_D04, INPUT);
    pinMode(GAMEBOY_D06, INPUT);
    pinMode(GAMEBOY_D01, INPUT);
    pinMode(GAMEBOY_D03, INPUT);
    pinMode(GAMEBOY_D05, INPUT);
    pinMode(GAMEBOY_D07, INPUT);

    Serial.begin(115200);

}

void writeAddress(uint16_t address)
{
    digitalWrite(GAMEBOY_A00, address & (1 << 0) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A01, address & (1 << 1) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A02, address & (1 << 2) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A03, address & (1 << 3) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A04, address & (1 << 4) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A05, address & (1 << 5) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A06, address & (1 << 6) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A07, address & (1 << 7) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A08, address & (1 << 8) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A09, address & (1 << 9) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A10, address & (1 << 10) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A11, address & (1 << 11) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A12, address & (1 << 12) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A13, address & (1 << 13) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A14, address & (1 << 14) ? HIGH : LOW);
    digitalWrite(GAMEBOY_A15, address & (1 << 15) ? HIGH : LOW);
}

uint8_t readData()
{
    uint8_t data = 0;
    data |= digitalRead(GAMEBOY_D00) << 0;
    data |= digitalRead(GAMEBOY_D01) << 1;
    data |= digitalRead(GAMEBOY_D02) << 2;
    data |= digitalRead(GAMEBOY_D03) << 3;
    data |= digitalRead(GAMEBOY_D04) << 4;
    data |= digitalRead(GAMEBOY_D05) << 5;
    data |= digitalRead(GAMEBOY_D06) << 6;
    data |= digitalRead(GAMEBOY_D07) << 7;

    return data;
}

void selectBank(uint32_t bank)
{
    // Set to write mode
    digitalWrite(GAMEBOY_RD, HIGH);
    digitalWrite(GAMEBOY_WT, LOW);

    // Change the pin typing
    pinMode(GAMEBOY_D00, OUTPUT);
    pinMode(GAMEBOY_D01, OUTPUT);
    pinMode(GAMEBOY_D02, OUTPUT);
    pinMode(GAMEBOY_D03, OUTPUT);
    pinMode(GAMEBOY_D04, OUTPUT);
    pinMode(GAMEBOY_D05, OUTPUT);
    pinMode(GAMEBOY_D06, OUTPUT);
    pinMode(GAMEBOY_D07, OUTPUT);

    // Write the bank address 
    writeAddress(BANK_SWITCH_ADDRESS);

    delay(5);

    // Write the bank to switch to
    digitalWrite(GAMEBOY_D00, bank & (1 << 0) ? HIGH : LOW);
    digitalWrite(GAMEBOY_D01, bank & (1 << 1) ? HIGH : LOW);
    digitalWrite(GAMEBOY_D02, bank & (1 << 2) ? HIGH : LOW);
    digitalWrite(GAMEBOY_D03, bank & (1 << 3) ? HIGH : LOW);
    digitalWrite(GAMEBOY_D04, bank & (1 << 4) ? HIGH : LOW);
    digitalWrite(GAMEBOY_D05, bank & (1 << 5) ? HIGH : LOW);
    digitalWrite(GAMEBOY_D06, bank & (1 << 6) ? HIGH : LOW);
    digitalWrite(GAMEBOY_D07, bank & (1 << 7) ? HIGH : LOW);

    delay(5);

    digitalWrite(GAMEBOY_RD, LOW); 
    digitalWrite(GAMEBOY_WT, HIGH);

    // Set the data to LOW
    digitalWrite(GAMEBOY_D00, LOW);
    digitalWrite(GAMEBOY_D01, LOW);
    digitalWrite(GAMEBOY_D02, LOW);
    digitalWrite(GAMEBOY_D03, LOW);
    digitalWrite(GAMEBOY_D04, LOW);
    digitalWrite(GAMEBOY_D05, LOW);
    digitalWrite(GAMEBOY_D06, LOW);
    digitalWrite(GAMEBOY_D07, LOW);

    // Set pins back as inputs
    pinMode(GAMEBOY_D00, INPUT);
    pinMode(GAMEBOY_D01, INPUT);
    pinMode(GAMEBOY_D02, INPUT);
    pinMode(GAMEBOY_D03, INPUT);
    pinMode(GAMEBOY_D04, INPUT);
    pinMode(GAMEBOY_D05, INPUT);
    pinMode(GAMEBOY_D06, INPUT);
    pinMode(GAMEBOY_D07, INPUT);

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