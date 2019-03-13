/*
 * ------------------------------------
 *             MFRC522      Arduino
 *             Reader/PCD   Uno/101
 * Signal      Pin          Pin
 * ------------------------------------
 * RST/Reset   RST          9
 * SPI SS      SDA(SS)      10
 * SPI MOSI    MOSI         11 / ICSP-4
 * SPI MISO    MISO         12 / ICSP-1
 * SPI SCK     SCK          13 / ICSP-3
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, see typical pin layout above

#define MASTER_BTN_PIN 2 // Enters master edit mode, in which a new master card is set
#define ADD_USER_BTN_PIN 3 // Configurable, see typical pin layout above

#define MAX_USER_CARDS 10 // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

byte masterCard[4];
byte readCard[4];
byte userCards[MAX_USER_CARDS][4];
int userCardsCount = 0;

bool isSettingMasterCard;
bool isLookingForAccess;
bool isAddingNewUserCard;

void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);                                // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                       // Init SPI bus
  mfrc522.PCD_Init();                // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details

  isSettingMasterCard = false;
  isLookingForAccess = true;
  isAddingNewUserCard = false;

  pinMode(MASTER_BTN_PIN, INPUT_PULLUP);
  pinMode(ADD_USER_BTN_PIN, INPUT_PULLUP);

  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop()
{
  if (isLookingForAccess)
  {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
    {
      // read new card
      for (uint8_t i = 0; i < 4; i++)
      {
        readCard[i] = mfrc522.uid.uidByte[i];
        // Serial.print(readCard[i], HEX);
      }

      bool isUser = isUserCard(readCard);

      if (isUser)
      {
        Serial.println(F("YES ACCESS! :D"));
        delay(100);
      }
      else
      {
        Serial.println(F("NO ACCESS!! :/"));
        delay(100);
      }

    }
  }

  // Tentando ler novo cartão master
  if (isSettingMasterCard)
  {
    isLookingForAccess = false;

    if (!mfrc522.PICC_IsNewCardPresent()) { return; }
    if (!mfrc522.PICC_ReadCardSerial()) { return; }

    // Grava o uid do cartão master
    for (uint8_t i = 0; i < 4; i++)
    {
      masterCard[i] = mfrc522.uid.uidByte[i];
    }

    Serial.println(F("New master card has been set"));

    delay(2000);
    isSettingMasterCard = false;
    isLookingForAccess = true;
  }

  // Tentando ler novo cartão master
  if (isAddingNewUserCard)
  {
    // nothing to do if all user slots are filled
    if (userCardsCount < MAX_USER_CARDS)
    {

      isLookingForAccess = false;

      if (!mfrc522.PICC_IsNewCardPresent()) { return; }
      if (!mfrc522.PICC_ReadCardSerial()) { return; }

      // grava o uid do novo cartão de usuário
      for (uint8_t i = 0; i < 4; i++)
      {
        userCards[userCardsCount][i] = mfrc522.uid.uidByte[i];
      }
      userCardsCount++;

      Serial.println(F("New user card has been added"));

      delay(2000);
      isLookingForAccess = true;
    }

    else
    {
      Serial.println(F("No more room for users"));
    }

    isAddingNewUserCard = false;
  }

  if (digitalRead(MASTER_BTN_PIN) == LOW) {
    Serial.println(F("Please set new master card."));
    isSettingMasterCard = true;
  }

  if (digitalRead(ADD_USER_BTN_PIN) == LOW) {
    Serial.println(F("Please add new user card."));
    isAddingNewUserCard = true;
  }
}

bool isUserCard(byte card[])
{
  // looping through all possible user cards
  for (uint8_t i = 0; i < MAX_USER_CARDS; i++)
  {
    bool didCardFail = false;
    // comparing one stored user card with the readCard
    for (uint8_t j = 0; j < 4; j++)
    {
      if (card[j] != userCards[i][j])
      {
        // bail early on checking a card if any byte fails
        didCardFail = true;
        break;
      }
    }

    // return early if a card is matched
    if (!didCardFail)
    {
      return true;
    }
  }

  return isMasterCard(card);
}

bool isMasterCard(byte card[])
{
  for (uint8_t i = 0; i < 4; i++)
  {
    if (card[i] != masterCard[i])
    {
      // return early on checking a card if any byte fails
      return false;
    }
  }

  return true;
}